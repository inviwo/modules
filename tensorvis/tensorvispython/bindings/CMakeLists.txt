# Create python bindings for tensorvis module
find_package(pybind11 CONFIG REQUIRED)

# Create python module
set(HEADER_FILES
    include/ivwtensorvis/ivwtensorvis.h
    include/ivwtensorvis/pytensorvis.h
)
ivw_group("Header Files" BASE ${CMAKE_CURRENT_SOURCE_DIR}/include/ivwtensorvis ${HEADER_FILES})

set(SOURCE_FILES
    src/ivwtensorvis.cpp
    src/pytensorvis.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

ivw_add_py_wrapper(ivwtensorvis)
target_sources(ivwtensorvis
    PUBLIC
    FILE_SET HEADERS
    TYPE HEADERS
    BASE_DIRS include
    FILES
        ${HEADER_FILES}
    PRIVATE
        ${SOURCE_FILES}
)

target_link_libraries(ivwtensorvis PUBLIC inviwo::module::tensorvisbase inviwo::module::python3)

ivw_install_helper(TARGET ivwtensorvis
    NAMESPACE inviwo
    DESTINATION inviwo
)
