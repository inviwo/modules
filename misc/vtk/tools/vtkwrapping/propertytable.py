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
import rich
import dataclasses

from . import vtkdata


def elide(text: str, chars: int) -> str:
    if len(text) == 0:
        return text
    chars = 4 if chars <= 3 else chars
    line = text.splitlines()[0]
    return line if len(line) <= chars - 3 else f"{line[:chars-3]}..."


def makeTable(items) -> rich.table.Table:
    table = rich.table.Table()
    if len(items) == 0:
        return table
    for key in dataclasses.asdict(items[0]).keys():
        table.add_column(key)
    for item in items:
        table.add_row(*map(lambda x: elide(str(x), 20), dataclasses.astuple(item)))
    return table


def generate_filter_table(filters: list[vtkdata.FilterData]) -> None:
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
