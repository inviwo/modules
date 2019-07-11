/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2014-2018 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#include <string>

#include <inviwo/dicom/io/gdcmvolumereader.h>
#include <inviwo/dicom/io/mevisvolumereader.h>
#include <inviwo/dicom/errorlogging.h>

#include <inviwo/core/util/filesystem.h>
#include <inviwo/core/util/formatconversion.h>
#include <inviwo/core/util/stringconversion.h>

#include <inviwo/core/datastructures/volume/volume.h>

#include <fmt/format.h>

#include <warn/push>
#include <warn/ignore/all>
#include <MediaStorageAndFileFormat/gdcmImage.h>
#include <MediaStorageAndFileFormat/gdcmImageHelper.h>
#include <MediaStorageAndFileFormat/gdcmImageReader.h>
#include <DataStructureAndEncodingDefinition/gdcmAttribute.h>
#include <DataStructureAndEncodingDefinition/gdcmMediaStorage.h>
#include <warn/pop>

namespace inviwo {

GdcmVolumeReader::GdcmVolumeReader()
    : DataReaderType<std::vector<std::shared_ptr<Volume>>>()
    , file_(std::string())
    , format_{DataUInt8::get()}
    , dimension_{0, 0, 0} {

    addExtension(FileExtension("dcm", "DICOM Volume file format"));

    enableGdcmLogging(LogVerbosity::Error);
}

GdcmVolumeReader* GdcmVolumeReader::clone() const { return new GdcmVolumeReader(*this); }

/**
 * Creates inviwo volume handle from DICOM series on disk.
 * Only metadata.
 */
std::shared_ptr<Volume> GdcmVolumeReader::getVolumeDescription(dicomdir::Series& series) {

    unsigned int maxWidth = 0, maxHeight = 0;  // use max image dimensions for volume

    // Metadata from DICOM dataset

    double smallestVoxelValue = std::numeric_limits<double>::infinity();
    double largestVoxelValue = -std::numeric_limits<double>::infinity();
    double sliceThickness = std::numeric_limits<double>::infinity();
    vec3 orientationPatientX = vec3(1, 0, 0);
    vec3 orientationPatientY = vec3(0, 1, 0);

    for (dicomdir::Image& imgInfo : series.images) {
        gdcm::ImageReader imageReader;
        std::ifstream imageInputStream(imgInfo.path, std::ios::binary);

        if (!imageInputStream.is_open()) {
            // could not open file => skip
            continue;
        }

        imageReader.SetStream(imageInputStream);
        if (!imageReader.CanRead()) {
            // not a DICOM file => skip
            continue;
        }

        if (imageReader.Read()) {

            const auto& image = imageReader.GetImage();
            const unsigned int* dimensions = image.GetDimensions();
            maxWidth = std::max<unsigned int>(maxWidth, dimensions[0]);
            maxHeight = std::max<unsigned int>(maxHeight, dimensions[1]);

            const auto& dataset = imageReader.GetFile().GetDataSet();

            {  // read window center/level
                const gdcm::Tag windowCenterTag(0x0028, 0x1050);
                if (dataset.FindDataElement(windowCenterTag)) {
                    const auto el = dataset.GetDataElement(windowCenterTag);
                    if (el.GetVR() == gdcm::VR::DS) {  // decimal string
                        std::stringstream windowCenterSS;
                        el.GetValue().Print(windowCenterSS);
                        imgInfo.windowCenter = windowCenterSS.str();
                    }
                }
            }

            {  // read window width
                const gdcm::Tag windowWidthTag(0x0028, 0x1051);
                if (dataset.FindDataElement(windowWidthTag)) {
                    const auto el = dataset.GetDataElement(windowWidthTag);
                    if (el.GetVR() == gdcm::VR::DS) {  // decimal string
                        std::stringstream windowWidthSS;
                        el.GetValue().Print(windowWidthSS);
                        imgInfo.windowWidth = windowWidthSS.str();
                    }
                }
            }

            {  // read slice thickness
                const gdcm::Tag sliceThicknessTag(0x0018, 0x0050);
                if (dataset.FindDataElement(sliceThicknessTag)) {
                    const auto el = dataset.GetDataElement(sliceThicknessTag);
                    if (el.GetVR() == gdcm::VR::DS) {  // decimal string
                        std::stringstream sliceThicknessSS;
                        el.GetValue().Print(sliceThicknessSS);
                        imgInfo.sliceThickness = sliceThicknessSS.str();

                        bool sliceThicknessConversionSuccessful{true};
                        try {
                            double SliceThicknessTmp = std::stod(imgInfo.sliceThickness);
                            sliceThickness = SliceThicknessTmp;
                        } catch (...) {
                            sliceThicknessConversionSuccessful = false;
                        }
                    }
                }
            }

            {  // read smallest image pixel value
                const gdcm::Tag smallestImagePixelValueTag(0x0028, 0x0106);
                if (dataset.FindDataElement(smallestImagePixelValueTag)) {
                    const gdcm::DataElement el = dataset.GetDataElement(smallestImagePixelValueTag);
                    const gdcm::ByteValue* ptr = el.GetByteValue();
                    if (el.GetVR() == gdcm::VR::SS) {  // signed short
                        short v;
                        ptr->GetBuffer((char*)&v, 2);
                        smallestVoxelValue = std::min(smallestVoxelValue, static_cast<double>(v));
                    } else if (el.GetVR() == gdcm::VR::US) {  // unsigned short
                        unsigned short v;
                        ptr->GetBuffer((char*)&v, 2);
                        smallestVoxelValue = std::min(smallestVoxelValue, static_cast<double>(v));
                    }
                }
            }

            {  // read largest image pixel value
                const gdcm::Tag largestImagePixelValueTag(0x0028, 0x0107);
                if (dataset.FindDataElement(largestImagePixelValueTag)) {
                    const gdcm::DataElement el = dataset.GetDataElement(largestImagePixelValueTag);
                    const gdcm::ByteValue* ptr = el.GetByteValue();
                    if (el.GetVR() == gdcm::VR::SS) {  // signed short
                        short v;
                        ptr->GetBuffer((char*)&v, 2);
                        largestVoxelValue = std::max(largestVoxelValue, static_cast<double>(v));
                    } else if (el.GetVR() == gdcm::VR::US) {  // unsigned short
                        unsigned short v;
                        ptr->GetBuffer((char*)&v, 2);
                        largestVoxelValue = std::max(largestVoxelValue, static_cast<double>(v));
                    }
                }
            }

            {  // read ImageOrientationPatient (IOP), i.e. rotation
                const gdcm::Tag imageOrientationPatientTag(0x0020, 0x0037);
                if (dataset.FindDataElement(imageOrientationPatientTag)) {
                    const gdcm::DataElement el = dataset.GetDataElement(imageOrientationPatientTag);
                    if (el.GetVR() == gdcm::VR::DS) {  // decimal string

                        std::stringstream strVal;
                        el.GetValue().Print(strVal);
                        imgInfo.orientationPatient = strVal.str();

                        auto tokens = splitString(strVal.str(), '\\');
                        if (tokens.size() != 6) {
                            throw DataReaderException(
                                fmt::format("invalid patient orientation tag: '{}' ('{}')",
                                            imgInfo.orientationPatient, imgInfo.path),
                                IVW_CONTEXT_CUSTOM("GdcmVolumeReader::getVolumeDescription"));
                        }

                        for (size_t i = 0; i < tokens.size(); ++i) {
                            if (i < 3) {
                                orientationPatientX[i] = std::stof(tokens[i]);
                            } else {
                                orientationPatientY[i - 3] = std::stof(tokens[i]);
                            }
                        }
                    }
                }
            }

            {  // read ImagePositionPatient (IPP), i.e. translation
                const gdcm::Tag imagePositionPatientTag(0x0020, 0x0032);
                if (dataset.FindDataElement(imagePositionPatientTag)) {
                    const gdcm::DataElement el = dataset.GetDataElement(imagePositionPatientTag);
                    if (el.GetVR() == gdcm::VR::DS) {  // decimal string

                        std::stringstream strVal;
                        el.GetValue().Print(strVal);
                        imgInfo.positionPatient = strVal.str();

                        auto tokens = splitString(strVal.str(), '\\');
                        if (tokens.size() != 3) {
                            throw DataReaderException(
                                fmt::format("invalid patient position tag: '{}' ('{}')",
                                            imgInfo.positionPatient, imgInfo.path),
                                IVW_CONTEXT_CUSTOM("GdcmVolumeReader::getVolumeDescription"));
                        }

                        vec3 positionPatient{0.0f};
                        for (size_t i = 0; i < tokens.size(); ++i) {
                            positionPatient[i] = std::stof(tokens[i]);
                        }

                        imgInfo.zPos =
                            dot(positionPatient, cross(orientationPatientX, orientationPatientY));
                    }
                }
            }
        }
    }

    // Instead of relying on file order, sort images by slice position in patient coords
    std::sort(series.images.begin(), series.images.end(),
              [](const dicomdir::Image& a, const dicomdir::Image& b) { return a.zPos > b.zPos; });

    // Read images

    std::vector<gdcm::Image> imageStack;  // DICOM series == image stack == volume

    for (dicomdir::Image& imgInfo : series.images) {

        gdcm::ImageReader imageReader;  // possible optimization:
                                        // dont read whole image (gdcm::ImageRegionReader)
        std::ifstream imageInputStream(imgInfo.path, std::ios::binary);

        if (!imageInputStream.is_open()) {
            // could not open file => skip
            continue;
        }

        imageReader.SetStream(imageInputStream);
        if (!imageReader.CanRead()) {
            // not a DICOM file => skip
            continue;
        }

        if (imageReader.Read()) {
            gdcm::Image image = imageReader.GetImage();
            imageStack.push_back(image);
        }
    }

    // Metadata from images

    size_t channelCount = 0;     // e.g. 3 for RGB
    size_t channelBits = 0;      // bit width of each channel
    size_t usedChannelBits = 0;  // bit width of data in each channel
    unsigned short isSignedInt = 0;
    gdcm::PixelFormat pixelformat;

    // Origin can be used to transform the xy image grid into xyz world space
    glm::dvec3 origin;

    // Spacing between voxels in millimeters
    // note that this must be reconstructed by GDCM with varying accuracy
    // http://gdcm.sourceforge.net/wiki/index.php/Imager_Pixel_Spacing#ImagerPixelSpacing_to_PixelSpacing
    glm::dvec3 spacing;

    // Rescale describes the conversion from disk data to hounsfield value
    // https://blog.kitware.com/dicom-rescale-intercept-rescale-slope-and-itk/
    double rescaleIntercept, rescaleSlope;

    // Photometric interpretation string
    // e.g. MONOCHROME1 where lowest value is white or MONOCHROME2 where lowest value is black
    // https://dicom.innolitics.com/ciods/mr-image/image-pixel/00280004
    std::string piString;

    if (imageStack.size() > 0) {
        const gdcm::Image& image =
            imageStack[0];  // just assume that all images have consistent values

        auto colorspace = image.GetPhotometricInterpretation();
        colorspace.GetString();
        channelCount = colorspace.GetSamplesPerPixel();

        origin.x = image.GetOrigin(0);
        origin.y = image.GetOrigin(1);
        origin.z = image.GetOrigin(2);

        spacing.x = image.GetSpacing(0);
        spacing.y = image.GetSpacing(1);

        if (sliceThickness == std::numeric_limits<double>::infinity()) {
            spacing.z = image.GetSpacing(2);  // fallback if no explicit slice thickness is set
        } else {
            spacing.z =
                sliceThickness;  // image.GetSpacing(2); seems to be not set correctly for volumes?
        }

        pixelformat = image.GetPixelFormat();

        rescaleIntercept = image.GetIntercept();
        rescaleSlope = image.GetSlope();

        channelBits = image.GetPixelFormat().GetBitsAllocated();
        usedChannelBits = image.GetPixelFormat().GetBitsStored();
        isSignedInt = image.GetPixelFormat().GetPixelRepresentation();
    } else {
        return 0;  // empty volume
    }

    // Save info that is needed during GCDMVolumeRAMLoader::dispatch
    series.slope = rescaleSlope;
    series.intercept = rescaleIntercept;

    // Now map metadata to inviwo data types

    // The volume should have enough space for the largest image
    series.dims = size3_t(maxWidth, maxHeight, imageStack.size());

    const auto voxelFormat =
        DataFormatBase::get(isSignedInt ? NumericType::SignedInteger : NumericType::UnsignedInteger,
                            channelCount, channelBits);

    const std::shared_ptr<Volume> outputVolume = std::make_shared<Volume>(series.dims, voxelFormat);

    // Compute data range from the used bits, differentiating between unsigned and 2's complement
    // or...
    outputVolume->dataMap_.dataRange =
        isSignedInt ? dvec2{-std::pow(2, usedChannelBits - 1), std::pow(2, usedChannelBits - 1) - 1}
                    : dvec2{0, std::pow(2, usedChannelBits) - 1};

    // ... use DICOM's explicit datarange if existing
    bool explicitRange = false;
    if (smallestVoxelValue < std::numeric_limits<double>::infinity()) {
        outputVolume->dataMap_.dataRange.x = smallestVoxelValue;
        explicitRange = true;
    }
    if (largestVoxelValue > -std::numeric_limits<double>::infinity()) {
        outputVolume->dataMap_.dataRange.y = largestVoxelValue;
        explicitRange = true;
    }

    // Convert disk data to hounsfield value using the DICOM rescale tags (this is only
    // valid for CT scans)
    if (explicitRange) {
        outputVolume->dataMap_.valueRange =
            outputVolume->dataMap_.dataRange * rescaleSlope + rescaleIntercept;
    } else {
        outputVolume->dataMap_.valueRange = outputVolume->dataMap_.dataRange;
    }

    if (series.modality == "CT") {
        outputVolume->dataMap_.valueUnit = "HU";
    } else {
        outputVolume->dataMap_.valueUnit = "unknown";
    }

    // TODO use spacing for basis and origin for world
    outputVolume->setBasis(glm::scale(vec3(1.0 / spacing)) *
                           mat4(vec4(orientationPatientX, 0), vec4(orientationPatientY, 0),
                                vec4(cross(orientationPatientX, orientationPatientY), 0),
                                vec4(vec3(origin), 1.0f)));  // ModelMatrix (data -> model)
    outputVolume->setWorldMatrix(mat4{1});                   // WorldMatrx (model -> world)

    return outputVolume;

    // More info:

    // Window center and window width https://www.dabsoft.ch/dicom/3/C.11.2.1.2/
    // Slice Location https://dicom.innolitics.com/ciods/mr-image/image-plane/00201041
    // Frame of Reference
    // https://dicom.innolitics.com/ciods/segmentation/frame-of-reference/00200052

    // Images with same series UID belong to same volume -> DiscriminateVolume.cxx example
    // (https://stackoverflow.com/questions/18529967/how-to-decide-if-a-dicom-series-is-a-3d-volume-or-a-series-of-images)

    // gdcminfo command line utility
    // http://gdcm.sourceforge.net/html/gdcminfo.html
}

/**
 * Tries to read all volumes contained in given directory path, including subdirectories.
 * Looks only at all the image files and ignores possibly existing DIOCMDIR.
 */
std::shared_ptr<VolumeSequence> GdcmVolumeReader::tryReadDICOMsequenceRecursive(
    const std::string& directory) {
    std::shared_ptr<VolumeSequence> outputVolumes = tryReadDICOMsequence(directory);

    const auto childDirectories =
        filesystem::getDirectoryContents(directory, filesystem::ListMode::Directories);
    for (const auto& childDir : childDirectories) {
        std::shared_ptr<VolumeSequence> childVolumes =
            tryReadDICOMsequenceRecursive(directory + '/' + childDir);
        for (const auto& childVolume : *childVolumes) {
            outputVolumes->push_back(childVolume);
        }
    }

    return outputVolumes;
}

/**
 * Non-recursive version of tryReadDICOMsequenceRecursive
 */
std::shared_ptr<VolumeSequence> GdcmVolumeReader::tryReadDICOMsequence(
    const std::string& sequenceDirectory) {
    const auto files = filesystem::getDirectoryContents(sequenceDirectory);
    std::shared_ptr<VolumeSequence> outputVolumes = std::make_shared<VolumeSequence>();
    std::map<std::string, dicomdir::Series> seriesByUID;

    for (const auto& f : files) {
        // try to load file as DICOM
        // if yes: read patient ID and sequence ID
        // group sequences by sequence ID
        // add sequences to "outputVolumes"
        std::string file = sequenceDirectory + '/' + f;
        if (!filesystem::fileExists(file)) {
            throw DataReaderException(fmt::format("file does not exist ('{}')", file),
                                      IVW_CONTEXT_CUSTOM("GdcmVolumeReader::tryReadDICOMsequence"));
        }

        gdcm::ImageReader imageReader;
        std::ifstream imageInputStream(file, std::ios::binary);
        if (!imageInputStream.is_open()) {
            throw DataReaderException(fmt::format("file cannot be opened ('{}')", file),
                                      IVW_CONTEXT_CUSTOM("GdcmVolumeReader::tryReadDICOMsequence"));
        }

        /*if (!imageReader.CanRead()) { // call this on file BrainSample 11.7T\4\pdata\1\2dseq
        => gdcm crash continue; // skip non-dicom files
        }*/
        imageReader.SetStream(imageInputStream);
        if (imageReader.Read()) {
            const auto dataset = imageReader.GetFile().GetDataSet();
            const auto seriesUIDTag = gdcm::Tag(0x0020, 0x000E);

            if (dataset.FindDataElement(seriesUIDTag)) {
                std::stringstream seriesUID;
                dataset.GetDataElement(seriesUIDTag).GetValue().Print(seriesUID);
                std::string suid = seriesUID.str();
                if (seriesByUID.find(suid) == seriesByUID.end()) {
                    gdcm::MediaStorage dicomMediaStorage;
                    dicomMediaStorage.SetFromFile(imageReader.GetFile());
                    dicomdir::Series newSeries;
                    newSeries.modality = dicomMediaStorage.GetModality();
                    seriesByUID[suid] = newSeries;
                }
                seriesByUID[suid].images.push_back(dicomdir::Image{file});
            } else {
                throw DataReaderException(
                    fmt::format("could not find DICOM series UID ('{}')", file),
                    IVW_CONTEXT_CUSTOM("GdcmVolumeReader::tryReadDICOMsequence"));
            }
        } else {
            continue;  // skip non-dicom files
        }
    }

    for (const auto& pair : seriesByUID) {
        dicomdir::Series series = pair.second;
        if (series.images.empty()) {
            continue;
        }
        std::shared_ptr<Volume> vol = getVolumeDescription(series);
        // on-demand loading via loader class
        auto diskRepr = std::make_shared<VolumeDisk>(sequenceDirectory, vol->getDimensions(),
                                                     vol->getDataFormat());
        auto loader = util::make_unique<GCDMVolumeRAMLoader>(
            sequenceDirectory, vol->getDimensions(), vol->getDataFormat(), true, series);
        diskRepr->setLoader(loader.release());
        vol->addRepresentation(diskRepr);
        outputVolumes->push_back(vol);
    }
    return outputVolumes;
}

/**
 * Try to read all volumes contained in given path using standard dicomdir:: format
 */
std::shared_ptr<VolumeSequence> GdcmVolumeReader::tryReadDICOMDIR(
    const std::string& fileOrDirectory) {
    std::string dicomdirPath = fileOrDirectory;
    std::ifstream dicomdirInputStream(dicomdirPath, std::ios::binary);
    if (!dicomdirInputStream.is_open()) {
        // Guess some file names
        dicomdirPath += "/dicomdir::";  // could have opened a folder
        dicomdirInputStream.open(dicomdirPath, std::ios::binary);
        if (!dicomdirInputStream.is_open()) {
            LogWarnCustom("GdcmVolumeReader", "dicomdir:: not found. Tested '"
                                                  << dicomdirPath << "' and '" << dicomdirPath
                                                  << "/dicomdir::'.");
            return 0;  // http://www.cplusplus.com/reference/memory/shared_ptr/operator%20bool/
        }
    }

    // Analog to gdcm example "ReadAndDumpdicomdir::"
    gdcm::Reader reader;
    reader.SetStream(dicomdirInputStream);
    if (!reader.Read()) {
        // LogInfo(dicomdirPath + " is no DICOM file");
        return 0;
    }

    gdcm::File& file = reader.GetFile();

    // First check meta info
    gdcm::FileMetaInformation& metainfo = file.GetHeader();
    gdcm::MediaStorage dicomMediaStorage;
    dicomMediaStorage.SetFromFile(file);
    if (dicomMediaStorage != gdcm::MediaStorage::MediaStorageDirectoryStorage) {
        return 0;
    }

    std::stringstream storageUID;
    if (metainfo.FindDataElement(gdcm::Tag(0x0002, 0x0002))) {
        storageUID.str("");
        metainfo.GetDataElement(gdcm::Tag(0x0002, 0x0002)).GetValue().Print(storageUID);
    } else {
        // Media Storage Sop Class UID not present
        return 0;
    }

    // Trim string because DICOM allows padding with spaces
    auto storageUIDstr = trim(storageUID.str());

    if ("1.2.840.10008.1.3.10" != storageUIDstr) {
        // This file is not a dicomdir::
        return 0;
    }

    // Now read actual dataset
    gdcm::DataSet& dataset = file.GetDataSet();

    auto getString = [](const auto& ds, const gdcm::Tag& tag) -> std::string {
        std::stringstream ss;
        if (ds.FindDataElement(tag)) {
            auto& elem = ds.GetDataElement(tag);
            if (!elem.IsEmpty() && !elem.IsUndefinedLength()) {
                elem.GetValue().Print(ss);
            }
        }
        return trim(ss.str());
    };

    const gdcm::Tag patientsNameTag(0x0010, 0x0010);
    const gdcm::Tag patientsIdTag(0x0010, 0x0020);
    const gdcm::Tag studyDateTag(0x0008, 0x0020);
    const gdcm::Tag studyDescTag(0x0008, 0x1030);
    const gdcm::Tag seriesDescTag(0x0008, 0x103e);

    const gdcm::Tag directoryRecordSequenceTag(0x0004, 0x1220);
    const gdcm::Tag directoryRecordTypeTag(0x0004,
                                           0x1430);  // value can be patient, study, series or image
    const gdcm::Tag referencedFileID(0x0004, 0x1500);  // value is e.g. image path

    int patientCount = 0;
    int studyCount = 0;
    int seriesCount = 0;
    int imageCount = 0;
    std::vector<dicomdir::Patient> dataPerPatient;
    // for each patient: for each study: for each series: get images (in this first loop only
    // the paths!)
    for (auto dataElement : dataset.GetDES()) {
        // only look at directory sequence
        if (dataElement.GetTag() != directoryRecordSequenceTag) {
            continue;
        }

        auto recordSequence = dataElement.GetValueAsSQ();
        // This loop also iterates all the nested records sequences (like depth-first tree
        // traversal)
        for (int recIndex = 1; recIndex <= recordSequence->GetNumberOfItems(); recIndex++) {
            // Records contain data about either a patient, study, series or image
            // image records reference image files
            gdcm::Item& record = recordSequence->GetItem(recIndex);
            if (!record.FindDataElement(directoryRecordTypeTag)) {
                continue;
            }

            std::stringstream recordType;
            record.GetDataElement(directoryRecordTypeTag).GetValue().Print(recordType);
            std::string recType = trim(recordType.str());

            if (recType == "PATIENT") {
                const auto patientName = getString(record, patientsNameTag);
                const auto patientId = getString(record, patientsIdTag);
                dataPerPatient.push_back({patientName, patientId, {}});

                patientCount++;
            } else if (recType == "STUDY") {
                const auto date = getString(record, studyDateTag);
                const auto desc = getString(record, studyDescTag);
                dataPerPatient.back().studies.push_back({date, desc, {}});
                studyCount++;
            } else if (recType == "SERIES") {
                dataPerPatient.back().studies.back().series.push_back({});
                dataPerPatient.back().studies.back().series.back().desc =
                    getString(record, seriesDescTag);
                seriesCount++;
            } else if (recType == "IMAGE") {
                // save referenced image path to be able to read it later if it's in the volume
                if (record.FindDataElement(referencedFileID)) {
                    std::stringstream imagePath;
                    record.GetDataElement(referencedFileID).GetValue().Print(imagePath);
                    std::string imagePathStr = trim(imagePath.str());

                    // relative to absolute path
                    imagePathStr =
                        dicomdirPath.substr(0, dicomdirPath.find_last_of("/\\") + 1) + imagePathStr;

                    auto& study = dataPerPatient.back().studies.back();
                    auto& series = study.series.back();

                    series.images.push_back(dicomdir::Image{imagePathStr});
                }

                imageCount++;
            }
        }
    }

    LogInfoCustom("GdcmVolumeReader", "Scanned dicomdir:: ('"
                                          << dicomdirPath << "'):\n    PatientCount = "
                                          << patientCount << "\n    StudyCount = " << studyCount
                                          << "\n    ImageCount = " << imageCount);

    if (patientCount == 0 || studyCount == 0 || seriesCount == 0 || imageCount == 0) {
        LogWarnCustom("GdcmVolumeReader",
                      "No volumes found in dicomdir::  ('" << dicomdirPath << "')");
        return 0;
    }

    // Build volumes from images
    std::shared_ptr<VolumeSequence> outputVolumes = std::make_shared<VolumeSequence>();
    for (dicomdir::Patient& patient : dataPerPatient) {  // push everything in one sequence
        for (dicomdir::Study& study : patient.studies) {
            for (dicomdir::Series& series : study.series) {
                if (series.images.empty()) {
                    continue;
                }

                series.modality = dicomMediaStorage.GetModality();

                std::shared_ptr<Volume> vol = getVolumeDescription(series);
                // on-demand loading via loader class
                auto diskRepr = std::make_shared<VolumeDisk>(dicomdirPath, vol->getDimensions(),
                                                             vol->getDataFormat());
                auto loader = util::make_unique<GCDMVolumeRAMLoader>(
                    dicomdirPath, vol->getDimensions(), vol->getDataFormat(), true, series);
                diskRepr->setLoader(loader.release());
                vol->addRepresentation(diskRepr);
                vol->setMetaData<StringMetaData>("name", series.desc);
                vol->setMetaData<StringMetaData>("patientName", patient.patientName);
                vol->setMetaData<StringMetaData>("patientId", patient.patientId);
                vol->setMetaData<StringMetaData>("studyDesc", study.desc);
                vol->setMetaData<StringMetaData>("studyDate", study.date);

                outputVolumes->push_back(vol);
            }
        }
    }

    // print a summary of all collected volumes
    auto createLine = [](const std::string& tag, auto text, int indent) {
        return fmt::format("{0:<{1}}{2:<12}{3}\n", "", indent, tag, text);
    };

    size_t volindex = 0;
    auto printSeries = [createLine, &volindex](const dicomdir::Series& series, int indent) {
        std::ostringstream ss;

        ss << createLine("[ DICOM Series", "", indent);
        if (glm::compMul(series.dims) > 0) {
            ++volindex;
            ss << createLine("Volume Idx:", volindex, indent + 2)
               << createLine("Dimensions:", toString(series.dims), indent + 2);
        }
        if (!series.desc.empty()) {
            ss << createLine("Desc.:", series.desc, indent + 2);
        }
        if (!series.modality.empty()) {
            ss << createLine("Modality:", series.modality, indent + 2);
        }
        ss << createLine("No. Images:", series.images.size(), indent + 2)
           << createLine("]", "", indent);

        return ss.str();
    };

    auto printStudy = [createLine, printSeries](const dicomdir::Study& study, int indent) {
        std::ostringstream ss;

        ss << createLine("[ DICOM Study", "", indent);
        if (!study.desc.empty()) {
            ss << createLine("Desc.:", study.desc, indent + 2);
        }
        if (!study.date.empty()) {
            ss << createLine("Date:", study.date, indent + 2);
        }
        ss << createLine("No. Series:", study.series.size(), indent + 2);
        for (const auto& s : study.series) {
            ss << printSeries(s, indent + 2);
        }
        ss << createLine("]", "", indent);

        return ss.str();
    };

    auto printPatient = [createLine, printStudy](const dicomdir::Patient& p) {
        std::ostringstream ss;

        ss << "[ DICOM Patient\n"
           << createLine("Name:", p.patientName, 2) << createLine("ID", p.patientId, 2)
           << createLine("No. Studies:", p.studies.size(), 2);
        for (const auto& s : p.studies) {
            ss << printStudy(s, 2);
        }
        ss << "]";

        return ss.str();
    };

    for (const auto& p : dataPerPatient) {
        LogInfoCustom("GdcmVolumeReader", printPatient(p));
    }

    return outputVolumes;
}

/**
 * Entry point of the reader, called from VolumeSource processor
 */
std::shared_ptr<VolumeSequence> GdcmVolumeReader::readData(const std::string& filePath) {
    auto path = filePath;
    if (!filesystem::fileExists(path)) {
        std::string newPath = filesystem::addBasePath(path);
        if (filesystem::fileExists(newPath)) {
            path = newPath;
        } else {
            throw DataReaderException(fmt::format("could not read input file ('{}')", path),
                                      IVW_CONTEXT);
        }
    }

    const auto directory = filesystem::getFileDirectory(path);
    // TODO sequence source calls here for each file in a folder
    if (directory == this->file_) {
        return this->volumes_;  // doesnt work
    }
    gdcm::Trace::DebugOff();  // prevent gdcm from spamming inviwo console

    // Try several dicom file structures

    {
        std::shared_ptr<VolumeSequence> outputVolumes = tryReadDICOMDIR(directory);
        if (outputVolumes) {
            // Return now if reading was already successful
            this->file_ = directory;
            this->volumes_ = outputVolumes;
            return outputVolumes;
        }
    }

    {
        std::shared_ptr<VolumeSequence> outputVolumes = tryReadDICOMsequenceRecursive(directory);
        if (outputVolumes) {
            this->file_ = directory;
            this->volumes_ = outputVolumes;
            return outputVolumes;
        }
    }

    // TODO read DICOM image file and detect the series it belongs to

    // TODO make different dimensions possible in one sequence

    // Otherwise keep trying
    this->file_ = path;
    std::shared_ptr<VolumeSequence> outputVolumes = std::make_shared<VolumeSequence>();
    gdcm::ImageReader reader;
    reader.SetFileName(file_.c_str());
    if (!reader.Read()) {
        MevisVolumeReader mvreader;
        if (mvreader.setFilenames(file_)) {
            LogInfo("This seems to be a MevisLab dcm/tif file - calling the appropriate reader...");
            std::shared_ptr<Volume> v = mvreader.readData(file_);
            outputVolumes->push_back(v);
            return outputVolumes;
        } else {
            throw DataReaderException(fmt::format("could not read input file ('{}')", path),
                                      IVW_CONTEXT);
        }
    }

    const gdcm::Image& image = reader.GetImage();
    const gdcm::File& file = reader.GetFile();
    std::shared_ptr<Volume> v = generateVolume(image, file);
    if (!v) {
        throw DataReaderException(fmt::format("could not read input file ('{}')", path),
                                  IVW_CONTEXT);
    }

    outputVolumes->push_back(v);

    return outputVolumes;
}

/**
 * Old function that tries to read single volume from single file.
 * Apparently this can be applied to the "mevis" format.
 */
std::shared_ptr<Volume> GdcmVolumeReader::generateVolume(const gdcm::Image& image,
                                                         const gdcm::File& file) {
    /*char* buf = new char[image.GetBufferLength()];
    bool codecFound = image.GetBuffer(buf);
    LogInfo(codecFound ? "Found codec and dumping image" : "Dumping image but no codec found");
    std::ofstream("C:\\Users\\mk\\Desktop\\im1.raw").write(buf, image.GetBufferLength());;
    delete[] buf;*/
    const gdcm::PixelFormat& pixelformat = image.GetPixelFormat();

    // Do not use attributes from reader.GetImage() since they might be incorrect
    // if reader.read() failed. Replicate by loading a dcm file from MeVisLab
    // Instead, use the helper functions used by gdcm
    std::vector<double> spacings = gdcm::ImageHelper::GetSpacingValue(file);
    std::vector<double> origin = gdcm::ImageHelper::GetOriginValue(file);
    std::vector<unsigned int> dims = gdcm::ImageHelper::GetDimensionsValue(file);

    if (dims.empty()) {
        throw DataReaderException("invalid dimensions", IVW_CONTEXT);
    }

    // Add default values if missing
    while (spacings.size() < 3) {
        spacings.push_back(1.0);
    }

    while (origin.size() < 3) {
        origin.push_back(0.0);
    }

    while (dims.size() < 3) {
        dims.push_back(1u);
    }
    // TODO get image/patient orientation?

    // construct volume info
    mat3 basis(1.0f);
    mat4 wtm(1.0f);
    vec3 offset(0.0f);
    vec3 spacing(1.0f);
    ivec3 dimension(1);

    for (std::size_t i = 0; i < 3; ++i) {
        offset[i] = static_cast<float>(origin[i]);
        spacing[i] = static_cast<float>(spacings[i]);
        dimension[i] = dims[i];
        basis[i][i] = static_cast<float>(dimension[i]) * spacing[i];
    }

    std::size_t components = pixelformat.GetSamplesPerPixel();
    std::size_t precision = (pixelformat.GetPixelSize() * 8) / components;

    const inviwo::DataFormatBase* tmp = DataFormatBase::get(pixelformat.GetScalarTypeAsString());
    if (nullptr == tmp)
        throw DataReaderException("Error: cant find corresponding voxel format in inviwo: " +
                                  std::string(pixelformat.GetScalarTypeAsString()));

    auto format = DataFormatBase::get(tmp->getNumericType(), components, precision);
    if (nullptr == format)
        throw DataReaderException("Error: cant find corresponding voxel format in inviwo");

    std::size_t voxelsz = (format->getSize()) * (format->getComponents());
    std::size_t size = dimension.x * dimension.y * dimension.z * voxelsz;

    // sanity check
    std::size_t len = image.GetBufferLength();

    // if gdcm says the volume size is LARGER than we compute - inviwo may crash because
    // the allocated buffer can be too small for image.GetBuffer(destination)
    if (size < len) {
        throw DataReaderException(std::string("Error: propably read inconsistent information:\n") +
                                  std::string("computed volume size is ") + std::to_string(size) +
                                  std::string(", but stored is a volume of size ") +
                                  std::to_string(len));
    } else if (size > len) {
        LogError("computed volume size is " << size << ", but stored is a volume of size " << len
                                            << "!");
    }

    gdcm::MediaStorage ms = gdcm::MediaStorage();

    // add some info in debug mode
#if defined(IVW_DEBUG)
    LogInfo("========================================================================");
    LogInfo(file_ << " - Volume Information:");

    std::string dimstr = std::to_string(dims[0]);
    std::string oristr = std::to_string(origin[0]);
    std::string spacestr = std::to_string(spacings[0]);
    for (std::size_t i = 1; i < dims.size(); ++i) {
        dimstr += "x" + std::to_string(dims[i]);
        oristr += "x" + std::to_string(origin[i]);
        spacestr += "x" + std::to_string(spacings[i]);
    }

    ms.SetFromFile(file);
    gdcm::MediaStorage::MSType typ = ms.GetMSType(ms.GetString());

    LogInfo("media storage type: " << ms.GetMSString(typ));
    LogInfo("volume is " << dims.size() << "D");
    LogInfo("size: " << dimstr);
    LogInfo("offset: " << oristr);
    LogInfo("spacing: " << spacestr);
    LogInfo("volume size: " << size);
    LogInfo("voxel size: " << voxelsz << "(components: " << (format->getComponents())
                           << ", component size: " << (format->getSize()) << ")");
    LogInfo("sample value range is [" << pixelformat.GetMin() << ", " << pixelformat.GetMax()
                                      << "].");
    pixelformat.Print(gdcm::Trace::GetDebugStream());
    LogInfo("corresponding inviwo format: " << format->getString());
    LogInfo("format: " << format->getString() << "[" << format->getMin() << ", " << format->getMax()
                       << "]");
    LogInfo("========================================================================");
#endif

    this->dimension_ = dimension;
    this->format_ = format;
    std::shared_ptr<Volume> volume = std::make_shared<Volume>(dimension, format);
    volume->setBasis(basis);
    volume->setOffset(offset);
    volume->setWorldMatrix(wtm);
    volume->dataMap_.dataRange = dvec2(pixelformat.GetMin(), pixelformat.GetMax());

    // image.GetSlope() and image.GetIntercept() can be incorrect (failure when calling
    // image.Read()) Use dicom attributes director
    std::vector<double> is = gdcm::ImageHelper::GetRescaleInterceptSlopeValue(file);
    auto intercept = is[0];
    auto slope = is[1];
    const std::string modality(ms.GetModality());
    auto maxValue = static_cast<double>(pixelformat.GetMax());

    // TODO: check this heuristics!!!
    if (modality == "CT") {
        // Computed Tomography
        volume->dataMap_.valueUnit = "HU";  // Hounsfield Unit
        if (format->getPrecision() == 16) {
            // Only show a subset (12-bit) of the data range
            // Should be valid for scans of the human body
            // where the range is [-1000 3095]
            volume->dataMap_.dataRange = {0.0, 4095};
            // Linearly map data into [-1024 3071] HU, assuming intercept = -1024
            maxValue = volume->dataMap_.dataRange.y;
        }
    }

    volume->dataMap_.valueRange.x = slope * static_cast<double>(pixelformat.GetMin()) + intercept;
    volume->dataMap_.valueRange.y = slope * maxValue + intercept;

    auto vd = std::make_shared<VolumeDisk>(file_, dimension, format);
    vd->setLoader(new GCDMVolumeRAMLoader(file_, dimension, format));
    volume->addRepresentation(vd);

    return volume;
}

GCDMVolumeRAMLoader::GCDMVolumeRAMLoader(std::string file, size3_t dimension,
                                         const DataFormatBase* format, bool isPartOfSequence,
                                         dicomdir::Series series)
    : file_(file)
    , dimension_(dimension)
    , format_(format)
    , isPartOfSequence_(isPartOfSequence)
    , series_(series) {}

GCDMVolumeRAMLoader* GCDMVolumeRAMLoader::clone() const { return new GCDMVolumeRAMLoader(*this); }

std::shared_ptr<VolumeRepresentation> GCDMVolumeRAMLoader::createRepresentation() const {
    return format_->dispatch(*this);
}

/**
 * Reads DICOM volume data from disk to RAM
 * @param series represents the volume as collection of image file paths
 */
void GCDMVolumeRAMLoader::getVolumeData(const dicomdir::Series& series, void* outData) const {
    unsigned long totalByteCount = 0;
    for (const auto& imgInfo : series.images) {
        gdcm::ImageReader imageReader;

        std::ifstream imageInputStream(imgInfo.path, std::ios::binary);
        if (!imageInputStream.is_open()) {
            throw DataReaderException(std::string("Could not open file: ") + imgInfo.path);
        }

        imageReader.SetStream(imageInputStream);
        if (!imageReader.CanRead()) {
            // Image is probably no DICOM file, its best to just skip it
            continue;
        }

        if (imageReader.Read()) {
            gdcm::Image image = imageReader.GetImage();
            // Get RAW image (gdcm does the decoding for us)
            if (!image.GetBuffer(reinterpret_cast<char*>(outData) + totalByteCount)) {
                throw DataReaderException(std::string("Could not decode image: ") + imgInfo.path);
            }

            totalByteCount += image.GetBufferLength();
        }
    }
}

template <class T>
std::shared_ptr<VolumeRAM> GCDMVolumeRAMLoader::dispatch() const {
    // T is the voxel memory format
    // F is the corresponding datatype (i.e. the template arg with which the format was created)
    typedef typename T::type F;
    const std::size_t size = dimension_[0] * dimension_[1] * dimension_[2];
    auto data = util::make_unique<F[]>(size);
    if (!isPartOfSequence_) {
        gdcm::ImageReader reader;
        reader.SetFileName(file_.c_str());
        if (false == reader.Read()) throw DataReaderException("Error could not read input file.");

        const gdcm::Image& image = reader.GetImage();
        image.GetBuffer(reinterpret_cast<char*>(data.get()));
        auto repr = std::make_shared<VolumeRAMPrecision<F>>(data.get(), dimension_);
        data.release();

        return repr;
    } else {
        getVolumeData(series_, (void*)data.get());

        auto repr = std::make_shared<VolumeRAMPrecision<F>>(data.get(), dimension_);

        // Remember that when you want to read "values" (e.g. in Hounsfield units) from the
        // volume you have to do that through the data mapper.

        // This is how it works assuming you have the
        // handle to "volume" and the filled RAM representation "volumeRAM":
        // LogInfo("value=" << volume->dataMap_.mapFromDataToValue(volumeRAM->getAsDouble({ 119,
        // 296, 0 })));

        data.release();

        return repr;
    }
}

void GCDMVolumeRAMLoader::updateRepresentation(std::shared_ptr<VolumeRepresentation> dest) const {
    if (!isPartOfSequence_) {
        gdcm::ImageReader reader;
        reader.SetFileName(file_.c_str());
        if (false == reader.Read()) throw DataReaderException("Error could not read input file.");

        const gdcm::Image& image = reader.GetImage();
        const std::size_t size = dimension_[0] * dimension_[1] * dimension_[2];
        auto volumeDst = std::static_pointer_cast<VolumeRAM>(dest);
        auto data = volumeDst->getData();
        image.GetBuffer(reinterpret_cast<char*>(data));
    } else {
        std::shared_ptr<VolumeRAM> volumeDst = std::static_pointer_cast<VolumeRAM>(dest);
        getVolumeData(series_, volumeDst->getData());
    }
}

}  // namespace inviwo
