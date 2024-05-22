# -*- coding: utf-8 -*-
# ********************************************************************************
#
# Inviwo - Interactive Visualization Workshop
#
# Copyright (c) 2024 Inviwo Foundation
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# ********************************************************************************
import re
from pathlib import Path
import xml.etree.ElementTree as ET
from typing import Callable, Optional, TypeVar
from functools import partial
from rich.console import Console
import requests
import traceback

from . import vtkdata, properties
from .exceptions import ParseException, PropertyException

console = Console()

# Variables for generic types in type specifiers see chain for example
T = TypeVar('T')
R = TypeVar('R')


def isValidIdentifierCharacter(char: str, extra: str = "") -> bool:
    """
    check whether the given character char is valid when used in an identifier

    see core/util/utilities.h

    Parameters
    ----------
    char : str
        single character.
    extra : str, optional
        additional characters allowed in identifiers. The default is "".

    Returns
    -------
    bool
        true if character is valid, false otherwise

    """
    return char.isalnum() or char == '_' or char in extra


def cleanIdentifier(identifier: str, extra: str = "") -> str:
    """
    create a valid identifer from the given string

    see core/util/utilities.h

    Parameters
    ----------
    identifier : str
        string to be cleaned up
    extra : str, optional
        additional characters allowed in identifiers. The default is "".

    Returns
    -------
    str
        valid identifier

    """
    return "".join([c for c in identifier if isValidIdentifierCharacter(c, extra)])


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
        value = xml.attrib.get(name, None)
        return value if value and value != "none" else None
    return get


def xmlfind(path: str):
    def get(xml: ET.Element):
        return xml.find(path)
    return get


def stripEachLine(text: str):
    return ('\n'.join(map(partial(str.strip), text.split('\n')))).strip()


def splitComma(text: str):
    return text.split(',')


def parse_file(file: Path, category: str, tags: str,
               showTraceback: bool,
               func: Optional[Callable[[str], str]] = None) \
        -> list[vtkdata.FilterData]:
    try:
        with open(file, 'r') as f:
            xmlstr = f.read()
            if func:
                xmlstr = func(xmlstr)
        return parse_xml(xmlstr, file, category, tags, showTraceback)
    except ParseException as e:
        if showTraceback:
            print(f"Error parsing {file}\n{e}")
            print(traceback.format_exc())
        else:
            print(f"Error parsing {file}")
    return []


def parse_url(url: str, category: str, tags: str, showTraceback: bool) -> list[vtkdata.FilterData]:
    try:
        response = requests.get(url)
        xmlstr = response.content.decode('utf-8')
        return parse_xml(xmlstr, Path(url), category, tags, showTraceback)
    except ParseException as e:
        if showTraceback:
            print(f"Error parsing {url}\n{e}")
            print(traceback.format_exc())
        else:
            print(f"Error parsing {url}")
    return []


def parse_xml(xmlstr: str, file: Path, category: str, tags: str,
              showTraceback: bool) -> list[vtkdata.FilterData]:
    '''
    Parse a paraview xml string, returns list of Filterdata
    '''
    xml = ET.fromstring(xmlstr)

    if xml.tag != "ServerManagerConfiguration":
        raise ParseException(
            f"Unexpected element, expected 'ServerManagerConfiguration' found '{xml.tag}'")

    filters = []
    for group in xml:
        if group.tag != "ProxyGroup":
            raise ParseException(f"Unexpected element, expected 'ProxyGroup' found '{group.tag}'")
        filters.extend(parseProxyGroup(group, file, category, tags, showTraceback))

    return filters

# Parsers for property like stuff


