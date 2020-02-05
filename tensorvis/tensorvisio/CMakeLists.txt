#--------------------------------------------------------------------
# Inviwo TensorVisIO Module
ivw_module(TensorVisIO)

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
    include/inviwo/tensorvisio/processors/amiratensorreader.h
    include/inviwo/tensorvisio/processors/flowguifilereader.h
    include/inviwo/tensorvisio/processors/nrrdreader.h
    include/inviwo/tensorvisio/processors/tensorfield2dexport.h
    include/inviwo/tensorvisio/processors/tensorfield2dimport.h
    include/inviwo/tensorvisio/processors/tensorfield2dtovtk.h
    include/inviwo/tensorvisio/processors/tensorfield3dexport.h
    include/inviwo/tensorvisio/processors/tensorfield3dimport.h
    include/inviwo/tensorvisio/processors/vtkdatasettotensorfield3d.h
    include/inviwo/tensorvisio/processors/vtktotensorfield2d.h
    include/inviwo/tensorvisio/tensorvisiomodule.h
    include/inviwo/tensorvisio/tensorvisiomoduledefine.h
    include/inviwo/tensorvisio/util/util.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    src/processors/amiratensorreader.cpp
    src/processors/flowguifilereader.cpp
    src/processors/nrrdreader.cpp
    src/processors/tensorfield2dexport.cpp
    src/processors/tensorfield2dimport.cpp
    src/processors/tensorfield2dtovtk.cpp
    src/processors/tensorfield3dexport.cpp
    src/processors/tensorfield3dimport.cpp
    src/processors/vtkdatasettotensorfield3d.cpp
    src/processors/vtktotensorfield2d.cpp
    src/tensorvisiomodule.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

#--------------------------------------------------------------------
# Add shaders
set(SHADER_FILES
    #${CMAKE_CURRENT_SOURCE_DIR}/glsl/tensorfieldioprocessor.frag
)
ivw_group("Shader Files" ${SHADER_FILES})

#--------------------------------------------------------------------
# Add Unittests
set(TEST_FILES
	#${CMAKE_CURRENT_SOURCE_DIR}/tests/unittests/tensorvisio-test.cpp
)
ivw_add_unittest(${TEST_FILES})

#--------------------------------------------------------------------
# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

#--------------------------------------------------------------------
# Add shader directory to pack
# ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)
