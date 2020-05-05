#--------------------------------------------------------------------
# Inviwo MolVisPython Module
ivw_module(MolVisPython)

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
    include/inviwo/molvispython/molvispythonmodule.h
    include/inviwo/molvispython/molvispythonmoduledefine.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    src/molvispythonmodule.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

#--------------------------------------------------------------------
# Add Unittests
set(TEST_FILES
    tests/unittests/molvispython-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

#--------------------------------------------------------------------
# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(inviwo-module-molvispython PUBLIC 
    pybind11::embed 
    $<BUILD_INTERFACE:${PYTHON_LIBRARIES}>
)

add_subdirectory(bindings)

if(IVW_UNITTESTS)
    add_dependencies(inviwo-unittests-molvispython ivwmolvis)
endif()
