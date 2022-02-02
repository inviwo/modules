ivw_module(ttk)

set(HEADER_FILES
    include/inviwo/ttk/processors/ttkgenericprocessor.h
    include/inviwo/ttk/ttkmodule.h
    include/inviwo/ttk/ttkmoduledefine.h
)
ivw_group("Header Files" ${HEADER_FILES})

set(SOURCE_FILES
    src/processors/ttkgenericprocessor.cpp
    src/ttkmodule.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

set(SHADER_FILES
	# Add shaders
)
ivw_group("Shader Files" ${SHADER_FILES})

set(TEST_FILES
    tests/unittests/ttk-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

file(GLOB ttkWrappers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} generated/*)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/generated PREFIX TTKWrappers FILES ${ttkWrappers})

ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES} ${ttkWrappers})

target_include_directories(inviwo-module-ttk PRIVATE generated)

list(APPEND CMAKE_PREFIX_PATH "C:/Users/petst55.AD/Documents/Inviwo/builds/msvc-dev-vcpkg/vcpkg_installed/x64-windows/share/topology-tool-kit")

find_package(freetype CONFIG REQUIRED)
add_library(Freetype::Freetype ALIAS freetype)

find_package(TTKVTK CONFIG REQUIRED)
target_link_libraries(inviwo-module-ttk PRIVATE ttk::vtk::ttkAll)

# Add shader directory to install package
#ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)

#   find_package(TTKBase CONFIG REQUIRED)
    # Note: 65 target(s) were omitted.
#    target_link_libraries(main PRIVATE ttk::base::common ttk::base::kdTree ttk::base::auction ttk::base::baseAll)

#    find_package(TTKVTK CONFIG REQUIRED)
    # Note: 86 target(s) were omitted.
#    target_link_libraries(main PRIVATE ttk::vtk::ttkAll ttk::vtk::ttkFiber ttk::vtk::ttkEndFor ttk::vtk::ttkExtract)
