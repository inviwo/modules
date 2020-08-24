/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020 Inviwo Foundation
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

#include <inviwo/vortexfeatures/processors/vortexsettovolumes.h>
#include <modules/opengl/volume/volumegl.h>
#include <modules/opengl/geometry/meshgl.h>
#include <modules/opengl/openglutils.h>
#include <inviwo/core/util/glm.h>

//
#include <inviwo/core/rendering/meshdrawerfactory.h>
#include <modules/opengl/rendering/meshdrawergl.h>

#include <modules/opengl/openglutils.h>
#include <modules/opengl/texture/textureutils.h>
#include <modules/opengl/image/imagegl.h>
#include <modules/base/algorithm/dataminmax.h>
//

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VortexSetToVolumes::processorInfo_{
    "org.inviwo.VortexSetToVolume",  // Class identifier
    "Vortex Set To Volume",          // Display name
    "Undefined",                     // Category
    CodeState::Experimental,         // Code state
    Tags::None,                      // Tags
};
const ProcessorInfo VortexSetToVolumes::getProcessorInfo() const { return processorInfo_; }

VortexSetToVolumes::VortexSetToVolumes()
    : Processor()
    , inVortices_("inVortices")
    , inVolumes_("inVolumes")
    , outVolumes_("outVolumes")
    , volumeSize_("volumeSize", "Dimensions Volume", ivec4(50, 50, 50, 50),
                  {ivec4(1, 1, 1, 1), ConstraintBehavior::Immutable},
                  {ivec4(1024, 1024, 1024, 1024), ConstraintBehavior::Ignore})
    , floatVolume_("floatVolume", "Float Volume", false)
    , shader_({{ShaderType::Vertex, "vortexraster.vert"},
               {ShaderType::Geometry, "vortexraster.geom"},
               {ShaderType::Fragment, "vortexraster.frag"}},
              Shader::Build::No)
    , fbo_() {

    addPort(inVortices_);
    addPort(inVolumes_);
    inVolumes_.setOptional(true);
    addPort(outVolumes_);
    addProperties(volumeSize_, floatVolume_);

    auto callProcess = [&]() { invalidate(InvalidationLevel::InvalidOutput); };

    inVolumes_.onChange(callProcess);
    shader_.onReload(callProcess);

    utilgl::GlBoolState depthTest(GL_DEPTH_TEST, false);
    utilgl::DepthMaskState depthMask(GL_FALSE);
    glFrontFace(GL_CCW);
    utilgl::CullFaceState culling(GL_NONE);
    utilgl::BlendModeState blendModeState(GL_ONE, GL_ONE);
    // Overwrite first binding position to float or int.
    shader_.getFragmentShaderObject()->addOutDeclaration("VortexData", 0,
                                                         floatVolume_ ? "float" : "int");
    shader_.build();
}

