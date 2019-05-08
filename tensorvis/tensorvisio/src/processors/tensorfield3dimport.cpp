/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2018 Inviwo Foundation
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
#include <modules/tensorvisbase/datastructures/tensorfieldmetadata.h>
#include <modules/tensorvisio/processors/tensorfield3dimport.h>
#include <unordered_map>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming
// scheme
const ProcessorInfo TensorField3DImport::processorInfo_{
    "org.inviwo.TensorField3DImport",  // Class identifier
    "Tensor Field 3D Import",          // Display name
    "Tensor Field IO",                 // Category
    CodeState::Experimental,           // Code state
    Tags::None,                        // Tags
};
const ProcessorInfo TensorField3DImport::getProcessorInfo() const { return processorInfo_; }

TensorField3DImport::TensorField3DImport()
    : Processor()
    , inFile_("inFile", "File")
    , outport_("outport")
    , normalizeExtends_("normalizeExtends", "Normalize extends", true)
    , extends_("extends", "Extends", vec3(1.f), vec3(0.f), vec3(1000.f), vec3(0.0001f),
               InvalidationLevel::Valid)
    , offset_("offset", "Offset", vec3(1.f), vec3(-1000.f), vec3(1000.f), vec3(0.0001f),
              InvalidationLevel::Valid)
    , dimensions_("dimensions", "Dimensions", ivec3(0), ivec3(0), ivec3(1024), ivec3(1),
                  InvalidationLevel::Valid)
    , tensorFieldOut_(nullptr) {
    addPort(outport_);

    addProperty(inFile_);

    addProperty(normalizeExtends_);

    extends_.setReadOnly(true);
    extends_.setCurrentStateAsDefault();
    addProperty(extends_);

    offset_.setReadOnly(true);
    offset_.setCurrentStateAsDefault();
    addProperty(offset_);

    dimensions_.setReadOnly(true);
    dimensions_.setCurrentStateAsDefault();
    addProperty(dimensions_);

    inFile_.onChange([this]() { invalidate(InvalidationLevel::InvalidResources); });
}

