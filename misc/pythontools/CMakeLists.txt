ivw_module(PythonTools)

set(HEADER_FILES
    include/inviwo/pythontools/pythontoolsmodule.h
    include/inviwo/pythontools/pythontoolsmoduledefine.h
)
ivw_group("Header Files" ${HEADER_FILES})

set(SOURCE_FILES
    src/pythontoolsmodule.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

set(PYTHON_FILES
    processors/CameraTrajectory.py
    processors/FileGlob.py
    processors/FishEyeHelper.py
    processors/ImageRecorder.py
    processors/VolumeExtractChannel.py
)
ivw_group("Python Files" ${PYTHON_FILES})

set(SHADER_FILES
    # Add shaders
)
ivw_group("Shader Files" ${SHADER_FILES})

set(TEST_FILES
    tests/unittests/pythontools-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES} ${PYTHON_FILES})

ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/processors)
