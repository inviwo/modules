# modules

Additional Inviwo modules grouped into different categories. To enable the modules of a specific category, the path of the category must be added to the CMake option `IVW_EXTERNAL_MODULES` in the Inviwo project. For example, the topovis and misc modules are enabled by setting `IVW_EXTERNAL_MODULES` to `c:/dev/inviwo/modules/topovis;c:/dev/inviwo/modules/misc`, assuming the repository was checked out into `c:\dev\inviwo\modules`. Paths are separated by semicolon `;`.

## flowvis - Flow Visualization
## infovis - Information Visualization
## medvis - Medical Visualization

## misc - Miscellaneous

* OpenMesh: interface to the (OpenMesh)[https://www.openmesh.org] library
* SpringSystem: implementation of a 3D spring mass system

## molvis
Molecular visualizations.

## tensorvis
Modules for tensor field visualization. 

## topovis 
Modules for topological methods and topology visualization. The functionality is provided by the Topology ToolKit (TTK)[https://topology-tool-kit.github.io/], which requires boost. See [readme](topovis/readme.md) for detailed instructions.
