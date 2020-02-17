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

#include <inviwo/tensorvisbase/processors/tensorglyphrenderer.h>
#include <modules/opengl/openglutils.h>
#include <inviwo/core/interaction/events/mouseevent.h>
#include <inviwo/core/rendering/meshdrawerfactory.h>
#include <inviwo/core/interaction/events/pickingevent.h>
#include <inviwo/tensorvisbase/tensorvisbasemodule.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorGlyphRenderer::processorInfo_{
    "org.inviwo.TensorGlyphRenderer",  // Class identifier
    "Tensor Glyph Renderer",           // Display name
    "Tensor visualization",            // Category
    CodeState::Experimental,           // Code state
    tag::OpenTensorVis | Tag::GL,      // Tags
};
const ProcessorInfo TensorGlyphRenderer::getProcessorInfo() const { return processorInfo_; }

TensorGlyphRenderer::TensorGlyphRenderer()
    : Processor()
    , meshInport_("meshInport")
    , tensorFieldInport_("tensorFieldInport")
    , offsetInport_("offsetInport")
    , imageInport_("imageInport")
    , selectedMeshOutport_("selectedMeshOutport")
    , indexOutport_("indexOutport")
    , outport_("outport")
    , camera_("camera", "Camera")
    , trackball_(&camera_)
    , cullFace_("cullFace", "Cull Face",
                {{"culldisable", "Disable", GL_NONE},
                 {"cullfront", "Front", GL_FRONT},
                 {"cullback", "Back", GL_BACK},
                 {"cullfrontback", "Front & Back", GL_FRONT_AND_BACK}})
    , lighting_("lighting", "Lighting", &camera_)
    , picking_(this, 1, [&](PickingEvent* p) { handlePickingEvent(p); })
    , selectedIDProperty_("selectedID", "Selected ID", 0, 0, std::numeric_limits<size_t>::max(), 1,
                          InvalidationLevel::Valid)
    , shader_("geometryrendering.vert", "tensorglyphpicking.frag", false)
    , selectedID_(-1)
    , previouslySelectedID_(-1)
    , selectMode_("selectMode", "Select mode", true)
    , glyphType_("glyphType", "Glyph type") {
    addPort(meshInport_);
    addPort(tensorFieldInport_);
    offsetInport_.setOptional(true);
    addPort(offsetInport_);
    addPort(imageInport_);
    imageInport_.setOptional(true);
    addPort(outport_);
    addPort(selectedMeshOutport_);
    addPort(indexOutport_);

    addProperty(selectMode_);
    addProperty(glyphType_);

    addProperty(camera_);
    addProperty(trackball_);

    addProperty(cullFace_);

    shader_.onReload([this]() { invalidate(InvalidationLevel::InvalidResources); });

    meshInport_.onChange([&]() {
        meshDrawers_.clear();

        for (const auto& mesh : *meshInport_.getData()) {
            meshDrawers_.emplace_back(
                InviwoApplication::getPtr()->getMeshDrawerFactory()->create(mesh.get()));
        }

        picking_.resize(meshInport_.getData()->size());
    });

    indexOutport_.setData(std::make_shared<size_t>(0));

    addProperty(selectedIDProperty_);
    selectedIDProperty_.setReadOnly(false);
}

void TensorGlyphRenderer::initializeResources() { addCommonShaderDefines(shader_); }

void TensorGlyphRenderer::handlePickingEvent(PickingEvent* p) {
    if (selectMode_.get()) {
        if (p->getState() == PickingState::Updated &&
            p->getEvent()->hash() == MouseEvent::chash()) {
            auto me = p->getEventAs<MouseEvent>();

            auto id = p->getPickedId();
            auto state = me->state();
            auto button = me->button();

            if (button & MouseButton::Left && state & MouseState::Press && !triggerSelect_) {
                triggerSelect_ = true;
            }

            if (triggerSelect_) {
                selectedID_ = static_cast<int>(id);

                size_t offset = 0;
                if (offsetInport_.isConnected() && offsetInport_.hasData()) {
                    offset = *offsetInport_.getData();
                }

                selectedIDProperty_.set(selectedID_ + offset);

                triggerSelect_ = false;

                invalidate(InvalidationLevel::InvalidOutput);
            }

            if (button & MouseButton::Left && state & MouseState::Release) {
                selectedID_ = -1;
                invalidate(InvalidationLevel::InvalidOutput);
            }
        }
    }
}

void TensorGlyphRenderer::addCommonShaderDefines(Shader& shader) {
    utilgl::addShaderDefines(shader, lighting_);

    // get a hold of the current output data
    auto prevData = outport_.getData();
    size_t numLayers = 1;
    if (prevData->getNumberOfColorLayers() != numLayers) {
        // create new image with matching number of layers
        auto image = std::make_shared<Image>(prevData->getDimensions(), prevData->getDataFormat());
        // update number of layers
        for (auto i = image->getNumberOfColorLayers(); i < numLayers; ++i) {
            image->addColorLayer(std::shared_ptr<Layer>(image->getColorLayer(0)->clone()));
        }

        outport_.setData(image);
    }

    shader.build();
}

void TensorGlyphRenderer::process() {
    utilgl::activateAndClearTarget(outport_, ImageType::ColorDepthPicking);

    shader_.activate();

    utilgl::setShaderUniforms(shader_, camera_, "camera");
    utilgl::setShaderUniforms(shader_, lighting_, "light");
    shader_.setUniform("pickingColor", picking_.getColor());

    signed long long i = 0;
    for (auto& drawer : meshDrawers_) {
        shader_.setUniform("pickingColor", picking_.getColor(i));
        utilgl::setShaderUniforms(shader_, *(drawer->getMesh()), "geometry");
        shader_.setUniform("highlight", i == selectedID_);
        {
            utilgl::CullFaceState culling1(cullFace_.get());
            drawer->draw();
        }
        i++;
    }

    shader_.deactivate();
    utilgl::deactivateCurrentTarget();
    if (imageInport_.hasData()) compositor_.composite(imageInport_, outport_, ImageType::AllLayers);

    if (selectedID_ != previouslySelectedID_) {
        previouslySelectedID_ = selectedID_;
        if (selectedID_ == -1) {
            return;
        }
        auto tensorField = tensorFieldInport_.getData();
        size_t offset = 0;
        if (offsetInport_.isConnected() && offsetInport_.hasData()) {
            offset = *offsetInport_.getData();
        }

        selectedMeshOutport_.setData(
            glyphType_.generateGlyph(tensorField, selectedID_ + offset, vec3(0.f)));

        indexOutport_.setData(std::make_shared<size_t>(selectedID_ + offset));
    } else {
        selectedMeshOutport_.setData(
            glyphType_.generateQuadric(dmat3(1), dvec3(0), glyphType_.size(), dvec4(1)));
    }
}

}  // namespace inviwo
