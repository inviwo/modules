#--------------------------------------------------------------------
# Inviwo SpringSystem Module
ivw_module(SpringSystem)

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
    include/inviwo/springsystem/datastructures/gravityspringsystem.h
    include/inviwo/springsystem/datastructures/springsystem.h
    include/inviwo/springsystem/datastructures/zerospringsystem.h
    include/inviwo/springsystem/processors/springsystemprocessor.h
    include/inviwo/springsystem/springsystemmodule.h
    include/inviwo/springsystem/springsystemmoduledefine.h
    include/inviwo/springsystem/utils/springsystemutils.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    src/datastructures/springsystem.cpp
    src/datastructures/gravityspringsystem.cpp
    src/datastructures/zerospringsystem.cpp
    src/processors/springsystemprocessor.cpp
    src/springsystemmodule.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

#--------------------------------------------------------------------
# Add shaders
set(SHADER_FILES
    #${CMAKE_CURRENT_SOURCE_DIR}/glsl/springsystemprocessor.frag
)
ivw_group("Shader Files" ${SHADER_FILES})


#--------------------------------------------------------------------
# Add Unittests
set(TEST_FILES
    #${CMAKE_CURRENT_SOURCE_DIR}/tests/springsystem-test.cpp
)
ivw_add_unittest(${TEST_FILES})

#--------------------------------------------------------------------
# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

#--------------------------------------------------------------------
# Add shader directory to pack
# ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)
