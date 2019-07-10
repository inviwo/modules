add_subdirectory(ext/gdcm)

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

target_include_directories(inviwo-module-dicom PUBLIC gdcm)

#--------------------------------------------------------------------
# Add dependency to ext/utf 
target_link_libraries(inviwo-module-dicom PUBLIC gdcm)
ivw_make_package(InviwoDICOMModule inviwo-module-dicom)
