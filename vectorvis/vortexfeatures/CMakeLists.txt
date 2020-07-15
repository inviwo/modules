#--------------------------------------------------------------------
# Inviwo VortexFeatures Module
ivw_module(VortexFeatures)

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
    include/inviwo/vortexfeatures/algorithms/calculatemeasure.h
    include/inviwo/vortexfeatures/processors/floodfillimage.h
    include/inviwo/vortexfeatures/processors/okuboweiss.h
    include/inviwo/vortexfeatures/processors/windingangle.h
    include/inviwo/vortexfeatures/vortexfeaturesmodule.h
    include/inviwo/vortexfeatures/vortexfeaturesmoduledefine.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    src/processors/floodfillimage.cpp
    src/processors/okuboweiss.cpp
    src/processors/windingangle.cpp
    src/vortexfeaturesmodule.cpp
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
    tests/unittests/vortexfeatures-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

#--------------------------------------------------------------------
# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

#--------------------------------------------------------------------
# Add shader directory to pack
# ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)
