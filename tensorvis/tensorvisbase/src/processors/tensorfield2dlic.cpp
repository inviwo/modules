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

#include <inviwo/tensorvisbase/processors/tensorfield2dlic.h>
#include <algorithm>
#include <inviwo/tensorvisbase/tensorvisbasemodule.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorField2DLIC::processorInfo_{
    "org.inviwo.TensorField2DLIC",  // Class identifier
    "Tensor Field 2D LIC",          // Display name
    "Tensor Visualization",         // Category
    CodeState::Experimental,        // Code state
    tag::OpenTensorVis | Tag::GL,   // Tags
};

const ProcessorInfo TensorField2DLIC::getProcessorInfo() const { return processorInfo_; }

TensorField2DLIC::TensorField2DLIC()
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
    , backgroundColor_("backgroundColor", "Background color", vec4(1.0f), vec4(0.0f), vec4(1.0f),
                       vec4(0.01f), InvalidationLevel::InvalidOutput, PropertySemantics::Color)
    , shader_("tensorlic2d.frag") {
    imageInport_.setOptional(true);

    addProperty(samples_);
    addProperty(stepLength_);
    addProperty(normalizeVectors_);
    addProperty(intensityMapping_);
    addProperty(useRK4_);
    addProperty(majorMinor_);
    addProperty(backgroundColor_);

    shader_.onReload([&]() { invalidate(InvalidationLevel::InvalidOutput); });

    addPort(inport_);
    addPort(noiseTexture_);
    addPort(outport_);
    addPort(imageInport_);

    inport_.onChange([&]() { invalidate(InvalidationLevel::InvalidResources); });
    majorMinor_.onChange([&]() { invalidate(InvalidationLevel::InvalidResources); });
}

void TensorField2DLIC::initializeResources() {
    if (!inport_.hasData() || !inport_.getData().get()) return;

    updateEigenValues();
}

void TensorField2DLIC::updateEigenValues() {
    auto subsampled = tensorutil::subsample2D(inport_.getData(),
                                              inport_.getData()->getDimensions() * size2_t(2, 2));

    std::vector<TensorField2D::value_type> eigenValues;
    if (majorMinor_.get()) {
        eigenValues = std::vector<TensorField2D::value_type>(subsampled->minorEigenValues());
    } else {
        eigenValues = std::vector<TensorField2D::value_type>(subsampled->majorEigenValues());
    }

    minVal_ = static_cast<float>(*std::min_element(eigenValues.begin(), eigenValues.end()));
    maxVal_ = static_cast<float>(*std::max_element(eigenValues.begin(), eigenValues.end()));

    if (minVal_ == 0.0) {
        // Delete zero entries to find actual minimum
        eigenValues.erase(
            std::remove_if(eigenValues.begin(), eigenValues.end(),
                           [](auto x) {
                               return std::abs(x) <
                                      std::numeric_limits<TensorField2D::value_type>::epsilon();
                           }),
            eigenValues.end());

        minVal_ = static_cast<float>(*std::min_element(eigenValues.begin(), eigenValues.end()));
        maxVal_ = static_cast<float>(*std::max_element(eigenValues.begin(), eigenValues.end()));
    }

    eigenValueRange_ = glm::abs(minVal_ - maxVal_);
}

void TensorField2DLIC::process() {
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
                        intensityMapping_, useRK4_, majorMinor_, backgroundColor_);
    shader_.setUniform("minEigenValue", minVal_);
    shader_.setUniform("maxEigenValue", maxVal_);
    shader_.setUniform("eigenValueRange", eigenValueRange_);
    shader_.setUniform("hasInputImage", hasInputImage);

    const auto dDimensions = vec2(outport_.getDimensions());
    const auto ratio =
        std::min(dDimensions.x, dDimensions.y) / std::max(dDimensions.x, dDimensions.y);

    shader_.setUniform("ratio", ratio);

    utilgl::singleDrawImagePlaneRect();
    shader_.deactivate();
    utilgl::deactivateCurrentTarget();
}

}  // namespace inviwo
