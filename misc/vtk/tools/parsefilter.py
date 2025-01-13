# ********************************************************************************
#
# Inviwo - Interactive Visualization Workshop
#
# Copyright (c) 2023-2025 Inviwo Foundation
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

from rich.console import Console
from rich import print

import vtkwrapping.generatefiles
import vtkwrapping.xmlparsing
import vtkwrapping.commandlineargs
import vtkwrapping.propertytable

import vtkwrapping.excludedfilters
import vtkwrapping.fixes

console = Console()


def vtk_paraview_filters(category: str, tags: str,
                         showTraceback: bool) -> list[vtkwrapping.vtkdata.FilterData]:
    """
    Parses VTK and ParaView XML filters

    The filters are located in ./filters/ and
    https://github.com/Kitware/ParaView/raw/master/Remoting/Application/Resources/readers_ioxml.xml

    Note that ./filters/filters.xml dates back to ParaView 5.7.0. Newer versions provide no longer
    any XML filter information.
    https://github.com/Kitware/ParaView/blob/v5.7.0/ParaViewCore/ServerManager/SMApplication/Resources/filters.xml

    Returns
    -------
    filters : list[vtkwrapping.vtkdata.FilterData]
        List of filters found and successfully parsed
    """

    filters: list[vtkwrapping.vtkdata.FilterData] = []

    paraviewXmls: list[str] = [
        "https://github.com/Kitware/ParaView/raw/master/"
        + "Remoting/Application/Resources/readers_ioxml.xml"
    ]
    for url in paraviewXmls:
        filters.extend(vtkwrapping.parse_url(url, category, tags + ",readers", showTraceback))

    # parse specific local VTK filters
    for file in Path("filters/").glob("*.xml"):
        filters.extend(vtkwrapping.parse_file(file, category, tags, showTraceback))
    return filters


def ttk_filters(category: str, tags: str,
                ttkrepo: Path, showTraceback: bool) -> list[vtkwrapping.vtkdata.FilterData]:
    """
    Parses XML filters of the Topology Toolkit

    The filters are located in <ttksource>/paraview/xmls/

    Returns
    -------
    filters : list[vtkwrapping.vtkdata.FilterData]
        List of filters found and successfully parsed
    """

    basedir = ttkrepo / "paraview" / "xmls"

    # TTK debug widgets provide additional filter properties
    debugWidgetsFile = ttkrepo / "CMake" / "debug_widgets.xml"
    with open(debugWidgetsFile, 'r') as f:
        debugWidgets = f.read()

    files = (xml for xml in basedir.glob("*.xml") if xml.stem not in
             vtkwrapping.excludedfilters.ttk_filters)

    filters: list[vtkwrapping.vtkdata.FilterData] = []
    for file in files:
        filters.extend(vtkwrapping.parse_file(file, category, tags, showTraceback,
                       lambda s: s.replace("${DEBUG_WIDGETS}", debugWidgets)))
    return filters


def custom_filters(filter_path: Path, category: str, tags: str,
                   showTraceback: bool) -> list[vtkwrapping.vtkdata.FilterData]:
    """
    Parses ParaView XML filters in filter_path

    Parameters
    ----------
    filter_path : Path
        base folder to look for XML files

    Returns
    -------
    filters : list[vtkwrapping.vtkdata.FilterData]
        List of filters found and successfully parsed
    """
    filters: list[vtkwrapping.vtkdata.FilterData] = []

    for file in filter_path.glob("*.xml"):
        filters.extend(vtkwrapping.parse_file(file, category, tags, showTraceback))
    return filters


if __name__ == '__main__':
    config = vtkwrapping.commandlineargs.parse_arguments()

    # Set clang format binary in case clang-format is not in path

    # config.clangformat = "C:/Program Files/Microsoft Visual Studio/2022/" \
    #     + "Enterprise/VC/Tools/Llvm/bin/clang-format.exe"

    filters: list[vtkwrapping.vtkdata.FilterData] = []
    if config.mode == vtkwrapping.Mode.vtk:
        filters = vtk_paraview_filters("vtk", "VTK", config.show_traceback)
    elif config.mode == vtkwrapping.Mode.ttk:
        filters = ttk_filters("topology", "TTK", config.ttkrepo, config.show_traceback)
    elif config.mode == vtkwrapping.Mode.custom:
        filters = custom_filters(config.filters, "vtk", "VTK,custom")

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

    filters = [f for f in filters if f.className not in
               vtkwrapping.excludedfilters.vtk_paraview_filters]

    if config.print_table:
        console.print(vtkwrapping.propertytable.generate_filter_table(filters))

    # some debug logging...
    # rich.inspect(filters["MorseSmaleComplex"])
    # header, source = generate(filters["MorseSmaleComplex"])
    # console.print(Syntax(header, lexer_name='c++', line_numbers=True))
    # console.print(Syntax(source, lexer_name='c++', line_numbers=True))

    prefix = config.mode.name
    if config.mode == vtkwrapping.Mode.custom:
        prefix = ""

    traits: list[vtkwrapping.CustomTrait] = []
    if config.mode == vtkwrapping.Mode.ttk:
        traits.append(vtkwrapping.CustomTrait(
            trait="ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;",
            include="#include <inviwo/ttk/util/ttkprocessorutils.h>"
        ))

    vtkwrapping.generatefiles.generate_files(
        destination=config.destination,
        filters=filters,
        uriPrefix=prefix,
        customTraits=traits,
        remove_old_files=config.remove_old_files,
        property_fixes=vtkwrapping.fixes.get_fixes_vtk(),
        clangformat=config.clangformat)


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
