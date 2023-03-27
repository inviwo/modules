# Inviwo MolVisPython Module
ivw_module(MolVisPython)

# Add header files
set(HEADER_FILES
    include/inviwo/molvispython/molvispythonmodule.h
    include/inviwo/molvispython/molvispythonmoduledefine.h
)
ivw_group("Header Files" ${HEADER_FILES})

# Add source files
set(SOURCE_FILES
    src/molvispythonmodule.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

set(PYTHON_FILES
    python/processors/MolecularStructureSource.py
    python/molviscommon.py
    python/xpdb.py
)
ivw_group("Python Files" ${PYTHON_FILES})

set(TEST_FILES
    tests/unittests/molvispython-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${PYTHON_FILES})
find_package(pybind11 CONFIG REQUIRED)
target_link_libraries(inviwo-module-molvispython PUBLIC 
    $<BUILD_INTERFACE:${PYTHON_LIBRARIES}>
)
target_compile_definitions(inviwo-module-molvispython PUBLIC PYBIND11_USE_SMART_HOLDER_AS_DEFAULT)

add_subdirectory(bindings)

if(IVW_UNITTESTS)
    add_dependencies(inviwo-unittests-molvispython ivwmolvis)
endif()
