# Inviwo MolVisBase Module
ivw_module(MolVisBase)

# Add header files
set(HEADER_FILES
    include/inviwo/molvisbase/algorithm/boundingbox.h
    include/inviwo/molvisbase/datastructures/molecularstructure.h
    include/inviwo/molvisbase/datastructures/molecularstructuretraits.h
    include/inviwo/molvisbase/datavisualizer/molecularmeshvisualizer.h
    include/inviwo/molvisbase/datavisualizer/molecularsourcevisualizer.h
    include/inviwo/molvisbase/io/basicpdbreader.h
    include/inviwo/molvisbase/molvisbasemodule.h
    include/inviwo/molvisbase/molvisbasemoduledefine.h
    include/inviwo/molvisbase/ports/molecularstructureport.h
    include/inviwo/molvisbase/processors/molecularstructuresource.h
    include/inviwo/molvisbase/processors/molecularstructuretomesh.h
    include/inviwo/molvisbase/util/aminoacid.h
    include/inviwo/molvisbase/util/atomicelement.h
    include/inviwo/molvisbase/util/chain.h
    include/inviwo/molvisbase/util/molvisutils.h
    include/inviwo/molvisbase/util/utilities.h
)
ivw_group("Header Files" ${HEADER_FILES})

# Add source files
set(SOURCE_FILES
    src/algorithm/boundingbox.cpp
    src/datastructures/molecularstructure.cpp
    src/datastructures/molecularstructuretraits.cpp
    src/datavisualizer/molecularmeshvisualizer.cpp
    src/datavisualizer/molecularsourcevisualizer.cpp
    src/io/basicpdbreader.cpp
    src/molvisbasemodule.cpp
    src/ports/molecularstructureport.cpp
    src/processors/molecularstructuresource.cpp
    src/processors/molecularstructuretomesh.cpp
    src/util/aminoacid.cpp
    src/util/atomicelement.cpp
    src/util/chain.cpp
    src/util/molvisutils.cpp
    src/util/utilities.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

# Add shaders
set(SHADER_FILES
)
ivw_group("Shader Files" ${SHADER_FILES})

# Add Unittests
set(TEST_FILES
    tests/unittests/molvisbase-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

# Add shader directory to pack
# ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)
