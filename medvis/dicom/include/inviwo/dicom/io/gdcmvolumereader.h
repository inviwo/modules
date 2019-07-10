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

#pragma once

#include <inviwo/dicom/dicommoduledefine.h>
#include <inviwo/core/common/inviwocoredefine.h>
#include <inviwo/core/common/inviwo.h>

#include <inviwo/core/datastructures/data.h>
#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/datastructures/volume/volumedisk.h>
#include <inviwo/core/datastructures/volume/volumeramprecision.h>

#include <inviwo/core/io/datareader.h>
#include <inviwo/core/io/datareaderexception.h>

#include <warn/push>
#include <warn/ignore/all>
#include <MediaStorageAndFileFormat/gdcmImageReader.h>
#include <warn/pop>

#include <string>
#include <vector>

namespace inviwo {

struct DICOMDIRImage {
    std::string path = "";
    std::string windowCenter = "";
    std::string windowWidth = "";
    std::string sliceThickness = "";
    std::string orientationPatient = "";
    std::string positionPatient = "";
    float zPos = -std::numeric_limits<float>::infinity();  // relative slice position, used to
                                                            // sort slices in volume
};

struct DICOMDIRSeries {
    std::string desc;
    std::string modality = "CT";  // e.g. "CT", "MR"...
    double slope = 1.0;
    double intercept = 0.0;
    std::vector<DICOMDIRImage> images;

    // Icon images from series: see tag (0088,0200)
};

struct DICOMDIRStudy {
    std::string date;
    std::string desc;
    std::vector<DICOMDIRSeries> series;
};

struct DICOMDIRPatient {
    std::string patientName;
    std::string patientId;
    std::vector<DICOMDIRStudy> studies;
};

using SharedVolumeSequence = std::shared_ptr<VolumeSequence>;
using SharedVolume = std::shared_ptr<Volume>;

class IVW_MODULE_DICOM_API GdcmVolumeReader : public DataReaderType<VolumeSequence> {
public:
    GdcmVolumeReader();
    virtual GdcmVolumeReader* clone() const;
    virtual ~GdcmVolumeReader() = default;

    /**
     * Old function that tries to read single volume from single file.
     * Apparently this can be applied to the "mevis" format.
     */
    SharedVolume generateVolume(const gdcm::Image& image, const gdcm::File& file);
    const DataFormatBase* getFormat() { return format_; }
    size3_t getDimension() { return dimension_; }

    /**
     * Entry point of the reader, called from VolumeSource processor
     */
    virtual std::shared_ptr<VolumeSequence> readData(const std::string& filePath);

private:
    /**
     * Try to read all volumes contained in given path using standard DICOMDIR format
     */
    static SharedVolumeSequence tryReadDICOMDIR(const std::string& fileOrDirectory);

    /**
     * Non-recursive version of tryReadDICOMsequenceRecursive
     */
    static SharedVolumeSequence tryReadDICOMsequence(const std::string& sequenceDirectory);

    /**
     * Tries to read all volumes contained in given directory path, including subdirectories.
     * Looks only at all the image files and ignores possibly existing DIOCMDIR.
     */
    static SharedVolumeSequence tryReadDICOMsequenceRecursive(const std::string& directory);

    /**
     * Creates inviwo volume handle from DICOM series on disk.
     * Only metadata, no actual voxels are returned.
     */
    static SharedVolume getVolumeDescription(DICOMDIRSeries& series);

    std::string file_;
    const DataFormatBase* format_;
    size3_t dimension_;
    SharedVolumeSequence volumes_;
};

/**
 * \brief A loader for dcm files. Used to create VolumeRAM representations.
 * This class us used by the GdcmVolumeReader.
 *
 * Also when the VolumeSource processor calls volume->getRepresentation<VolumeRAM>() this class gets
 active
 * and loads the actual voxels into RAM.

 * Remember that when you want to read "values" (e.g. in Hounsfield units) from the volume you have
 to do that through the data mapper.
 * This is how it works assuming you have the handle to "volume" and the filled RAM representation
 "volumeRAM":
 * LogInfo("value=" << volume->dataMap_.mapFromDataToValue(volumeRAM->getAsDouble({ 119, 296, 0
 })));
 */
class IVW_MODULE_DICOM_API GCDMVolumeRAMLoader
    : public DiskRepresentationLoader<VolumeRepresentation> {
public:
    GCDMVolumeRAMLoader(std::string file, size3_t dimension, const DataFormatBase* format,
                        bool isPartOfSequence = false, DICOMDIRSeries series = {});
    virtual GCDMVolumeRAMLoader* clone() const override;
    virtual ~GCDMVolumeRAMLoader() = default;

    virtual std::shared_ptr<VolumeRepresentation> createRepresentation() const override;
    virtual void updateRepresentation(std::shared_ptr<VolumeRepresentation> dest) const override;

    using type = std::shared_ptr<VolumeRAM>;

    template <class T>
    std::shared_ptr<VolumeRAM> dispatch() const;

private:
    void getVolumeData(const DICOMDIRSeries& series, void* outData) const;  // static here?
    std::string file_;  // only relevant for single volumes
    size3_t dimension_;
    const DataFormatBase* format_;
    bool isPartOfSequence_;
    DICOMDIRSeries series_;
};

}  // namespace inviwo

