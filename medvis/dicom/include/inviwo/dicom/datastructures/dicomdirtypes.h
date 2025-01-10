/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2025 Inviwo Foundation
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
#include <inviwo/core/common/inviwo.h>

#include <vector>
#include <ostream>
#include <filesystem>

#include <warn/push>
#include <warn/ignore/all>
#include <gdcmPixelFormat.h>
#include <warn/pop>

namespace gdcm {
class DataSet;
class ImageReader;
}  // namespace gdcm

namespace inviwo {

namespace dicomdir {

struct IVW_MODULE_DICOM_API Image {
    std::string path = "";
    std::string windowCenter = "";
    std::string windowWidth = "";
    double sliceThickness = 0.0;  // in mm

    size3_t dims{0u};  // 0 indicates empty image
    dvec3 orientationX{1.0, 0.0, 0.0};
    dvec3 orientationY{0.0, 1.0, 0.0};
    dvec3 origin{0.0};        // in mm
    dvec3 pixelSpacing{0.0};  // in mm

    bool empty() const;

    double zPos = -std::numeric_limits<float>::infinity();  // relative slice position, used to
                                                            // sort slices in volume

    /**
     * update the image info from corresponding tags in the gdcm \p dataset
     *
     * @param dataset    dataset associated with the image
     */
    void updateInfo(const gdcm::DataSet& dataset);
    /**
     * update the image info from corresponding tags in the gdcm dataset, which is associated with
     * the \p reader
     *
     * @param reader     reader used to load the image
     */
    void updateInfo(const gdcm::ImageReader& reader);

    /**
     * compute z coordinate of the image slice
     * @see https://nipy.org/nibabel/dicom/spm_dicom.html#spm-volume-sorting
     */
    void updateZpos();
};

struct IVW_MODULE_DICOM_API Series {
    Series() = default;
    Series(const std::string& description);

    /**
     * try to read every image of the \p series and update its metadata (image dimension, origin,
     * etc.). The following series properties will be updated accordingly: dimensions, pixel format,
     * slope, and intercept.
     *
     * Also applies sanity checks across all images of the DICOM series. According to the
     * standard the following parameters should be the same for all images within the series:
     *   * dimensions
     *   * pixel format
     *   * color space (i.e. Photometric Interpretation) containing  no. of samples per pixel
     *   * intercept and slope
     *   * pixel spacing
     *   * image orientation (patient) with summed squared difference < 1e-4
     *   * image position (patient) with summed squared difference < 1e-4
     *
     * @param series     series containing images with valid paths
     * @param dicompath  used only in case of an error and should refer to the main DICOM dataset
     * @throws DataReaderException if sanity check fails.
     * @see dicomdir::Image
     */
    void updateImageInformation(const std::filesystem::path& dicompath = std::filesystem::path{});

    /**
     * sort images by slice position in patient coords (zPos)
     *
     * TODO: proper volume sorting with more heuristics according to
     * https://nipy.org/nibabel/dicom/spm_dicom.html#spm-volume-sorting
     */
    void sortImages();

    bool empty() const;

    std::string desc;
    std::string modality = "CT";  // e.g. "CT", "MR"...
    double slope = 1.0;
    double intercept = 0.0;
    std::vector<Image> images;
    size3_t dims{0};
    dvec3 pixelSpacing{0.0};  // in mm
    gdcm::PixelFormat pixelformat;
    // Icon images from series: see tag (0088,0200)
};

struct IVW_MODULE_DICOM_API Study {
    std::string date;
    std::string desc;
    std::vector<Series> series;
};

struct IVW_MODULE_DICOM_API Patient {
    std::string patientName;
    std::string patientId;
    std::vector<Study> studies;
};

}  // namespace dicomdir

template <class Elem, class Traits>
std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& ss,
                                             const dicomdir::Patient& patient) {
    ss << "[ DICOM Patient"
       << "\n  Name: " << patient.patientName << "\n  ID:   " << patient.patientId
       << "\n  No. Studies: " << patient.studies.size() << " ]";
    return ss;
}

template <class Elem, class Traits>
std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& ss,
                                             const dicomdir::Study& study) {
    ss << "[ DICOM Study"
       << "\n  Desc.: " << study.desc << "\n  Date: " << study.date
       << "\n  No. Series: " << study.series.size();
    return ss;
}

template <class Elem, class Traits>
std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& ss,
                                             const dicomdir::Series& series) {
    ss << "[ DICOM Series"
       << "\n  Desc.: " << series.desc << "\n  Modality: " << series.modality
       << "\n  No. Images: " << series.images.size() << "\n  Dimensions: " << series.dims << " ]";
    return ss;
}

template <class Elem, class Traits>
std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& ss,
                                             const dicomdir::Image& image) {
    // clang-format off
    ss << "[ DICOM Image" << "\n  Path:         " << image.path
       << "\n  OrientationX: " << image.orientationX
       << "\n  OrientationY: " << image.orientationY
       << "\n  Origin:       " << image.origin
       << "\n  Thickness:    " << image.sliceThickness 
       << "\n  z pos:        " << image.zPos
       << "\n  Window (center, width): " << image.windowCenter << ", " << image.windowWidth << " ]";
    // clang-format on
    return ss;
}

}  // namespace inviwo
