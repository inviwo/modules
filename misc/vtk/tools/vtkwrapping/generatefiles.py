# -*- coding: utf-8 -*-
# ********************************************************************************
#
# Inviwo - Interactive Visualization Workshop
#
# Copyright (c) 2024-2025 Inviwo Foundation
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
from pathlib import Path
from operator import attrgetter
import os
import dataclasses
import subprocess
from rich.console import Console
from typing import Tuple, Optional, TypeVar

from . import vtkdata, properties, propertytemplates, cpptemplates, fixes


console = Console()

_T = TypeVar("T")


@dataclasses.dataclass
class CustomTrait:
    trait: str
    include: str = ""


def generate_files(destination: Path,
                   filters: list[vtkdata.FilterData],
                   *,
                   uriPrefix: str = "vtk",
                   customTraits: list[CustomTrait] = [],
                   remove_old_files: bool = False,
                   property_fixes: Optional[dict[fixes.PropertyKey,
                                                 fixes.TextReplacement]] = None,
                   clangformat: Path = Path("clangformat")
                   ) -> None:
    """
    Generate header and source files in destination, one for each FilterData object in filters

    Parameters
    ----------
    destination : Path
        generated files are written to this folder.
    filters : list[vtkdata.FilterData]
        list of filters
    uriPrefix : str
        inserted between 'org.inviwo.' and '.className'
    customTraits: list[CustomTrait]
        list of additional traits to be added to the template
    remove_old_files : bool, optional
        If set, existing header and source files matching 'ivw_*' are deleted. The
        default is False.
    property_fixes: dict[fixes.PropertyKey, fixes.TextReplacement], optional
        applies textual replacements to properties listed in the dictionary after source code
        has been generated

    """
    destination.mkdir(exist_ok=True)

    if remove_old_files:
        for file in destination.glob('ivw_*.h'):
            os.unlink(file)
        for file in destination.glob('ivw_*.cpp'):
            os.unlink(file)

    includes = []
    register = []
    used: list[str] = []
    for data in sorted(filters, key=attrgetter('className')):
        if data.className in used:
            console.print(f"[bold red]class name aleady used {data}")
            continue

        used.append(data.className)
        name = data.className
        header, source = generate_source_files(
            data, uriPrefix, customTraits=customTraits,
            property_fixes=property_fixes)
        with open(destination / f"ivw_{name.lower()}.h", 'w') as f:
            f.write(header)
        with open(destination / f"ivw_{name.lower()}.cpp", 'w') as f:
            f.write(f'#include "ivw_{name.lower()}.h"\n{source}')

        includes.append(f'#include "ivw_{name.lower()}.h"')
        register.append(f"    register{data.className}(module);")

        formatFile(destination / f"ivw_{name.lower()}.h", clangformat)
        formatFile(destination / f"ivw_{name.lower()}.cpp", clangformat)

    with open(destination / "registerfilters.h", 'w') as f:
        f.write(cpptemplates.mainHeader)

    with open(destination / "registerfilters.cpp", 'w') as f:
        f.write('#include "registerfilters.h"\n'
                + cpptemplates.mainSourceTemplate.format(includes='\n'.join(includes),
                                                         register='\n'.join(register)))


def valueOr(value: Optional[_T], default: _T) -> _T:
    if value is None:
        return default
    else:
        return value


def formatFile(file: Path, clangformat: Path = Path("clang-format")) -> None:
    subprocess.run([clangformat, "-i", file])


