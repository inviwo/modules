import argparse
import re
import os
import dataclasses
import traceback
import subprocess
import xml.etree.ElementTree as ET
from pathlib import Path
from functools import partial
from collections import defaultdict
from typing import Callable, Optional, TypeVar, Union, Tuple

import rich
from rich.console import Console
from rich import print

import requests


# Variables for generic types in type specifiers see chain for example
T = TypeVar('T')
R = TypeVar('R')

console = Console()


def chain(val: Optional[T], *funcs: Callable[[T], R]):
    '''
        Apply a chain of functions 'funcs' to a value 'val'
        i.e. fn(fn-1(...(f2(f1(val)))))
    '''
    if val is None:
        return val
    elif len(funcs) > 1:
        return chain(funcs[0](val), *funcs[1:])
    else:
        return funcs[0](val)


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


def valueOr(value, default):
    if value is None:
        return default
    else:
        return value


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
class ReadOnlyStringProperty:
    pass


@dataclasses.dataclass
class FileProperty:
    defaultValue: Optional[str]


@dataclasses.dataclass
class ButtonProperty:
    pass


@dataclasses.dataclass
class IntOptionProperty:
    options: list[Tuple[str, int]] = dataclasses.field(default_factory=list)
    defaultValue: Optional[int] = None


@dataclasses.dataclass
class RequiredProperty:
    function: str
    name: str


@dataclasses.dataclass
class ExtentProperty:
    defaultValue: Optional[list[int]]
    requiredProperties: list[RequiredProperty] = dataclasses.field(default_factory=list)


@dataclasses.dataclass
class FieldSelectionProperty:
    defaultValue: Optional[list[str]]
    inport: Optional[str]


@dataclasses.dataclass
class FilterPropertyData:
    identifier: str
    displayName: str
    command: str
    numElem: Optional[int] = None
    informationOnly: bool = False
    kind: Union[IntVecProperty, DoubleVecProperty, IntOptionProperty,
                StringProperty, FileProperty, ButtonProperty, BoolProperty,
                ExtentProperty, FieldSelectionProperty, None] = None
    doc: Optional[str] = None


@dataclasses.dataclass
class InputData:
    identifier: str
    dataType: Optional[str]
    numComp: Optional[int]


@dataclasses.dataclass
class OutputData:
    identifier: str
    displayName: str
    index: int


@dataclasses.dataclass
class FilterData:
    identifier: str
    displayName: str
    className: str
    desc: str
    doc: str = dataclasses.field(repr=False)
    props: list[FilterPropertyData] = dataclasses.field(default_factory=list)
    inports: list[InputData] = dataclasses.field(default_factory=list)
    outports: list[OutputData] = dataclasses.field(default_factory=list)
    groups: defaultdict[str, list[str]] \
        = dataclasses.field(default_factory=lambda: defaultdict(list))

# Parsers for property like stuff


def parseHelperProperty(xml: ET.Element) -> FilterPropertyData:
    '''
    Helper that parses information that is in most properties
    '''
    data = FilterPropertyData(
        identifier=xml.attrib["name"].replace(' ', ''),
        displayName=xml.attrib["label" if "label" in xml.attrib else "name"],
        command=xml.attrib["command"] if "command" in xml.attrib else "",
        numElem=(int(xml.attrib["number_of_elements"])
                 if "number_of_elements" in xml.attrib else None),
        informationOnly=((int(xml.attrib["information_only"]) > 0)
                         if "information_only" in xml.attrib else False)
    )
    if (doc := xml.find('Documentation')) is not None:
        data.doc = stripEachLine(doc.text)

    return data


