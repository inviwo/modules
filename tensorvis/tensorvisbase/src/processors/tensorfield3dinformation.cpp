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

#include <inviwo/tensorvisbase/processors/tensorfield3dinformation.h>
#include <inviwo/tensorvisbase/util/tensorutil.h>
#include <inviwo/tensorvisbase/util/tensorfieldutil.h>
#include <inviwo/tensorvisbase/tensorvisbasemodule.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorField3DInformation::processorInfo_{
    "org.inviwo.TensorField3DInformation",  // Class identifier
    "Tensor Field 3D Information",          // Display name
    "Information",                          // Category
    CodeState::Experimental,                // Code state
    Tags::CPU | tag::OpenTensorVis,         // Tags
};

const ProcessorInfo TensorField3DInformation::getProcessorInfo() const { return processorInfo_; }

TensorField3DInformation::TensorField3DInformation()
    : Processor()
    , inport_("inport")
    , index_("index", "Index", ivec3(0), ivec3(0), ivec3(std::numeric_limits<int>::max()), ivec3(1))
    , tensor_("tensor", "Tensor", mat3(0.f), mat3(std::numeric_limits<float>::lowest()),
              mat3(std::numeric_limits<float>::max()), mat3(std::numeric_limits<float>::epsilon()))
    , eigenVectors_("eigenVectors", "Eigenvectors", mat3(0.f),
                    mat3(std::numeric_limits<float>::lowest()),
                    mat3(std::numeric_limits<float>::max()))
    , eigenValues_("eigenValues", "Eigenvalues", vec3(0.f),
                   vec3(std::numeric_limits<float>::lowest()),
                   vec3(std::numeric_limits<float>::max()))
    , basisAndOffset_("basisAndOffset", "Basis and offset", mat4{1.f}, mat4{-1000.f}, mat4{1000.f},
                      mat4{0.00001f}) {
    addPort(inport_);

    inport_.onChange([this]() { invalidate(InvalidationLevel::InvalidResources); });

    addProperties(index_, tensor_, eigenVectors_, eigenValues_, basisAndOffset_);

    tensor_.setReadOnly(true);
    eigenVectors_.setReadOnly(true);
    eigenValues_.setReadOnly(true);

    tensor_.setCurrentStateAsDefault();
    eigenVectors_.setCurrentStateAsDefault();
    eigenValues_.setCurrentStateAsDefault();
}

void TensorField3DInformation::initializeResources() {
    if (!inport_.hasData()) return;

    index_.setMinValue(ivec3(0));

    auto dimensions = inport_.getData()->getDimensions();

    index_.setMaxValue(ivec3(dimensions) - ivec3(1));
}

void TensorField3DInformation::process() {
    auto tensorField = inport_.getData();

    tensor_.set(mat3(tensorField->at(index_.get())));

    auto eigenValuesAndEigenVectors =
        tensorutil::getSortedEigenValuesAndEigenVectorsForTensor(tensorField, index_.get());

    auto eigenVectors =
        mat3(vec3(eigenValuesAndEigenVectors[0].second), vec3(eigenValuesAndEigenVectors[1].second),
             vec3(eigenValuesAndEigenVectors[2].second));
    eigenVectors_.set(eigenVectors);

    auto eigenValues =
        vec3(eigenValuesAndEigenVectors[0].first, eigenValuesAndEigenVectors[1].first,
             eigenValuesAndEigenVectors[2].first);
    eigenValues_.set(eigenValues);

    basisAndOffset_.set(tensorField->getBasisAndOffset());
}

}  // namespace inviwo
