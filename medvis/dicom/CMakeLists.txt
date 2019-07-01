# Add grassroots Dicom reader
set(GDCM_ALWAYS_TRACE_MACRO ON CACHE BOOL "enable errors/warnings/debug messages from GDCM" FORCE)
set(GDCM_BUILD_DOCBOOK_MANPAGES OFF CACHE BOOL "disable build of docbook manpages" FORCE)

set(GDCM_EXT_DIR ext/gdcm)

add_subdirectory(${GDCM_EXT_DIR})

ivw_get_targets_in_dir_recursive(gdcm_targets ${GDCM_EXT_DIR})
foreach(target ${gdcm_targets})
    ivw_folder(${target} ext/gdcm/)
endforeach()
ivw_suppress_compiler_warnings(${gdcm_targets})

#--------------------------------------------------------------------
# Inviwo DICOM Module
ivw_module(DICOM)

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
    include/inviwo/dicom/dicommodule.h
    include/inviwo/dicom/dicommoduledefine.h
    include/inviwo/dicom/errorlogging.h
    include/inviwo/dicom/io/gdcmvolumereader.h
    include/inviwo/dicom/io/mevisvolumereader.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    src/dicommodule.cpp
    src/errorlogging.cpp
    src/io/gdcmvolumereader.cpp
    src/io/mevisvolumereader.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES})

# gdcm targets does not include interface include directories
target_include_directories(inviwo-module-dicom PUBLIC
   ${GDCM_EXT_DIR}/Source/
   ${GDCM_EXT_DIR}/Source/DataStructureAndEncodingDefinition
   ${GDCM_EXT_DIR}/Source/Common
   ${GDCM_EXT_DIR}/Source/Attribute
   ${GDCM_EXT_DIR}/Source/DataDictionary
   ${GDCM_EXT_DIR}/Source/InformationObjectDefinition
   ${GDCM_EXT_DIR}/Source/MessageExchangeDefinition
   # gdcmConfigure.h
   ${GDCM_BINARY_DIR}/Source/Common/
)

# we need the libtiff4 sources, so add it as dependency
# but actually not freeimage functionality is used
# TODO: link instead of compile with the libtiff sources
# => modification of the freeimage library/module needed!
target_link_libraries(inviwo-module-dicom
  PUBLIC 
    inviwo::tiff
    gdcmCommon
    gdcmDICT
    gdcmDSED
    gdcmIOD
    gdcmMSFF
    gdcmMEXD
    gdcmcharls
    gdcmzlib 
    gdcmjpeg8 
    gdcmjpeg12 
    gdcmjpeg16 
    gdcmexpat 
    gdcmopenjp2
)
