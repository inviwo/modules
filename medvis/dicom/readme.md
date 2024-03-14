# Inviwo DICOM module

Adds data reading support for the DICOM image/volume file format (.dcm file ending). Uses the Grassroots DICOM library [GDCM](https://github.com/malaterre/GDCM) for reading DICOM files and directories.

When using vcpkg, set the CMake option `IVW_USE_EXTERNAL_GDCM` to false and add `dicom` to the `VCPKG_MANIFEST_FEATURES` as in the following CMake preset:

```json
    {
      "name": "dicom",
      "displayName": "GDCM DICOM",
      "inherits" : ["modules", "msvc-dev-vcpkg"],
      "cacheVariables": {
        "VCPKG_OVERLAY_PORTS" :    "${sourceDir}/tools/vcpkg;${sourceParentDir}/modules/tools/vcpkg",
        "VCPKG_MANIFEST_FEATURES": "dicom",

        "IVW_MODULE_DICOM" :         { "type": "BOOL", "value": "ON" },
        "IVW_USE_EXTERNAL_GDCM" :    { "type": "BOOL", "value": "ON" }
      }
    }
```
