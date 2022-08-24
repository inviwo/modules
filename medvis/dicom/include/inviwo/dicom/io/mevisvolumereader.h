/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2014-2021 Inviwo Foundation
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

#include <inviwo/core/io/datareader.h>
#include <inviwo/core/io/datareaderexception.h>
#include <inviwo/core/datastructures/data.h>
#include <inviwo/core/datastructures/volume/volumedisk.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/core/datastructures/volume/volumerepresentation.h>

#include <functional>

namespace inviwo {

class IVW_MODULE_DICOM_API MevisVolumeReader : public DataReaderType<Volume> {
public:
    MevisVolumeReader();
    virtual MevisVolumeReader* clone() const override;
    virtual ~MevisVolumeReader() = default;

    // interface gdcm - mevis reader
    bool setFilenames(std::string_view filePath);

    virtual std::shared_ptr<Volume> readData(const std::string_view filePath) override;

private:
    std::string dcm_file_;
    std::string tif_file_;
    const DataFormatBase* format;
    size3_t dimension_;
};
/**
 * \brief A loader for dcm files. Used to create VolumeRAM representations.
 * This class us used by the GdcmVolumeReader.
 */
class IVW_MODULE_DICOM_API MevisVolumeRAMLoader
    : public DiskRepresentationLoader<VolumeRepresentation> {
public:
    MevisVolumeRAMLoader(std::string file, size3_t dimension, const DataFormatBase* format);
    virtual MevisVolumeRAMLoader* clone() const override;
    virtual ~MevisVolumeRAMLoader() = default;

    virtual std::shared_ptr<VolumeRepresentation> createRepresentation(
        const VolumeRepresentation&) const override;
    virtual void updateRepresentation(std::shared_ptr<VolumeRepresentation> dest,
                                      const VolumeRepresentation&) const override;

    template <typename Result, typename T>
    std::shared_ptr<VolumeRAM> operator()() const {
        typedef typename T::type F;

        const std::size_t size = glm::compMul(dimension_);
        auto data = util::make_unique<F[]>(size);
        if (!data) {
            throw DataReaderException(
                "Error: Could not allocate memory for loading mevis volume data: " + tif_file_,
                IVW_CONTEXT);
        }

        readDataInto(reinterpret_cast<char*>(data.get()));
        auto repr = std::make_shared<VolumeRAMPrecision<F>>(data.get(), dimension_);
        data.release();
        return repr;
    }

private:
    void readDataInto(void* destination) const;
    std::string tif_file_;
    size3_t dimension_;
    const DataFormatBase* format_;
};

}  // namespace inviwo
