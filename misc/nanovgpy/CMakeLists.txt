#--------------------------------------------------------------------
# Inviwo NanoVGPy Module
ivw_module(NanoVGPy)

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
    include/inviwo/nanovgpy/interface/nanovgpyinterface.h
    include/inviwo/nanovgpy/nanovgpymodule.h
    include/inviwo/nanovgpy/nanovgpymoduledefine.h
    include/inviwo/nanovgpy/processors/nanovgpyprocessor.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    src/interface/nanovgpyinterface.cpp
    src/nanovgpymodule.cpp
    src/processors/nanovgpyprocessor.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})


#--------------------------------------------------------------------
# Add shaders
set(SHADER_FILES
    #${CMAKE_CURRENT_SOURCE_DIR}/glsl/nanovgpyprocessor.frag
)
ivw_group("Shader Files" ${SHADER_FILES})


#--------------------------------------------------------------------
# Add Unittests
set(TEST_FILES
	#${CMAKE_CURRENT_SOURCE_DIR}/tests/unittests/nanovgpy-test.cpp
)
ivw_add_unittest(${TEST_FILES})

#--------------------------------------------------------------------
# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

#--------------------------------------------------------------------
# Add shader directory to pack
# ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)
