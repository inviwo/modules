ivw_module(Gaussian)

set(HEADER_FILES
    include/inviwo/gaussian/gaussianmodule.h
    include/inviwo/gaussian/gaussianmoduledefine.h
)
ivw_group("Header Files" ${HEADER_FILES})

set(SOURCE_FILES
    src/gaussianmodule.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

set(SHADER_FILES
	# Add shaders
)
ivw_group("Shader Files" ${SHADER_FILES})

set(PYTHON_FILES
    python/processors/cubesource.py
    python/gaussianutil.py
)
ivw_group("Python Files" ${PYTHON_FILES})

set(TEST_FILES
    tests/unittests/gaussian-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES} ${PYTHON_FILES})

# Add shader directory to install package
#ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)
