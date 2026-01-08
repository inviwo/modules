# -*- coding: utf-8 -*-
# ********************************************************************************
#
# Inviwo - Interactive Visualization Workshop
#
# Copyright (c) 2024-2026 Inviwo Foundation
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
from typing import Optional, Tuple


@dataclasses.dataclass
class BoolProperty:
    defaultValue: Optional[int]


@dataclasses.dataclass
class IntVecProperty:
    defaultValue: Optional[list[int]]
    minValue: Optional[list[int]] = dataclasses.field(default_factory=lambda: [0])
    maxValue: Optional[list[int]] = dataclasses.field(default_factory=lambda: [100])


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
