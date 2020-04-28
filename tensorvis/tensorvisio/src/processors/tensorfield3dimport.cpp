/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2020 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 *FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#include <fstream>
#include <inviwo/core/datastructures/datamapper.h>
#include <ios>
#include <inviwo/tensorvisio/processors/tensorfield3dimport.h>
#include <unordered_map>
#include <inviwo/tensorvisbase/datastructures/attributes.h>
#include <inviwo/tensorvisbase/tensorvisbasemodule.h>
#include <inviwo/core/util/constexprhash.h>
#include <inviwo/tensorvisio/util/util.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming
// scheme
const ProcessorInfo TensorField3DImport::processorInfo_{
    "org.inviwo.TensorField3DImport",  // Class identifier
    "Tensor Field 3D Import",          // Display name
    "Data Input",                      // Category
    CodeState::Experimental,           // Code state
    tag::OpenTensorVis | Tag::CPU,     // Tags
};
const ProcessorInfo TensorField3DImport::getProcessorInfo() const { return processorInfo_; }

TensorField3DImport::TensorField3DImport()
    : Processor()
    , inFile_("inFile", "File", "", "tensorfield")
    , outport_("outport")
    , normalizeExtents_("normalizeExtents", "Normalize extents", true)
    , extents_("extents", "Extents", vec3(1.f), vec3(0.f), vec3(1000.f), vec3(0.0001f),
               InvalidationLevel::Valid)
    , offset_("offset", "Offset", vec3(1.f), vec3(-1000.f), vec3(1000.f), vec3(0.0001f),
              InvalidationLevel::Valid)
    , dimensions_("dimensions", "Dimensions", ivec3(0), ivec3(0), ivec3(1024), ivec3(1),
                  InvalidationLevel::Valid) {
    addPort(outport_);

    extents_.setReadOnly(true);
    extents_.setCurrentStateAsDefault();

    offset_.setReadOnly(true);
    offset_.setCurrentStateAsDefault();

    dimensions_.setReadOnly(true);
    dimensions_.setCurrentStateAsDefault();

    addProperties(inFile_, normalizeExtents_, extents_, offset_, dimensions_);

    inFile_.onChange([this]() { invalidate(InvalidationLevel::InvalidOutput); });

    inFile_.set(InviwoApplication::getPtr()->getModuleByType<TensorVisBaseModule>()->getPath(
                    ModulePath::Data) +
                "/tensorfields/two_point_load.tfb");
}

void TensorField3DImport::initializeResources() {}

