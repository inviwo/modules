# Inviwo C3D Module
ivw_module(C3D)

# Add header files
set(HEADER_FILES
    include/inviwo/c3d/c3dmodule.h
    include/inviwo/c3d/c3dmoduledefine.h
    include/inviwo/c3d/datastructures/c3ddata.h
    include/inviwo/c3d/datastructures/c3ddatatraits.h
    include/inviwo/c3d/io/c3dreader.h
    include/inviwo/c3d/ports/c3dport.h
    include/inviwo/c3d/processors/c3daveragedpositions.h
    include/inviwo/c3d/processors/c3dpointalignment.h
    include/inviwo/c3d/processors/c3dsource.h
    include/inviwo/c3d/processors/c3dtodataframe.h
    include/inviwo/c3d/processors/c3dtomesh.h
    include/inviwo/c3d/processors/c3dtransformpoints.h
)
ivw_group("Header Files" ${HEADER_FILES})

# Add source files
set(SOURCE_FILES
    src/c3dmodule.cpp
    src/datastructures/c3ddata.cpp
    src/datastructures/c3ddatatraits.cpp
    src/io/c3dreader.cpp
    src/ports/c3dport.cpp
    src/processors/c3daveragedpositions.cpp
    src/processors/c3dpointalignment.cpp
    src/processors/c3dsource.cpp
    src/processors/c3dtodataframe.cpp
    src/processors/c3dtomesh.cpp
    src/processors/c3dtransformpoints.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

# Add shaders
set(SHADER_FILES
)
ivw_group("Shader Files" ${SHADER_FILES})

# Add Unittests
set(TEST_FILES
)
ivw_add_unittest(${TEST_FILES})

# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

find_package(ezc3d CONFIG REQUIRED)
ivw_vcpkg_install(ezc3d MODULE C3D)
target_link_libraries(inviwo-module-c3d PUBLIC ezc3d)
