project(gdcm)

# Add grassroots Dicom reader
set(GDCM_ALWAYS_TRACE_MACRO ON CACHE BOOL "enable errors/warnings/debug messages from GDCM" FORCE)
set(GDCM_BUILD_DOCBOOK_MANPAGES OFF CACHE BOOL "disable build of docbook manpages" FORCE)
set(GDCM_BUILD_TESTING OFF CACHE BOOL "disable build of gdcm testing" FORCE)

set(GDCM_EXT_DIR gdcm)

add_subdirectory(${GDCM_EXT_DIR})

ivw_get_targets_in_dir_recursive(gdcm_targets ${GDCM_EXT_DIR})
foreach(target ${gdcm_targets})
    ivw_folder(${target} ext/gdcm/)
endforeach()
ivw_suppress_compiler_warnings(${gdcm_targets})

add_library(gdcm INTERFACE)
add_library(inviwo::gdcm ALIAS gdcm)

# gdcm target does not include interface include directories
target_include_directories(gdcm INTERFACE
   ${GDCM_EXT_DIR}/Source/Attribute
   ${GDCM_EXT_DIR}/Source/Common
   ${GDCM_EXT_DIR}/Source/DataDictionary
   ${GDCM_EXT_DIR}/Source/DataStructureAndEncodingDefinition
   ${GDCM_EXT_DIR}/Source/InformationObjectDefinition
   ${GDCM_EXT_DIR}/Source/MediaStorageAndFileFormat
   ${GDCM_EXT_DIR}/Source/MessageExchangeDefinition
   ${GDCM_BINARY_DIR}/Source/Common/
)

# we need the libtiff4 sources, so add it as dependency
target_link_libraries(gdcm INTERFACE 
    gdcmcharls
    gdcmCommon
    gdcmDICT
    gdcmDSED
    gdcmexpat 
    gdcmIOD
    gdcmjpeg12 
    gdcmjpeg16 
    gdcmjpeg8 
    gdcmMEXD
    gdcmMSFF
    gdcmopenjp2
    gdcmzlib 
)
