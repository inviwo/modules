# Inviwo VASP Module
ivw_module(VASP)

# Add header files
set(HEADER_FILES
    include/inviwo/vasp/vaspmodule.h
    include/inviwo/vasp/vaspmoduledefine.h
    include/inviwo/vasp/processors/chgcarsource.h
)
ivw_group("Header Files" ${HEADER_FILES})

# Add source files
set(SOURCE_FILES
    src/vaspmodule.cpp
    src/processors/chgcarsource.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

# Add shaders
set(SHADER_FILES
)
ivw_group("Shader Files" ${SHADER_FILES})

set(PYTHON_FILES
    python/processors/chgcarsource.py
    python/vasputil.py
)
ivw_group("Python Files" ${PYTHON_FILES})

# Add Unittests
set(TEST_FILES
    tests/unittests/vasp-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

# Create module
ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/python)
ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/python/processors)
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES} ${PYTHON_FILES})

ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/python)

find_path(BXZSTR_INCLUDE_DIRS "bxzstr.hpp" PATH_SUFFIXES "include/bxzstr")
target_include_directories(inviwo-module-vasp PRIVATE ${BXZSTR_INCLUDE_DIRS})

find_package(BZip2 REQUIRED)
find_package(ZLIB REQUIRED)
find_package(liblzma CONFIG REQUIRED)
find_package(FastFloat CONFIG REQUIRED)
target_link_libraries(inviwo-module-vasp PUBLIC BZip2::BZip2 ZLIB::ZLIB liblzma::liblzma FastFloat::fast_float)


ivw_vcpkg_install(bzip2 MODULE VASP)
ivw_vcpkg_install(zlib MODULE VASP)
ivw_vcpkg_install(liblzma MODULE VASP)
ivw_vcpkg_install(fast-float MODULE VASP)

#  HACK: have the files showing in the IDE
if(NOT TARGET bxzstr_vcpkg)
    file(GLOB bxzstrHeaders "${BXZSTR_INCLUDE_DIRS}/*")
    add_custom_target(bxzstr_vcpkg SOURCES ${bxzstrHeaders})
    source_group(
        TREE "${BXZSTR_INCLUDE_DIRS}"
        PREFIX "Header Files"
        FILES ${bxzstrHeaders}
    )
    set_target_properties(bxzstr_vcpkg PROPERTIES FOLDER vcpkg)
endif()
