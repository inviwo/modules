# Inviwo VTK module

This module includes and links agains VTK making VTK datastructures, algorithms, and filters available for use within Inviwo processors.

Additionally, we provide some processors for reading, writing, and converting VTK data.

__*Note:*__ VTK is not a git submodule of Inviwo and therefore needs to be compiled/installed separately.


## Usage
If you want to use VTK in your module, simply make your module depend on InviwoVTKModule (in the ```depends.cmake``` file).

A VTK file reader and VTK file writer are available as Inviwo processors. In order to work with VTK data in your processor, you can use the file reader and add a ```VTKDataSetInport``` to you processor. From here on out you can build a standard VTK pipeline. Once done, you can save your data via the VTK file writer.

As an example, you can have a look at the ```VTKDataSetInformation``` processor.


## VTK version
There is currently no restriction regarding the VTK version. We handle both, the old and new way of including VTK via CMake as well as the legacy and XML file versions.
The recommended version is 8.2.0 (or higher) since this is the one used in the regression tests.
