ivw_module(WebBrowserExamples)

set(HEADER_FILES
    include/inviwo/webbrowserexamples/processors/heatmapd3.h
    include/inviwo/webbrowserexamples/processors/minimalwebbrowser.h
    include/inviwo/webbrowserexamples/webbrowserexamplesmodule.h
    include/inviwo/webbrowserexamples/webbrowserexamplesmoduledefine.h
)
ivw_group("Header Files" ${HEADER_FILES})

set(SOURCE_FILES
    src/processors/heatmapd3.cpp
    src/processors/minimalwebbrowser.cpp
    src/webbrowserexamplesmodule.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

set(SHADER_FILES
	# Add shaders
)
ivw_group("Shader Files" ${SHADER_FILES})

set(TEST_FILES
    tests/unittests/webbrowserexamples-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

# Add shader directory to install package
#ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)