void VortexSetToVolumes::process() {
    utilgl::GlBoolState depthTest(GL_DEPTH_TEST, false);
    utilgl::DepthMaskState depthMask(GL_FALSE);
    glFrontFace(GL_CCW);
    utilgl::CullFaceState culling(GL_NONE);
    utilgl::BlendModeState blendModeState(GL_ONE, GL_ONE);

    if (!shader_.isReady()) shader_.build();
    if (inVolumes_.isChanged()) {
        // rebindVolume_ = true;
        if (inVolumes_.hasData() && inVolumes_.getData()->size() > 0) {

            const auto volumes = inVolumes_.getData();
            volumeSize_.set(ivec4(volumes->at(0)->getDimensions(), volumes->size()));
            volumeSize_.setReadOnly(true);

        } else {
            volumeSize_.setReadOnly(false);
        }
    }

    if (floatVolume_.isModified()) {
        utilgl::GlBoolState depthTest(GL_DEPTH_TEST, false);
        shader_.getFragmentShaderObject()->addOutDeclaration("VortexData", 0,
                                                             floatVolume_ ? "float" : "int");
        shader_.build();
    }

    if (!inVortices_.getData() || inVortices_.getData()->size() == 0) return;
    auto vortices = inVortices_.getData();

    if (volumeSize_.isModified() || floatVolume_.isModified()) {
        volumes_ = std::make_shared<VolumeSequence>();
        const DataFormatBase* dataFormat =
            floatVolume_ ? static_cast<const DataFormatBase*>(DataFloat32::get())
                         : static_cast<const DataFormatBase*>(DataInt32::get());
        for (size_t time = 0; time < volumeSize_->w; ++time) {
            volumes_->push_back(
                std::make_shared<Volume>(util::glm_convert<size3_t>(*volumeSize_), dataFormat));
            volumes_->back()->setModelMatrix(vortices->getModelMatrix());
            volumes_->back()->setWorldMatrix(vortices->getWorldMatrix());
            volumes_->back()->dataMap_.dataRange = {0, vortices->numGroups()};
            volumes_->back()->dataMap_.valueRange = {0, vortices->numGroups()};
        }
    }
    outVolumes_.setData(volumes_);
    size2_t timeSize = vortices->getTimeRange();
    size2_t heightSize = vortices->getHeightRange();
    timeSize.y = std::min(timeSize.y, volumeSize_->w);
    heightSize.y = std::min(heightSize.y, volumeSize_->z);
    bool newMesh = inVortices_.isChanged();
    if (newMesh) createMesh();

    shader_.activate();
    fbo_.activate();

    for (size_t time = 0; time <= timeSize.y - timeSize.x; ++time) {
        if (fanCounts_[time].size() == 0) continue;

        glViewport(0, 0, static_cast<GLsizei>(volumeSize_->x),
                   static_cast<GLsizei>(volumeSize_->y));

        // We always need to ask for an editable representation
        // this will invalidate any other representations.
        VolumeGL* outVolumeGL = volumes_->at(time)->getEditableRepresentation<VolumeGL>();
        fbo_.detachAllTextures();
        fbo_.attachColorTexture(outVolumeGL->getTexture().get(), 0);
        fbo_.checkStatus();

        auto meshGL = meshes_[time].getRepresentation<MeshGL>();
        utilgl::Enable<MeshGL> enable(meshGL);

        glMultiDrawArrays(GL_TRIANGLE_FAN, fanStarts_[time].data(), fanCounts_[time].data(),
                          fanCounts_[time].size());
    }
    shader_.deactivate();
    fbo_.deactivate();
}

void VortexSetToVolumes::createMesh() {
    if (!inVortices_.hasData() || inVortices_.getData()->size() == 0) return;
    auto vortices = inVortices_.getData();

    size2_t timeSize = vortices->getTimeRange();
    timeSize.y = std::min(timeSize.y, volumeSize_->w);

    meshes_.clear();
    fanStarts_.clear();
    fanStarts_.resize(timeSize.y - timeSize.x + 1);
    fanCounts_.clear();
    fanCounts_.resize(timeSize.y - timeSize.x + 1);

    for (auto& starts : fanStarts_) {
        meshes_.emplace_back(DrawType::Triangles, ConnectivityType::Fan);
        starts.push_back(0);
    }

    for (size_t group = 0; group < vortices->numGroups(); ++group) {
        size_t groupID = group + 1;
        for (auto vort = vortices->beginGroup(group); vort != vortices->endGroup(group); ++vort) {
            size_t time = vort->timeSlice - timeSize.x;
            std::vector<VortexMesh::Vertex> verts;

            auto pushVert = [&](const dvec2& pos) {
                verts.push_back({vec3(pos.x, pos.y, float(vort->heightSlice)), uint32_t(groupID)});
            };
            pushVert(vort->center);

            // Always add in CCW order.
            if (vort->rotation == Vortex::Turning::CounterClockwise) {
                for (dvec2 pos : vort->boundary) pushVert(pos);
            } else {
                for (int idx = vort->size() - 1; idx >= 0; --idx) pushVert(vort->boundary[idx]);
            }
            fanStarts_[time].push_back(fanStarts_[time].back() + vort->size() + 1);
            fanCounts_[time].push_back(vort->size() + 1);
            meshes_[time].addVertices(verts);
        }
    }
    for (auto& starts : fanStarts_) {
        starts.pop_back();
    }
}

}  // namespace inviwo
