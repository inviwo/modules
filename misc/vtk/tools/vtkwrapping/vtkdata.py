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
import dataclasses
from typing import Optional, Union
from collections import defaultdict

from . import properties


@dataclasses.dataclass
class FilterPropertyData:
    identifier: str
    displayName: str
    command: str
    numElem: Optional[int] = None
    informationOnly: bool = False
    kind: Union[properties.IntVecProperty, properties.DoubleVecProperty,
                properties.IntOptionProperty, properties.StringProperty,
                properties.FileProperty, properties.ButtonProperty, properties.BoolProperty,
                properties.ExtentProperty, properties.FieldSelectionProperty, None] = None
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
