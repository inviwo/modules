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

#include <inviwo/tensorvisbase/processors/tensorfieldlic.h>

#include <inviwo/core/datastructures/image/layer.h>
#include <modules/opengl/image/layergl.h>
#include <modules/opengl/texture/textureunit.h>
#include <modules/opengl/texture/textureutils.h>
#include <modules/opengl/shader/shaderutils.h>
#include <modules/opengl/openglutils.h>

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
    , noiseTexture_("noiseTexture")
    , outport_("outport")

    , dimensions_{"Dimensions", "Dimensions"}
    , sizing_{"sizing",
              "Sizing",
              "Sizing modes determining the dimensions of the resampled layer"_help,
              {{"sameAsInput", "Same as input", Sizing::SameAsInput},
               {"scalingFactor", "Scaling factor", Sizing::ScalingFactor},
               {"custom", "Custom size", Sizing::Custom}},
              1}
    , inputDimensions_{"inputDimensions", "Input Dimensions",
                       util::ordinalCount(size2_t{0}, size2_t{4096})
                           .set(PropertySemantics::Text)
                           .set("Size of the input layer (read-only)"_help)}
    , outputDimensions_{"outputDimensions", "Output Dimensions",
                        util::ordinalCount(size2_t{0}, size2_t{4096})
                            .set("Size of the resampled layer"_help)}
    , scaling_{"scaling", "Scaling",
               util::ordinalScale(1.0f, 4.0f)
                   .set("Scaling factor for Sizing Mode 'scaling factor'"_help)}

    , samples_("samples", "Number of steps", 20, 3, 100)
    , stepLength_("stepLength", "Step Length", 0.003f, 0.0001f, 0.01f, 0.0001f)
    , normalizeVectors_("normalizeVectors", "Normalize vectors", true)
    , useRK4_("useRK4", "Use Runge-Kutta4", true)
    , majorMinor_("useMinor", "Use minor eigenvectors", false)
    , backgroundColor_("backgroundColor", "Background color", vec4(1.0f), vec4(0.0f), vec4(1.0f),
                       vec4(0.01f), InvalidationLevel::InvalidOutput, PropertySemantics::Color)

    , postProcessing_{"postProcessing", "Post Processing",
                      "Apply some basic image operations to enhance the LIC"_help, true}
    , intensityMapping_{"intensityMapping", "Enable Intensity Remapping",
                        "Remap the resulting intensity values $v$ with $v^(5 / (v + 1)^4)$. "
                        "This will increase contrast."_help,
                        false}
    , brightness_{"brightness",
                  "Brightness",
                  "Adjusts the overall brightness"_help,
                  0.0f,
                  {-1.0f, ConstraintBehavior::Immutable},
                  {1.0f, ConstraintBehavior::Immutable}}
    , contrast_{"contrast",
                "Contrast",
                "Adjusts the overall contrast"_help,
                0.0f,
                {-1.0f, ConstraintBehavior::Immutable},
                {1.0f, ConstraintBehavior::Immutable}}
    , gamma_{"gamma",
             "Gamma Correction",
             "Gamma correction using $v^gamma$"_help,
             1.0f,
             {0.0f, ConstraintBehavior::Immutable},
             {2.0f, ConstraintBehavior::Immutable}}
    , shader_("tensorlic2d.frag") {

    addPorts(inport_, noiseTexture_, outport_);

    dimensions_.addProperties(sizing_, inputDimensions_, outputDimensions_, scaling_);

    addProperties(dimensions_, samples_, stepLength_, normalizeVectors_, useRK4_, majorMinor_,
                  backgroundColor_, postProcessing_);
    postProcessing_.addProperties(intensityMapping_, brightness_, contrast_, gamma_);

    inport_.onChange([&]() { invalidate(InvalidationLevel::InvalidResources); });
    majorMinor_.onChange([&]() { invalidate(InvalidationLevel::InvalidResources); });

    inputDimensions_.setReadOnly(true);
    outputDimensions_.readonlyDependsOn(
        sizing_, [](OptionProperty<Sizing>& p) { return p.get() != Sizing::Custom; });
    scaling_.readonlyDependsOn(
        sizing_, [](OptionProperty<Sizing>& p) { return p.get() != Sizing::ScalingFactor; });
    outputDimensions_.setReadOnly(sizing_.get() != Sizing::Custom);
    scaling_.setReadOnly(sizing_.get() != Sizing::ScalingFactor);

    shader_.onReload([&]() { invalidate(InvalidationLevel::InvalidOutput); });
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
    const size2_t sourceDimensions{inport_.getData()->getDimensions()};
    inputDimensions_.set(sourceDimensions);

    switch (sizing_) {
        case Sizing::SameAsInput:
            outputDimensions_.set(sourceDimensions);
            break;
        case Sizing::Custom:
            break;
        case Sizing::ScalingFactor:
            outputDimensions_.set(ivec2{vec2{sourceDimensions} * scaling_.get()});
            break;
        default:
            outputDimensions_.set(sourceDimensions);
            break;
    }

    LayerConfig newConfig{.dimensions = outputDimensions_.get(),
                          .format = DataFormatBase::get(DataFormatId::Vec4Float32),
                          .swizzleMask = swizzlemasks::defaultData(1),
                          .dataRange = dvec2{0.0, 1.0},
                          .valueRange = dvec2{0.0, 1.0}};

    if (config_ != newConfig) {
        cache_.clear();
        config_ = newConfig;
    }

    auto&& [fbo, layer] = [&]() -> decltype(auto) {
        auto unusedIt = std::ranges::find_if(
            cache_, [](const std::pair<FrameBufferObject, std::shared_ptr<Layer>>& item) {
                return item.second.use_count() == 1;
            });
        if (unusedIt != cache_.end()) {
            return *unusedIt;
        } else {
            auto& item = cache_.emplace_back(FrameBufferObject{}, std::make_shared<Layer>(config_));
            auto* layerGL = item.second->getEditableRepresentation<LayerGL>();
            utilgl::Activate activateFbo{&item.first};
            item.first.attachColorTexture(layerGL->getTexture().get(), 0);
            return item;
        }
    }();

    shader_.activate();
    TextureUnitContainer units;
    std::shared_ptr<Image> tensorFieldTexture;
    tensorutil::bindTensorFieldAsColorTexture(tensorFieldTexture, inport_.getData(), shader_,
                                              units);
    utilgl::bindAndSetUniforms(shader_, units, noiseTexture_);

    // Create a subsampled tensorfield to find possible deviations from the initial min/max values
    // for eigenvalues
    updateEigenValues();

    utilgl::setShaderUniforms(shader_, *layer, "outportParameters");
    utilgl::setUniforms(shader_, samples_, stepLength_, normalizeVectors_, useRK4_, majorMinor_,
                        backgroundColor_);
    shader_.setUniform("minEigenValue", minVal_);
    shader_.setUniform("maxEigenValue", maxVal_);
    shader_.setUniform("eigenValueRange", eigenValueRange_);

    shader_.setUniform("postProcessing", postProcessing_.isChecked());
    if (postProcessing_) {
        utilgl::setUniforms(shader_, intensityMapping_, brightness_, contrast_, gamma_);
    }

    const auto dim = layer->getDimensions();
    const vec2 dDimensions{dim};
    const float ratio =
        std::min(dDimensions.x, dDimensions.y) / std::max(dDimensions.x, dDimensions.y);

    shader_.setUniform("ratio", ratio);

    {
        utilgl::Activate activateFbo{&fbo};
        utilgl::ViewportState viewport{0, 0, static_cast<GLsizei>(dim.x),
                                       static_cast<GLsizei>(dim.y)};
        utilgl::DepthMaskState depthMask{GL_FALSE};
        utilgl::GlBoolState depthTest{GL_DEPTH_TEST, false};

        // We always need to ask for an editable representation, this will invalidate any other
        // representations
        layer->getEditableRepresentation<LayerGL>();
        utilgl::singleDrawImagePlaneRect();
    }
    shader_.deactivate();

    outport_.setData(layer);
}

}  // namespace inviwo
