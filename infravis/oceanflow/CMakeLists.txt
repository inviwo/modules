ivw_module(OceanFlow)

set(HEADER_FILES
    include/inviwo/oceanflow/oceanflowmodule.h
    include/inviwo/oceanflow/oceanflowmoduledefine.h
    include/inviwo/oceanflow/processors/oceanraycaster.h
)
ivw_group("Header Files" ${HEADER_FILES})

set(SOURCE_FILES
    src/oceanflowmodule.cpp
    src/processors/oceanraycaster.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

set(SHADER_FILES
	# Add shaders
)
ivw_group("Shader Files" ${SHADER_FILES})

set(TEST_FILES
    tests/unittests/oceanflow-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

# Add shader directory to install package
#ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)
