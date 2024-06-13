ivw_module(Graphviz)

set(HEADER_FILES
    include/inviwo/graphviz/graphvizmodule.h
    include/inviwo/graphviz/graphvizmoduledefine.h
    include/inviwo/graphviz/graphvizsettings.h
    include/inviwo/graphviz/processors/layoutmergetree.h
)
ivw_group("Header Files" ${HEADER_FILES})

set(SOURCE_FILES
    src/graphvizmodule.cpp
    src/graphvizsettings.cpp
    src/processors/layoutmergetree.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

set(SHADER_FILES
	# Add shaders
)
ivw_group("Shader Files" ${SHADER_FILES})

set(TEST_FILES
    tests/unittests/graphviz-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

#find_package(PkgConfig REQUIRED)
#pkg_check_modules(Graphviz REQUIRED IMPORTED_TARGET libcdt libcgraph libgvc)
#target_link_libraries(inviwo-module-graphviz PUBLIC PkgConfig::Graphviz)



add_library(graphviz INTERFACE)
add_library(unofficial::graphviz::graphviz ALIAS graphviz)
foreach(item IN ITEMS cdt cgraph gvc gvplugin_dot_layout)
    find_library(Graphviz_${item}_release
        NAMES ${item}
        PATHS ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib
              ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/graphviz
        REQUIRED
        NO_DEFAULT_PATH
    )
    find_library(Graphviz_${item}_debug
        NAMES ${item}
        PATHS ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/lib
              ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/lib/graphviz
        REQUIRED
        NO_DEFAULT_PATH
    )
    target_link_libraries(graphviz INTERFACE $<IF:$<CONFIG:DEBUG>,${Graphviz_${item}_debug},${Graphviz_${item}_release}>)

    message("${item} rel: ${Graphviz_${item}_release} deb: ${Graphviz_${item}_debug}")
endforeach()

# ../graphviz/cgraph.h
find_file(Graphviz_include_path
    NAMES graphviz/cgraph.h
    PATHS ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include
    REQUIRED
    NO_DEFAULT_PATH
)
# get the parent/parent folder.
cmake_path(GET Graphviz_include_path PARENT_PATH Graphviz_include_path)
cmake_path(GET Graphviz_include_path PARENT_PATH Graphviz_include_path)

target_include_directories(graphviz INTERFACE
    $<BUILD_INTERFACE:${Graphviz_include_path}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(inviwo-module-graphviz PUBLIC unofficial::graphviz::graphviz)
