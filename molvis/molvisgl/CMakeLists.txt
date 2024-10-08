#--------------------------------------------------------------------
# Inviwo MolVisGL Module
ivw_module(MolVisGL)

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
    include/inviwo/molvisgl/datavisualizer/molecularmeshrendervisualizer.h
    include/inviwo/molvisgl/datavisualizer/molecularstructurevisualizer.h
    include/inviwo/molvisgl/molvisglmodule.h
    include/inviwo/molvisgl/molvisglmoduledefine.h
    include/inviwo/molvisgl/processors/molecularmeshrenderer.h
    include/inviwo/molvisgl/processors/molecularrasterizer.h
    include/inviwo/molvisgl/processors/molecularrenderer.h
    include/inviwo/molvisgl/util/molvisglutils.h
    include/inviwo/molvisgl/util/vertexflags.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    src/datavisualizer/molecularmeshrendervisualizer.cpp
    src/datavisualizer/molecularstructurevisualizer.cpp
    src/molvisglmodule.cpp
    src/processors/molecularmeshrenderer.cpp
    src/processors/molecularrasterizer.cpp
    src/processors/molecularrenderer.cpp
    src/util/molvisglutils.cpp
    src/util/vertexflags.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

#--------------------------------------------------------------------
# Add shaders
set(SHADER_FILES
    glsl/licorice-oit.frag
    glsl/licorice.frag
    glsl/licorice.geom
    glsl/licorice.vert
    glsl/vdw-oit.frag
    glsl/vdw.frag
    glsl/vdw.geom
    glsl/vdw.vert
    glsl/intersection/raycapsule.glsl
    glsl/utils/vertexflags.glsl
)
ivw_group("Shader Files" ${SHADER_FILES})

#--------------------------------------------------------------------
# Add Unittests
set(TEST_FILES
    tests/unittests/molvisgl-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

#--------------------------------------------------------------------
# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

#--------------------------------------------------------------------
# Add shader directory to pack
ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)

ivw_register_license_file(NAME "Ray Capsule Intersection (GLSL)" MODULE MolVisGL
    URL https://www.shadertoy.com/view/Xt3SzX
    TYPE "Copyright © 2016 Inigo Quilez, The MIT License"
    FILES glsl/intersection/LICENSE.txt
)

ivw_register_license_file(NAME "mdUtils - Atomic element lookup tables and functions" MODULE MolVisGL
    URL https://github.com/scanberg/mdutils/
    TYPE "Copyright (c) 2020 scanberg, The MIT License"
    FILES LICENSE_atomicelement.txt
)