def parseIntVectorProperty(xml: ET.Element) -> FilterPropertyData:
    data = parseHelperProperty(xml)

    if data.informationOnly:
        raise Exception("Unhandled IntProperty informationOnly not handled")

    elif xml.find('BooleanDomain') is not None:
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
    elif (extent := xml.find('ExtentDomain')) is not None:
        ep = ExtentProperty(
            defaultValue=chain(xml, xmlattr("default_values"),
                               str.split, partial(map, int), tuple)
        )
        if rp := xml.find("RequiredProperties") is not None:
            for p in rp:
                ep.requiredProperties.append(RequiredProperty(
                    function=xml.attrib["function"],
                    name=xml.attrib["name"]
                ))

        data.kind = ep

    elif data.numElem is not None and data.numElem >= 0 and data.numElem <= 4:
        data.kind = IntVecProperty(
            defaultValue=chain(xml, xmlattr("default_values"),
                               str.split, partial(map, int), tuple),
            minValue=chain(xml, xmlfind('IntRangeDomain'), xmlattr("min"), int),
            maxValue=chain(xml, xmlfind('IntRangeDomain'), xmlattr("max"), int)
        )
    else:
        raise Exception("Unhandled IntProperty")
    return data


def parseDoubleVectorProperty(xml: ET.Element) -> FilterPropertyData:
    data = parseHelperProperty(xml)

    if data.informationOnly:
        raise Exception("Unhandled DoubleProperty informationOnly not handled")

    if data.numElem is not None and data.numElem >= 0 and data.numElem <= 4:
        data.kind = DoubleVecProperty(
            defaultValue=chain(xml, xmlattr("default_values"),
                               str.split, partial(map, float), tuple),
            minValue=chain(xml, xmlfind('DoubleRangeDomain'), xmlattr("min"), float),
            maxValue=chain(xml, xmlfind('DoubleRangeDomain'), xmlattr("max"), float)
        )
    else:
        raise Exception("Unhandled DoubleProperty")
    return data


def parseStringVectorProperty(xml: ET.Element) -> FilterPropertyData:
    data = parseHelperProperty(xml)

    if data.informationOnly:
        raise Exception("Unhandled StringVectorProperty informationOnly not handled")

    elif data.command == "SetInputArrayToProcess":
        defaultValue = [0, 0, 0, 0, None]
        if "default_values" in xml.attrib and xml.attrib["default_values"] != "None":
            defaults = xml.attrib["default_values"].split()
            if len(defaults) > 0:
                defaultValue[0] = int(defaults[0])
            if len(defaults) > 1:
                defaultValue[1] = int(defaults[1])
            if len(defaults) > 2:
                defaultValue[2] = int(defaults[2])
            if len(defaults) > 3:
                defaultValue[3] = int(defaults[3])
            if len(defaults) > 4:
                defaultValue[4] = defaults[4]
        data.kind = FieldSelectionProperty(
            defaultValue=defaultValue,
            inport=chain(xml, xmlfind('ArrayListDomain/RequiredProperties/Property'),
                         xmlattr("name")),
        )
    elif xml.find('FileListDomain') is not None:
        data.kind = FileProperty(
            defaultValue=xml.attrib.get("default_values", None))
    else:
        data.kind = StringProperty(
            defaultValue=xml.attrib.get("default_values", None))
    return data


def parseProperty(xml: ET.Element) -> Optional[FilterPropertyData]:
    data = parseHelperProperty(xml)
    if "panel_widget" in xml.attrib and xml.attrib["panel_widget"] == "command_button":
        data.kind = ButtonProperty()
        return data
    else:
        raise Exception("Unhandled Property")


def parseInputProperty(xml: ET.Element) -> InputData:
    '''
    Parse Input properties, Will be mapped to inviwo Inports
    '''
    return InputData(
        identifier=xml.attrib["name"],
        dataType=chain(xml, xmlfind('DataTypeDomain/DataType'), xmlattr("value")),
        numComp=chain(xml, xmlfind('InputArrayDomain'), xmlattr("number_of_components"), int)
    )


def parseOutport(xml: ET.Element) -> OutputData:
    '''
    Parse Outports, Will be mapped to inviwo Outports
    '''
    return OutputData(
        identifier=xml.attrib["id"],
        displayName=xml.attrib["name"],
        index=int(xml.attrib["index"])
    )


def parse(xmlstr: str, file: Path) -> list[FilterData]:
    '''
    Parse a paraview xml string, returns list of Filterdata
    '''
    xml = ET.fromstring(xmlstr)

    if xml.tag != "ServerManagerConfiguration":
        raise Exception(f"Unexpected element, expected 'ServerManagerConfiguration' found '{xml.tag}'")

    filters = []
    for group in xml:
        if group.tag != "ProxyGroup":
            raise Exception(f"Unexpected element, expected 'ProxyGroup' found '{group.tag}'")
        filters.extend(parseProxyGroup(group, file))

    return filters


