#--------------------------------------------------------------------
# Inviwo TensorVisBase Module
ivw_module(TensorVisBase)

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
    include/inviwo/tensorvisbase/algorithm/tensorfieldslicing.h
    include/inviwo/tensorvisbase/algorithm/tensorfieldsampling.h
    include/inviwo/tensorvisbase/datastructures/deformablecube.h
    include/inviwo/tensorvisbase/datastructures/deformablecylinder.h
    include/inviwo/tensorvisbase/datastructures/deformablesphere.h
    include/inviwo/tensorvisbase/datastructures/hyperstreamlinetracer.h
    include/inviwo/tensorvisbase/datastructures/invariantspace.h
    include/inviwo/tensorvisbase/datastructures/tensorfield2d.h
    include/inviwo/tensorvisbase/datastructures/tensorfield3d.h
    include/inviwo/tensorvisbase/datastructures/tensorfieldmetadata.h
    include/inviwo/tensorvisbase/datastructures/tensorfieldmetadataspecializations.h
    include/inviwo/tensorvisbase/datavisualizer/anisotropyraycastingvisualizer.h
    include/inviwo/tensorvisbase/datavisualizer/hyperlicvisualizer2d.h
    include/inviwo/tensorvisbase/datavisualizer/hyperlicvisualizer3d.h
    include/inviwo/tensorvisbase/ports/tensorfieldport.h
    include/inviwo/tensorvisbase/processors/eigenvaluefieldtoimage.h
    include/inviwo/tensorvisbase/processors/hyperstreamlines.h
    include/inviwo/tensorvisbase/processors/imagetospherefield.h
    include/inviwo/tensorvisbase/processors/invariantspacecombine.h
    include/inviwo/tensorvisbase/processors/invariantspacefilter.h
    include/inviwo/tensorvisbase/processors/invariantspaceselection.h
    include/inviwo/tensorvisbase/processors/invariantspacetodataframe.h
    include/inviwo/tensorvisbase/processors/tensorfield2danisotropy.h
    include/inviwo/tensorvisbase/processors/tensorfield2dsubsample.h
    include/inviwo/tensorvisbase/processors/tensorfield2dsubset.h
    include/inviwo/tensorvisbase/processors/tensorfield3dangle.h
    include/inviwo/tensorvisbase/processors/tensorfield3danisotropy.h
    include/inviwo/tensorvisbase/processors/tensorfield3dbasismanipulation.h
    include/inviwo/tensorvisbase/processors/tensorfield3dboundingbox.h
    include/inviwo/tensorvisbase/processors/tensorfield3dmasktovolume.h
    include/inviwo/tensorvisbase/processors/tensorfield3dmetadata.h
    include/inviwo/tensorvisbase/processors/tensorfield3dsubsample.h
    include/inviwo/tensorvisbase/processors/tensorfield3dsubset.h
    include/inviwo/tensorvisbase/processors/tensorfieldgenerator.h
    include/inviwo/tensorvisbase/processors/tensorfieldlic.h
    include/inviwo/tensorvisbase/processors/tensorfieldslice.h
    include/inviwo/tensorvisbase/processors/tensorfieldtorgba.h
    include/inviwo/tensorvisbase/processors/tensorfieldtovolume.h
    include/inviwo/tensorvisbase/processors/tensorglyphprocessor.h
    include/inviwo/tensorvisbase/processors/tensorglyphrenderer.h
    include/inviwo/tensorvisbase/processors/tensorinformation.h
    include/inviwo/tensorvisbase/processors/volumeactualdataandvaluerange.h
    include/inviwo/tensorvisbase/properties/eigenvalueproperty.h
    include/inviwo/tensorvisbase/properties/tensorglyphproperty.h
    include/inviwo/tensorvisbase/tensorvisbasemodule.h
    include/inviwo/tensorvisbase/tensorvisbasemoduledefine.h
    include/inviwo/tensorvisbase/util/distancemetrics.h
    include/inviwo/tensorvisbase/util/misc.h
    include/inviwo/tensorvisbase/util/tensorfieldutil.h
    include/inviwo/tensorvisbase/util/tensorutil.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    src/algorithm/tensorfieldslicing.cpp
    src/algorithm/tensorfieldsampling.cpp
    src/datastructures/deformablecube.cpp
    src/datastructures/deformablecylinder.cpp
    src/datastructures/deformablesphere.cpp
    src/datastructures/hyperstreamlinetracer.cpp
    src/datastructures/invariantspace.cpp
    src/datastructures/tensorfield2d.cpp
    src/datastructures/tensorfield3d.cpp
    src/datavisualizer/anisotropyraycastingvisualizer.cpp
    src/datavisualizer/hyperlicvisualizer2d.cpp
    src/datavisualizer/hyperlicvisualizer3d.cpp
    src/processors/eigenvaluefieldtoimage.cpp
    src/processors/hyperstreamlines.cpp
    src/processors/imagetospherefield.cpp
    src/processors/invariantspacecombine.cpp
    src/processors/invariantspacefilter.cpp
    src/processors/invariantspaceselection.cpp
    src/processors/invariantspacetodataframe.cpp
    src/processors/tensorfield2danisotropy.cpp
    src/processors/tensorfield2dsubsample.cpp
    src/processors/tensorfield2dsubset.cpp
    src/processors/tensorfield3dangle.cpp
    src/processors/tensorfield3danisotropy.cpp
    src/processors/tensorfield3dbasismanipulation.cpp
    src/processors/tensorfield3dboundingbox.cpp
    src/processors/tensorfield3dmasktovolume.cpp
    src/processors/tensorfield3dmetadata.cpp
    src/processors/tensorfield3dsubsample.cpp
    src/processors/tensorfield3dsubset.cpp
    src/processors/tensorfieldgenerator.cpp
    src/processors/tensorfieldlic.cpp
    src/processors/tensorfieldslice.cpp
    src/processors/tensorfieldtorgba.cpp
    src/processors/tensorfieldtovolume.cpp
    src/processors/tensorglyphprocessor.cpp
    src/processors/tensorglyphrenderer.cpp
    src/processors/tensorinformation.cpp
    src/processors/volumeactualdataandvaluerange.cpp
    src/properties/eigenvalueproperty.cpp
    src/properties/tensorglyphproperty.cpp
    src/tensorvisbasemodule.cpp
    src/util/tensorfieldutil.cpp
    src/util/tensorutil.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})


#--------------------------------------------------------------------
# Add shaders
set(SHADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/eigen.glsl
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/eigenvaluefield.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/magnitudefield.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/quadrender.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/quadrender.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/tensorfieldtorgba.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/tensorglyphpicking.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/tensorlic2d.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/tensorutil.glsl
)
ivw_group("Shader Files" ${SHADER_FILES})


#--------------------------------------------------------------------
# Add Unittests
set(TEST_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/unittests/tensorvisbase-unittest-main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/unittests/arithmic-operations.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/unittests/de_normalization.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/unittests/distance-measures.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/unittests/set-operations.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/unittests/to-string.cpp
)
ivw_add_unittest(${TEST_FILES})

#--------------------------------------------------------------------
# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

#--------------------------------------------------------------------
# Package or build shaders into resources
ivw_handle_shader_resources(${CMAKE_CURRENT_SOURCE_DIR}/glsl ${SHADER_FILES})
