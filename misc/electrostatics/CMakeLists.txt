#--------------------------------------------------------------------
# Inviwo electrostatics Module
ivw_module(electrostatics)

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
    include/inviwo/electrostatics/electrostaticsmodule.h
    include/inviwo/electrostatics/electrostaticsmoduledefine.h
    include/inviwo/electrostatics/processors/electrostatics.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    src/electrostaticsmodule.cpp
    src/processors/electrostatics.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

#--------------------------------------------------------------------
# Add shaders
set(SHADER_FILES
)
ivw_group("Shader Files" ${SHADER_FILES})

#--------------------------------------------------------------------
# Add Unittests
set(TEST_FILES
    tests/unittests/electrostatics-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

#--------------------------------------------------------------------
# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

#--------------------------------------------------------------------
# Add shader directory to pack
# ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)