def parseProxyGroup(group: ET.Element, file: Path) -> list[FilterData]:
    filters = []
    for proxy in group:
        if proxy.tag not in ["SourceProxy", "WriterProxy", "Proxy"]:
            raise Exception(f"Unexpected element, expected a proxy found '{proxy.tag}'")
        filters.append(parseProxy(proxy, file))
    return filters


def parseProxy(proxy: ET.Element, file: Path) -> FilterData:

    doc = proxy.find('Documentation')
    if doc is None:
        docShort = ""
        docStr = ""
    else:
        docShort = re.sub(' +', ' ', doc.attrib['short_help']) if 'short_help' in doc.attrib else ""
        docStr = stripEachLine(doc.text)
    data = FilterData(
        identifier=proxy.attrib["name"],
        displayName=proxy.attrib["label"] if "label" in proxy.attrib else proxy.attrib["name"],
        className=proxy.attrib["class"],
        desc=docShort,
        doc=docStr
    )

    propertyParsers: dict[str, Callable[[ET.Element], FilterPropertyData]] = {
        "IntVectorProperty": parseIntVectorProperty,
        "StringVectorProperty": parseStringVectorProperty,
        "DoubleVectorProperty": parseDoubleVectorProperty,
        "Property": parseProperty
    }

    for elem in proxy:
        if (parser := propertyParsers.get(elem.tag, None)) is not None:
            try:
                data.props.append(parser(elem))
            except Exception:
                console.print(f"[bold red]Error parsing {data.identifier}")
                console.print_exception(extra_lines=1)
                console.print(ET.tostring(elem, encoding="unicode"))
                # sys.exit()
        elif elem.tag == "InputProperty":
            data.inports.append(parseInputProperty(elem))
        elif elem.tag == "OutputPort":
            data.outports.append(parseOutport(elem))
        elif elem.tag == "PropertyGroup":
            group = elem.attrib["label"]
            members = [i.attrib["name"] for i in elem if i.tag == "Property"]
            data.groups[group] += members

        elif elem.tag == "Documentation":
            # Should be mapped the the property help when that is merged in inviwo
            pass
        elif elem.tag == "Hints":
            # Not handled yet. Not the most important maybe.
            pass
        elif elem.tag == "Property" and elem.attrib["name"] == "Debug_Execute":
            # Just for debug to rerun the fiter, in Inviwo we can just right click and invalidate
            pass
        else:
            console.print(f"[bold red]No parser for elem: {elem.tag} in {file}")
            console.print(ET.tostring(elem, encoding="unicode"))

    return data


doubleTemplate = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get());
        return true;
    }}
    DoubleProperty property{{"{identifier}", "{displayName}",
                             {defaultValue[0]},
                             std::pair{{{minValue}, ConstraintBehavior::Ignore}},
                             std::pair{{{maxValue}, ConstraintBehavior::Ignore}}}};
}};
"""

doubleVec2Template = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get(0), property.get(1));
        return true;
    }}
    DoubleVec2Property property{{"{identifier}", "{displayName}",
                                dvec2{{{defaultValue[0]}, {defaultValue[1]}}},
                                std::pair{{dvec2{{{minValue}}}, ConstraintBehavior::Ignore}},
                                std::pair{{dvec2{{{maxValue}}}, ConstraintBehavior::Ignore}}}};
}};
"""

doubleVec3Template = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get(0), property.get(1), property.get(2));
        return true;
    }}
    DoubleVec3Property property{{"{identifier}", "{displayName}",
                                dvec3{{{defaultValue[0]}, {defaultValue[1]}, {defaultValue[2]}}},
                                std::pair{{dvec3{{{minValue}}}, ConstraintBehavior::Ignore}},
                                std::pair{{dvec3{{{maxValue}}}, ConstraintBehavior::Ignore}}}};
}};
"""

intTemplate = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get());
        return true;
    }}
    IntProperty property{{"{identifier}", "{displayName}",
                         {defaultValue[0]},
                         std::pair{{{minValue}, ConstraintBehavior::Ignore}},
                         std::pair{{{maxValue}, ConstraintBehavior::Ignore}}}};
}};
"""