void TensorField3DImport::initializeResources() {
    tensorFieldOut_.reset();
    tensorFieldOut_ = nullptr;

    std::ifstream inFile(inFile_.get(), std::ios::in | std::ios::binary);

    if (!inFile) {
        LogError("Couldn't open file");
        return;
    }

    size_t version;
    size3_t dimensions;
    auto extends = dvec3(1.0);
    auto offset = dvec3(0.0);
    size_t rank;
    size_t dimensionality;
    glm::uint8 hasMetaData;
    std::array<DataMapper, 3> dataMapperEigenValues;
    std::array<DataMapper, 3> dataMapperEigenVectors;
    std::vector<glm::uint8> mask;
    std::unordered_map<uint64_t, std::unique_ptr<MetaDataBase>> metaData;

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

    inFile.read(reinterpret_cast<char *>(&dimensionality), sizeof(size_t));
    inFile.read(reinterpret_cast<char *>(&rank), sizeof(size_t));
    inFile.read(reinterpret_cast<char *>(&hasMetaData), sizeof(glm::uint8));

    if (dimensionality != 3) {
        LogError("The loaded file is not a 3D tensor field. Try the 2D reader.");
        return;
    }

    inFile.read(reinterpret_cast<char *>(&dimensions.x), sizeof(size_t));
    inFile.read(reinterpret_cast<char *>(&dimensions.y), sizeof(size_t));
    inFile.read(reinterpret_cast<char *>(&dimensions.z), sizeof(size_t));

    // Read the extends

    inFile.read(reinterpret_cast<char *>(&extends.x), sizeof(double));
    inFile.read(reinterpret_cast<char *>(&extends.y), sizeof(double));
    inFile.read(reinterpret_cast<char *>(&extends.z), sizeof(double));

    inFile.read(reinterpret_cast<char *>(&offset.x), sizeof(double));
    inFile.read(reinterpret_cast<char *>(&offset.y), sizeof(double));
    inFile.read(reinterpret_cast<char *>(&offset.z), sizeof(double));

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

    std::vector<double> data;
    data.resize(numValues);
    auto dataRaw = data.data();

    inFile.read(reinterpret_cast<char *>(dataRaw), sizeof(double) * numValues);

    std::vector<dmat3> tensors;
    buildTensors(data, tensors);

    glm::uint8 hasMask;
    inFile.read(reinterpret_cast<char *>(&hasMask), sizeof(glm::uint8));

    if (hasMask) {
        mask.resize(numElements);
        auto maskData = mask.data();
        inFile.read(reinterpret_cast<char *>(maskData), sizeof(glm::uint8) * numElements);
    }

    if (hasMetaData) {
        size_t numMetaDataEntries;
        inFile.read(reinterpret_cast<char *>(&numMetaDataEntries), sizeof(size_t));

        for (size_t i = 0; i < numMetaDataEntries; i++) {
            uint64_t id;
            inFile.read(reinterpret_cast<char *>(&id), sizeof(uint64_t));

            std::unique_ptr<MetaDataBase> ptr = nullptr;

            switch (id) {
                case MajorEigenVectors::id():
                    ptr = std::make_unique<MajorEigenVectors>();
                    break;
                case IntermediateEigenVectors::id():
                    ptr = std::make_unique<IntermediateEigenVectors>();
                    break;
                case MinorEigenVectors::id():
                    ptr = std::make_unique<MinorEigenVectors>();
                    break;
                case MajorEigenValues::id():
                    ptr = std::make_unique<MajorEigenValues>();
                    break;
                case IntermediateEigenValues::id():
                    ptr = std::make_unique<IntermediateEigenValues>();
                    break;
                case MinorEigenValues::id():
                    ptr = std::make_unique<MinorEigenValues>();
                    break;
                case I1::id():
                    ptr = std::make_unique<I1>();
                    break;
                case I2::id():
                    ptr = std::make_unique<I2>();
                    break;
                case I3::id():
                    ptr = std::make_unique<I3>();
                    break;
                case J1::id():
                    ptr = std::make_unique<J1>();
                    break;
                case J2::id():
                    ptr = std::make_unique<J2>();
                    break;
                case J3::id():
                    ptr = std::make_unique<J3>();
                    break;
                case LodeAngle::id():
                    ptr = std::make_unique<LodeAngle>();
                    break;
                case Anisotropy::id():
                    ptr = std::make_unique<Anisotropy>();
                    break;
                case LinearAnisotropy::id():
                    ptr = std::make_unique<LinearAnisotropy>();
                    break;
                case PlanarAnisotropy::id():
                    ptr = std::make_unique<PlanarAnisotropy>();
                    break;
                case SphericalAnisotropy::id():
                    ptr = std::make_unique<SphericalAnisotropy>();
                    break;
                case Diffusivity::id():
                    ptr = std::make_unique<Diffusivity>();
                    break;
                case ShearStress::id():
                    ptr = std::make_unique<ShearStress>();
                    break;
                case PureShear::id():
                    ptr = std::make_unique<PureShear>();
                    break;
                case ShapeFactor::id():
                    ptr = std::make_unique<ShapeFactor>();
                    break;
                case IsotropicScaling::id():
                    ptr = std::make_unique<IsotropicScaling>();
                    break;
                case Rotation::id():
                    ptr = std::make_unique<Rotation>();
                    break;
                case FrobeniusNorm::id():
                    ptr = std::make_unique<FrobeniusNorm>();
                    break;
                default:
                    LogError(
                        "Default case reached. Revise tensor field import for missing "
                        "meta data "
                        "entry.");
                    LogError(
                        "The imported tensor field now does not have all the meta "
                        "data with which "
                        "it was stored.");
                    continue;
            }

            ptr->deserialize(inFile, numElements);

            metaData.insert(std::make_pair(id, std::move(ptr)));
        }
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

    if (data.size() != numValues) {
        LogWarn("Dimensions do not match data size");
        return;
    }

    dextends_ = extends;

    tensorFieldOut_ = std::make_shared<TensorField3D>(dimensions, tensors, metaData, extends);

    tensorFieldOut_->dataMapEigenValues_ = dataMapperEigenValues;
    tensorFieldOut_->dataMapEigenVectors_ = dataMapperEigenVectors;

    tensorFieldOut_->setOffset(offset);

    tensorFieldOut_->setMask(mask);

    extends_.set(extends);
    offset_.set(offset);
    dimensions_.set(dimensions);
}

void TensorField3DImport::process() {
    if (!tensorFieldOut_) return;

    auto extends = dextends_;

    if (normalizeExtends_.get()) {
        extends /= std::max(std::max(extends.x, extends.y), extends.z);
    }

    extends_.set(vec3(extends));

    tensorFieldOut_->setExtends(extends);

    outport_.setData(tensorFieldOut_);
}

void TensorField3DImport::buildTensors(const std::vector<double> &data,
                                       std::vector<dmat3> &tensors) const {
    for (size_t i{0}; i < data.size() / 9; i++) {
        size_t offset = i * 9;
        tensors.emplace_back(vec3(data[offset + 0], data[offset + 3], data[offset + 6]),
                             vec3(data[offset + 1], data[offset + 4], data[offset + 7]),
                             vec3(data[offset + 2], data[offset + 5], data[offset + 8]));
    }
}

}  // namespace inviwo
