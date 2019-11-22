#--------------------------------------------------------------------
# Inviwo MolVisBase Module
ivw_module(MolVisBase)

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
    include/inviwo/molvisbase/datastructures/molecularstructure.h
    include/inviwo/molvisbase/molvisbasemodule.h
    include/inviwo/molvisbase/molvisbasemoduledefine.h
    include/inviwo/molvisbase/ports/molecularstructureport.h
    include/inviwo/molvisbase/processors/molecularstructuretomesh.h
    include/inviwo/molvisbase/util/molvisutils.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    src/datastructures/molecularstructure.cpp
    src/molvisbasemodule.cpp
    src/ports/molecularstructureport.cpp
    src/processors/molecularstructuretomesh.cpp
    src/util/molvisutils.cpp
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
    tests/unittests/molvisbase-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

#--------------------------------------------------------------------
# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

#--------------------------------------------------------------------
# Add shader directory to pack
# ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)
