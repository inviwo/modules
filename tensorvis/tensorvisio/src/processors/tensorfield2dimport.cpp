/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2017-2024 Inviwo Foundation
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

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorField2DImport::processorInfo_{
    "org.inviwo.TensorField2DImport",  // Class identifier
    "Tensor Field 2D Import",          // Display name
    "Tensor Field IO",                 // Category
    CodeState::Experimental,           // Code state
    Tags::None,                        // Tags
};
const ProcessorInfo TensorField2DImport::getProcessorInfo() const { return processorInfo_; }

TensorField2DImport::TensorField2DImport()
    : Processor()
    , inFile_("inFile", "File", "")
    , outport_("outport")
    , extents_("", "", vec3(1.f), vec3(0.f), vec3(1000.f), vec3(0.0001f)) {
    addPort(outport_);

    addProperty(inFile_);
    extents_.setReadOnly(true);
    extents_.setCurrentStateAsDefault();
    addProperty(extents_);
}

void TensorField2DImport::buildTensors(const std::vector<double>& data,
                                       std::vector<dmat2>& tensors) const {
    for (size_t i{0}; i < data.size() / 3; i++) {
        size_t offset = i * 3;
        tensors.emplace_back(dvec2(data[offset], data[offset + 2]),
                             dvec2(data[offset + 2], data[offset + 1]));
    }
}

void TensorField2DImport::process() {
    std::ifstream inFile(inFile_.get(), std::ios::in | std::ios::binary);

    if (!inFile) {
        LogError("Couldn't open file");
        return;
    }

    size_t version;
    size2_t dimensions;
    auto extents = dvec2(1.0);
    size_t rank;
    size_t dimensionality;
    bool hasEigenInfo;

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

    if (version < 2) {
        LogWarn("Please update the tfb file.")
    }

    inFile.read(reinterpret_cast<char*>(&dimensionality), sizeof(size_t));
    inFile.read(reinterpret_cast<char*>(&rank), sizeof(size_t));
    inFile.read(reinterpret_cast<char*>(&hasEigenInfo), sizeof(bool));

    if (dimensionality == 2) {
        inFile.read(reinterpret_cast<char*>(&dimensions.x), sizeof(size_t));
        inFile.read(reinterpret_cast<char*>(&dimensions.y), sizeof(size_t));
    }

    if (dimensionality == 3) {
        LogError("The loaded file is a 3D tensor field. Aborting.");
        return;
    }

    if (version > 1) {
        inFile.read(reinterpret_cast<char*>(&extents.x), sizeof(double));
        inFile.read(reinterpret_cast<char*>(&extents.y), sizeof(double));
    }

    auto numElements = dimensions.x * dimensions.y;
    auto numValues = numElements * 3;

    std::vector<double> data;
    data.resize(numValues);
    auto dataRaw = data.data();

    std::vector<double> majorEigenvalues;
    majorEigenvalues.resize(numElements);
    auto majorEigenValuesRaw = majorEigenvalues.data();

    std::vector<double> minorEigenvalues;
    minorEigenvalues.resize(numElements);
    auto minorEigenvaluesRaw = minorEigenvalues.data();

    std::vector<dvec2> majorEigenvectors;
    majorEigenvectors.resize(numElements);
    auto majorEigenvectorsRaw = majorEigenvectors.data();

    std::vector<dvec2> minorEigenvectors;
    minorEigenvectors.resize(numElements);
    auto minorEigenvectorsRaw = minorEigenvectors.data();

    inFile.read(reinterpret_cast<char*>(dataRaw), sizeof(double) * numValues);

    std::vector<dmat2> tensors;
    buildTensors(data, tensors);

    if (hasEigenInfo) {
        inFile.read(reinterpret_cast<char*>(majorEigenValuesRaw), sizeof(double) * numElements);

        inFile.read(reinterpret_cast<char*>(minorEigenvaluesRaw), sizeof(double) * numElements);

        inFile.read(reinterpret_cast<char*>(majorEigenvectorsRaw),
                    sizeof(double) * numElements * 2);

        inFile.read(reinterpret_cast<char*>(minorEigenvectorsRaw),
                    sizeof(double) * numElements * 2);

        std::string str;
        inFile.read(reinterpret_cast<char*>(&size), sizeof(size_t));
        str.resize(size);
        inFile.read(&str[0], size);

        if (str != "EOFreached") {
            LogError("EOF not reached");
        }
    }

    inFile.close();

    if (data.size() != numValues) {
        LogWarn("Dimensions do not match data size");
        return;
    }
    if (hasEigenInfo) {
        auto tensorField =
            std::make_shared<TensorField2D>(dimensions, tensors, majorEigenvalues, minorEigenvalues,
                                            majorEigenvectors, minorEigenvectors, extents);

        extents_.set(tensorField->getExtents());

        outport_.setData(tensorField);
    } else {
        auto tensorField = std::make_shared<TensorField2D>(dimensions, tensors, extents);

        extents_.set(tensorField->getExtents());

        outport_.setData(tensorField);
    }
}

}  // namespace inviwo
