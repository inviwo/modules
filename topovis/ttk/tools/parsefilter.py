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

def splitComma(text: str):
    return text.split(',')

def elide(text: str, chars: int):
    if len(text) == 0: 
        return text
    chars = 4 if chars <= 3 else chars
    line = text.splitlines()[0]
    return line if len(line) <= chars - 3 else f"{line[:chars-3]}..."

@dataclasses.dataclass
class BoolProperty:
    defaultValue: Optional[int]


@dataclasses.dataclass
class IntVecProperty:
    defaultValue: Optional[list[int]]
    minValue: Optional[list[int]]
    maxValue: Optional[list[int]]


@dataclasses.dataclass
class DoubleVecProperty:
    defaultValue: Optional[list[float]]
    minValue: Optional[list[float]]
    maxValue: Optional[list[float]]


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
    numComp: Optional[list[int]]
    doc: Optional[str] = None


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
    category: str
    tags: str
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
        raise Exception("Unhandled IntProperty information_only not handled")

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
    elif (xml.find('ExtentDomain')) is not None:
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
            minValue=chain(xml, xmlfind('IntRangeDomain'), xmlattr("min"),
                           str.split, partial(map, int), tuple),
            maxValue=chain(xml, xmlfind('IntRangeDomain'), xmlattr("max"),
                           str.split, partial(map, int), tuple)
        )
    else:
        raise Exception("Unhandled IntProperty")
    return data


def parseDoubleVectorProperty(xml: ET.Element) -> FilterPropertyData:
    data = parseHelperProperty(xml)

    if data.informationOnly:
        raise Exception("Unhandled DoubleProperty information_only not handled")

    if data.numElem is not None and data.numElem >= 0 and data.numElem <= 4:
        data.kind = DoubleVecProperty(
            defaultValue=chain(xml, xmlattr("default_values"),
                               str.split, partial(map, float), tuple),
            minValue=chain(xml, xmlfind('DoubleRangeDomain'), xmlattr("min"),
                           str.split, partial(map, float), tuple),
            maxValue=chain(xml, xmlfind('DoubleRangeDomain'), xmlattr("max"),
                           str.split, partial(map, float), tuple)
        )
    else:
        raise Exception("Unhandled DoubleProperty")
    return data


def parseStringVectorProperty(xml: ET.Element) -> FilterPropertyData:
    data = parseHelperProperty(xml)

    if data.informationOnly:
        raise Exception("Unhandled StringVectorProperty information_only not handled")

    elif data.command == "SetInputArrayToProcess":
        defaultValue = [0, 0, 0, 3, None]
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
    
    data = InputData(
        identifier=xml.attrib["name"],
        dataType=chain(xml, xmlfind('DataTypeDomain/DataType'), xmlattr("value")),
        numComp=chain(xml, xmlfind('InputArrayDomain'), 
                      xmlattr("number_of_components"), 
                      splitComma, partial(map, int), tuple)
    )
    
    if (doc := xml.find('Documentation')) is not None:
        data.doc = stripEachLine(doc.text)
    
    return data


def parseOutport(xml: ET.Element) -> OutputData:
    '''
    Parse Outports, Will be mapped to inviwo Outports
    '''
    
    index = int(xml.attrib["index"])
    identifier = xml.attrib["id"] if "id" in xml.attrib else f"outport{index}"
    
    return OutputData(
        identifier=identifier,
        displayName=xml.attrib["name"],
        index=index
    )


def parse(xmlstr: str, file: Path, category: str, tags: str) -> list[FilterData]:
    '''
    Parse a paraview xml string, returns list of Filterdata
    '''
    xml = ET.fromstring(xmlstr)

    if xml.tag != "ServerManagerConfiguration":
        raise Exception(
            f"Unexpected element, expected 'ServerManagerConfiguration' found '{xml.tag}'")

    filters = []
    for group in xml:
        if group.tag != "ProxyGroup":
            raise Exception(f"Unexpected element, expected 'ProxyGroup' found '{group.tag}'")
        filters.extend(parseProxyGroup(group, file, category, tags))

    return filters


