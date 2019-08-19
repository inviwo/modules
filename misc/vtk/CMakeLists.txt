#--------------------------------------------------------------------
# Inviwo VTK Module
ivw_module(VTK)
find_package(VTK REQUIRED)

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/include/inviwo/vtk/util/lambda2func.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/inviwo/vtk/util/vtkoutputlogger.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vtkmodule.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/util/vtkoutputlogger.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

#--------------------------------------------------------------------
# Add Unittests
#set(TEST_FILES
#	${CMAKE_CURRENT_SOURCE_DIR}/tests/vtk-test.cpp
#)k
#ivw_add_unittest(${TEST_FILES})

#--------------------------------------------------------------------
# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES})

if (VTK_VERSION VERSION_LESS "8.90.0")
    include(${VTK_USE_FILE})
    target_link_libraries(${${mod}_target} PUBLIC ${VTK_LIBRARIES})
    target_include_directories(${${mod}_target} PUBLIC ${VTK_INCLUDE_DIRS})
else ()
    target_link_libraries(${${mod}_target} PUBLIC ${VTK_LIBRARIES})
    vtk_module_autoinit(TARGETS ${${mod}_target} MODULES ${VTK_LIBRARIES})
endif() 
