ivw_module(ComputeUtils)

set(HEADER_FILES
    include/inviwo/computeutils/algorithm/volumechannelsplitgl.h
    include/inviwo/computeutils/algorithm/volumeminmaxgl.h
    include/inviwo/computeutils/algorithm/volumenormalizationgl.h
    include/inviwo/computeutils/algorithm/volumereductiongl.h
    include/inviwo/computeutils/algorithm/volumeshrinktonormalrangegl.h
    include/inviwo/computeutils/computeutilsmodule.h
    include/inviwo/computeutils/computeutilsmoduledefine.h
    include/inviwo/computeutils/processors/volumechannelsplitglprocessor.h
    include/inviwo/computeutils/processors/volumeminmaxglprocessor.h
    include/inviwo/computeutils/processors/volumenormalizationglprocessor.h
    include/inviwo/computeutils/processors/volumereductionglprocessor.h
    include/inviwo/computeutils/processors/volumeshrinktonormalrangeglprocessor.h
)
ivw_group("Header Files" ${HEADER_FILES})

set(SOURCE_FILES
    src/algorithm/volumechannelsplitgl.cpp
    src/algorithm/volumeminmaxgl.cpp
    src/algorithm/volumenormalizationgl.cpp
    src/algorithm/volumereductiongl.cpp
    src/algorithm/volumeshrinktonormalrangegl.cpp
    src/computeutilsmodule.cpp
    src/processors/volumechannelsplitglprocessor.cpp
    src/processors/volumeminmaxglprocessor.cpp
    src/processors/volumenormalizationglprocessor.cpp
    src/processors/volumereductionglprocessor.cpp
    src/processors/volumeshrinktonormalrangeglprocessor.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

set(SHADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/glsl/volumesplit.comp
	${CMAKE_CURRENT_SOURCE_DIR}/glsl/volumereduction.comp
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/volumenormalization.comp
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/volumeshrinktonormalrange.comp
)
ivw_group("Shader Files" ${SHADER_FILES})

set(TEST_FILES
    tests/unittests/computeutils-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

# Add shader directory to install package
#ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)
