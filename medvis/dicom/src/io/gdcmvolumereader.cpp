/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2014-2024 Inviwo Foundation
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

#include <inviwo/dicom/io/gdcmvolumereader.h>
#include <inviwo/dicom/io/mevisvolumereader.h>
#include <inviwo/dicom/utils/gdcmutils.h>
#include <inviwo/dicom/errorlogging.h>

#include <inviwo/core/util/filesystem.h>
#include <inviwo/core/util/formatconversion.h>
#include <inviwo/core/util/stringconversion.h>
#include <inviwo/core/datastructures/volume/volume.h>

#include <fmt/format.h>
#include <fmt/std.h>

#include <warn/push>
#include <warn/ignore/all>
#include <MediaStorageAndFileFormat/gdcmImage.h>
#include <MediaStorageAndFileFormat/gdcmImageHelper.h>
#include <MediaStorageAndFileFormat/gdcmImageReader.h>
#include <DataStructureAndEncodingDefinition/gdcmAttribute.h>
#include <DataStructureAndEncodingDefinition/gdcmMediaStorage.h>
#include <warn/pop>

#include <functional>
#include <algorithm>

namespace inviwo {

// More info on DICOM format:
//
// Window center and window width https://www.dabsoft.ch/dicom/3/C.11.2.1.2/
// Slice Location https://dicom.innolitics.com/ciods/mr-image/image-plane/00201041
// Frame of Reference
// https://dicom.innolitics.com/ciods/segmentation/frame-of-reference/00200052

// Images with same series UID belong to same volume -> DiscriminateVolume.cxx example
// (https://stackoverflow.com/questions/18529967/how-to-decide-if-a-dicom-series-is-a-3d-volume-or-a-series-of-images)

// gdcminfo command line utility
// http://gdcm.sourceforge.net/html/gdcminfo.html
//

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
std::shared_ptr<Volume> GdcmVolumeReader::getVolumeDescription(dicomdir::Series& series,
                                                               const std::filesystem::path& path) {

    series.updateImageInformation(path);
    if (series.empty()) {
        throw DataReaderException(
            fmt::format("DICOM series '{}' does not contain any images ({})", series.desc, path),
            IVW_CONTEXT_CUSTOM("GdcmVolumeReader::getVolumeDescription"));
    }

    // sort images by slice position (z) in patient coords
    //
    // TODO: proper volume sorting according to
    // https://nipy.org/nibabel/dicom/spm_dicom.html#spm-volume-sorting
    series.sortImages();

    // create an Inviwo Volume
    const DataFormatBase* format = gdcmutil::getDataFormatBase(series.pixelformat);
    if (!format) {
        throw DataReaderException(
            fmt::format("unsupported image format in DICOM series '{}': {} ({})", series.desc,
                        series.pixelformat.GetScalarTypeAsString(), path),
            IVW_CONTEXT_CUSTOM("GdcmVolumeReader::getVolumeDescription"));
    }

    auto volume = std::make_shared<Volume>(series.dims, format);

    // set data range according to used bits, e.g. 12bits for signed and unsigned
    volume->dataMap_.dataRange = gdcmutil::getDataRange(series.pixelformat);

    // TODO: determine data range based on DICOMDIR tags (largest/smallest pixel value),
    // which might not exist!
    /*
    if (explicitRanges) {
        // check tags for all images
        //  - largest image pixel value gdcm::Tag(0x0028, 0x0107)
        //  - smallest image pixel value, gdm::Tag(0x0028, 0x0106)
        volume->dataMap_.dataRange = ...;
    }
    */

    volume->dataMap_.valueRange = volume->dataMap_.dataRange * series.slope + series.intercept;

    if (series.modality == "CT") {
        volume->dataMap_.valueAxis = Axis{"Hounsfield", units::unit_from_string("HU")};
    }

    // basis and offset computations

    // CHECK: can we use origin and direction cosines of refImage instead?
    const auto& dicomImg = series.images.front();
    dvec3 spacing{series.pixelSpacing};
    if (series.pixelSpacing.z == 0.0) {
        if (dicomImg.sliceThickness == 0.0) {
            LogWarnCustom("GdcmVolumeReader::getVolumeDescription",
                          fmt::format("DICOM series '{}' does not define pixel spacing in z or "
                                      "slice thickness, using 1.0 for z ({})",
                                      series.desc, path));
            spacing.z = 1.0;
        } else {
            spacing.z = dicomImg.sliceThickness;
        }
    }

    dvec3 extent{spacing * dvec3{series.dims}};

    mat3 basis{dicomImg.orientationX * extent.x, dicomImg.orientationY * extent.y,
               glm::cross(dicomImg.orientationX, dicomImg.orientationY) * extent.z};

    // TODO: do we need to consider the pixel spacing and slice thickness?

    volume->setBasis(basis);
    volume->setOffset(dicomImg.origin);

    return volume;
}

/**
 * Tries to read all volumes contained in given directory path, including subdirectories.
 * Looks only at all the image files and ignores possibly existing DIOCMDIR.
 */
std::shared_ptr<VolumeSequence> GdcmVolumeReader::tryReadDICOMsequenceRecursive(
    const std::filesystem::path& directory) {
    std::shared_ptr<VolumeSequence> outputVolumes = tryReadDICOMsequence(directory);

    const auto childDirectories =
        filesystem::getDirectoryContents(directory, filesystem::ListMode::Directories);
    for (const auto& childDir : childDirectories) {
        std::shared_ptr<VolumeSequence> childVolumes =
            tryReadDICOMsequenceRecursive(directory / childDir);
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
    const std::filesystem::path& sequenceDirectory) {
    const auto files = filesystem::getDirectoryContents(sequenceDirectory);
    std::shared_ptr<VolumeSequence> outputVolumes = std::make_shared<VolumeSequence>();
    std::map<std::string, dicomdir::Series> seriesByUID;

    for (const auto& f : files) {
        // try to load file as DICOM
        // if yes: read patient ID and sequence ID
        // group sequences by sequence ID
        // add sequences to "outputVolumes"
        std::filesystem::path file = sequenceDirectory / f;
        if (!std::filesystem::is_regular_file(file)) {
            throw DataReaderException(fmt::format("file does not exist ({})", file),
                                      IVW_CONTEXT_CUSTOM("GdcmVolumeReader::tryReadDICOMsequence"));
        }

        gdcm::ImageReader imageReader;
        std::ifstream imageInputStream(file, std::ios::binary);
        if (!imageInputStream.is_open()) {
            throw DataReaderException(fmt::format("file cannot be opened ({})", file),
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
                seriesByUID[suid].images.push_back(dicomdir::Image{file.string()});
            } else {
                throw DataReaderException(
                    fmt::format("could not find DICOM series UID ({})", file),
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
        std::shared_ptr<Volume> vol = getVolumeDescription(series, sequenceDirectory);
        // on-demand loading via loader class
        auto diskRepr = std::make_shared<VolumeDisk>(sequenceDirectory, vol->getDimensions(),
                                                     vol->getDataFormat());
        auto loader = std::make_unique<GCDMVolumeRAMLoader>(sequenceDirectory, vol->getDimensions(),
                                                            vol->getDataFormat(), true, series);
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
    const std::filesystem::path& dicomdirPath) {

    std::ifstream dicomdirInputStream(dicomdirPath, std::ios::binary);
    if (!dicomdirInputStream.is_open()) {
        throw DataReaderException(fmt::format("could not open DICOM file ({})", dicomdirPath),
                                  IVW_CONTEXT_CUSTOM("GdcmVolumeReader::tryReadDICOMDIR"));
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
                // study UID gdcm::Tag (0x0020, 0x000d)
                const auto date = getString(record, studyDateTag);
                const auto desc = getString(record, studyDescTag);
                dataPerPatient.back().studies.push_back({date, desc, {}});
                studyCount++;
            } else if (recType == "SERIES") {
                // series UID gdcm::Tag (0x0020, 0x000e)
                dataPerPatient.back().studies.back().series.emplace_back(
                    getString(record, seriesDescTag));
                seriesCount++;
            } else if (recType == "IMAGE") {
                // image UID gdcm::Tag (0x0004, 0x1511))

                // save referenced image path to be able to read it later if it's in the volume
                if (record.FindDataElement(referencedFileID)) {
                    std::stringstream imagePathStr;
                    record.GetDataElement(referencedFileID).GetValue().Print(imagePathStr);

                    // relative to absolute path
                    std::filesystem::path imagePath{dicomdirPath.parent_path() /
                                                    trim(imagePathStr.str())};

                    auto& study = dataPerPatient.back().studies.back();
                    auto& series = study.series.back();

                    series.images.push_back(dicomdir::Image{imagePath.string()});
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

                std::shared_ptr<Volume> vol = getVolumeDescription(series, dicomdirPath);
                // on-demand loading via loader class
                auto diskRepr = std::make_shared<VolumeDisk>(dicomdirPath, vol->getDimensions(),
                                                             vol->getDataFormat());
                auto loader = std::make_unique<GCDMVolumeRAMLoader>(
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
std::shared_ptr<VolumeSequence> GdcmVolumeReader::readData(const std::filesystem::path& filePath) {
    auto path = filePath;
    if (!std::filesystem::is_regular_file(path)) {
        auto newPath = filesystem::addBasePath(path);
        if (std::filesystem::is_regular_file(newPath)) {
            path = newPath;
        } else {
            throw DataReaderException(fmt::format("could not read input file ({})", path),
                                      IVW_CONTEXT);
        }
    }

    const auto directory = path.parent_path();
    // TODO sequence source calls here for each file in a folder
    if (directory == file_) {
        return volumes_;  // doesnt work
    }
    gdcm::Trace::DebugOff();  // prevent gdcm from spamming inviwo console

    {
        std::shared_ptr<VolumeSequence> outputVolumes = tryReadDICOMsequenceRecursive(directory);
        if (outputVolumes) {
            file_ = directory;
            volumes_ = outputVolumes;
            return outputVolumes;
        }
    }

    // TODO read DICOM image file and detect the series it belongs to

    // TODO make different dimensions possible in one sequence

    // Otherwise keep trying
    file_ = path;
    std::shared_ptr<VolumeSequence> outputVolumes = std::make_shared<VolumeSequence>();
    gdcm::ImageReader reader;
    reader.SetFileName(file_.string().c_str());
    if (!reader.Read()) {
        MevisVolumeReader mvreader;
        if (mvreader.setFilenames(file_)) {
            LogInfo("This seems to be a MevisLab dcm/tif file - calling the appropriate reader...");
            std::shared_ptr<Volume> v = mvreader.readData(file_);
            outputVolumes->push_back(v);
            return outputVolumes;
        } else {
            throw DataReaderException(fmt::format("could not read input file ({})", path),
                                      IVW_CONTEXT);
        }
    }

    const gdcm::Image& image = reader.GetImage();
    const gdcm::File& file = reader.GetFile();
    std::shared_ptr<Volume> v = generateVolume(image, file);
    if (!v) {
        throw DataReaderException(fmt::format("could not read input file ({})", path), IVW_CONTEXT);
    }

    outputVolumes->push_back(v);

    return outputVolumes;
}

/**
 * TODO: refactor! outdated function but used by mevis reader and volume source
 */
std::shared_ptr<Volume> GdcmVolumeReader::generateVolume(const gdcm::Image& image,
                                                         const gdcm::File& file) {
    const gdcm::PixelFormat& pixelformat = image.GetPixelFormat();

    // Do not use attributes from reader.GetImage() since they might be incorrect
    // if reader.read() failed. Replicate by loading a dcm file from MeVisLab
    // Instead, use the helper functions used by gdcm
    std::vector<double> spacings = gdcm::ImageHelper::GetSpacingValue(file);
    std::vector<double> origin = gdcm::ImageHelper::GetOriginValue(file);
    std::vector<unsigned int> dims = gdcm::ImageHelper::GetDimensionsValue(file);

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

    // create an Inviwo Volume
    const DataFormatBase* format = gdcmutil::getDataFormatBase(image);
    if (!format) {
        throw DataReaderException(fmt::format("unsupported image format in DICOM image: {} ({})",
                                              image.GetPixelFormat().GetScalarTypeAsString(),
                                              file.GetHeader().GetMediaStorageAsString()),
                                  IVW_CONTEXT);
    }

    std::size_t voxelsz = (format->getSize()) * (format->getComponents());
    std::size_t size = glm::compMul(dimension) * voxelsz;

    // sanity check
    std::size_t len = image.GetBufferLength();

    // if gdcm says the volume size is LARGER than we compute - inviwo may crash because
    // the allocated buffer can be too small for image.GetBuffer(destination)
    if (size != len) {
        throw DataReaderException(
            fmt::format(
                "inconsistent format size information: {} byte (Inviwo), {} byte (gdcm) ({})", size,
                len, file.GetHeader().GetMediaStorageAsString()),
            IVW_CONTEXT);
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

    dimension_ = dimension;
    format_ = format;
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

    // TODO: check this heuristics!!!
    if (modality == "CT") {
        // Computed Tomography
        volume->dataMap_.valueAxis = Axis{"Hounsfield", units::unit_from_string("HU")};
        if (format->getPrecision() == 16) {
            volume->dataMap_.dataRange = {0.0, 4095};
        }
    }

    volume->dataMap_.valueRange = volume->dataMap_.dataRange * slope + intercept;

    auto vd = std::make_shared<VolumeDisk>(file_, dimension, format);
    vd->setLoader(new GCDMVolumeRAMLoader(file_, dimension, format));
    volume->addRepresentation(vd);

    return volume;
}

GCDMVolumeRAMLoader::GCDMVolumeRAMLoader(const std::filesystem::path& file, size3_t dimension,
                                         const DataFormatBase* format, bool isPartOfSequence,
                                         dicomdir::Series series)
    : file_(file)
    , dimension_(dimension)
    , format_(format)
    , isPartOfSequence_(isPartOfSequence)
    , series_(series) {}

GCDMVolumeRAMLoader* GCDMVolumeRAMLoader::clone() const { return new GCDMVolumeRAMLoader(*this); }

std::shared_ptr<VolumeRepresentation> GCDMVolumeRAMLoader::createRepresentation(
    const VolumeRepresentation& src) const {

    const std::size_t voxels = dimension_[0] * dimension_[1] * dimension_[2];
    const auto voxelSize = format_->getSize();
    auto data = std::make_unique<char[]>(voxels * voxelSize);

    if (!isPartOfSequence_) {
        gdcm::ImageReader reader;
        reader.SetFileName(file_.string().c_str());
        if (!reader.Read()) {
            throw DataReaderException(fmt::format("could not read file ({})", file_), IVW_CONTEXT);
        }
        const gdcm::Image& image = reader.GetImage();
        image.GetBuffer(data.get());
    } else {
        getVolumeData(series_, static_cast<void*>(data.get()));
    }
    auto volumeRAM =
        createVolumeRAM(src.getDimensions(), src.getDataFormat(), data.get(), src.getSwizzleMask(),
                        src.getInterpolation(), src.getWrapping());
    data.release();

    return volumeRAM;
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
            throw DataReaderException(fmt::format("file cannot be opened ({})", imgInfo.path),
                                      IVW_CONTEXT);
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
                throw DataReaderException(
                    fmt::format("could not read image data ({})", imgInfo.path), IVW_CONTEXT);
            }

            totalByteCount += image.GetBufferLength();
        }
    }
}

void GCDMVolumeRAMLoader::updateRepresentation(std::shared_ptr<VolumeRepresentation> dest,
                                               const VolumeRepresentation&) const {
    if (!isPartOfSequence_) {
        gdcm::ImageReader reader;
        reader.SetFileName(file_.string().c_str());
        if (!reader.Read()) {
            throw DataReaderException(fmt::format("could not read file ({})", file_), IVW_CONTEXT);
        }

        const gdcm::Image& image = reader.GetImage();
        auto volumeDst = std::static_pointer_cast<VolumeRAM>(dest);
        auto data = volumeDst->getData();
        image.GetBuffer(reinterpret_cast<char*>(data));
    } else {
        std::shared_ptr<VolumeRAM> volumeDst = std::static_pointer_cast<VolumeRAM>(dest);
        getVolumeData(series_, volumeDst->getData());
    }
}

}  // namespace inviwo
