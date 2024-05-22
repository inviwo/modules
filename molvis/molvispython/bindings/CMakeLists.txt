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

ivw_add_py_wrapper(ivwmolvis)
target_sources(ivwmolvis
    PUBLIC
    FILE_SET HEADERS
    TYPE HEADERS
    BASE_DIRS include
    FILES
        ${HEADER_FILES}
    PRIVATE
        ${SOURCE_FILES}
)
target_link_libraries(ivwmolvis PUBLIC inviwo::module::molvisbase inviwo::module::python3)

ivw_install_helper(TARGET ivwmolvis
    NAMESPACE inviwo
    DESTINATION inviwo
)
