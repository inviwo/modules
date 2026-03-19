ivw_module(ortools)

set(HEADER_FILES
    include/inviwo/ortools/ortoolsmodule.h
    include/inviwo/ortools/ortoolsmoduledefine.h
    include/inviwo/ortools/processors/hungarian.h
)
ivw_group("Header Files" ${HEADER_FILES})

set(SOURCE_FILES
    src/ortoolsmodule.cpp
    src/processors/hungarian.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

set(SHADER_FILES
	# Add shaders
)
ivw_group("Shader Files" ${SHADER_FILES})

set(TEST_FILES
    tests/unittests/hungarian-tests.cpp
    tests/unittests/ortools-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

find_package(ortools CONFIG REQUIRED)
target_link_libraries(inviwo-module-ortools PRIVATE ortools::ortools)
target_link_libraries(inviwo-unittests-ortools PRIVATE ortools::ortools)

# Add shader directory to install package
#ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)
