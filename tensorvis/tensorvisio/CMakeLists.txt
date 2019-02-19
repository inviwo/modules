#--------------------------------------------------------------------
# Inviwo TensorVisIO Module
ivw_module(TensorVisIO)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
    include/modules/tensorvisio/tensorvisiomodule.h
    include/modules/tensorvisio/tensorvisiomoduledefine.h
    include/modules/tensorvisio/processors/vtkstructuredgridreader.h
    include/modules/tensorvisio/processors/vtkunstructuredgridtorectilineargrid.h
    include/modules/tensorvisio/processors/witofilereader.h
    include/modules/tensorvisio/processors/tensorfield2dexport.h
    include/modules/tensorvisio/processors/tensorfield2dimport.h
    include/modules/tensorvisio/processors/nrrdreader.h
    include/modules/tensorvisio/processors/vtktotensorfield3d.h
    include/modules/tensorvisio/processors/tensorfield3dexport.h
    include/modules/tensorvisio/processors/tensorfield3dimport.h
    include/modules/tensorvisio/processors/amiratensorreader.h
    include/modules/tensorvisio/processors/vtkrectilineargridreader.h
    include/modules/tensorvisio/processors/vtkxmlrectilineargridreader.h
    include/modules/tensorvisio/processors/vtkstructuredpointsreader.h
    include/modules/tensorvisio/processors/vtkvolumereader.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    src/tensorvisiomodule.cpp
    src/processors/vtkstructuredgridreader.cpp
    src/processors/vtkunstructuredgridtorectilineargrid.cpp
    src/processors/witofilereader.cpp
    src/processors/amiratensorreader.cpp
    src/processors/nrrdreader.cpp
    src/processors/tensorfield2dexport.cpp
    src/processors/tensorfield2dimport.cpp
    src/processors/tensorfield3dexport.cpp
    src/processors/tensorfield3dimport.cpp
    src/processors/vtktotensorfield3d.cpp
    src/processors/vtkrectilineargridreader.cpp
    src/processors/vtkxmlrectilineargridreader.cpp
    src/processors/vtkstructuredpointsreader.cpp
    src/processors/vtkvolumereader.cpp
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