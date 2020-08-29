#--------------------------------------------------------------------
# Inviwo VortexFeatures Module
ivw_module(VortexFeatures)

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
    include/inviwo/vortexfeatures/algorithms/calculatemeasure.h
    include/inviwo/vortexfeatures/algorithms/vortexutils.h
    include/inviwo/vortexfeatures/datastructures/vortex.h
    include/inviwo/vortexfeatures/ports/vortexport.h
    include/inviwo/vortexfeatures/processors/assemblewindingangle.h
    include/inviwo/vortexfeatures/processors/floodfillimage.h
    include/inviwo/vortexfeatures/processors/lineplot.h
    include/inviwo/vortexfeatures/processors/okuboweiss.h
    include/inviwo/vortexfeatures/processors/vortexselector.h
    include/inviwo/vortexfeatures/processors/vortexsettovolumes.h
    include/inviwo/vortexfeatures/processors/vortexstatistics.h
    include/inviwo/vortexfeatures/processors/vortextomesh.h
    include/inviwo/vortexfeatures/processors/windingangle.h
    include/inviwo/vortexfeatures/vortexfeaturesmodule.h
    include/inviwo/vortexfeatures/vortexfeaturesmoduledefine.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    src/algorithms/vortexutils.cpp
    src/datastructures/vortex.cpp
    src/ports/vortexport.cpp
    src/processors/assemblewindingangle.cpp
    src/processors/floodfillimage.cpp
    src/processors/lineplot.cpp
    src/processors/okuboweiss.cpp
    src/processors/vortexselector.cpp
    src/processors/vortexsettovolumes.cpp
    src/processors/vortexstatistics.cpp
    src/processors/vortextomesh.cpp
    src/processors/windingangle.cpp
    src/vortexfeaturesmodule.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

#--------------------------------------------------------------------
# Add shaders
set(SHADER_FILES
    glsl/vortexraster.vert
    glsl/vortexraster.geom
    glsl/vortexraster.frag
    glsl/lineplotdot.vert
)
ivw_group("Shader Files" ${SHADER_FILES})

#--------------------------------------------------------------------
# Add Unittests
set(TEST_FILES
)
ivw_add_unittest(${TEST_FILES})

#--------------------------------------------------------------------
# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

#--------------------------------------------------------------------
# Add shader directory to pack
ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)