def parseProxyGroup(group: ET.Element, file: Path, category: str, tags: str) -> list[FilterData]:
    filters = []
    for proxy in group:
        try:
            if proxy.tag in ["CompoundSourceProxy"]:
                # Maybe handle in the future?
                continue
        
            if proxy.tag not in ["SourceProxy", "WriterProxy", "Proxy"]:
                raise Exception(f"Unexpected element, expected a proxy found '{proxy.tag}'")
                
            filters.append(parseProxy(proxy, file, category, tags))
        
        except Exception as e:
            print(f"Error parsing {proxy.tag} in {file} \n{e}")
            #print(traceback.format_exc())
            #print(ET.tostring(proxy, encoding='unicode'))
            
    return filters


def parseProxy(proxy: ET.Element, file: Path, category: str, tags: str) -> FilterData:

    doc = proxy.find('Documentation')
    if doc is None:
        docShort = ""
        docStr = ""
    else:
        docShort = re.sub(' +', ' ', doc.attrib['short_help']) if 'short_help' in doc.attrib else ""
        if isinstance(doc.text, str):
            docStr = stripEachLine(doc.text)
        elif 'long_help' in doc.attrib:
            docStr = doc.attrib['long_help']
        else:
            docStr = docShort
            
    data = FilterData(
        identifier=proxy.attrib["name"],
        displayName=proxy.attrib["label"] if "label" in proxy.attrib else proxy.attrib["name"],
        className=proxy.attrib["class"],
        category=category,
        tags=tags,
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
        elif elem.tag == "PropertyGroup" and "label" in elem.attrib:
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
    DoubleProperty property{{"{identifier}", "{displayName}", R"({doc})"_help,
                             {defaultValue[0]},
                             std::pair{{{minValue[0]}, ConstraintBehavior::Ignore}},
                             std::pair{{{maxValue[0]}, ConstraintBehavior::Ignore}}}};
}};
"""

doubleVec2Template = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get(0), property.get(1));
        return true;
    }}
    DoubleVec2Property property{{"{identifier}", "{displayName}", R"({doc})"_help,
                                dvec2{{{defaultValue[0]}, {defaultValue[1]}}},
                                std::pair{{dvec2{{{minValue[0]},{minValue[1]}}}, ConstraintBehavior::Ignore}},
                                std::pair{{dvec2{{{maxValue[0]},{maxValue[1]}}}, ConstraintBehavior::Ignore}}}};
}};
"""

doubleVec3Template = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get(0), property.get(1), property.get(2));
        return true;
    }}
    DoubleVec3Property property{{"{identifier}", "{displayName}", R"({doc})"_help,
                                dvec3{{{defaultValue[0]}, {defaultValue[1]}, {defaultValue[2]}}},
                                std::pair{{dvec3{{{minValue[0]},{minValue[1]},{minValue[2]}}}, ConstraintBehavior::Ignore}},
                                std::pair{{dvec3{{{maxValue[0]},{maxValue[1]},{maxValue[2]}}}, ConstraintBehavior::Ignore}}}};
}};
"""

doubleVec4Template = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get(0), property.get(1), property.get(2), property.get(3));
        return true;
    }}
    DoubleVec3Property property{{"{identifier}", "{displayName}", R"({doc})"_help,
                                dvec4{{{defaultValue[0]}, {defaultValue[1]}, {defaultValue[2]}, {defaultValue[3]}}},
                                std::pair{{dvec4{{{minValue[0]},{minValue[1]},{minValue[2]},{minValue[3]}}}, ConstraintBehavior::Ignore}},
                                std::pair{{dvec4{{{maxValue[0]},{maxValue[1]},{maxValue[2]},{maxValue[3]}}}, ConstraintBehavior::Ignore}}}};
}};
"""

