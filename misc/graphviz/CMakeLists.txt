ivw_module(Graphviz)

set(HEADER_FILES
    include/inviwo/graphviz/graphvizmodule.h
    include/inviwo/graphviz/graphvizmoduledefine.h
    include/inviwo/graphviz/processors/layoutmergetree.h
)
ivw_group("Header Files" ${HEADER_FILES})

set(SOURCE_FILES
    src/graphvizmodule.cpp
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

find_package(PkgConfig REQUIRED)
pkg_check_modules(Graphviz REQUIRED IMPORTED_TARGET libcdt libcgraph libgvc)

target_link_libraries(inviwo-module-graphviz PUBLIC PkgConfig::Graphviz)

message("graphviz:  ${Graphviz_LINK_LIBRARIES}, ${Graphviz_INCLUDE_DIRS}, ${Graphviz_CFLAGS_OTHER}")

