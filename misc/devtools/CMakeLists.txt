#--------------------------------------------------------------------
# Inviwo DevTools Module
ivw_module(DevTools)

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
    include/inviwo/devtools/devtoolsmodule.h
    include/inviwo/devtools/devtoolsmoduledefine.h
    include/inviwo/devtools/processors/eventlogger.h
    include/inviwo/devtools/processors/inviwostatsplot.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    src/devtoolsmodule.cpp
    src/processors/eventlogger.cpp
    src/processors/inviwostatsplot.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

#--------------------------------------------------------------------
# Add shaders
set(SHADER_FILES
	glsl/stat_lines.frag
	glsl/stat_lines.geom
	glsl/stat_lines.vert
)
ivw_group("Shader Files" ${SHADER_FILES})

#--------------------------------------------------------------------
# Add Unittests
set(TEST_FILES
    tests/unittests/devtools-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

#--------------------------------------------------------------------
# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

#--------------------------------------------------------------------
# Add shader directory to pack
ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)
