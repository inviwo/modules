ivw_module(VTK)

set(HEADER_FILES
    include/inviwo/vtk/datastructures/volumevtk.h
    include/inviwo/vtk/datastructures/vtktetramesh.h
    include/inviwo/vtk/vtkmodule.h
    include/inviwo/vtk/vtkmoduledefine.h
    include/inviwo/vtk/ports/vtkinport.h
    include/inviwo/vtk/ports/vtkoutport.h
    include/inviwo/vtk/processors/imagetovtk.h
    include/inviwo/vtk/processors/layertovtk.h
    include/inviwo/vtk/processors/volumetovtk.h
    include/inviwo/vtk/processors/vtkdatasettovtkimagedata.h
    include/inviwo/vtk/processors/vtkgenericprocessor.h
    include/inviwo/vtk/processors/vtksource.h
    include/inviwo/vtk/processors/vtktodataframe.h
    include/inviwo/vtk/processors/vtktoimage.h
    include/inviwo/vtk/processors/vtktolayer.h
    include/inviwo/vtk/processors/vtktomesh.h
    include/inviwo/vtk/processors/vtktotetramesh.h
    include/inviwo/vtk/processors/vtktovolume.h
    include/inviwo/vtk/util/arrayutils.h
    include/inviwo/vtk/util/vtkbufferutils.h
)
ivw_group("Header Files" ${HEADER_FILES})

set(SOURCE_FILES
    src/datastructures/volumevtk.cpp
    src/datastructures/vtktetramesh.cpp
    src/vtkmodule.cpp
    src/ports/vtkinport.cpp
    src/ports/vtkoutport.cpp
    src/processors/imagetovtk.cpp
    src/processors/layertovtk.cpp
    src/processors/volumetovtk.cpp
    src/processors/vtkdatasettovtkimagedata.cpp
    src/processors/vtkgenericprocessor.cpp
    src/processors/vtksource.cpp
    src/processors/vtktodataframe.cpp
    src/processors/vtktoimage.cpp
    src/processors/vtktolayer.cpp
    src/processors/vtktomesh.cpp
    src/processors/vtktotetramesh.cpp
    src/processors/vtktovolume.cpp
    src/util/arrayutils.cpp
    src/util/vtkbufferutils.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

set(TEST_FILES
    tests/unittests/vtk-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

file(GLOB vtkWrapperHeaders RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} generated/*.h)
file(GLOB vtkWrapperSourceFiles RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} generated/*.cpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/generated 
    PREFIX 
        VTKWrappers 
    FILES 
        ${vtkWrapperHeaders} 
        ${vtkWrapperSourceFiles}
)

ivw_create_module(${SOURCE_FILES} ${HEADER_FILES})

target_sources(inviwo-module-vtk
    PUBLIC
    FILE_SET HEADERS
    TYPE HEADERS
    BASE_DIRS
        ${CMAKE_CURRENT_BINARY_DIR}/generated
        ${CMAKE_CURRENT_SOURCE_DIR}/generated
    FILES
        ${vtkWrapperHeaders}
    PRIVATE
        ${vtkWrapperSourceFiles}
)

find_package(freetype CONFIG REQUIRED)
add_library(Freetype::Freetype ALIAS freetype)

find_package(utf8cpp CONFIG REQUIRED)

find_package(VTK CONFIG REQUIRED)

target_link_libraries(inviwo-module-vtk 
    PRIVATE
        VTK::CommonExecutionModel
        VTK::DomainsChemistry
        VTK::FiltersAMR
        VTK::FiltersCore  
        VTK::FiltersExtraction
        VTK::FiltersFlowPaths
        VTK::FiltersGeneric
        VTK::FiltersGeometry
        VTK::FiltersHybrid
        VTK::FiltersHyperTree
        VTK::FiltersModeling
        VTK::FiltersParallel
        VTK::FiltersPoints
        VTK::FiltersStatistics
        VTK::FiltersTexture
        VTK::FiltersVerdict
        VTK::ImagingGeneral
        VTK::ImagingHybrid
        VTK::IOXML
)
