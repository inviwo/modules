#--------------------------------------------------------------------
# Inviwo TensorVisIO Module
ivw_module(TensorVisIO)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
    include/modules/tensorvisio/datastructures/vtkdataset.h
    include/modules/tensorvisio/ports/vtkdatasetport.h
    include/modules/tensorvisio/processors/amiratensorreader.h
    include/modules/tensorvisio/processors/nrrdreader.h
    include/modules/tensorvisio/processors/tensorfield2dexport.h
    include/modules/tensorvisio/processors/tensorfield2dimport.h
    include/modules/tensorvisio/processors/tensorfield3dexport.h
    include/modules/tensorvisio/processors/tensorfield3dimport.h
    include/modules/tensorvisio/processors/vtkdatasettotensorfield3d.h
    include/modules/tensorvisio/processors/vtkdatasetinformation.h
    include/modules/tensorvisio/processors/vtkreader.h
    include/modules/tensorvisio/processors/vtkunstructuredgridtorectilineargrid.h
    include/modules/tensorvisio/processors/vtkwriter.h
    include/modules/tensorvisio/processors/flowguifilereader.h
    include/modules/tensorvisio/tensorvisiomodule.h
    include/modules/tensorvisio/tensorvisiomoduledefine.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    src/datastructures/vtkdataset.cpp
    src/ports/vtkdatasetport.cpp
    src/processors/amiratensorreader.cpp
    src/processors/nrrdreader.cpp
    src/processors/tensorfield2dexport.cpp
    src/processors/tensorfield2dimport.cpp
    src/processors/tensorfield3dexport.cpp
    src/processors/tensorfield3dimport.cpp
    src/processors/vtkdatasettotensorfield3d.cpp
    src/processors/vtkdatasetinformation.cpp
    src/processors/vtkreader.cpp
    src/processors/vtkunstructuredgridtorectilineargrid.cpp
    src/processors/vtkwriter.cpp
    src/processors/flowguifilereader.cpp
    src/tensorvisiomodule.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})


#--------------------------------------------------------------------
# Add shaders
set(SHADER_FILES
    #${CMAKE_CURRENT_SOURCE_DIR}/glsl/tensorfieldioprocessor.frag
)
ivw_group("Shader Files" ${SHADER_FILES})


#--------------------------------------------------------------------
# Add Unittests
set(TEST_FILES
	#${CMAKE_CURRENT_SOURCE_DIR}/tests/unittests/tensorvisio-test.cpp
)
ivw_add_unittest(${TEST_FILES})

#--------------------------------------------------------------------
# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

if(WIN32)
    SET(SUFFIX ".lib")
else()
    SET(SUFFIX ".so")
endif()

SET(PREFIX "")

if(NOT WIN32)
    SET(PREFIX "lib")
endif()

target_link_libraries(${${mod}_target} PUBLIC ${VTK_DIR}/../../${PREFIX}vtkCommonExecutionModel-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}${SUFFIX})
target_link_libraries(${${mod}_target} PUBLIC ${VTK_DIR}/../../${PREFIX}vtkCommonDataModel-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}${SUFFIX})
target_link_libraries(${${mod}_target} PUBLIC ${VTK_DIR}/../../${PREFIX}vtkCommonMisc-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}${SUFFIX})
target_link_libraries(${${mod}_target} PUBLIC ${VTK_DIR}/../../${PREFIX}vtkCommonSystem-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}${SUFFIX})
target_link_libraries(${${mod}_target} PUBLIC ${VTK_DIR}/../../${PREFIX}vtkCommonTransforms-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}${SUFFIX})
target_link_libraries(${${mod}_target} PUBLIC ${VTK_DIR}/../../${PREFIX}vtkCommonMath-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}${SUFFIX})
target_link_libraries(${${mod}_target} PUBLIC ${VTK_DIR}/../../${PREFIX}vtkCommonCore-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}${SUFFIX})
target_link_libraries(${${mod}_target} PUBLIC ${VTK_DIR}/../../${PREFIX}vtkIOEnSight-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}${SUFFIX})
target_link_libraries(${${mod}_target} PUBLIC ${VTK_DIR}/../../${PREFIX}vtkIOLegacy-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}${SUFFIX})
target_link_libraries(${${mod}_target} PUBLIC ${VTK_DIR}/../../${PREFIX}vtksys-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}${SUFFIX})
target_link_libraries(${${mod}_target} PUBLIC ${VTK_DIR}/../../${PREFIX}vtkFiltersCore-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}${SUFFIX})
target_link_libraries(${${mod}_target} PUBLIC ${VTK_DIR}/../../${PREFIX}vtkIOXML-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}${SUFFIX})
target_link_libraries(${${mod}_target} PUBLIC ${VTK_DIR}/../../${PREFIX}vtkFiltersGeneral-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}${SUFFIX})
target_link_libraries(${${mod}_target} PUBLIC ${VTK_DIR}/../../${PREFIX}vtkverdict-${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}${SUFFIX})

#--------------------------------------------------------------------
# Add shader directory to pack
# ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)