intVec2Template = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get(0), property.get(1));
        return true;
    }}
    IntVec2Property property{{"{identifier}", "{displayName}",
                            ivec2{{{defaultValue[0]}, {defaultValue[1]}}},
                            std::pair{{ivec2{{{minValue}}}, ConstraintBehavior::Ignore}},
                            std::pair{{ivec2{{{maxValue}}}, ConstraintBehavior::Ignore}}}};
}};
"""

intVec3Template = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get(0), property.get(1), property.get(2)));
        return true;
    }}
    IntVec3Property property{{"{identifier}", "{displayName}",
                            ivec3{{{defaultValue[0]}, {defaultValue[1]}, {defaultValue[2]}}},
                            std::pair{{ivec3{{{minValue}}}, ConstraintBehavior::Ignore}},
                            std::pair{{ivec3{{{maxValue}}}, ConstraintBehavior::Ignore}}}};
}};
"""


boolTemplate = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get());
        return true;
    }}
    BoolProperty property{{"{identifier}", "{displayName}", {defaultValue}}};
}};
"""

stringTemplate = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get().c_str());
        return true;
    }}
    StringProperty property{{"{identifier}", "{displayName}", "{defaultValue}"}};
}};
"""

fileTemplate = """
struct {structName} {{
    bool set({className}& filter) {{
        if(property.get().empty()) return false;
        filter.{command}(property.get().c_str());
        return true;
    }}
    FileProperty property{{"{identifier}", "{displayName}", "{defaultValue}"}};
}};
"""

buttonTemplate = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}();
        return true;
    }}
    ButtonProperty property{{"{identifier}", "{displayName}"}};
}};
"""

extentTemplate = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(x.getStart(), x.getEnd(), y.getStart(), y.getEnd(),
            z.getStart(), z.getEnd());
        return true;
    }

    IntSizeTMinMaxProperty x{{"xExtent", "Y Extent", 0, 100, 0, 1000}};
    IntSizeTMinMaxProperty y{{"yExtent", "Y Extent", 0, 100, 0, 1000}};
    IntSizeTMinMaxProperty z{{"zExtent", "X Extent", 0, 100, 0, 1000}};

    CompositeProperty property{{[&](){{
            CompositeProperty tmp{{"{identifier}", "{displayName}"}};
            tmp.add_properties(x,y,z);
            return tmp;
        }}()
    }};
}};
"""

optionTemplate = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get());
        return true;
    }}
    OptionPropertyInt property{{"{identifier}", "{displayName}", {{{options}}}, {defaultValue}}};
}};
"""

fieldSelectionTemplate = """
struct {structName} : FieldSelection {{
    bool set({className}& filter) {{
        if(property.size() == 0) return false;
        filter.{command}({defaultValue[0]}, {defaultValue[1]},
                         {defaultValue[2]}, {defaultValue[3]}, property.get().c_str());
        return true;
    }}
    OptionPropertyString property{{"{identifier}", "{displayName}", {{{defaultoptions}}}, 0}};

    static constexpr std::string_view inport = "{inport}";
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
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/fileproperty.h>

#include <tuple>
#include <array>
#include <string_view>

#include <warn/push>
#include <warn/ignore/all>
#include "{className}.h"
#include <warn/pop>

namespace inviwo {{
namespace ttkwrapper {{
namespace {{

#include <warn/push>
#include <warn/ignore/conversion>

{propClasses}

#include <warn/pop>

}}
template <>
struct TTKTraits<{className}> {{
    static constexpr std::string_view identifier = "{identifier}";
    static constexpr std::string_view displayName = "{displayName}";
    inline static std::array<InputData, {nInput}> inports = {{{inputData}}};
    inline static std::array<OutputData, {nOutput}> outports = {{{outputData}}};
    inline static std::array<Group, {nGroup}> groups = {{{groupList}}};
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
    extra = {}

