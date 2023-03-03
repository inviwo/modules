/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2023 Inviwo Foundation
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

#include <inviwo/core/io/datareaderexception.h>
#include <inviwo/core/util/stringconversion.h>

#include <inviwo/dicom/datastructures/dicomdirtypes.h>

#include <fmt/format.h>

namespace gdcm {
class DataSet;
class Image;
class PixelFormat;
class PhotometricInterpretation;
}  // namespace gdcm

namespace inviwo {

class DataFormatBase;

namespace gdcmutil {

template <int N, typename T>
glm::vec<N, T> toGlmVec(const T* data);

/**
 * extract DICOM tag as string from a gdcm \p dataset
 *
 * @param dataset   tag is extracted from this dataset
 * @param group     DICOM group identifier
 * @param element   DICOM element identifier
 *
 * @see https://dicom.innolitics.com/ciods
 */
IVW_MODULE_DICOM_API std::string getTag(const gdcm::DataSet& dataset, uint16_t group,
                                        uint16_t element);

/**
 * construct a DataFormatBase from the given gdcm \p pixelformat.
 * Might return a nullptr in case of unsupported formats like single bit/monochrome or 12bit data.
 *
 * @param pixelformat    gdcm pixel format
 * @return Inviwo data format matching the gdcm pixel format
 */
IVW_MODULE_DICOM_API const DataFormatBase* getDataFormatBase(const gdcm::PixelFormat& pixelformat);
/**
 * construct a DataFormatBase from the given gdcm \p image.
 * Might return a nullptr in case of unsupported formats like single bit/monochrome or 12bit data.
 *
 * @param image        format information is extracted from this gdcm image
 * @return Inviwo data format matching the format of the image
 *
 * @see getDataFormatBase
 */
IVW_MODULE_DICOM_API const DataFormatBase* getDataFormatBase(const gdcm::Image& image);

/**
 * determine a data range based on the given \p pixelformat
 *
 * @param pixelformat   for integer types, the range is determined by the bits used,
 *                      e.g. 12bit of 16bit
 * @return data range for the pixel format
 */
IVW_MODULE_DICOM_API dvec2 getDataRange(const gdcm::PixelFormat& pixelformat);

template <int N, typename T>
glm::vec<N, T> toGlmVec(const T* data) {
    glm::vec<N, T> result{0};
    for (int i = 0; i < N; ++i) {
        result[i] = data[i];
    }
    return result;
}

}  // namespace gdcmutil

}  // namespace inviwo