doubleVec6Template = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(x.get(0), x.get(1), y.get(0), y.get(1), z.get(0), z.get(1));
        return true;
    }}
    DoubleVec2Property x{{"x", "X",
        dvec2{{{defaultValue[0]}, {defaultValue[1]}}},
        std::pair{{dvec2{{{minValue[0]},{minValue[1]}}}, ConstraintBehavior::Ignore}},
        std::pair{{dvec2{{{maxValue[0]},{maxValue[1]}}}, ConstraintBehavior::Ignore}}}};

    DoubleVec2Property y{{"y", "Y",
        dvec2{{{defaultValue[2]}, {defaultValue[3]}}},
        std::pair{{dvec2{{{minValue[2]},{minValue[3]}}}, ConstraintBehavior::Ignore}},
        std::pair{{dvec2{{{maxValue[2]},{maxValue[3]}}}, ConstraintBehavior::Ignore}}}};

    DoubleVec2Property z{{"z", "Z",
        dvec2{{{defaultValue[4]}, {defaultValue[5]}}},
        std::pair{{dvec2{{{minValue[4]},{minValue[5]}}}, ConstraintBehavior::Ignore}},
        std::pair{{dvec2{{{maxValue[4]},{maxValue[5]}}}, ConstraintBehavior::Ignore}}}};

    CompositeProperty property{{[&](){{
        CompositeProperty tmp{{"{identifier}", "{displayName}", R"({doc})"_help}};
        tmp.addProperties(x,y,z);
        return tmp;
        }}()
    }};

}};
"""


intTemplate = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get());
        return true;
    }}
    IntProperty property{{"{identifier}", "{displayName}", R"({doc})"_help,
                         {defaultValue[0]},
                         std::pair{{{minValue[0]}, ConstraintBehavior::Ignore}},
                         std::pair{{{maxValue[0]}, ConstraintBehavior::Ignore}}}};
}};
"""

intVec2Template = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get(0), property.get(1));
        return true;
    }}
    IntVec2Property property{{"{identifier}", "{displayName}", R"({doc})"_help,
                            ivec2{{{defaultValue[0]}, {defaultValue[1]}}},
                            std::pair{{ivec2{{{minValue[0]},{minValue[1]}}}, ConstraintBehavior::Ignore}},
                            std::pair{{ivec2{{{maxValue[0]},{maxValue[1]}}}, ConstraintBehavior::Ignore}}}};
}};
"""

intVec3Template = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get(0), property.get(1), property.get(2));
        return true;
    }}
    IntVec3Property property{{"{identifier}", "{displayName}", R"({doc})"_help,
                            ivec3{{{defaultValue[0]}, {defaultValue[1]}, {defaultValue[2]}}},
                            std::pair{{ivec3{{{minValue[0]},{minValue[1]},{minValue[2]}}}, ConstraintBehavior::Ignore}},
                            std::pair{{ivec3{{{maxValue[0]},{maxValue[1]},{maxValue[2]}}}, ConstraintBehavior::Ignore}}}};
}};
"""

