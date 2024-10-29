# ttk Module

This module provides processors wrapping the [Topology ToolKit](https://topology-tool-kit.github.io/), short TTK. The processors are based on the XML information on the various TTK ParaView filters.

The ttk module depends on the Inviwo VTK module (`misc/vtk`) as well as the TTK and VTK libraries. The VTK functionality is provided by the Inviwo module. To simplify things on Windows, we recommend the usage of vcpkg for both TTK and VTK using the following CMake preset.

```json
    {
      "name": "ttk-msvc-vcpkg",
      "displayName": "TTK MSVC VCPKG",
      "inherits" : ["msvc-developer-modules"],
      "cacheVariables": {
        "VCPKG_OVERLAY_PORTS" :    "${sourceDir}/tools/vcpkg;${sourceParentDir}/modules/tools/vcpkg",
        "VCPKG_MANIFEST_FEATURES": "vtk;ttk",

        "OpenMP_CXX_FLAGS":       "/openmp:llvm",
        "OpenMP_C_FLAGS":         "/openmp:llvm",

        "IVW_MODULE_VTK" :         { "type": "BOOL", "value": "ON" },
        "IVW_MODULE_TTK" :         { "type": "BOOL", "value": "ON" }
      }
    }
```

See the Inviwo VTK module for more details.
