# modules

Additional Inviwo modules grouped into different categories. To enable the modules of a specific category, the path of the category must be added to the CMake option `IVW_EXTERNAL_MODULES` in the Inviwo project. For example, the **topovis** and **misc** modules are enabled by setting `IVW_EXTERNAL_MODULES` to `c:/dev/inviwo/modules/topovis;c:/dev/inviwo/modules/misc`, assuming the repository was checked out into `c:\dev\inviwo\modules`. Paths are separated by semicolon `;`.

## vectorvis - Vector Visualization
* IntegralLineFiltering: Provides functionality to convert a `IntegralLineSet` to `DataFrame` containing various metrics of the lines that can be used together with the plotting functionality in the `Plotting` and `PlottingGL` modules in core.  See example workspace on how to use it. 

## infovis - Information Visualization
## medvis - Medical Visualization

* DICOM: functionality for loading and writing DICOM files using the Grassroots DICOM ([GDCM](https://sourceforge.net/projects/gdcm/))

See [readme](medvis/readme.md) for detailed instructions.

## misc - Miscellaneous

* DevTools: useful tools for developing and testing Inviwo networks
* NanoVGUtils: A module integrating the Vector Graphics rendering library [NanoVG](https://github.com/memononen/nanovg)
* NanoVGPY: Python bindings for the `NanoVGUtils` module
* OpenMesh: interface to the [OpenMesh](https://www.openmesh.org) library
* SpringSystem: implementation of a 3D spring mass system
* VTK: basic interface with the Visualization Toolkit [VTK](https://vtk.org)

## molvis
Molecular visualizations.

## tensorvis
Modules for tensor field visualization. 

## topovis 
Modules for topological methods and topology visualization. The functionality is provided by the Topology ToolKit [TTK](https://topology-tool-kit.github.io/), which requires boost. See [readme](topovis/readme.md) for detailed instructions.