intVec4Template = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get(0), property.get(1), property.get(2), property.get(3));
        return true;
    }}
    IntVec4Property property{{"{identifier}", "{displayName}", R"({doc})"_help,
                            ivec4{{{defaultValue[0]}, {defaultValue[1]}, {defaultValue[2]},{defaultValue[3]}}},
                            std::pair{{ivec4{{{minValue[0]},{minValue[1]},{minValue[2]},{minValue[3]}}}, ConstraintBehavior::Ignore}},
                            std::pair{{ivec4{{{maxValue[0]},{maxValue[1]},{maxValue[2]},{maxValue[3]}}}, ConstraintBehavior::Ignore}}}};
}};
"""

intVec6Template = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(x.get(0), x.get(1), y.get(0), y.get(1), z.get(0), z.get(1));
        return true;
    }}
    IntVec2Property x{{"x", "X",
        dvec2{{{defaultValue[0]}, {defaultValue[1]}}},
        std::pair{{dvec2{{{minValue[0]},{minValue[1]}}}, ConstraintBehavior::Ignore}},
        std::pair{{dvec2{{{maxValue[0]},{maxValue[1]}}}, ConstraintBehavior::Ignore}}}};

    IntVec2Property y{{"y", "Y",
        dvec2{{{defaultValue[2]}, {defaultValue[3]}}},
        std::pair{{dvec2{{{minValue[2]},{minValue[3]}}}, ConstraintBehavior::Ignore}},
        std::pair{{dvec2{{{maxValue[2]},{maxValue[3]}}}, ConstraintBehavior::Ignore}}}};

    IntVec2Property z{{"z", "Z",
        dvec2{{{defaultValue[4]}, {defaultValue[5]}}},
        std::pair{{dvec2{{{minValue[4]},{minValue[5]}}}, ConstraintBehavior::Ignore}},
        std::pair{{dvec2{{{maxValue[4]},{maxValue[5]}}}, ConstraintBehavior::Ignore}}}};

    CompositeProperty property{{[&](){{
        CompositeProperty tmp{{"{identifier}", "{displayName}", R"({doc})"_help}};
        tmp.addProperties(x,y,z);
        return tmp;
        }}()
    }};

}};
"""


boolTemplate = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get());
        return true;
    }}
    BoolProperty property{{"{identifier}", "{displayName}", R"({doc})"_help, {defaultValue}}};
}};
"""

stringTemplate = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get().c_str());
        return true;
    }}
    StringProperty property{{"{identifier}", "{displayName}", R"({doc})"_help, "{defaultValue}"}};
}};
"""

fileTemplate = """
struct {structName} {{
    bool set({className}& filter) {{
        if(property.get().empty()) return false;
        filter.{command}(property.get().string().c_str());
        return true;
    }}
    FileProperty property{{"{identifier}", "{displayName}", R"({doc})"_help, 
                           std::filesystem::path{{"{defaultValue}"}}}};
}};
"""

buttonTemplate = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}();
        return true;
    }}
    ButtonProperty property{{"{identifier}", "{displayName}", R"({doc})"_help}};
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
            CompositeProperty tmp{{"{identifier}", "{displayName}", R"({doc})"_help}};
            tmp.addProperties(x,y,z);
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
    OptionPropertyInt property{{"{identifier}", "{displayName}", R"({doc})"_help, {{{options}}}, {defaultValue}}};
}};
"""

fieldSelectionTemplate = """
struct {structName} : FieldSelection {{
    bool set({className}& filter) {{
        if(name.size() == 0) return false;
        filter.{command}({defaultValue[0]}, {defaultValue[1]},
                         {defaultValue[2]}, fieldAssociation.get(), name.get().c_str());
        return true;
    }}
    OptionPropertyString name{{"name", "Name", {{{defaultoptions}}}, 0}};

    OptionProperty<vtkDataObject::FieldAssociations> fieldAssociation{{"fieldAssociation", "Field Association",
        {{
            {{"points","Points",vtkDataObject::FIELD_ASSOCIATION_POINTS}},
            {{"cells","Cells",vtkDataObject::FIELD_ASSOCIATION_CELLS}},
            {{"none","None",vtkDataObject::FIELD_ASSOCIATION_NONE}},
            {{"pointsThenCells","Points then Cells",vtkDataObject::FIELD_ASSOCIATION_POINTS_THEN_CELLS}}
        }}, {defaultValue[3]} }};

