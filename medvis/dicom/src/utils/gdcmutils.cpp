/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2024 Inviwo Foundation
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

#include <inviwo/dicom/utils/gdcmutils.h>

#include <warn/push>
#include <warn/ignore/all>
#include <gdcmDataSet.h>
#include <gdcmPixelFormat.h>
#include <gdcmImage.h>
#include <gdcmImageHelper.h>
#include <gdcmImageReader.h>
#include <gdcmAttribute.h>
#include <gdcmMediaStorage.h>
#include <warn/pop>

namespace inviwo {

namespace gdcmutil {

// extract DICOM tag from gdcm dataset as string
std::string getTag(const gdcm::DataSet& dataset, uint16_t group, uint16_t element) {
    const gdcm::Tag tag(group, element);
    if (dataset.FindDataElement(tag)) {
        std::stringstream ss;
        dataset.GetDataElement(tag).GetValue().Print(ss);
        return ss.str();
    }
    return std::string{};
}

const DataFormatBase* getDataFormatBase(const gdcm::PixelFormat& pixelformat) {
    const size_t numchannels = pixelformat.GetSamplesPerPixel();
    switch (pixelformat.GetScalarType()) {
        case gdcm::PixelFormat::UINT8:
            return DataFormatBase::get(NumericType::UnsignedInteger, numchannels, 8);
        case gdcm::PixelFormat::INT8:
            return DataFormatBase::get(NumericType::SignedInteger, numchannels, 8);
        case gdcm::PixelFormat::UINT12:
            return DataFormatBase::get(NumericType::UnsignedInteger, numchannels, 12);
        case gdcm::PixelFormat::INT12:
            return DataFormatBase::get(NumericType::SignedInteger, numchannels, 12);
        case gdcm::PixelFormat::UINT16:
            return DataFormatBase::get(NumericType::UnsignedInteger, numchannels, 16);
        case gdcm::PixelFormat::INT16:
            return DataFormatBase::get(NumericType::SignedInteger, numchannels, 16);
        case gdcm::PixelFormat::UINT32:
            return DataFormatBase::get(NumericType::UnsignedInteger, numchannels, 32);
        case gdcm::PixelFormat::INT32:
            return DataFormatBase::get(NumericType::SignedInteger, numchannels, 32);
        case gdcm::PixelFormat::UINT64:
            return DataFormatBase::get(NumericType::UnsignedInteger, numchannels, 64);
        case gdcm::PixelFormat::INT64:
            return DataFormatBase::get(NumericType::SignedInteger, numchannels, 64);
        case gdcm::PixelFormat::FLOAT16:
            return DataFormatBase::get(NumericType::Float, numchannels, 16);
        case gdcm::PixelFormat::FLOAT32:
            return DataFormatBase::get(NumericType::Float, numchannels, 32);
        case gdcm::PixelFormat::FLOAT64:
            return DataFormatBase::get(NumericType::Float, numchannels, 64);
        case gdcm::PixelFormat::SINGLEBIT:
            return DataFormatBase::get(NumericType::UnsignedInteger, numchannels, 1);
        case gdcm::PixelFormat::UNKNOWN:
            return DataFormatBase::get(NumericType::NotSpecialized, numchannels, 0);
    }
    return nullptr;
}

const DataFormatBase* getDataFormatBase(const gdcm::Image& image) {
    return getDataFormatBase(image.GetPixelFormat());
}

dvec2 getDataRange(const gdcm::PixelFormat& pixelformat) {
    const DataFormatBase* format = getDataFormatBase(pixelformat);
    dvec2 range{0.0, format->getMax()};
    switch (format->getNumericType()) {
        case NumericType::Float:
            range.x = -format->getMax();
            break;
        case NumericType::UnsignedInteger: {
            range.x = 0.0;
            range.y = std::pow(2, pixelformat.GetBitsStored()) - 1;
            break;
        }
        case NumericType::SignedInteger:
            range.x = -std::pow(2, pixelformat.GetBitsStored() - 1);
            range.y = std::pow(2, pixelformat.GetBitsStored() - 1) - 1;
            break;
        case NumericType::NotSpecialized:
            range.x = format->getMin();
            break;
    }
    return range;
}

}  // namespace gdcmutil

}  // namespace inviwo
