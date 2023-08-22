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

I add the path to dumpbin.exe to the environment Path since vtk want to use that while running cmake.

The msvc-dev-vcpkg assumes that that vcpkg is next to the inviwo repo.
This is the recommended structure
SomeDir/
 - builds   <-- build folder, might use subfolder for different builds
 - inviwo   <-- the inviwo repo
 - modules  <-- the modules repo
 - vcpkg    <-- the vcpkg repo


To generate the wrappers run

>  python.exe .\tools\parsefilter.py --ttkrepo <path to a ttk repo> --output .\generated

for example: 

> python.exe .\tools\parsefilter.py  --ttkrepo ../../../vcpkg/buildtrees/topology-tool-kit/src/8230f85d12-cfe2266a70.clean --output .\generated

from the same directory as this readme.

We need to have a ttk repo around to since we need to get to the paraview xml files which not 
included in the installation from vcpkg.


filters/filters.xml Comes from
https://github.com/Kitware/ParaView/blob/v5.7.0/ParaViewCore/ServerManager/SMApplication/Resources/filters.xml

After 5.7 paraview split up the filter in many more files, and even more filter became vtkPV filter
ie defined for a derived paraview subclas

