/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2017-2020 Inviwo Foundation
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

#include <inviwo/tensorvisio/processors/tensorfield2dimport.h>
#include <inviwo/tensorvisio/util/util.h>
#include <inviwo/tensorvisbase/tensorvisbasemodule.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorField2DImport::processorInfo_{
    "org.inviwo.TensorField2DImport",  // Class identifier
    "Tensor Field 2D Import",          // Display name
    "Data Input",                      // Category
    CodeState::Experimental,           // Code state
    tag::OpenTensorVis | Tag::CPU,     // Tags
};
const ProcessorInfo TensorField2DImport::getProcessorInfo() const { return processorInfo_; }

TensorField2DImport::TensorField2DImport()
    : Processor()
    , inFile_("inFile", "File", "")
    , outport_("outport")
    , extents_("extends", "Extends", vec2(1.f), vec2(0.f), vec2(1000.f), vec2(0.0001f))
    , offset_("offset", "Offset", vec2(1.f), vec2(-1000.f), vec2(1000.f), vec2(0.0001f),
              InvalidationLevel::Valid)
    , dimensions_("dimensions", "Dimensions", ivec2(0), ivec2(0), ivec2(1024), ivec2(1),
                  InvalidationLevel::Valid) {
    addPort(outport_);

    addProperty(inFile_);
    extents_.setReadOnly(true);
    extents_.setCurrentStateAsDefault();

    offset_.setReadOnly(true);
    offset_.setCurrentStateAsDefault();

    dimensions_.setReadOnly(true);
    dimensions_.setCurrentStateAsDefault();

    addProperties(extents_, offset_, dimensions_);
}

void TensorField2DImport::process() {

    std::ifstream inFile(inFile_.get(), std::ios::in | std::ios::binary);

    if (!inFile) {
        LogError("Couldn't open file");
        return;
    }

    size_t version;
    size2_t dimensions{};
    auto extents = vec2{};
    auto offset = vec2{};
    glm::uint8 hasMetaData;
    std::array<DataMapper, 2> dataMapperEigenValues;
    std::array<DataMapper, 2> dataMapperEigenVectors;

    std::string versionStr;
    size_t size;
    inFile.read(reinterpret_cast<char*>(&size), sizeof(size_t));
    versionStr.resize(size);
    inFile.read(&versionStr[0], size);

    if (versionStr != "TFBVersion:") {
        LogError("No valid tfb file!");
        return;
    }

    inFile.read(reinterpret_cast<char*>(&version), sizeof(size_t));

    if (version < TFB_CURRENT_VERSION) {
        LogError("Please update the tfb file.");
        LogError("Current version is " << TFB_CURRENT_VERSION << ", file has " << version);
        return;
    }

    inFile.read(reinterpret_cast<char*>(&hasMetaData), sizeof(glm::uint8));

    inFile.read(reinterpret_cast<char*>(&dimensions.x), sizeof(size_t));
    inFile.read(reinterpret_cast<char*>(&dimensions.y), sizeof(size_t));

    // Read the extents

    inFile.read(reinterpret_cast<char*>(&extents.x), sizeof(float));
    inFile.read(reinterpret_cast<char*>(&extents.y), sizeof(float));

    inFile.read(reinterpret_cast<char*>(&offset.x), sizeof(float));
    inFile.read(reinterpret_cast<char*>(&offset.y), sizeof(float));

    // Read the data maps

    inFile.read(reinterpret_cast<char*>(&dataMapperEigenValues[0].dataRange), sizeof(double) * 2);
    inFile.read(reinterpret_cast<char*>(&dataMapperEigenValues[1].dataRange), sizeof(double) * 2);
    dataMapperEigenValues[0].valueRange = dataMapperEigenValues[0].dataRange;
    dataMapperEigenValues[1].valueRange = dataMapperEigenValues[1].dataRange;

    inFile.read(reinterpret_cast<char*>(&dataMapperEigenVectors[0].dataRange), sizeof(double) * 2);
    inFile.read(reinterpret_cast<char*>(&dataMapperEigenVectors[1].dataRange), sizeof(double) * 2);
    dataMapperEigenVectors[0].valueRange = dataMapperEigenVectors[0].dataRange;
    dataMapperEigenVectors[1].valueRange = dataMapperEigenVectors[1].dataRange;

    auto numElements = dimensions.x * dimensions.y;
    auto numValues = numElements * 4;

    std::vector<TensorField2D::value_type> data;
    data.resize(numValues);
    auto dataRaw = data.data();

    inFile.read(reinterpret_cast<char*>(dataRaw), sizeof(TensorField2D::value_type) * numValues);

    auto tensors = std::make_shared<std::vector<TensorField2D::matN>>();
    buildTensors(data, tensors);

    auto tensorFieldOut = std::make_shared<TensorField2D>(dimensions, tensors);

    tensorFieldOut->dataMapEigenValues_ = dataMapperEigenValues;
    tensorFieldOut->dataMapEigenVectors_ = dataMapperEigenVectors;

    tensorFieldOut->setExtents(extents);
    tensorFieldOut->setOffset(offset);

    extents_.set(extents);
    offset_.set(offset);
    dimensions_.set(dimensions);

    std::shared_ptr<DataFrame> dataFrame;

    if (hasMetaData) {
        dataFrame = std::make_shared<DataFrame>();
        size_t numMetaDataEntries;
        inFile.read(reinterpret_cast<char*>(&numMetaDataEntries), sizeof(size_t));

        for (size_t i = 0; i < numMetaDataEntries; i++) {
            size_t id{0};
            inFile.read(reinterpret_cast<char*>(&id), sizeof(size_t));
            size_t numItems{0};
            inFile.read(reinterpret_cast<char*>(&numItems), sizeof(size_t));

            util::for_each_type<attributes::types2D>{}(util::DeserializeColumn{}, id,
                                                       std::ref(inFile), dataFrame, numItems);
        }

        dataFrame->updateIndexBuffer();

        tensorFieldOut->setMetaData(dataFrame);
    }

    std::string str;
    inFile.read(reinterpret_cast<char*>(&size), sizeof(size_t));
    str.resize(size);
    inFile.read(&str[0], size);

    if (str != "EOFreached") {
        LogError("EOF not reached");
        return;
    }

    inFile.close();

    outport_.setData(tensorFieldOut);
}

void TensorField2DImport::buildTensors(
    const std::vector<TensorField2D::value_type>& data,
    std::shared_ptr<std::vector<TensorField2D::matN>> tensors) const {
    for (size_t i{0}; i < data.size() / 4; i++) {
        size_t offset = i * 4;
        tensors->emplace_back(TensorField2D::vecN(data[offset + 0], data[offset + 2]),
                              TensorField2D::vecN(data[offset + 1], data[offset + 3]));
    }
}

}  // namespace inviwo
