#--------------------------------------------------------------------
# Inviwo IntegralLineFiltering Module
ivw_module(IntegralLineFiltering)

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
    include/inviwo/integrallinefiltering/algorithm/shannonentropy.h
    include/inviwo/integrallinefiltering/algorithm/uniformspherepartitioning.h
    include/inviwo/integrallinefiltering/datastructures/directionalhistogram.h
    include/inviwo/integrallinefiltering/datastructures/sparsehistogram.h
    include/inviwo/integrallinefiltering/integrallinefilteringmodule.h
    include/inviwo/integrallinefiltering/integrallinefilteringmoduledefine.h
    include/inviwo/integrallinefiltering/processors/integrallinestodataframe.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    src/algorithm/shannonentropy.cpp
    src/algorithm/uniformspherepartitioning.cpp
    src/datastructures/directionalhistogram.cpp
    src/datastructures/sparsehistogram.cpp
    src/integrallinefilteringmodule.cpp
    src/processors/integrallinestodataframe.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})


#--------------------------------------------------------------------
# Add shaders
set(SHADER_FILES
    #${CMAKE_CURRENT_SOURCE_DIR}/glsl/integrallinefilteringprocessor.frag
)
ivw_group("Shader Files" ${SHADER_FILES})


#--------------------------------------------------------------------
# Add Unittests
set(TEST_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/tests/unittests/integrallinefiltering-unittest-main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/unittests/shannonentropy-test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/unittests/sparsehistorgram-test.cpp
)
ivw_add_unittest(${TEST_FILES})

#--------------------------------------------------------------------
# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

#--------------------------------------------------------------------
# Add shader directory to pack
# ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)
#ivw_folder(${${mod}_target} mystuff)
