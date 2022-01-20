/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2021 Inviwo Foundation
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

#include <inviwo/dicom/datastructures/dicomdirtypes.h>
#include <inviwo/dicom/utils/gdcmutils.h>

#include <inviwo/core/io/datareaderexception.h>
#include <inviwo/core/util/stdextensions.h>
#include <inviwo/core/util/filesystem.h>

#include <fmt/format.h>

#include <warn/push>
#include <warn/ignore/all>
#include <DataStructureAndEncodingDefinition/gdcmDataSet.h>
#include <DataStructureAndEncodingDefinition/gdcmFile.h>
#include <MediaStorageAndFileFormat/gdcmImage.h>

#include <MediaStorageAndFileFormat/gdcmImageHelper.h>
#include <MediaStorageAndFileFormat/gdcmImageReader.h>
#include <DataStructureAndEncodingDefinition/gdcmAttribute.h>
#include <DataStructureAndEncodingDefinition/gdcmMediaStorage.h>
#include <warn/pop>

#include <algorithm>

namespace inviwo {

namespace dicomdir {

bool Image::empty() const { return glm::compMul(dims) == 0; }

void Image::updateInfo(const gdcm::DataSet& dataset) {
    windowCenter = gdcmutil::getTag(dataset, 0x0028, 0x1050);
    windowWidth = gdcmutil::getTag(dataset, 0x0028, 0x1051);

    // slice thickness
    gdcm::Attribute<0x0018, 0x0050> sliceThicknessAttr{0.0};
    sliceThicknessAttr.Set(dataset);
    sliceThickness = sliceThicknessAttr.GetValue();

    // Patient Orientation https://dicom.innolitics.com/ciods/ct-image/image-plane/00200037
    gdcm::Attribute<0x0020, 0x0037> orientationAttr{{1.0, 0.0, 0.0, 0.0, 1.0, 0.0}};
    orientationAttr.Set(dataset);
    const auto v = orientationAttr.GetValues();
    orientationX = gdcmutil::toGlmVec<3>(v);
    orientationY = gdcmutil::toGlmVec<3>(v + 3);

    // Patient Position https://dicom.innolitics.com/ciods/ct-image/image-plane/00200032
    gdcm::Attribute<0x0020, 0x0032> positionAttr{{0.0, 0.0, 0.0}};
    positionAttr.Set(dataset);
    origin = gdcmutil::toGlmVec<3>(positionAttr.GetValues());

    // Pixel Spacing https://dicom.innolitics.com/ciods/ct-image/image-plane/00280030
    gdcm::Attribute<0x0028, 0x0030> pixelSpacingAttr{{1.0, 1.0}};
    pixelSpacingAttr.Set(dataset);
    pixelSpacing = gdcmutil::toGlmVec<3>(pixelSpacingAttr.GetValues());

    updateZpos();
}

void Image::updateInfo(const gdcm::ImageReader& reader) {
    const auto& file = reader.GetFile();

    windowCenter = gdcmutil::getTag(file.GetDataSet(), 0x0028, 0x1050);
    windowWidth = gdcmutil::getTag(file.GetDataSet(), 0x0028, 0x1051);

    // slice thickness
    gdcm::Attribute<0x0018, 0x0050> sliceThicknessAttr{0.0};
    sliceThicknessAttr.Set(file.GetDataSet());
    sliceThickness = sliceThicknessAttr.GetValue();

    // patient orientation, position, dimensions, etc. are stored in different tags depending on the
    // MediaStorage type. That is, updateInfo(gdcm::DataSet&) will not work for all supported files.
    //
    // Using gdcm::ImageHelper since it encapsulates the various lookups

    auto dimVec = gdcm::ImageHelper::GetDimensionsValue(file);
    dims = size3_t{gdcmutil::toGlmVec<3>(dimVec.data())};

    auto cosinesVec = gdcm::ImageHelper::GetDirectionCosinesValue(file);
    orientationX = gdcmutil::toGlmVec<3>(cosinesVec.data());
    orientationY = gdcmutil::toGlmVec<3>(cosinesVec.data() + 3);

    auto originVec = gdcm::ImageHelper::GetOriginValue(file);
    origin = gdcmutil::toGlmVec<3>(originVec.data());

    auto spacingVec = gdcm::ImageHelper::GetSpacingValue(file);
    pixelSpacing = gdcmutil::toGlmVec<3>(spacingVec.data());

    updateZpos();
}

void Image::updateZpos() {
    // see https://nipy.org/nibabel/dicom/spm_dicom.html#spm-volume-sorting
    zPos = glm::dot(origin, glm::cross(orientationX, orientationY));
}

namespace {

struct ImageMetaData {
    ImageMetaData() = default;
    ImageMetaData(const gdcm::ImageReader& reader, const dicomdir::Image& imgInfo) {
        set(reader, imgInfo);
    }

    void set(const gdcm::ImageReader& reader, const dicomdir::Image& imgInfo) {
        dims = size2_t{imgInfo.dims};
        pixelSpacing = imgInfo.pixelSpacing;
        orientation[0] = imgInfo.orientationX;
        orientation[1] = imgInfo.orientationY;
        origin = imgInfo.origin;

        const gdcm::File& file = reader.GetFile();

        photometric = gdcm::ImageHelper::GetPhotometricInterpretationValue(file);
        pixelformat = gdcm::ImageHelper::GetPixelFormatValue(file);
        auto interceptSlopeVec = gdcm::ImageHelper::GetRescaleInterceptSlopeValue(file);
        intercept = interceptSlopeVec[0];
        slope = interceptSlopeVec[1];
    }

