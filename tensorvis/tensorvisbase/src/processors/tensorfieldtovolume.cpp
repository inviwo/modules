/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2017-2018 Inviwo Foundation
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

#include <modules/tensorvisbase/processors/tensorfieldtovolume.h>
#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/core/util/volumeramutils.h>
#include <modules/tensorvisbase/util/tensorutil.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorFieldToVolume::processorInfo_{
    "org.inviwo.TensorFieldToVolume",  // Class identifier
    "Tensor Field To Volume",          // Display name
    "Tensor visualization",            // Category
    CodeState::Stable,                 // Code state
    Tags::CPU,                         // Tags
};
const ProcessorInfo TensorFieldToVolume::getProcessorInfo() const { return processorInfo_; }

TensorFieldToVolume::TensorFieldToVolume()
    : Processor()
    , inport_("inport")
    , outport_("outport")
    , feature_("feature", "Feature")
    , normalizeVectors_("normalize", "Normalize eigenvectors") {
    addPort(inport_);
    addPort(outport_);

    feature_.addOption("majorEigenVector", "Major eigenvector", TensorFeature::MajorEigenVector);
    feature_.addOption("intermediateEigenVector", "Intermediate eigenvector",
                       TensorFeature::IntermediateEigenVector);
    feature_.addOption("minorEigenVector", "Minor eigenvector", TensorFeature::MinorEigenVector);
    feature_.addOption("majorEigenValue", "Major eigenvalue", TensorFeature::Sigma1);
    feature_.addOption("intermediateEigenValue", "Intermediate eigenvalue", TensorFeature::Sigma2);
    feature_.addOption("minorEigenValue", "Minor eigenvalue", TensorFeature::Sigma3);
    feature_.addOption("hill", "Hill", TensorFeature::HillYieldCriterion);

    addProperty(feature_);
    addProperty(normalizeVectors_);

    /*feature_.onChange([this]() {
        if (feature_.get() > 2)
            normalizeVectors_.setVisible(false);
        else
            normalizeVectors_.setVisible(true);
    });*/
}

void TensorFieldToVolume::process() {
    auto tensorField = inport_.getData();

    auto option = feature_.get();

    const auto &metaData = *tensorField->getMetaDataContainer(uint64_t(option));

    const auto numberOfComponents = metaData.getNumberOfComponents();

    auto vol = std::make_shared<Volume>(tensorField->getDimensions(), DataFormatBase::get(NumericType::Float, numberOfComponents, 32));

    auto volRAM = vol->getEditableRepresentation<VolumeRAM>();

    /*volRAM->dispatch<void, dispatching::filter::Floats>([this](auto repr) {
        auto ptr=repr->getTypedData();

        //std::copy(--,--,ptr);
    });*/



















    /*if (option < 3) {
        auto vol = std::make_shared<Volume>(tensorField->getDimensions(), DataVec3Float32::get());
        auto volRAM = vol->getEditableRepresentation<VolumeRAM>();
        auto data = static_cast<glm::f32vec3*>(volRAM->getData());

        util::IndexMapper3D indexMapper(tensorField->getDimensions());

        vol->setModelMatrix(tensorField->getBasisAndOffset());

        auto func = [&](size3_t pos) {
            auto eigenVector =
                glm::f32vec3(tensorField->getSortedEigenVectorsForTensor(pos)[option]);

            if (normalizeVectors_.get()) {
                eigenVector = glm::normalize(eigenVector);
            }
            data[indexMapper(pos)] = eigenVector;
        };

        util::forEachVoxelParallel(*volRAM, func);

        vol->dataMap_ = tensorField->dataMapEigenVectors_[option];

        outport_.setData(vol);
    } else {
        auto vol = std::make_shared<Volume>(tensorField->getDimensions(), DataFloat32::get());
        auto volRAM = vol->getEditableRepresentation<VolumeRAM>();
        auto data = static_cast<glm::f32*>(volRAM->getData());

        util::IndexMapper3D indexMapper(tensorField->getDimensions());

        vol->setModelMatrix(tensorField->getBasisAndOffset());

        auto func = [&](size3_t pos) {
            data[indexMapper(pos)] =
                glm::f32(tensorField->getSortedEigenValuesForTensor(pos)[option - 3]);
        };

        util::forEachVoxelParallel(*volRAM, func);

        vol->dataMap_ = tensorField->dataMapEigenValues_[option - 3];

        outport_.setData(vol);
    }*/
}

}  // namespace inviwo
