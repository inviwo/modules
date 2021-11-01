#--------------------------------------------------------------------
# Inviwo VTK Module
ivw_module(VTK)
find_package(VTK REQUIRED)

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
    include/inviwo/vtk/datastructures/vtkdataset.h
    include/inviwo/vtk/datavisualizer/vtktovolumevisualizer.h
    include/inviwo/vtk/ports/vtkdatasetport.h
	include/inviwo/vtk/util/vtkoutputlogger.h
    include/inviwo/vtk/processors/vtkdatasetinformation.h
    include/inviwo/vtk/processors/vtkreader.h
    include/inviwo/vtk/processors/vtktovolume.h
    include/inviwo/vtk/processors/vtkunstructuredgridtorectilineargrid.h
    include/inviwo/vtk/processors/vtkwriter.h
    include/inviwo/vtk/util/vtkutil.h
    include/inviwo/vtk/vtkmodule.h
    include/inviwo/vtk/vtkmoduledefine.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    src/datastructures/vtkdataset.cpp
    src/datavisualizer/vtktovolumevisualizer.cpp
    src/ports/vtkdatasetport.cpp
    src/vtkmodule.cpp
    src/util/vtkoutputlogger.cpp
    src/processors/vtkdatasetinformation.cpp
    src/processors/vtkreader.cpp
    src/processors/vtktovolume.cpp
    src/processors/vtkunstructuredgridtorectilineargrid.cpp
    src/processors/vtkwriter.cpp
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