    for p in data.props:
        kind = p.kind
        if isinstance(kind, DoubleVecProperty):
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

        elif isinstance(kind, IntVecProperty):
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

        elif isinstance(kind, IntOptionProperty) and p.numElem == 1:
            template = optionTemplate
            kind.options = ', '.join(f'{{"{n}","{n}",{v}}}' for n, v in kind.options)

        elif isinstance(kind, StringProperty) and p.numElem == 1:
            template = stringTemplate
            kind.defaultValue = valueOr(kind.defaultValue, "")

        elif isinstance(kind, FileProperty) and p.numElem == 1:
            template = fileTemplate
            kind.defaultValue = valueOr(kind.defaultValue, "")

        elif isinstance(kind, ButtonProperty) and p.numElem == 1:
            template = buttonTemplate

        elif isinstance(kind, ButtonProperty) and p.numElem == 1:
            template = extentTemplate

        elif isinstance(kind, BoolProperty) and p.numElem == 1:
            template = boolTemplate
            kind.defaultValue = "true" if kind.defaultValue else "false"

        elif isinstance(kind, FieldSelectionProperty):
            template = fieldSelectionTemplate
            if len(p.kind.defaultValue) > 4 and p.kind.defaultValue[4] is not None:
                extra["defaultoptions"] = '{{"{0}","{0}","{0}"}}'.format(p.kind.defaultValue[4])
            else:
                extra["defaultoptions"] = ""
        else:
            print(f"Missing template for {type(kind)} in property {p.identifier}"
                  f" in filter {data.identifier}")
            continue  # WIP:::

        structName = f"Wrapper{len(proplist)}"
        propClasses += template.format(
            structName=structName,
            className=data.className,
            identifier=p.identifier,
            displayName=p.displayName,
            command=p.command,
            **dataclasses.asdict(kind),
            **extra
        )
        proplist.append(structName)

    inputData = ", ".join(
        f'InputData{{"{d.identifier}", "{valueOr(d.dataType, "")}", {valueOr(d.numComp,-1)}}}'
        for d in data.inports)
    outputData = ", ".join(f'OutputData{{"{d.identifier}", "{d.displayName}", {d.index}}}'
                           for d in data.outports)

    groups = []
    for name, members in data.groups.items():
        m = ", ".join(f'"{x}"' for x in members)
        groups.append(f'Group{{"{name}", {{{m}}}}}')

    source = sourceTemplate.format(identifier=data.identifier,
                                   displayName=data.displayName,
                                   className=data.className,
                                   proplist=', '.join(proplist),
                                   propClasses=propClasses,
                                   nInput=len(data.inports),
                                   nOutput=len(data.outports),
                                   nGroup=len(data.groups),
                                   inputData=inputData,
                                   outputData=outputData,
                                   groupList=", ".join(groups)
                                   )

    header = headerTemplate.format(className=data.className)

