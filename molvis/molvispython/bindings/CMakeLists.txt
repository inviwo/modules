#--------------------------------------------------------------------
# Create python module
set(HEADER_FILES
    include/ivwmolvis/ivwmolvis.h
    include/ivwmolvis/pymolvis.h
)
ivw_group("Header Files" BASE ${CMAKE_CURRENT_SOURCE_DIR}/include/ivwmolvis ${HEADER_FILES})

set(SOURCE_FILES
    src/ivwmolvis.cpp
    src/pymolvis.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

ivw_add_py_wrapper(ivwmolvis ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(ivwmolvis PUBLIC inviwo::module::molvisbase inviwo::module::python3)
target_include_directories(ivwmolvis PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

ivw_default_install_comp_targets(python ivwmolvis)
