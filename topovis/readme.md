# Topology visualization modules

## Topology ToolKit module
Module incorporating the Topology ToolKit (TTK)[https://topology-tool-kit.github.io/].
Requires boost as a prerequisite.

### Setup
1. checkout https://github.com/inviwo/modules.git
2. init submodules of the modules repo
    ```
    git submodule update --init --recursive
    ```
3. download and install (boost)[https://www.boost.org/users/download/] libraries (binaries for Visual Studio: https://sourceforge.net/projects/boost/files/boost-binaries/1.70.0/)
4. in CMake
    a) append `topovis` directory of the modules repo to `IVW_EXTERNAL_MODULES`, e.g. `c:/dev/inviwo/modules/topovis;c:/dev/inviwo/modules/misc`
    b) if boost is not found, set `Boost_INCLUDE_DIR` to the boost directory, e.g. `C:\dev\boost_1_70_0` 
    c) run CMake
    d) make sure the `TopologyToolkit` module is enabled (`IVW_MODULE_TOPOLOGYTOOLKIT`)
5. compile
6. run Inviwo. Examples available under `Examples -> TopologyToolkit`
