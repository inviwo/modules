ivw_module(ttk)

set(HEADER_FILES
    include/inviwo/ttk/arrayutils.h
    include/inviwo/ttk/datastructures/volumevtk.h
    include/inviwo/ttk/datastructures/vtktetramesh.h
    include/inviwo/ttk/ports/vtkinport.h
    include/inviwo/ttk/ports/vtkoutport.h
    include/inviwo/ttk/processors/imagetovtk.h
    include/inviwo/ttk/processors/layertovtk.h
    include/inviwo/ttk/processors/ttkgenericprocessor.h
    include/inviwo/ttk/processors/volumetovtk.h
    include/inviwo/ttk/processors/vtkdatasettovtkimagedata.h
    include/inviwo/ttk/processors/vtksource.h
    include/inviwo/ttk/processors/vtktodataframe.h
    include/inviwo/ttk/processors/vtktoimage.h
    include/inviwo/ttk/processors/vtktolayer.h
    include/inviwo/ttk/processors/vtktomesh.h
    include/inviwo/ttk/processors/vtktotetramesh.h
    include/inviwo/ttk/processors/vtktovolume.h
    include/inviwo/ttk/ttkmodule.h
    include/inviwo/ttk/ttkmoduledefine.h
    include/inviwo/ttk/util/vtkbufferutils.h
)
ivw_group("Header Files" ${HEADER_FILES})

set(SOURCE_FILES
    src/arrayutils.cpp
    src/datastructures/volumevtk.cpp
    src/datastructures/vtktetramesh.cpp
    src/ports/vtkinport.cpp
    src/ports/vtkoutport.cpp
    src/processors/imagetovtk.cpp
    src/processors/layertovtk.cpp
    src/processors/ttkgenericprocessor.cpp
    src/processors/volumetovtk.cpp
    src/processors/vtkdatasettovtkimagedata.cpp
    src/processors/vtksource.cpp
    src/processors/vtktodataframe.cpp
    src/processors/vtktoimage.cpp
    src/processors/vtktolayer.cpp
    src/processors/vtktomesh.cpp
    src/processors/vtktotetramesh.cpp
    src/processors/vtktovolume.cpp
    src/ttkmodule.cpp
    src/util/vtkbufferutils.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

set(TEST_FILES
    tests/unittests/ttk-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

file(GLOB ttkWrapperHeaders RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} generated/*.h)
file(GLOB ttkWrapperSourceFiles RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} generated/*.cpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/generated 
    PREFIX 
        TTKWrappers 
    FILES 
        ${ttkWrapperHeaders} 
        ${ttkWrapperSourceFiles}
)

ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

target_sources(inviwo-module-ttk
    PUBLIC
    FILE_SET HEADERS
    TYPE HEADERS
    BASE_DIRS
        ${CMAKE_CURRENT_BINARY_DIR}/generated
        ${CMAKE_CURRENT_SOURCE_DIR}/generated
    FILES
        ${ttkWrapperHeaders}
    PRIVATE
        ${ttkWrapperSourceFiles}
)

target_include_directories(inviwo-module-ttk PRIVATE generated)

find_package(freetype CONFIG REQUIRED)
add_library(Freetype::Freetype ALIAS freetype)

find_package(utf8cpp CONFIG REQUIRED)

find_package(TTKBase CONFIG REQUIRED)
find_package(TTKVTK CONFIG REQUIRED)

find_package(VTK CONFIG REQUIRED)

target_link_libraries(inviwo-module-ttk 
    PRIVATE 
        ttk::base::baseAll
        ttk::vtk::ttkAll


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
)