void TensorField3DImport::process() {
    std::ifstream inFile(inFile_.get(), std::ios::in | std::ios::binary);

    if (!inFile) {
        LogError("Couldn't open file");
        return;
    }

    size_t version;
    size3_t dimensions;
    auto extents = vec3{};
    auto offset = vec3{};
    glm::uint8 hasMetaData;
    std::array<DataMapper, 3> dataMapperEigenValues;
    std::array<DataMapper, 3> dataMapperEigenVectors;
    std::vector<glm::uint8> mask;

    std::string versionStr;
    size_t size;
    inFile.read(reinterpret_cast<char *>(&size), sizeof(size_t));
    versionStr.resize(size);
    inFile.read(&versionStr[0], size);

    if (versionStr != "TFBVersion:") {
        LogError("No valid tfb file!");
        return;
    }

    inFile.read(reinterpret_cast<char *>(&version), sizeof(size_t));

    if (version < TFB_CURRENT_VERSION) {
        LogError("Please update the tfb file.");
        LogError("Current version is " << TFB_CURRENT_VERSION << ", file has " << version);
        return;
    }

    inFile.read(reinterpret_cast<char *>(&hasMetaData), sizeof(glm::uint8));

    inFile.read(reinterpret_cast<char *>(&dimensions.x), sizeof(size_t));
    inFile.read(reinterpret_cast<char *>(&dimensions.y), sizeof(size_t));
    inFile.read(reinterpret_cast<char *>(&dimensions.z), sizeof(size_t));

    // Read the extents

    inFile.read(reinterpret_cast<char *>(&extents.x), sizeof(float));
    inFile.read(reinterpret_cast<char *>(&extents.y), sizeof(float));
    inFile.read(reinterpret_cast<char *>(&extents.z), sizeof(float));

    inFile.read(reinterpret_cast<char *>(&offset.x), sizeof(float));
    inFile.read(reinterpret_cast<char *>(&offset.y), sizeof(float));
    inFile.read(reinterpret_cast<char *>(&offset.z), sizeof(float));

    // Read the data maps

    inFile.read(reinterpret_cast<char *>(&dataMapperEigenValues[0].dataRange), sizeof(double) * 2);
    inFile.read(reinterpret_cast<char *>(&dataMapperEigenValues[1].dataRange), sizeof(double) * 2);
    inFile.read(reinterpret_cast<char *>(&dataMapperEigenValues[2].dataRange), sizeof(double) * 2);
    dataMapperEigenValues[0].valueRange = dataMapperEigenValues[0].dataRange;
    dataMapperEigenValues[1].valueRange = dataMapperEigenValues[1].dataRange;
    dataMapperEigenValues[2].valueRange = dataMapperEigenValues[2].dataRange;

    inFile.read(reinterpret_cast<char *>(&dataMapperEigenVectors[0].dataRange), sizeof(double) * 2);
    inFile.read(reinterpret_cast<char *>(&dataMapperEigenVectors[1].dataRange), sizeof(double) * 2);
    inFile.read(reinterpret_cast<char *>(&dataMapperEigenVectors[2].dataRange), sizeof(double) * 2);
    dataMapperEigenVectors[0].valueRange = dataMapperEigenVectors[0].dataRange;
    dataMapperEigenVectors[1].valueRange = dataMapperEigenVectors[1].dataRange;
    dataMapperEigenVectors[2].valueRange = dataMapperEigenVectors[2].dataRange;

    auto numElements = dimensions.x * dimensions.y * dimensions.z;
    auto numValues = numElements * 9;

    std::vector<TensorField3D::value_type> data;
    data.resize(numValues);
    auto dataRaw = data.data();

    inFile.read(reinterpret_cast<char *>(dataRaw), sizeof(TensorField3D::value_type) * numValues);

    auto tensors = std::make_shared<std::vector<TensorField3D::matN>>();
    buildTensors(data, tensors);

    glm::uint8 hasMask;
    inFile.read(reinterpret_cast<char *>(&hasMask), sizeof(glm::uint8));

    if (hasMask) {
        mask.resize(numElements);
        auto maskData = mask.data();
        inFile.read(reinterpret_cast<char *>(maskData), sizeof(glm::uint8) * numElements);
    }

    auto tensorFieldOut = std::make_shared<TensorField3D>(dimensions, tensors);

    tensorFieldOut->dataMapEigenValues_ = dataMapperEigenValues;
    tensorFieldOut->dataMapEigenVectors_ = dataMapperEigenVectors;

    if (normalizeExtents_.get()) {
        extents /= std::max(std::max(extents.x, extents.y), extents.z);
    }

    tensorFieldOut->setExtents(extents);
    tensorFieldOut->setOffset(offset);
    tensorFieldOut->setMask(mask);

    extents_.set(extents);
    offset_.set(offset);
    dimensions_.set(dimensions);

    std::shared_ptr<DataFrame> dataFrame;

    if (hasMetaData) {
        dataFrame = std::make_shared<DataFrame>();
        size_t numMetaDataEntries;
        inFile.read(reinterpret_cast<char *>(&numMetaDataEntries), sizeof(size_t));

        for (size_t i = 0; i < numMetaDataEntries; i++) {
            size_t id{0};
            inFile.read(reinterpret_cast<char *>(&id), sizeof(size_t));
            size_t numItems{0};
            inFile.read(reinterpret_cast<char *>(&numItems), sizeof(size_t));

            util::for_each_type<attributes::types3D>{}(util::DeserializeColumn{}, id,
                                                       std::ref(inFile), dataFrame, numItems);
        }

        dataFrame->updateIndexBuffer();

        tensorFieldOut->setMetaData(dataFrame);
    }

    std::string str;
    inFile.read(reinterpret_cast<char *>(&size), sizeof(size_t));
    str.resize(size);
    inFile.read(&str[0], size);

    if (str != "EOFreached") {
        LogError("EOF not reached");
        return;
    }

    inFile.close();

    outport_.setData(tensorFieldOut);
}

void TensorField3DImport::buildTensors(
    const std::vector<TensorField3D::value_type> &data,
    std::shared_ptr<std::vector<TensorField3D::matN>> tensors) const {
    for (size_t i{0}; i < data.size() / 9; i++) {
        size_t offset = i * 9;
        tensors->emplace_back(
            TensorField3D::vecN(data[offset + 0], data[offset + 3], data[offset + 6]),
            TensorField3D::vecN(data[offset + 1], data[offset + 4], data[offset + 7]),
            TensorField3D::vecN(data[offset + 2], data[offset + 5], data[offset + 8]));
    }
}

}  // namespace inviwo
