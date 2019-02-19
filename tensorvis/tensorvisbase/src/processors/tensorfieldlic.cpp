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

#include <modules/tensorvisbase/processors/tensorfieldlic.h>
#include <algorithm>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorFieldLIC::processorInfo_{
    "org.inviwo.TensorFieldLIC",  // Class identifier
    "Tensor Field LIC",           // Display name
    "Tensor Visualization",       // Category
    CodeState::Experimental,      // Code state
    Tags::GL,                     // Tags
};

const ProcessorInfo TensorFieldLIC::getProcessorInfo() const { return processorInfo_; }

TensorFieldLIC::TensorFieldLIC()
    : Processor()
    , inport_("inport")
    , noiseTexture_("noiseTexture", true)
    , imageInport_("imageInport")
    , outport_("outport")
    , samples_("samples", "Number of steps", 20, 3, 100)
    , stepLength_("stepLength", "Step Length", 0.003f, 0.0001f, 0.01f, 0.0001f)
    , normalizeVectors_("normalizeVectors", "Normalize vectors", true)
    , intensityMapping_("intensityMapping", "Enable intensity remapping", false)
    , useRK4_("useRK4", "Use Runge-Kutta4", true)
    , majorMinor_("useMinor", "Use minor eigenvectors", false)
    , shader_("tensorlic2d.frag") {
    imageInport_.setOptional(true);

    addProperty(samples_);
    addProperty(stepLength_);
    addProperty(normalizeVectors_);
    addProperty(intensityMapping_);
    addProperty(useRK4_);
    addProperty(majorMinor_);

    shader_.onReload([&]() { invalidate(InvalidationLevel::InvalidOutput); });

    addPort(inport_);
    addPort(noiseTexture_);
    addPort(outport_);
    addPort(imageInport_);

    inport_.onChange([&]() { invalidate(InvalidationLevel::InvalidResources); });
    majorMinor_.onChange([&]() { invalidate(InvalidationLevel::InvalidResources); });
}

void TensorFieldLIC::initializeResources() {
    if (!inport_.hasData() || !inport_.getData().get()) return;

    updateEigenValues();
}

void TensorFieldLIC::updateEigenValues() {
    auto subsampled = tensorutil::subsample2D(inport_.getData(),
                                              inport_.getData()->getDimensions() * size2_t(2, 2));

    std::vector<double> eigenValues;
    if (majorMinor_.get()) {
        eigenValues = std::vector<double>(subsampled->minorEigenValues());
    } else {
        eigenValues = std::vector<double>(subsampled->majorEigenValues());
    }

    minVal_ = static_cast<float>(*std::min_element(eigenValues.begin(), eigenValues.end()));
    maxVal_ = static_cast<float>(*std::max_element(eigenValues.begin(), eigenValues.end()));

    if (minVal_ == 0.0) {
        // Delete zero entries to find actual minimum
        eigenValues.erase(
            std::remove_if(eigenValues.begin(), eigenValues.end(),
                           [](double x) { return x < std::numeric_limits<double>::epsilon(); }),
            eigenValues.end());

        minVal_ = static_cast<float>(*std::min_element(eigenValues.begin(), eigenValues.end()));
        maxVal_ = static_cast<float>(*std::max_element(eigenValues.begin(), eigenValues.end()));
    }

    eigenValueRange_ = glm::abs(minVal_ - maxVal_);
}

void TensorFieldLIC::process() {
    utilgl::activateAndClearTarget(outport_);

    shader_.activate();
    TextureUnitContainer units;
    std::shared_ptr<Image> tensorFieldTexture;
    // add tensorfield to texture unit container
    tensorutil::bindTensorFieldAsColorTexture(tensorFieldTexture, inport_.getData(), shader_,
                                              units);
    // add noise texture to texture unit container
    utilgl::bindAndSetUniforms(shader_, units, noiseTexture_, ImageType::ColorOnly);

    bool hasInputImage = imageInport_.isConnected();

    if (hasInputImage) {
        utilgl::bindAndSetUniforms(shader_, units, imageInport_, ImageType::ColorOnly);
    }

    // Create a subsampled tensorfield to find possible deviations from the initial min/max values
    // for eigenvalues
    updateEigenValues();

    utilgl::setUniforms(shader_, outport_, samples_, stepLength_, normalizeVectors_,
                        intensityMapping_, useRK4_, majorMinor_);
    shader_.setUniform("minEigenValue", minVal_);
    shader_.setUniform("maxEigenValue", maxVal_);
    shader_.setUniform("eigenValueRange", eigenValueRange_);
    shader_.setUniform("hasInputImage", hasInputImage);

    utilgl::singleDrawImagePlaneRect();
    shader_.deactivate();
    utilgl::deactivateCurrentTarget();
}

}  // namespace inviwo
