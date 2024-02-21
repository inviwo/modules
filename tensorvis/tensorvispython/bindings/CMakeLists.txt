#--------------------------------------------------------------------
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

ivw_add_py_wrapper(ivwtensorvis ${SOURCE_FILES} ${HEADER_FILES})

find_package(VTK CONFIG REQUIRED)

target_link_libraries(ivwtensorvis PUBLIC inviwo::module::tensorvisbase inviwo::module::python3)
target_include_directories(ivwtensorvis PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

ivw_default_install_targets(ivwtensorvis)