    size2_t dims;
    gdcm::PixelFormat pixelformat;
    gdcm::PhotometricInterpretation photometric;
    double intercept;
    double slope;
    dvec3 pixelSpacing;
    dvec3 orientation[2];
    dvec3 origin;
};

}  // namespace

Series::Series(const std::string& description) : desc{description} {}

void Series::updateImageInformation(const std::string& dicompath) {

    // According to the standard the following parameters should be the same within a
    // series:
    //   * dimensions
    //   * pixel format
    //   * color space (i.e. Photometric Interpretation) containing  no. of samples per
    //   pixel
    //   * intercept and slope
    //   * pixel spacing
    //   * image orientation (patient) with summed squared difference < 1e-4
    //   * image position (patient) with summed squared difference < 1e-4
    bool warnSlopeIntercept = false;
    bool warnPixelSpacing = false;
    bool warnOrientation = false;
    bool warnOrigin = false;
    auto sanityCheck = [&](const ImageMetaData& ref, const ImageMetaData& img) {
        if (ref.dims != img.dims) {
            throw DataReaderException(
                fmt::format(
                    "image sizes differ in DICOM series '{}', expected {} but found {} ('{}')",
                    desc, toString(ref.dims), toString(img.dims), dicompath),
                IVW_CONTEXT);
        }
        if (ref.pixelformat != img.pixelformat) {
            throw DataReaderException(
                fmt::format("pixel formats differ in DICOM series '{}' ('{}')", desc, dicompath),
                IVW_CONTEXT);
        }
        if (!ref.photometric.IsSameColorSpace(img.photometric)) {
            throw DataReaderException(
                fmt::format("photometric info differ in DICOM series '{}' ('{}')", desc, dicompath),
                IVW_CONTEXT);
        }
        const double dicomDelta = 1.0e-4;
        if (std::abs(ref.slope - img.slope) > dicomDelta ||
            std::abs(ref.intercept - img.intercept) < dicomDelta) {
            warnSlopeIntercept = true;
        }

        auto ssd = [](auto vec) {  // compute squared sums
            return glm::dot(vec, vec);
        };

        if (ssd(ref.pixelSpacing - img.pixelSpacing) > dicomDelta) {
            warnPixelSpacing = true;
        }
        if ((ssd(ref.orientation[0] - img.orientation[0]) > dicomDelta) ||
            (ssd(ref.orientation[1] - img.orientation[1]) > dicomDelta)) {
            warnOrientation = true;
        }
        if (ssd(ref.origin - img.origin) > dicomDelta) {
            warnOrigin = true;
        }
    };

    bool first = true;
    ImageMetaData refImage;
    for (auto& imgInfo : images) {
        gdcm::ImageReader imageReader;
        auto imageInputStream = filesystem::ifstream(imgInfo.path, std::ios::binary);
        imageReader.SetStream(imageInputStream);

        if (!imageInputStream.is_open() || !imageReader.CanRead()) {
            imgInfo.dims = {0, 0, 0};
            continue;
        }
        if (!imageReader.Read()) {
            throw DataReaderException(
                fmt::format("could not read image '{}' in DICOM series '{}' ('{}')", imgInfo.path,
                            desc, dicompath),
                IVW_CONTEXT);
        }

        imgInfo.updateInfo(imageReader);

        if (first) {
            first = false;
            refImage.set(imageReader, imgInfo);
        } else {
            sanityCheck(refImage, ImageMetaData{imageReader, imgInfo});
        }
    }

    if (warnSlopeIntercept) {
        LogWarn(
            fmt::format("varying slopes/intercepts in DICOM series '{}' ('{}')", desc, dicompath));
    }
    if (warnPixelSpacing) {
        LogWarn(
            fmt::format("pixel spacings differ in DICOM series '{}', "
                        "expected {} but found {} ('{}')",
                        desc, toString(refImage.pixelSpacing), dicompath));
    }
    if (warnOrientation) {
        LogWarn(
            fmt::format("image orientations differ in DICOM series '{}' ('{}')", desc, dicompath));
    }
    if (warnOrigin) {
        LogWarn(fmt::format("origins differ in DICOM series '{}' ('{}')", desc, dicompath));
    }

    util::erase_remove_if(images, [](dicomdir::Image& image) { return image.empty(); });

    // determine stack height, i.e. number of slices/images
    const size_t numslices =
        std::accumulate(images.begin(), images.end(), size_t{0},
                        [](size_t sum, const auto& item) { return sum + item.dims.z; });

    // update series information
    dims = size3_t{refImage.dims, numslices};
    pixelSpacing = refImage.pixelSpacing;
    intercept = refImage.intercept;
    slope = refImage.slope;
    pixelformat = refImage.pixelformat;
}

void Series::sortImages() {
    // sort images by slice position in patient coords
    //
    // TODO: proper volume sorting according to
    // https://nipy.org/nibabel/dicom/spm_dicom.html#spm-volume-sorting
    std::sort(images.begin(), images.end(),
              [](const auto& a, const auto& b) { return a.zPos > b.zPos; });
}

bool Series::empty() const { return images.empty(); }

}  // namespace dicomdir

}  // namespace inviwo