def parseHelperProperty(xml: ET.Element) -> vtkdata.FilterPropertyData:
    '''
    Helper that parses information that is in most properties
    '''
    data = vtkdata.FilterPropertyData(
        identifier=cleanIdentifier(xml.attrib["name"]),
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


def parseIntVectorProperty(xml: ET.Element) -> vtkdata.FilterPropertyData:
    data = parseHelperProperty(xml)

    if data.informationOnly:
        raise PropertyException("Unhandled IntProperty information_only not handled")

    elif xml.find('BooleanDomain') is not None:
        if data.numElem is not None and data.numElem >= 2:
            raise PropertyException(
                f"Unhandled BooleanDomain with more than 1 component {data.numElem}")
        else:
            data.kind = properties.BoolProperty(
                defaultValue=chain(xml, xmlattr("default_values"), int, bool)
            )
    elif (enumeration := xml.find('EnumerationDomain')) is not None:
        data.kind = properties.IntOptionProperty(
            options=[(value.attrib["text"], int(value.attrib["value"])) for value in enumeration],
        )
        defaultValue = chain(xml, xmlattr("default_values"), int)
        data.kind.defaultValue = next((i for i, v in enumerate(data.kind.options)
                                       if v[1] == defaultValue), None)
    elif (xml.find('ExtentDomain')) is not None:
        ep = properties.ExtentProperty(
            defaultValue=chain(xml, xmlattr("default_values"),
                               str.split, partial(map, int), tuple)
        )
        if rp := xml.find("RequiredProperties") is not None:
            for p in rp:
                ep.requiredProperties.append(properties.RequiredProperty(
                    function=xml.attrib["function"],
                    name=xml.attrib["name"]
                ))

        data.kind = ep

    elif data.numElem is not None and data.numElem >= 0 and data.numElem <= 4:
        data.kind = properties.IntVecProperty(
            defaultValue=chain(xml, xmlattr("default_values"),
                               str.split, partial(map, int), tuple),
            minValue=chain(xml, xmlfind('IntRangeDomain'), xmlattr("min"),
                           str.split, partial(map, int), tuple),
            maxValue=chain(xml, xmlfind('IntRangeDomain'), xmlattr("max"),
                           str.split, partial(map, int), tuple)
        )
    else:
        raise PropertyException("Unhandled IntProperty")
    return data


def parseDoubleVectorProperty(xml: ET.Element) -> vtkdata.FilterPropertyData:
    data = parseHelperProperty(xml)

    if data.informationOnly:
        raise PropertyException("Unhandled DoubleProperty information_only not handled")

    if data.numElem is not None and data.numElem >= 0 and data.numElem <= 4:
        data.kind = properties.DoubleVecProperty(
            defaultValue=chain(xml, xmlattr("default_values"),
                               str.split, partial(map, float), tuple),
            minValue=chain(xml, xmlfind('DoubleRangeDomain'), xmlattr("min"),
                           str.split, partial(map, float), tuple),
            maxValue=chain(xml, xmlfind('DoubleRangeDomain'), xmlattr("max"),
                           str.split, partial(map, float), tuple)
        )
    else:
        raise PropertyException("Unhandled DoubleProperty")
    return data


def parseStringVectorProperty(xml: ET.Element) -> vtkdata.FilterPropertyData:
    data = parseHelperProperty(xml)

    if data.informationOnly:
        raise PropertyException("Unhandled StringVectorProperty information_only not handled")

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
        data.kind = properties.FieldSelectionProperty(
            defaultValue=defaultValue,
            inport=chain(xml, xmlfind('ArrayListDomain/RequiredProperties/Property'),
                         xmlattr("name")),
        )
    elif xml.find('FileListDomain') is not None:
        data.kind = properties.FileProperty(
            defaultValue=xml.attrib.get("default_values", None))
    else:
        data.kind = properties.StringProperty(
            defaultValue=xml.attrib.get("default_values", None))
    return data


def parseProperty(xml: ET.Element) -> Optional[vtkdata.FilterPropertyData]:
    data = parseHelperProperty(xml)
    if "panel_widget" in xml.attrib and xml.attrib["panel_widget"] == "command_button":
        data.kind = properties.ButtonProperty()
        return data
    else:
        raise PropertyException("Unhandled Property")


def parseInputProperty(xml: ET.Element) -> vtkdata.InputData:
    '''
    Parse Input properties, Will be mapped to inviwo Inports
    '''

    data = vtkdata.InputData(
        identifier=xml.attrib["name"],
        dataType=chain(xml, xmlfind('DataTypeDomain/DataType'), xmlattr("value")),
        numComp=chain(xml, xmlfind('InputArrayDomain'),
                      xmlattr("number_of_components"),
                      splitComma, partial(map, int), tuple)
    )

    if (doc := xml.find('Documentation')) is not None:
        data.doc = stripEachLine(doc.text)

    return data


def parseOutport(xml: ET.Element) -> vtkdata.OutputData:
    '''
    Parse Outports, Will be mapped to inviwo Outports
    '''

    index = int(xml.attrib["index"])
    identifier = xml.attrib["id"] if "id" in xml.attrib else f"outport{index}"

    return vtkdata.OutputData(
        identifier=identifier,
        displayName=xml.attrib["name"],
        index=index
    )


def parseProxyGroup(group: ET.Element, file: Path,
                    category: str, tags: str, showTraceback: bool) -> list[vtkdata.FilterData]:
    filters = []
    for proxy in group:
        try:
            if proxy.tag in ["CompoundSourceProxy"]:
                # Maybe handle in the future?
                continue

            if proxy.tag not in ["SourceProxy", "WriterProxy", "Proxy"]:
                raise ParseException(f"Unexpected element, expected a proxy found '{proxy.tag}'")

            filters.append(parseProxy(proxy, file, category, tags, showTraceback))

        except ParseException as e:
            print(f"Error parsing {proxy.tag} in {file} \n{e}")
            # print(traceback.format_exc())
            # print(ET.tostring(proxy, encoding='unicode'))

    return filters


def parseProxy(proxy: ET.Element, file: Path, category: str, tags: str,
               showTraceback: bool) -> vtkdata.FilterData:

    doc = proxy.find('Documentation')
    if doc is None:
        docShort = ""
        docStr = ""
    else:
        docShort = re.sub(' +', ' ', doc.attrib['short_help']
                          ) if 'short_help' in doc.attrib else ""
        if isinstance(doc.text, str):
            docStr = stripEachLine(doc.text)
        elif 'long_help' in doc.attrib:
            docStr = doc.attrib['long_help']
        else:
            docStr = docShort

    data = vtkdata.FilterData(
        identifier=proxy.attrib["name"],
        displayName=proxy.attrib["label"] if "label" in proxy.attrib else proxy.attrib["name"],
        className=proxy.attrib["class"],
        category=category,
        tags=tags,
        desc=docShort,
        doc=docStr
    )

    propertyParsers: dict[str, Callable[[ET.Element], vtkdata.FilterPropertyData]] = {
        "IntVectorProperty": parseIntVectorProperty,
        "StringVectorProperty": parseStringVectorProperty,
        "DoubleVectorProperty": parseDoubleVectorProperty,
        "Property": parseProperty
    }

    for elem in proxy:
        if (parser := propertyParsers.get(elem.tag, None)) is not None:
            try:
                data.props.append(parser(elem))
            except ParseException:
                console.print(f"[bold red]Error parsing '{data.identifier}' in {file}")
                if showTraceback:
                    console.print_exception(extra_lines=1)
                    print(ET.tostring(elem, encoding="unicode"))
            except Exception:
                console.print(f"[bold orange]Error parsing '{data.identifier}' in {file}")
                if showTraceback:
                    console.print_exception(extra_lines=1)
                    print(ET.tostring(elem, encoding="unicode"))
                raise
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
            if showTraceback:
                print(ET.tostring(elem, encoding="unicode"))

    return data
