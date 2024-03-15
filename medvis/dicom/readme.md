# Inviwo DICOM module

Adds data reading support for the DICOM image/volume file format (.dcm file ending). Uses the Grassroots DICOM library [GDCM](https://github.com/malaterre/GDCM) for reading DICOM files and directories.

To use this module, make sure that you include the following in your CMake preset:

```json
{
  "inherits" : ["modules", "msvc-dev-vcpkg"],
  "cacheVariables": {
    "VCPKG_MANIFEST_FEATURES": "dicom",
    "IVW_MODULE_DICOM" :       { "type": "BOOL", "value": "ON" }
  }
}
```
