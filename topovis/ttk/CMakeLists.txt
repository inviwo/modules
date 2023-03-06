ivw_module(ttk)

set(HEADER_FILES
    include/inviwo/ttk/arrayutils.h
    include/inviwo/ttk/datastructures/volumevtk.h
    include/inviwo/ttk/ports/vtkinport.h
    include/inviwo/ttk/ports/vtkoutport.h
    include/inviwo/ttk/processors/mergetreetonodes.h
    include/inviwo/ttk/processors/ttkgenericprocessor.h
    include/inviwo/ttk/processors/volumetovtk.h
    include/inviwo/ttk/processors/vtkdatasettovtkimagedata.h
    include/inviwo/ttk/processors/vtktodataframe.h
    include/inviwo/ttk/processors/vtktomesh.h
    include/inviwo/ttk/processors/vtktovolume.h
    include/inviwo/ttk/ttkmodule.h
    include/inviwo/ttk/ttkmoduledefine.h
)
ivw_group("Header Files" ${HEADER_FILES})

set(SOURCE_FILES
    src/arrayutils.cpp
    src/datastructures/volumevtk.cpp
    src/ports/vtkinport.cpp
    src/ports/vtkoutport.cpp
    src/processors/mergetreetonodes.cpp
    src/processors/ttkgenericprocessor.cpp
    src/processors/volumetovtk.cpp
    src/processors/vtkdatasettovtkimagedata.cpp
    src/processors/vtktodataframe.cpp
    src/processors/vtktomesh.cpp
    src/processors/vtktovolume.cpp
    src/ttkmodule.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

set(TEST_FILES
    tests/unittests/ttk-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

file(GLOB ttkWrappers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} generated/*)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/generated PREFIX TTKWrappers FILES ${ttkWrappers})

ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES} ${ttkWrappers})

target_include_directories(inviwo-module-ttk PRIVATE generated)

find_package(freetype CONFIG REQUIRED)
add_library(Freetype::Freetype ALIAS freetype)


# potential vtk workarounds for issues in <build>/vcpkg_installed/x64-windows/share/vtk/FindPEGTL.cmake
#add_library(PEGTL::PEGTL ALIAS pegtl)
#find_package(PEGTL CONFIG REQUIRED)

find_package(TTKBase CONFIG REQUIRED)
find_package(TTKVTK CONFIG REQUIRED)

target_link_libraries(inviwo-module-ttk PRIVATE ttk::base::baseAll ttk::vtk::ttkAll)
