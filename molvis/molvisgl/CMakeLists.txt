#--------------------------------------------------------------------
# Inviwo MolVisGL Module
ivw_module(MolVisGL)

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
    include/inviwo/molvisgl/molvisglmodule.h
    include/inviwo/molvisgl/molvisglmoduledefine.h
    include/inviwo/molvisgl/processors/molecularrenderer.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    src/molvisglmodule.cpp
    src/processors/molecularrenderer.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

#--------------------------------------------------------------------
# Add shaders
set(SHADER_FILES
    glsl/licorice.vert
    glsl/licorice.geom
    glsl/licorice.frag
    glsl/vdw.vert
    glsl/vdw.geom
    glsl/vdw.frag
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