def generate_source_files(data: vtkdata.FilterData, uriPrefix: str, *,
                          customTraits: list[CustomTrait] = [],
                          property_fixes: Optional[dict[fixes.PropertyKey,
                                                        fixes.TextReplacement]] = None
                          ) -> Tuple[str, str]:
    """
    Generate the file contents for a header and source file based on the filter data

    Parameters
    ----------
    data : vtkdata.FilterData
        annotated vtk filter.
    customTraits: list[CustomTrait]
        list of additional traits to be added to the template
    property_fixes: dict[fixes.PropertyKey, fixes.TextReplacement], optional
        applies textual replacements to properties listed in the dictionary after source code
        has been generated

    Returns
    -------
    Tuple[str, str]
        Contents of the header and source files.

    """
    propClasses = ""
    proplist = []
    extra = {}

    for p in data.props:
        kind = p.kind
        if isinstance(kind, properties.DoubleVecProperty):
            if p.numElem == 1:
                template = propertytemplates.doubleTemplate
            elif p.numElem == 2:
                template = propertytemplates.doubleVec2Template
            elif p.numElem == 3:
                template = propertytemplates.doubleVec3Template
            elif p.numElem == 4:
                template = propertytemplates.doubleVec4Template
            elif p.numElem == 6:
                template = propertytemplates.doubleVec6Template
            else:
                print(f"Missing template for DoubleVecProperty of dim {p.numElem}")
                continue

            if not kind.minValue:
                kind.minValue = [0.0] * p.numElem

            if kind.minValue:
                if len(kind.minValue) == 1:
                    kind.minValue = kind.minValue * p.numElem

            if not kind.maxValue:
                kind.maxValue = [100.0] * p.numElem

            if kind.maxValue:
                if len(kind.maxValue) == 1:
                    kind.maxValue = kind.maxValue * p.numElem

        elif isinstance(kind, properties.IntVecProperty):
            if p.numElem == 1:
                template = propertytemplates.intTemplate
            elif p.numElem == 2:
                template = propertytemplates.intVec2Template
            elif p.numElem == 3:
                template = propertytemplates.intVec3Template
            elif p.numElem == 4:
                template = propertytemplates.intVec4Template
            elif p.numElem == 6:
                template = propertytemplates.intVec6Template
            else:
                print(f"Missing template for IntVecProperty of dim {p.numElem}")
                continue

            if not kind.minValue:
                kind.minValue = [0] * p.numElem

            if kind.minValue:
                if len(kind.minValue) == 1:
                    kind.minValue = kind.minValue * p.numElem

            if not kind.maxValue:
                kind.maxValue = [100] * p.numElem

            if kind.maxValue:
                if len(kind.maxValue) == 1:
                    kind.maxValue = kind.maxValue * p.numElem

        elif isinstance(kind, properties.IntOptionProperty) and p.numElem == 1:
            template = propertytemplates.optionTemplate
            kind.defaultValue = valueOr(kind.defaultValue, 0)
            kind.options = ', '.join(f'{{"{n}","{n}",{v}}}' for n, v in kind.options)

        elif isinstance(kind, properties.StringProperty) and p.numElem == 1:
            template = propertytemplates.stringTemplate
            kind.defaultValue = valueOr(kind.defaultValue, "")

        elif isinstance(kind, properties.FileProperty) and p.numElem == 1:
            template = propertytemplates.fileTemplate
            kind.defaultValue = valueOr(kind.defaultValue, "")

        elif isinstance(kind, properties.ButtonProperty) and (not p.numElem or p.numElem == 1):
            template = propertytemplates.buttonTemplate

        elif isinstance(kind, properties.ExtentProperty) and p.numElem == 1:
            template = propertytemplates.extentTemplate

        elif isinstance(kind, properties.BoolProperty) and p.numElem == 1:
            template = propertytemplates.boolTemplate
            kind.defaultValue = "true" if kind.defaultValue else "false"

        elif isinstance(kind, properties.FieldSelectionProperty):
            template = propertytemplates.fieldSelectionTemplate
            if len(p.kind.defaultValue) > 4 and p.kind.defaultValue[4] is not None:
                extra["defaultoptions"] = '{{"{0}","{0}","{0}"}}'.format(p.kind.defaultValue[4])
            else:
                extra["defaultoptions"] = ""
        else:
            print(f"Missing template for {type(kind)} in property '{p.identifier}'"
                  + f"{f' (dim {p.numElem})' if p.numElem else ''} in filter {data.identifier}")
            continue  # WIP:::

        structName = f"Wrapper{len(proplist)}"

        propertySource = template.format(
            structName=structName,
            className=data.className,
            identifier=p.identifier,
            displayName=p.displayName,
            command=p.command,
            doc=p.doc if p.doc else "",
            **dataclasses.asdict(kind),
            **extra
        )

        key = fixes.PropertyKey(data.className, p.identifier)
        if key in property_fixes:
            replacement = property_fixes[key]
            propertySource = propertySource.replace(
                replacement.old_value, replacement.new_value)

        propClasses += propertySource
        proplist.append(structName)

    inputData = ", ".join(f'InputData{{.identifier = "{d.identifier}", \
                                       .dataType = "{valueOr(d.dataType, "")}", \
                                       .numComp = {valueOr(d.numComp,[-1])[0]}, \
                                       .doc = R"({valueOr(d.doc, "")})"}}'
                          for d in data.inports)
    outputData = ", ".join(f'OutputData{{.identifier = "{d.identifier}", \
                                         .displayName = "{d.displayName}", \
                                         .index = {d.index}}}'
                           for d in data.outports)

    groups = []
    for name, members in data.groups.items():
        m = ", ".join(f'"{x}"' for x in members)
        groups.append(f'Group{{"{name}", {{{m}}}}}')

    uri = f'{uriPrefix}.{data.className}' if uriPrefix else data.className

    traits: str = "\n    ".join(t.trait for t in customTraits)
    includes: str = "\n".join(t.include for t in customTraits)

    source = cpptemplates.sourceTemplate.format(
        customIncludes=includes,
        customTraits=traits,
        identifier=data.identifier,
        displayName=data.displayName,
        uri=uri,
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

    header = cpptemplates.headerTemplate.format(className=data.className)

    return header, source