    return header, source


def formatFile(file: Path):
    clangFormat = "C:/Program Files/Microsoft Visual Studio/2022/Enterprise/" \
                  "VC/Tools/Llvm/bin/clang-format.exe"
    subprocess.run([clangFormat, "-i", file])


def makeTable(items) -> rich.table.Table:
    table = rich.table.Table()
    if len(items) == 0:
        return table
    for key in dataclasses.asdict(items[0]).keys():
        table.add_column(key)
    for item in items:
        table.add_row(*map(str, dataclasses.astuple(item)))
    return table


def makeFilterTable(filters: list[FilterData]):
    table = rich.table.Table()
    table.add_column("displayName")
    table.add_column("props")
    table.add_column("inport/outport")
    table.add_column("groups")
    for data in filters:
        table.add_row(
            data.displayName,
            makeTable(data.props),
            # rich.console.RenderGroup(
            rich.console.Group(
                makeTable(data.inports),
                makeTable(data.outports)
            )
        )
    return table


def makeCmdParser():
    parser = argparse.ArgumentParser(
        description="Parse paraview xml",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    parser.add_argument('--ttkrepo', type=Path,
                        help='Path to the ttk repo, needed for paraview xml files', required=True)
    parser.add_argument('-o', '--output', type=Path,
                        help='Path to output directory for generated files', required=True)
    parser.add_argument('-c', '--clear', action='store_true',
                        help='Clear any old file in the output directory')
    return parser


if __name__ == '__main__':
    parser = makeCmdParser()
    args = parser.parse_args()

    basedir = args.ttkrepo / "paraview" / "xmls"
    debugWidgetsFile = args.ttkrepo / "CMake" / "debug_widgets.xml"

    with open(debugWidgetsFile, 'r') as f:
        debugWidgets = f.read()

    denyList = [
        "Algorithm"       # Empty?
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
        "FTRGraph",                          # Missing include
        "ArrayPreconditioning",              # Could not find SetBurstSize (member/function)
        "GhostCellPreconditioning",          # Could not find import ttkGhostCellPreconditioning.h
        "MergeTreeDistanceMatrix",     # Could not find SetUseFieldDataParameters (member/funcion)
        "PersistentGenerators",        # Could not find import ttkPersistentGenerators.h
        "ProjectionFromTable",         # Could not find import ttkProjectionFromTable.h
        "MergeTreeClustering"       # Could not find SetBarycenterSizeLimitPercent (member/function)
    ]

    files = (xml for xml in basedir.glob("*.xml") if xml.stem not in denyList)
    filters = []
    for file in files:
        try:
            with open(file, 'r') as f:
                xmlstr = f.read().replace("${DEBUG_WIDGETS}", debugWidgets)
            filters.extend(parse(xmlstr, file))
        except Exception as e:
            print(f"Error parsing {file} \n{e}")
            print(traceback.format_exc())

    paraviewXmls = [
        "https://github.com/Kitware/ParaView/raw/master/Remoting/Application/Resources/readers_ioxml.xml"
    ]
    for url in paraviewXmls:
        try:
            response = requests.get(url)
            xmlstr = response.content
            filters.extend(parse(xmlstr, url))
        except Exception as e:
            print(f"Error parsing {url} \n{e}")
            print(traceback.format_exc())

    # myfile = requests.get(url)

    # To parse specific VTK filters
    dir = Path("C:/Users/sigsi52/Development/Inviwo/local/modules-worktree/topovis/ttk/data/vtk_filters")
    vtk_files = (xml for xml in dir.glob("*.xml"))

    for file in vtk_files:
        try:
            with open(file, 'r') as f:
                xmlstr = f.read()
            filters.extend(parse(xmlstr, file))
        except Exception as e:
            print(f"Error parsing {file} \n{e}")
            print(traceback.format_exc())

    console.print(makeFilterTable(filters))

    # some debug logging...
    # rich.inspect(filters["MorseSmaleComplex"])
    # header, source = generate(filters["MorseSmaleComplex"])
    # console.print(Syntax(header, lexer_name='c++', line_numbers=True))
    # console.print(Syntax(source, lexer_name='c++', line_numbers=True))

    # remove all old files
    gen = Path(args.output)
    if args.clear:
        for file in gen.glob('ivw_*.h'):
            os.unlink(file)
        for file in gen.glob('ivw_*.cpp'):
            os.unlink(file)

    includes = []
    register = []
    for data in filters:
        name = data.className
        header, source = generate(data)
        with open(gen / f"ivw_{name.lower()}.h", 'w') as f:
            f.write(header)
        with open(gen / f"ivw_{name.lower()}.cpp", 'w') as f:
            f.write(f'#include "ivw_{name.lower()}.h"\n{source}')

        includes.append(f'#include "ivw_{name.lower()}.h"')
        register.append(f"    register{data.className}(module);")

        formatFile(gen / f"ivw_{name.lower()}.h")
        formatFile(gen / f"ivw_{name.lower()}.cpp")

    with open(gen / "registerttkfilters.h", 'w') as f:
        f.write(mainHeader)

    with open(gen / "registerttkfilters.cpp", 'w') as f:
        f.write('#include "registerttkfilters.h"\n'
                + mainSourceTemplate.format(includes='\n'.join(includes),
                                            register='\n'.join(register)))


# Some notes about the structure of the files.
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
