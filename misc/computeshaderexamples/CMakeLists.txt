#--------------------------------------------------------------------
# Inviwo ComputeShaderExamples Module
ivw_module(ComputeShaderExamples)

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
    include/inviwo/computeshaderexamples/computeshaderexamplesmodule.h
    include/inviwo/computeshaderexamples/computeshaderexamplesmoduledefine.h
    include/inviwo/computeshaderexamples/processors/computeshaderbufferexample.h
    include/inviwo/computeshaderexamples/processors/computeshaderimageexample.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    src/computeshaderexamplesmodule.cpp
    src/processors/computeshaderbufferexample.cpp
    src/processors/computeshaderimageexample.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

#--------------------------------------------------------------------
# Add shaders
set(SHADER_FILES
    glsl/roll.comp
    glsl/spiral.comp
)
ivw_group("Shader Files" ${SHADER_FILES})

#--------------------------------------------------------------------
# Add Unittests
set(TEST_FILES
    tests/unittests/computeshaderexamples-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

#--------------------------------------------------------------------
# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

#--------------------------------------------------------------------
# Add shader directory to pack
# ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)
