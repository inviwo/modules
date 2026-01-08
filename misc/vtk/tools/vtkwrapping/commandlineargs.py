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
import argparse
from pathlib import Path
import sys

from . import configuration

parse_modes = ("vtk", "ttk", "custom")


def parse_arguments(cmdargs=None, namespace=None) -> configuration.Configuration:
    parser = argparse.ArgumentParser(
        description="Parse paraview xml",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    parser.add_argument('-o', '--output', type=Path,
                        help='Path to output directory for generated files', required=True)
    parser.add_argument('--mode', choices=parse_modes, default='vtk',
                        help='Selects a set of filters to parse and generate. '
                        + '"vtk": VTK and ParaView filters (looks for XML filters in "./filters/"). '  # noqa: E501
                        + '"ttk:" only TTK filters are considered. '
                        + '"custom": use a custom XML filter path.')
    parser.add_argument('--ttkrepo', type=Path,
                        help='Path to the ttk sources, needed for paraview xml files. Required if '
                        + ' mode is set to "ttk". The vcpkg build tree can be used directly, '
                        + 'for example "--ttkrepo ../../../vcpkg/buildtrees/topology-tool-kit/src/8230f85d12-cfe2266a70.clean"')  # noqa: E501
    parser.add_argument('--filters', type=Path,
                        help='Path to custom XML filters. Required if '
                        + ' mode is set to "custom".')
    parser.add_argument('-c', '--clear', action='store_true',
                        help='Clear any old file in the output directory')
    parser.add_argument('-p', '--printtable', action='store_true',
                        help='Print a table of all generated processors and  properties')
    parser.add_argument('--clangformat', type=Path,
                        help='Path to clang format', required=False, default=Path("clang-format"))
    parser.add_argument('-q', '--quiet', action='store_true',
                        help='Suppress detailed error logging')

    args = parser.parse_args(cmdargs, namespace)

    if args.mode == "ttk" and not args.ttkrepo:
        parser.print_usage()
        print(f'{parser.prog} error: mode set to "ttk" requires the following argument: --ttkrepo',
              file=sys.stderr)
        parser.exit()
    if args.mode == "custom" and not args.filters:
        parser.print_usage()
        print(f'{parser.prog} error: mode set to "custom" requires the following argument: --filters',  # noqa: E501
              file=sys.stderr)
        parser.exit()

    config = configuration.Configuration(show_traceback=not args.quiet,
                                         remove_old_files=args.clear,
                                         print_table=args.printtable,
                                         mode=configuration.Mode[args.mode],
                                         destination=args.output,
                                         filters=args.filters,
                                         ttkrepo=args.ttkrepo,
                                         clangformat=args.clangformat
                                         )

    return config
