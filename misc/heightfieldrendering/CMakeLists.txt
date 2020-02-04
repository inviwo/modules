#--------------------------------------------------------------------
# Inviwo HeightFieldRendering Module
ivw_module(HeightFieldRendering)

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
    include/inviwo/heightfieldrendering/heightfieldrenderingmodule.h
    include/inviwo/heightfieldrendering/heightfieldrenderingmoduledefine.h
    include/inviwo/heightfieldrendering/processors/cubeheightfieldrenderer.h
    include/inviwo/heightfieldrendering/processors/imagetopoints.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    src/heightfieldrenderingmodule.cpp
    src/processors/cubeheightfieldrenderer.cpp
    src/processors/imagetopoints.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

#--------------------------------------------------------------------
# Add shaders
set(SHADER_FILES
    glsl/cubepillarglyph.frag
    glsl/cubepillarglyph.geom
    glsl/cubepillarglyph.vert
)
ivw_group("Shader Files" ${SHADER_FILES})

#--------------------------------------------------------------------
# Add Unittests
set(TEST_FILES
    tests/unittests/heightfieldrendering-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

#--------------------------------------------------------------------
# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

#--------------------------------------------------------------------
# Add shader directory to pack
# ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)
