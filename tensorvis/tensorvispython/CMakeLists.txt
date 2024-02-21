ivw_module(TensorVisPython)

set(HEADER_FILES
    include/inviwo/tensorvispython/tensorvispythonmodule.h
    include/inviwo/tensorvispython/tensorvispythonmoduledefine.h
)
ivw_group("Header Files" ${HEADER_FILES})

set(SOURCE_FILES
    src/tensorvispythonmodule.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

set(PYTHON_FILES
    python/processors/TensorField3DExample.py
    python/processors/TensorField3DSource.py
)
ivw_group("Python Files" ${PYTHON_FILES})

set(TEST_FILES
    tests/unittests/tensorvispython-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${PYTHON_FILES})

target_link_libraries(inviwo-module-tensorvispython
    PUBLIC 
        inviwo::module::python3
)

add_subdirectory(bindings)

if(IVW_UNITTESTS)
    add_dependencies(inviwo-unittests-tensorvispython ivwtensorvis)
endif()

ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/python)
