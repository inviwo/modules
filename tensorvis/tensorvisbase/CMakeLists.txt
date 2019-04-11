#--------------------------------------------------------------------
# Inviwo TensorVisBase Module
ivw_module(TensorVisBase)

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
	include/modules/tensorvisbase/tensorvisbasemodule.h
	include/modules/tensorvisbase/tensorvisbasemoduledefine.h
    include/modules/tensorvisbase/datastructures/deformablecube.h
    include/modules/tensorvisbase/datastructures/deformablecylinder.h
    include/modules/tensorvisbase/datastructures/deformablesphere.h
    include/modules/tensorvisbase/datastructures/hyperstreamlinetracer.h
    include/modules/tensorvisbase/datastructures/tensorfield2d.h
    include/modules/tensorvisbase/datastructures/tensorfield3d.h
    include/modules/tensorvisbase/datastructures/tensorfieldmetadata.h
    include/modules/tensorvisbase/datastructures/tensorfieldmetadataimpl.h
    include/modules/tensorvisbase/datastructures/tensorfieldmetadataspecializations.h
    include/modules/tensorvisbase/datavisualizer/anisotropyraycastingvisualizer.h
    include/modules/tensorvisbase/datavisualizer/hyperlicvisualizer3d.h
    include/modules/tensorvisbase/ports/tensorfieldport.h
    include/modules/tensorvisbase/processors/eigenvaluefieldtoimage.h
    include/modules/tensorvisbase/processors/hyperstreamlines.h
    include/modules/tensorvisbase/processors/imagetospherefield.h
    include/modules/tensorvisbase/processors/mohrcircleprocessor.h
    include/modules/tensorvisbase/processors/tensorfield2danisotropy.h
    include/modules/tensorvisbase/processors/tensorfield2dsubsample.h
    include/modules/tensorvisbase/processors/tensorfield2dsubset.h
    include/modules/tensorvisbase/processors/tensorfield3danisotropy.h
    include/modules/tensorvisbase/processors/tensorfield3dbasismanipulation.h
    include/modules/tensorvisbase/processors/tensorfield3dboundingbox.h
    include/modules/tensorvisbase/processors/tensorfield3dmasktovolume.h
    include/modules/tensorvisbase/processors/tensorfield3dmetadata.h
    include/modules/tensorvisbase/processors/tensorfield3dsubsample.h
    include/modules/tensorvisbase/processors/tensorfield3dsubset.h
    include/modules/tensorvisbase/processors/tensorfieldgenerator.h
    include/modules/tensorvisbase/processors/tensorfieldlic.h
    include/modules/tensorvisbase/processors/tensorfieldslice.h
    include/modules/tensorvisbase/processors/tensorfieldtorgba.h
    include/modules/tensorvisbase/processors/tensorfieldtovolume.h
    include/modules/tensorvisbase/processors/tensorglyphprocessor.h
    include/modules/tensorvisbase/processors/tensorglyphrenderer.h
    include/modules/tensorvisbase/processors/tensorinformation.h
    include/modules/tensorvisbase/processors/tensorstarplotprocessor.h
    include/modules/tensorvisbase/processors/volumeactualdataandvaluerange.h
    include/modules/tensorvisbase/properties/eigenvalueproperty.h
    include/modules/tensorvisbase/properties/tensorglyphproperty.h
	include/modules/tensorvisbase/util/misc.h
	include/modules/tensorvisbase/util/tensorfieldutil.h
	include/modules/tensorvisbase/util/tensorutil.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
	src/tensorvisbasemodule.cpp
    src/datastructures/deformablecube.cpp
    src/datastructures/deformablecylinder.cpp
    src/datastructures/deformablesphere.cpp
    src/datastructures/hyperstreamlinetracer.cpp
    src/datastructures/tensorfield2d.cpp
    src/datastructures/tensorfield3d.cpp
	src/datavisualizer/anisotropyraycastingvisualizer.cpp
	src/datavisualizer/hyperlicvisualizer3d.cpp
    src/processors/eigenvaluefieldtoimage.cpp
    src/processors/hyperstreamlines.cpp
    src/processors/imagetospherefield.cpp
    src/processors/mohrcircleprocessor.cpp
    src/processors/tensorfield2danisotropy.cpp
    src/processors/tensorfield2dsubsample.cpp
    src/processors/tensorfield2dsubset.cpp
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
    src/processors/tensorstarplotprocessor.cpp
    src/processors/volumeactualdataandvaluerange.cpp
    src/properties/eigenvalueproperty.cpp
    src/properties/tensorglyphproperty.cpp
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

if (NOT OPENMP_FOUND)
	find_package(OpenMP QUIET)
	if (OPENMP_FOUND)
		MESSAGE(STATUS "OpenMP flag set in TensorVisBase module.")
		set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
		set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
	endif()
endif()


#--------------------------------------------------------------------
# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

#--------------------------------------------------------------------
# Package or build shaders into resources
ivw_handle_shader_resources(${CMAKE_CURRENT_SOURCE_DIR}/glsl ${SHADER_FILES})