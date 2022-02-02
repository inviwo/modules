import argparse
import re
import os
import dataclasses
import traceback
import subprocess
import xml.etree.ElementTree as ET
from pathlib import Path
from functools import partial
from collections.abc import Iterable
from typing import Callable, Optional, TypeVar, Union, Tuple

import rich
from rich.console import Console
from rich.syntax import Syntax


def makeCmdParser():
    parser = argparse.ArgumentParser(
        description="Parse paraview xml",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    parser.add_argument('-x', '--xml', nargs='*', action="store", dest="xml",
                        help='xmlfiles', required=True)


T = TypeVar('T')
R = TypeVar('R')


def chain(val: Optional[T], *funcs: Callable[[T], R]):
    if val is None:
        return val
    elif len(funcs) > 1:
        return chain(funcs[0](val), *funcs[1:])
    else:
        return funcs[0](val)


def fjoin(iter: Iterable):
    class formatter:
        def __init__(self, iter: Iterable):
            self.iter = iter

        def __format__(self, format_spec: str):
            parts = format_spec.split('|', maxsplit=1)
            spec = f"{{:{parts[0]}}}"
            delm = parts[1] if len(parts) > 1 else ", "
            return delm.join(spec.format(i) for i in iter)

    return formatter(iter)


def xmlattr(name: str):
    def get(xml: ET.Element):
        return xml.attrib.get(name, None)
    return get


def xmlfind(path: str):
    def get(xml: ET.Element):
        return xml.find(path)
    return get


def stripEachLine(text: str):
    return ('\n'.join(map(partial(str.strip), text.split('\n')))).strip()


@dataclasses.dataclass
class BoolProperty:
    defaultValue: Optional[int]


@dataclasses.dataclass
class IntVecProperty:
    defaultValue: Optional[list[int]]
    minValue: Optional[int]
    maxValue: Optional[int]


@dataclasses.dataclass
class DoubleVecProperty:
    defaultValue: Optional[list[float]]
    minValue: Optional[float]
    maxValue: Optional[float]


@dataclasses.dataclass
class StringProperty:
    defaultValue: Optional[str]


@dataclasses.dataclass
class IntOptionProperty:
    options: list[Tuple[str, int]] = dataclasses.field(default_factory=list)
    defaultValue: Optional[int] = None


@dataclasses.dataclass
class FilterPropertyData:
    identifier: str
    displayName: str
    command: str
    numElem: Optional[int] = None
    kind: Union[IntVecProperty, DoubleVecProperty, IntOptionProperty,
                StringProperty, BoolProperty, None] = None
    doc: Optional[str] = None


@dataclasses.dataclass
class FilterData:
    identifier: str
    displayName: str
    className: str
    desc: str
    doc: str = dataclasses.field(repr=False)
    props: list[FilterPropertyData] = dataclasses.field(default_factory=list)


def parseHelperProperty(xml: ET.Element) -> FilterPropertyData:
    data = FilterPropertyData(
        identifier=xml.attrib["name"].replace(' ', ''),
        displayName=xml.attrib["label" if "label" in xml.attrib else "name"],
        command=xml.attrib["command"],
        numElem=int(xml.attrib["number_of_elements"])
        if "number_of_elements" in xml.attrib else None
    )
    if (doc := xml.find('Documentation')) is not None:
        data.doc = stripEachLine(doc.text)

    return data


def parseIntVectorProperty(xml: ET.Element) -> Optional[FilterPropertyData]:
    data = parseHelperProperty(xml)

    if xml.find('BooleanDomain') is not None:
        data.kind = BoolProperty(
            defaultValue=chain(xml, xmlattr("default_values"), int, bool)
        )
    elif (enumeration := xml.find('EnumerationDomain')) is not None:
        data.kind = IntOptionProperty(
            options=[(value.attrib["text"], int(value.attrib["value"])) for value in enumeration],
        )
        defaultValue = chain(xml, xmlattr("default_values"), int)
        data.kind.defaultValue = next((i for i, v in enumerate(data.kind.options)
                                       if v[1] == defaultValue), None)
    elif data.numElem >= 0 and data.numElem <= 4:
        data.kind = IntVecProperty(
            defaultValue=chain(xml, xmlattr("default_values"),
                               str.split, partial(map, int), tuple),
            minValue=chain(xml, xmlfind('IntRangeDomain'), xmlattr("min"), int),
            maxValue=chain(xml, xmlfind('IntRangeDomain'), xmlattr("max"), int)
        )
    else:
        raise Exception("Unhandled IntProperty")
    return data


def parseDoubleVectorProperty(xml: ET.Element) -> Optional[FilterPropertyData]:
    data = parseHelperProperty(xml)

    if data.numElem >= 0 and data.numElem <= 4:
        data.kind = DoubleVecProperty(
            defaultValue=chain(xml, xmlattr("default_values"),
                               str.split, partial(map, float), tuple),
            minValue=chain(xml, xmlfind('DoubleRangeDomain'), xmlattr("min"), float),
            maxValue=chain(xml, xmlfind('DoubleRangeDomain'), xmlattr("max"), float)
        )
    else:
        raise Exception("Unhandled DoubleProperty")
    return data


def parseStringVectorProperty(xml: ET.Element) -> Optional[FilterPropertyData]:
    data = parseHelperProperty(xml)
    data.kind = StringProperty(
        defaultValue=xml.attrib.get("default_values", None))
    return data


propertyParsers: dict[str, Callable[[ET.Element], Optional[FilterPropertyData]]] = {
    "IntVectorProperty": parseIntVectorProperty,
    "StringVectorProperty": parseStringVectorProperty,
    "DoubleVectorProperty": parseDoubleVectorProperty
}


def parse(file: str) -> FilterData:
    tree = ET.parse(file)
    proxy = tree.getroot()[0][0]

    doc = proxy.find('Documentation')
    docShort = re.sub(' +', ' ', doc.attrib['short_help'])
    docStr = stripEachLine(doc.text)
    data = FilterData(
        identifier=proxy.attrib["name"],
        displayName=proxy.attrib["label"],
        className=proxy.attrib["class"],
        desc=docShort,
        doc=docStr
    )

    for elem in proxy:
        if (parser := propertyParsers.get(elem.tag, None)) is not None:
            try:
                if (propData := parser(elem)) is not None:
                    data.props.append(propData)
            except Exception as e:
                print(f"Error parsing {file} \n{e}")
                ET.dump(elem)
                print(traceback.format_exc())

    return data


doubleTemplate = """
struct {structName} {{
    void set({className}& filter) {{
        filter.{command}(property.get());
    }}
    DoubleProperty property{{"{identifier}", "{displayName}",
                             {defaultValue[0]},
                             std::pair{{{minValue}, ConstraintBehavior::Ignore}},
                             std::pair{{{maxValue}, ConstraintBehavior::Ignore}}}};
}};
"""

doubleVec2Template = """
struct {structName} {{
    void set({className}& filter) {{
        filter.{command}(property.get(0), property.get(1));
    }}
    DoubleVec2Property property{{"{identifier}", "{displayName}",
                                dvec2{{{defaultValue[0]}, {defaultValue[1]}}},
                                std::pair{{dvec2{{{minValue}}}, ConstraintBehavior::Ignore}},
                                std::pair{{dvec2{{{maxValue}}}, ConstraintBehavior::Ignore}}}};
}};
"""

doubleVec3Template = """
struct {structName} {{
    void set({className}& filter) {{
        filter.{command}(property.get(0), property.get(1), property.get(2)));
    }}
    DoubleVec3Property property{{"{identifier}", "{displayName}",
                                dvec3{{{defaultValue[0]}, {defaultValue[1]}, {defaultValue[2]}}},
                                std::pair{{dvec3{{{minValue}}}, ConstraintBehavior::Ignore}},
                                std::pair{{dvec3{{{maxValue}}}, ConstraintBehavior::Ignore}}}};
}};
"""

intTemplate = """
struct {structName} {{
    void set({className}& filter) {{
        filter.{command}(property.get());
    }}
    IntProperty property{{"{identifier}", "{displayName}",
                         {defaultValue[0]},
                         std::pair{{{minValue}, ConstraintBehavior::Ignore}},
                         std::pair{{{maxValue}, ConstraintBehavior::Ignore}}}};
}};
"""

intVec2Template = """
struct {structName} {{
    void set({className}& filter) {{
        filter.{command}(property.get(0), property.get(1));
    }}
    IntVec2Property property{{"{identifier}", "{displayName}",
                            ivec2{{{defaultValue[0]}, {defaultValue[1]}}},
                            std::pair{{ivec2{{{minValue}}}, ConstraintBehavior::Ignore}},
                            std::pair{{ivec2{{{maxValue}}}, ConstraintBehavior::Ignore}}}};
}};
"""

intVec3Template = """
struct {structName} {{
    void set({className}& filter) {{
        filter.{command}(property.get(0), property.get(1), property.get(2)));
    }}
    IntVec3Property property{{"{identifier}", "{displayName}",
                            ivec3{{{defaultValue[0]}, {defaultValue[1]}, {defaultValue[2]}}},
                            std::pair{{ivec3{{{minValue}}}, ConstraintBehavior::Ignore}},
                            std::pair{{ivec3{{{maxValue}}}, ConstraintBehavior::Ignore}}}};
}};
"""


boolTemplate = """
struct {structName} {{
    void set({className}& filter) {{
        filter.{command}(property.get());
    }}
    BoolProperty property{{"{identifier}", "{displayName}", {defaultValue}}};
}};
"""

stringTemplate = """
struct {structName} {{
    void set({className}& filter) {{
        filter.{command}(property.get().c_str());
    }}
    StringProperty property{{"{identifier}", "{displayName}", "{defaultValue}"}};
}};
"""

optionTemplate = """
struct {structName} {{
    void set({className}& filter) {{
        filter.{command}(property.get());
    }}
    OptionPropertyInt property{{"{identifier}", "{displayName}", {{{options}}}, {defaultValue}}};
}};
"""

headerTemplate = """#pragma once
namespace inviwo {{
class InviwoModule;
namespace ttkwrapper {{
void register{className}(InviwoModule* module);
}}  // namespace ttkwrapper
}}  // namespace inviwo
"""

sourceTemplate = """
#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/ttk/processors/ttkgenericprocessor.h>

#include <tuple>
#include <string_view>

#include <warn/push>
#include <warn/ignore/all>
#include "{className}.h"
#include <warn/pop>

namespace inviwo {{
namespace ttkwrapper {{
namespace {{

{propClasses}

}}
template <>
struct TTKTraits<{className}> {{
    static constexpr std::string_view identifier = "{identifier}";
    static constexpr std::string_view displayName = "{displayName}";
    std::tuple<{proplist}> properties;
}};

void register{className}(InviwoModule* module) {{
    module->registerProcessor<TTKGenericProcessor<{className}>>();
}}

}}  // namespace ttkwrapper
}}  // namespace inviwo
"""

mainHeader = """
#pragma once
namespace inviwo {
class InviwoModule;
namespace ttkwrapper {

void registerTTKFilters(InviwoModule* module);

}  // namespace ttkwrapper
}  // namespace inviwo
"""
mainSourceTemplate = """
{includes}

namespace inviwo {{
namespace ttkwrapper {{

void registerTTKFilters(InviwoModule* module) {{
{register}
}}

}}  // namespace ttkwrapper
}}  // namespace inviwo
"""


def generate(data: FilterData) -> str:
    propClasses = ""
    proplist = []

    for p in data.props:
        if p.command == 'SetInputArrayToProcess':
            continue

        kind = p.kind
        if isinstance(p.kind, DoubleVecProperty):
            if p.numElem == 1:
                template = doubleTemplate
            elif p.numElem == 2:
                template = doubleVec2Template
            elif p.numElem == 3:
                template = doubleVec3Template
            else:
                print(f"Missing template for DoubleVecProperty of dim {p.numElem}")
                continue
            kind.minValue = kind.minValue if kind.minValue else 0.0
            kind.maxValue = kind.maxValue if kind.maxValue else 100.0

        if isinstance(p.kind, IntVecProperty):
            if p.numElem == 1:
                template = intTemplate
            elif p.numElem == 2:
                template = intVec2Template
            elif p.numElem == 3:
                template = intVec3Template
            else:
                print(f"Missing template for IntVecProperty of dim {p.numElem}")
                continue
            kind.minValue = kind.minValue if kind.minValue else 0
            kind.maxValue = kind.maxValue if kind.maxValue else 100

        elif isinstance(p.kind, IntOptionProperty) and p.numElem == 1:
            template = optionTemplate
            kind.options = ', '.join(f'{{"{n}","{n}",{v}}}' for n, v in kind.options)

        elif isinstance(p.kind, StringProperty) and p.numElem == 1:
            template = stringTemplate

        elif isinstance(p.kind, BoolProperty) and p.numElem == 1:
            template = boolTemplate
            kind.defaultValue = "true" if kind.defaultValue else "false"

        else:
            continue  # WIP:::

        structName = f"Wrapper{len(proplist)}"
        propClasses += template.format(
            structName=structName,
            className=data.className,
            identifier=p.identifier,
            displayName=p.displayName,
            command=p.command,
            **dataclasses.asdict(kind)
        )
        proplist.append(structName)

    source = sourceTemplate.format(**dataclasses.asdict(data),
                                   proplist=', '.join(proplist),
                                   propClasses=propClasses)

    header = headerTemplate.format(**dataclasses.asdict(data))

    return header, source


def formatFile(file: Path):
    clangFormat = "C:/Program Files (x86)/Microsoft Visual Studio/2019/Enterprise/" \
                  "VC/Tools/Llvm/bin/clang-format.exe"
    subprocess.run([clangFormat, "-i", file])


if __name__ == '__main__':
    console = Console()
    parser = makeCmdParser()
    # args = parser.parse_args()
    basedir = Path("C:/Users/petst55.AD/Documents/Inviwo/ttk/paraview")
    denyList = [
        "Compatibility",  # Very odd one
        "Extract",        # Has double prop of dim 6...

        "ContourTreeAlignment",              # BinaryTree missing ContourTreeAlignment.h:40
        "CinemaDarkroom",                    # Seems strange
        "PersistenceDiagramClustering",      # KDTree.h:423 missing include buildWeights.h
        "PersistenceDiagramDistanceMatrix",  # KDTree.h:423 missing include buildWeights.h
        "ContourForests",                    # MergeTree.h compile problem
        "BottleneckDistance",                # Munkres.h:200 missing include MunkresImpl.h
        "TrackingFromFields",                # Munkres.h:200 missing include MunkresImpl.h
        "TrackingFromPersistenceDiagrams",   # Munkres.h:200 missing include MunkresImpl.h
        "FTRGraph"                           # Missing include
    ]
    files = (xml for item in basedir.glob('*') for xml in item.glob("*.xml")
             if xml.stem not in denyList)

    allowList = [
        "DistanceField", "HelloWorld", "MorseSmaleComplex", "PersistenceCurve"
    ]
    # files = (item for item in files if item.stem in allowList)

    filters = {}

    table = rich.table.Table()
    table.add_column("displayName")
    table.add_column("props")
    for file in files:
        try:
            f = parse(file)
            filters[file.stem] = f

            propTable = rich.table.Table()
            propTable.add_column("name")
            propTable.add_column("kind")
            propTable.add_column("dim")
            propTable.add_column("com")
            for prop in f.props:
                propTable.add_row(prop.displayName,
                                  str(prop.kind),
                                  str(prop.numElem),
                                  prop.command)

            table.add_row(f.displayName, propTable)
        except Exception as e:
            print(f"Error parsing {file} \n{e}")

    # console.print(table)
    # rich.inspect(filters["MorseSmaleComplex"])
    # header, source = generate(filters["MorseSmaleComplex"])
    # console.print(Syntax(header, lexer_name='c++', line_numbers=True))
    # console.print(Syntax(source, lexer_name='c++', line_numbers=True))

    gen = Path("C:/Users/petst55.AD/Documents/Inviwo/modules/topovis/ttk/generated")
    for file in gen.glob('*'):
        os.unlink(file)

    includes = []
    register = []
    for name, data in filters.items():
        header, source = generate(data)
        with open(gen / f"ivwwrap{name.lower()}.h", 'w') as f:
            f.write(header)
        with open(gen / f"ivwwrap{name.lower()}.cpp", 'w') as f:
            f.write(f'#include "ivwwrap{name.lower()}.h"\n{source}')

        includes.append(f'#include "ivwwrap{name.lower()}.h"')
        register.append(f"    register{data.className}(module);")

        formatFile(gen / f"ivwwrap{name.lower()}.h")
        formatFile(gen / f"ivwwrap{name.lower()}.cpp")

    with open(gen / "registerttkfilters.h", 'w') as f:
        f.write(mainHeader)

    with open(gen / "registerttkfilters.cpp", 'w') as f:
        f.write('#include "registerttkfilters.h"\n'
                + mainSourceTemplate.format(includes='\n'.join(includes),
                                            register='\n'.join(register)))

#: for i,f in enumerate(files):
#     r = ET.parse(f).getroot()
#     print(f"{i}, {len(r)} {r[0].tag} {r[1].tag if len(r)>1 else '-'} {f}")

# for key in sorted(keys):
#     attrs = set()
#     childs = set()
#     for i,f in enumerate(files):
#         r = ET.parse(f).getroot()[0][0]
#         # print(f"{i:2} {f.stem:30}: {len(r)} {' '.join(set(i.tag for i in r))}")
#         attrs.update([a for i in r.findall(f".//{key}") for a in i.attrib.keys()])
#         childs.update([c.tag for i in r.findall(f".//{key}") for c in i])
#     print(f"# {key:30}\n#    {' '.join(sorted(attrs))}\n#    {' '.join(sorted(childs))}")


# 1 ServerManagerConfiguration (2)
# 2 ProxyGroup (1) CinemaDarkroom and Compatibility has 2 ProxyGroup
# 3 ( SourceProxy | WriterProxy ) majority are SourceProxy
#
# 4
#   - Deprecated
#   - Documentation
#   - Hints
#   - OutputPort
#   - Property
#   - PropertyGroup
#   - InputProperty
#   - IntVectorProperty
#   - DoubleVectorProperty
#   - StringVectorProperty

# - Deprecated
#   * Attrs: deprecated_in to_remove_in
#   * Child:
# - Documentation
#   * Attrs: long_help short_help shorthelp
#   * Child: em
# - DoubleVectorProperty
#   * Attrs: command default_values label name number_of_elements panel_visibility
#   * Child: BoundsDomain Documentation DoubleRangeDomain Hints
# - Hints
#   * Attrs:
#   * Child: AcceptAnyFile ArraySelectionWidget Expression NoDefault Optional
#            Property PropertyWidgetDecorator ReaderFactory RepresentationType
#            ShowInMenu UseDirectoryName View Visibility Widget Write Factory
# - InputProperty
#   * Attrs: clean_command command label multiple_input name port_index
#   * Child: DataTypeDomain Documentation Hints InputArrayDomain ProxyGroupDomain
# - IntVectorProperty
#   * Attrs: animateable command default_values label name number_of_elements
#            panel_visibility
#   * Child: BooleanDomain Documentation EnumerationDomain Hints IntRangeDomain
# - OutputPort
#   * Attrs: default_values id index name
#   * Child:
# - Property
#   * Attrs: command function label name panel_widget show
#   * Child: Documentation
# - PropertyGroup
#   * Attrs: label panel_widget
#   * Child: Hints Property
# - StringVectorProperty
#   * Attrs: animatable animateable clean_command command default_values
#            element_types information_only label name number_of_elements
#            number_of_elements_per_command panel_visibility repeat_command si_lass
#   * Child: ArrayListDomain Documentation FileListDomain Hints StringListDomain
