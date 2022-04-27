# ttk Module

Generate wrappers around all the ttkvtk filters by parsing the paraview xml files
describing the filters.

to build with vcpkg add 
> "topology-tool-kit"
to the vcpkg.json file in the main inviwo repo
that will install vtk and ttk.

For setting stuff up in cmake is use this preset:
    {
      "name": "ttk-msvc-vcpkg",
      "displayName": "TTK MSVC VCPKG",
      "inherits" : ["modules", "msvc-dev-vcpkg"],
      "binaryDir" : "${sourceParentDir}/builds/inviwo-ttk",
      "environment": {
        "VCPKG_OVERLAY_PORTS" : "${sourceDir}/tools/vcpkg;${sourceParentDir}/modules/tools/vcpkg",
        "Path" : "C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Tools/MSVC/14.31.31103/bin/Hostx64/x64;$penv{Path}"
      },
      "cacheVariables": {
        "IVW_CFG_VCPKG_OVERLAYS" : "${sourceDir}/tools/vcpkg;${sourceParentDir}/modules/tools/vcpkg",
        "IVW_MODULE_TTK" :          { "type": "BOOL", "value": "ON" }
      }
    }

The msvc-dev-vcpkg assumes that that vcpkg is next to the inviwo repo.
This is the recommended structure
SomeDir/
 - builds   <-- build folder, might use subfolder for different builds
 - inviwo   <-- the inviwo repo
 - modules  <-- the modules repo
 - vcpkg    <-- the vcpkg repo


In
<build>/vcpkg_installed/x64-windows/share/vtk/FindPEGTL.cmake
comment out line 26:
    set_target_properties(taocpp::pegtl PROPERTIES IMPORTED_GLOBAL TRUE)

Have not figured out how to solve that one yet.
Think the issue is that inviwo also comes with a version of PEGTL. 
But I have not had time to change the one in inviwo to use vcpkg yet


To generate the wrappers run

>  python.exe .\tools\parsefilter.py --ttkrepo <path to a ttk repo> --output .\generated

from the same directory as this readme.

We need to have a ttk repo around to since we need to get to the paraview xml files which not 
included in the installation from vcpkg.