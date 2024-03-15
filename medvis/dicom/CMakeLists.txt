#--------------------------------------------------------------------
# Inviwo DICOM Module
ivw_module(DICOM)

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
    include/inviwo/dicom/datastructures/dicomdirtypes.h
    include/inviwo/dicom/dicommodule.h
    include/inviwo/dicom/dicommoduledefine.h
    include/inviwo/dicom/errorlogging.h
    include/inviwo/dicom/io/gdcmvolumereader.h
    include/inviwo/dicom/io/mevisvolumereader.h
    include/inviwo/dicom/utils/gdcmutils.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    src/datastructures/dicomdirtypes.cpp
    src/dicommodule.cpp
    src/errorlogging.cpp
    src/io/gdcmvolumereader.cpp
    src/io/mevisvolumereader.cpp
    src/utils/gdcmutils.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES})

find_package(TIFF REQUIRED)
find_package(gdcm CONFIG REQUIRED)

target_link_libraries(inviwo-module-dicom 
    PUBLIC 
    gdcmDICT
    gdcmDSED
    gdcmIOD
    gdcmjpeg8
    gdcmjpeg12
    gdcmjpeg16
    gdcmMEXD
    gdcmMSFF
    gdcmcharls
    gdcmCommon
    PRIVATE
    TIFF::TIFF
)
target_include_directories(inviwo-module-dicom PUBLIC gdcm)
ivw_vcpkg_install(gdcm MODULE DICOM)

#--------------------------------------------------------------------
ivw_make_package(InviwoDICOMModule inviwo-module-dicom)