    CompositeProperty property{{[&](){{
            CompositeProperty tmp{{"{identifier}", "{displayName}", R"({doc})"_help}};
            tmp.addProperties(name, fieldAssociation);
            return tmp;
        }}()
    }};
    
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
#include <inviwo/core/properties/buttonproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/fileproperty.h>

#include <tuple>
#include <array>
#include <string_view>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkDataObject.h>
#include <{className}.h>
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
    static constexpr std::string_view className = "{className}";
    static constexpr std::string_view identifier = "{identifier}";
    static constexpr std::string_view displayName = "{displayName}";
    static constexpr std::string_view category = "{category}";
    static constexpr std::string_view tags = "{tags}";
    inline static std::array<InputData, {nInput}> inports = {{{inputData}}};
    inline static std::array<OutputData, {nOutput}> outports = {{{outputData}}};
    inline static std::array<Group, {nGroup}> groups = {{{groupList}}};
    std::tuple<{proplist}> properties;
    static constexpr std::string_view doc = R"({doc})";
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
            elif p.numElem == 4:
                template = doubleVec4Template
            elif p.numElem == 6:
                template = doubleVec6Template             
            else:
                print(f"Missing template for DoubleVecProperty of dim {p.numElem}")
                continue

            if not kind.minValue:
                kind.minValue = [0.0]*p.numElem
            
            if kind.minValue:
                if len(kind.minValue) == 1:
                    kind.minValue = kind.minValue*p.numElem
                    
            if not kind.maxValue:
                kind.maxValue = [100.0]*p.numElem
                    
            if kind.maxValue:
                if len(kind.maxValue) == 1:
                    kind.maxValue = kind.maxValue*p.numElem    


        elif isinstance(kind, IntVecProperty):
            if p.numElem == 1:
                template = intTemplate
            elif p.numElem == 2:
                template = intVec2Template
            elif p.numElem == 3:
                template = intVec3Template
            elif p.numElem == 4:
                template = intVec4Template
            elif p.numElem == 6:
                template = intVec6Template
            else:
                print(f"Missing template for IntVecProperty of dim {p.numElem}")
                continue
            
            if not kind.minValue:
                kind.minValue = [0]*p.numElem
            
            if kind.minValue:
                if len(kind.minValue) == 1:
                    kind.minValue = kind.minValue*p.numElem
                    
            if not kind.maxValue:
                kind.maxValue = [100]*p.numElem
                    
            if kind.maxValue:
                if len(kind.maxValue) == 1:
                    kind.maxValue = kind.maxValue*p.numElem        


        elif isinstance(kind, IntOptionProperty) and p.numElem == 1:
            template = optionTemplate
            kind.defaultValue = valueOr(kind.defaultValue, 0)
            kind.options = ', '.join(f'{{"{n}","{n}",{v}}}' for n, v in kind.options)

        elif isinstance(kind, StringProperty) and p.numElem == 1:
            template = stringTemplate
            kind.defaultValue = valueOr(kind.defaultValue, "")

        elif isinstance(kind, FileProperty) and p.numElem == 1:
            template = fileTemplate
            kind.defaultValue = valueOr(kind.defaultValue, "")

        elif isinstance(kind, ButtonProperty) and (not p.numElem or p.numElem == 1):
            template = buttonTemplate

        elif isinstance(kind, ExtentProperty) and p.numElem == 1:
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
            doc=p.doc if p.doc else "",
            **dataclasses.asdict(kind),
            **extra
        )
        proplist.append(structName)

