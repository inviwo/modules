#--------------------------------------------------------------------
# Inviwo OpenVDB Module
ivw_module(OpenVDB)

#--------------------------------------------------------------------
# Add OpenVDB add set correct folders
add_subdirectory(ext/openvdb)


#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
    include/inviwo/openvdb/openvdbmodule.h
    include/inviwo/openvdb/openvdbmoduledefine.h
    include/inviwo/openvdb/openvdbreader.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    src/openvdbmodule.cpp
    src/openvdbreader.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})


#--------------------------------------------------------------------
# Add shaders
set(SHADER_FILES
)
ivw_group("Shader Files" ${SHADER_FILES})


#--------------------------------------------------------------------
# Add Unittests
set(TEST_FILES
	#${CMAKE_CURRENT_SOURCE_DIR}/tests/unittests/mesh-test.cpp
)
ivw_add_unittest(${TEST_FILES})

#--------------------------------------------------------------------
# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

target_link_libraries(${${mod}_target} PUBLIC 
  openvdb
)
target_include_directories(${${mod}_target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ext/openvdb/openvdb)

ivw_register_license_file(NAME OpenVDB VERSION 7.0 MODULE OpenVDB TYPE "Mozilla Public License Version 2.0"
    URL https://www.openvdb.org/
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/ext/openvdb/openvdb/LICENSE
)


#--------------------------------------------------------------------
# Add shader directory to pack
# ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)


