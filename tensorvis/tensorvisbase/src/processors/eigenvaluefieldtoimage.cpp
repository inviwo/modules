/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2024 Inviwo Foundation
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

#include <inviwo/tensorvisbase/processors/eigenvaluefieldtoimage.h>
#include <inviwo/tensorvisbase/util/tensorfieldutil.h>

#include <modules/opengl/texture/textureunit.h>
#include <modules/opengl/texture/textureutils.h>
#include <modules/opengl/shader/shaderutils.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo EigenvalueFieldToImage::processorInfo_{
    "org.inviwo.EigenvalueFieldToImage",  // Class identifier
    "Eigenvalue Field To Image",          // Display name
    "Tensor visualization",               // Category
    CodeState::Experimental,              // Code state
    Tags::None,                           // Tags
};
const ProcessorInfo& EigenvalueFieldToImage::getProcessorInfo() const { return processorInfo_; }

EigenvalueFieldToImage::EigenvalueFieldToImage()
    : Processor()
    , inport_("inport")
    , outport_("outport", false)
    , valueType_("valueType", "Value type")
    , grayscale_("grayscale", "Grayscale", false)
    , dimensions_("dimensions", "Dimensions", ivec2(400), ivec2(128), ivec2(2000))
    , tf_("tf", "Transfer function")
    , shader_("eigenvaluefield.frag")
    , majorMinor_("useMinor", "Use minor eigenvectors", false) {
    addPort(inport_);
    addPort(outport_);
    addProperty(tf_);
    addProperty(majorMinor_);

    valueType_.addOption("majorEV", "Major eigenvalue", 0);
    valueType_.addOption("anisotropy", "Anisotropy (||e1-e2||)", 1);

    addProperty(valueType_);

    addProperty(grayscale_);

    addProperty(dimensions_);

    tf_.onChange([&]() { invalidate(InvalidationLevel::InvalidResources); });
    majorMinor_.onChange([&]() { invalidate(InvalidationLevel::InvalidResources); });
    shader_.onReload([&]() { invalidate(InvalidationLevel::InvalidOutput); });
    inport_.onChange([&]() { invalidate(InvalidationLevel::InvalidResources); });
}

void EigenvalueFieldToImage::initializeResources() {
    tf_texture_ = Image(std::make_shared<Layer>(
        std::shared_ptr<LayerRAM>(tf_.getRepresentation<LayerRAM>()->clone())));

    if (!inport_.hasData() || !inport_.getData().get()) return;
}

void EigenvalueFieldToImage::process() {
    outport_.setDimensions(dimensions_.get());

    utilgl::activateAndClearTarget(outport_);

    shader_.activate();

    TextureUnitContainer units;

    std::shared_ptr<Image> tensorFieldTexture;

    // add tensorfield to texture unit container
    tensorutil::bindTensorFieldAsColorTexture(tensorFieldTexture, inport_, shader_, units);

    TextureUnit tf;

    // Create a subsampled tensorfield to find possible deviations from the initial min/max values
    // for eigenvalues
    updateEigenValues();
    updateAnisotropy();

    shader_.setUniform("tf", tf.getUnitNumber());
    utilgl::bindColorTexture(tf_texture_, tf);

    utilgl::setUniforms(shader_, outport_, majorMinor_);

    shader_.setUniform("valueType", valueType_.get());
    shader_.setUniform("grayscale", grayscale_.get());

    shader_.setUniform("minEigenValue", minVal_);
    shader_.setUniform("maxEigenValue", maxVal_);
    shader_.setUniform("eigenValueRange", eigenValueRange_);

    shader_.setUniform("anisotropyMinVal", anisotropyMinVal_);
    shader_.setUniform("anisotropyMaxVal", anisotropyMaxVal_);
    shader_.setUniform("anisotropyValueRange", anisotropyValueRange_);

    utilgl::singleDrawImagePlaneRect();
    shader_.deactivate();
    utilgl::deactivateCurrentTarget();
}

void EigenvalueFieldToImage::updateEigenValues() {
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

void EigenvalueFieldToImage::updateAnisotropy() {
    auto subsampled = tensorutil::subsample2D(inport_.getData(),
                                              inport_.getData()->getDimensions() * size2_t(2, 2));

    std::vector<double> minorEigenValues;
    std::vector<double> majorEigenValues;
    std::vector<double> anisotropyValues;
    minorEigenValues = std::vector<double>(subsampled->minorEigenValues());
    majorEigenValues = std::vector<double>(subsampled->majorEigenValues());
    anisotropyValues.resize(minorEigenValues.size());

    for (size_t i = 0; i < minorEigenValues.size(); ++i) {
        anisotropyValues[i] = glm::abs(majorEigenValues[i] - minorEigenValues[i]);
    }

    anisotropyMinVal_ =
        static_cast<float>(*std::min_element(anisotropyValues.begin(), anisotropyValues.end()));
    anisotropyMaxVal_ =
        static_cast<float>(*std::max_element(anisotropyValues.begin(), anisotropyValues.end()));

    if (anisotropyMinVal_ == 0.0) {
        // Delete zero entries to find actual minimum
        anisotropyValues.erase(
            std::remove_if(anisotropyValues.begin(), anisotropyValues.end(),
                           [](double x) { return x < std::numeric_limits<double>::epsilon(); }),
            anisotropyValues.end());

        anisotropyMinVal_ =
            static_cast<float>(*std::min_element(anisotropyValues.begin(), anisotropyValues.end()));
        anisotropyMaxVal_ =
            static_cast<float>(*std::max_element(anisotropyValues.begin(), anisotropyValues.end()));
    }

    anisotropyValueRange_ = glm::abs(anisotropyMinVal_ - anisotropyMaxVal_);
}

}  // namespace inviwo