    inputData = ", ".join(
        f'InputData{{"{d.identifier}", "{valueOr(d.dataType, "")}", {valueOr(d.numComp,[-1])[0]}, R"({valueOr(d.doc, "")})"}}'
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
                                   category=data.category,
                                   tags=data.tags,
                                   doc=data.doc,
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
        table.add_row(*map(lambda x : elide(str(x), 20), dataclasses.astuple(item)))
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
   # files = []
    filters = []
    for file in files:
        try:
            with open(file, 'r') as f:
                xmlstr = f.read().replace("${DEBUG_WIDGETS}", debugWidgets)
            filters.extend(parse(xmlstr, file, "topology", "TTK")) 
        except Exception as e:
            print(f"Error parsing {file} \n{e}")
            print(traceback.format_exc())

    paraviewXmls = [
        "https://github.com/Kitware/ParaView/raw/master/"
        "Remoting/Application/Resources/readers_ioxml.xml"
    ]
    for url in paraviewXmls:
        try:
            response = requests.get(url)
            xmlstr = response.content
            filters.extend(parse(xmlstr, url, "vtk", "VTK,readers"))
        except Exception as e:
            print(f"Error parsing {url} \n{e}")
            print(traceback.format_exc())

    # myfile = requests.get(url)

    # To parse specific VTK filters
    dir = Path("filters/")
    vtk_files = (xml for xml in dir.glob("*.xml"))

    for file in vtk_files:
        try:
            with open(file, 'r') as f:
                xmlstr = f.read()
            filters.extend(parse(xmlstr, file, "vtk", "VTK"))
        except Exception as e:
            print(f"Error parsing {file} \n{e}")
            print(traceback.format_exc())

    # remove all filters with class name vtkFileSeriesReaders
    #
    # TODO: needs parsing of the nested SubProxy, then using the proxyname as class name
    #
    # <SourceProxy class="vtkFileSeriesReader"
    #              file_name_method="SetFileName"
    #              label="XML PolyData Reader"
    #              name="XMLPolyDataReader"
    #              si_class="vtkSIMetaReaderProxy">
    #   <Documentation>...</Documentation>
    #   <SubProxy>
    #     <Proxy name="Reader"
    #            proxygroup="internal_sources"
    #            proxyname="XMLPolyDataReaderCore"></Proxy>
    filterCount = len(filters)
    filters = [f for f in filters if f.className != "vtkFileSeriesReader"]
    print(f"Removed {filterCount - len(filters)} vtkFileSeriesReaders.")


    filters = [f for f in filters if not f.className.startswith("vtkPV")]
    filters = [f for f in filters if not f.className.startswith("vtkAMR")]

    missingHeader = [
        "vtkAttributeDataToTableFilter",
        "vtkAdditionalFieldReader",
        "vtkCleanUnstructuredGrid",
        "vtkCleanUnstructuredGridCells",
        "vtkClientServerMoveData",
        "vtkCTHSimpleSurface",
        "vtkDataObjectTreeToPointSetFilter",
        "vtkDistributedPointCloudFilter",
        "vtkEnvironmentAnnotationFilter",
        "vtkExtractSelectionRange",
        "vtkGridConnectivity",
        "vtkHybridProbeFilter",
        "vtkIntegrateFlowThroughSurface",
        "vtkInSituPParticlePathFilter",
        "vtkIsoVolume",
        "vtkIntersectFragments",
        "vtkMaterialInterfaceFilter",
        "vtkMinMax",
        "vtkMergeCompositeDataSet",
        "vtkMPIMoveData",
        "vtkNonOverlappingAMRLevelIdScalars",
        "vtkMultiBlockFromTimeSeriesFilter",
        "vtkOrderedCompositeDistributor",
        "vtkPCellSizeFilter",
        "vtkPConvertSelection",
        "vtkPExtractHistogram",
        "vtkPlotEdges",
        "vtkPolyLineToRectilinearGridFilter",
        "vtkPSciVizContingencyStats",
        "vtkPSciVizKMeans",
        "vtkPSciVizDescriptiveStats",
        "vtkPSciVizMultiCorrelativeStats",
        "vtkPSciVizPCAStats",
        "vtkRectilinearGridConnectivity",
        "vtkReductionFilter",
        "vtkRulerLineForInput",
        "vtkSliceAlongPolyPlane",
        "vtkSurfaceVectors",
        "vtkTimeStepProgressFilter",
        "vtkTimeToTextConvertor"
    ]
    filters = [f for f in filters if f.className not in missingHeader]

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
