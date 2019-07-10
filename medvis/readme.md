# Medical visualization modules for Inviwo

## DICOM module
Inviwo module providing functionality for loading and writing DICOM files using the Grassroots DICOM (GDCM)[https://sourceforge.net/projects/gdcm/] library.

### Setup
1. checkout Inviwo https://github.com/inviwo/inviwo.git
2. init submodules of the Inviwo repo
    ```
    git submodule update --init --recursive
    ```
3. checkout https://github.com/inviwo/modules.git
4. init submodules of the modules repo
    ```
    git submodule update --init --recursive
    ```
5. in CMake
    a) append `medvis` directory of the modules repo to `IVW_EXTERNAL_MODULES`, e.g. `c:/dev/inviwo/modules/medvis;c:/dev/inviwo/modules/misc`
    b) run CMake
    c) make sure the `DICOM` module is enabled (`IVW_MODULE_DICOM`)
7. compile
8. run Inviwo.