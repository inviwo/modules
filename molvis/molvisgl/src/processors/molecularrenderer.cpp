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

#include <inviwo/molvisgl/processors/molecularrenderer.h>

#include <modules/opengl/rendering/meshdrawergl.h>
#include <modules/opengl/shader/shaderutils.h>
#include <modules/opengl/shader/shadertype.h>
#include <modules/opengl/openglutils.h>
#include <inviwo/core/algorithm/boundingbox.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo MolecularRenderer::processorInfo_{
    "org.inviwo.molvis.MolecularRenderer",  // Class identifier
    "Molecular Renderer",                   // Display name
    "MolVis",                               // Category
    CodeState::Experimental,                // Code state
    "GL, MolVis",                           // Tags
};
const ProcessorInfo MolecularRenderer::getProcessorInfo() const { return processorInfo_; }

MolecularRenderer::MolecularRenderer()
    : Processor()
    , inport_("geometry")
    , imageInport_("imageInport")
    , outport_("outport")
    , representation_("representation", "Representation",
                      {{"vdw", "VDW", Representation::VDW},
                       {"licorice", "Licorice", Representation::Licorice},
                       {"ballAndStick", "Ball & Stick", Representation::BallAndStick}},
                      0)
    , radiusScaling_("radiusScaling", "Radius Scaling", 1.0f, 0.0f, 2.0f)
    , forceRadius_("forceRadius", "Force Radius", false, InvalidationLevel::InvalidResources)
    , defaultRadius_("defaultRadius", "Default Radius", 0.15f, 0.00001f, 2.0f, 0.01f)
    , camera_("camera", "Camera", util::boundingBox(inport_))
    , lighting_("lighting", "Lighting", &camera_)
    , trackball_(&camera_)

    , vdwShaders_{{{ShaderType::Vertex, std::string{"vdw.vert"}},
                   {ShaderType::Geometry, std::string{"vdw.geom"}},
                   {ShaderType::Fragment, std::string{"vdw.frag"}}},
                  {{BufferType::PositionAttrib, MeshShaderCache::Mandatory, "vec3"},
                   {BufferType::ColorAttrib, MeshShaderCache::Optional, "vec4"},
                   {BufferType::RadiiAttrib, MeshShaderCache::Optional, "float"},
                   {BufferType::PickingAttrib, MeshShaderCache::Optional, "uint"},
                   {BufferType::ScalarMetaAttrib, MeshShaderCache::Optional, "float"}},

                  [&](Shader& shader) -> void {
                      shader.onReload(
                          [this]() { invalidate(InvalidationLevel::InvalidResources); });
                      configureVdWShader(shader);
                  }}
    , licoriceShaders_{
          {{ShaderType::Vertex, std::string{"licorice.vert"}},
           {ShaderType::Geometry, std::string{"licorice.geom"}},
           {ShaderType::Fragment, std::string{"licorice.frag"}}},
          {{BufferType::PositionAttrib, MeshShaderCache::Mandatory, "vec3"},
           {BufferType::ColorAttrib, MeshShaderCache::Optional, "vec4"},
           {BufferType::ScalarMetaAttrib, MeshShaderCache::Optional, "float"}},

          [&](Shader& shader) -> void {
              shader.onReload([this]() { invalidate(InvalidationLevel::InvalidResources); });
              configureLicoriceShader(shader);
          }} {
    addPort(inport_);
    addPort(imageInport_);
    addPort(outport_);

    imageInport_.setOptional(true);

    addProperties(representation_, radiusScaling_, forceRadius_, defaultRadius_, camera_, lighting_,
                  trackball_);
}

void MolecularRenderer::process() {
    utilgl::activateTargetAndClearOrCopySource(outport_, imageInport_);

    auto drawMesh = [](std::shared_ptr<const Mesh> mesh, MeshDrawerGL::DrawObject& drawer,
                       DrawType dt) {
        if ((mesh->getNumberOfIndicies() == 0) && (mesh->getDefaultMeshInfo().dt == dt)) {
            drawer.draw();
        } else {
            for (size_t i = 0; i < mesh->getNumberOfIndicies(); ++i) {
                if (mesh->getIndexMeshInfo(i).dt == dt) {
                    drawer.draw(i);
                }
            }
        }
    };

    auto drawVdW = [&](float radius) {
        for (auto mesh : inport_) {
            auto& shader = vdwShaders_.getShader(*mesh);

            shader.activate();
            utilgl::setUniforms(shader, camera_, lighting_, defaultRadius_);
            shader.setUniform("viewport", vec4(0.0f, 0.0f, 2.0f / outport_.getDimensions().x,
                                               2.0f / outport_.getDimensions().y));
            shader.setUniform("radiusScaling_", radius);
            utilgl::setShaderUniforms(shader, *mesh, "geometry");

            MeshDrawerGL::DrawObject drawer{mesh->getRepresentation<MeshGL>(),
                                            mesh->getDefaultMeshInfo()};
            drawMesh(mesh, drawer, DrawType::Points);
            shader.deactivate();
        }
    };
    auto drawLicorice = [&](float radius) {
        for (auto mesh : inport_) {
            auto& shader = licoriceShaders_.getShader(*mesh);
            shader.activate();
            utilgl::setUniforms(shader, camera_, lighting_, defaultRadius_);
            shader.setUniform("radius_", 0.25f * radius);
            utilgl::setShaderUniforms(shader, *mesh, "geometry");

            MeshDrawerGL::DrawObject drawer{mesh->getRepresentation<MeshGL>(),
                                            mesh->getDefaultMeshInfo()};
            drawMesh(mesh, drawer, DrawType::Lines);
            shader.deactivate();
        }
    };

    switch (representation_) {
        case Representation::VDW:
            drawVdW(radiusScaling_);
            break;
        case Representation::Licorice:
            drawLicorice(radiusScaling_);
            break;
        case Representation::BallAndStick:
            drawVdW(radiusScaling_ * BallAndStickVDWScale);
            drawLicorice(radiusScaling_ * BallAndStickLicoriceScale);
            break;
        case Representation::Ribbon:
            throw Exception("Unsupported representation: 'Ribbon'", IVW_CONTEXT);
            break;
        case Representation::Cartoon:
            throw Exception("Unsupported representation: 'Cartoon'", IVW_CONTEXT);
            break;
        default:
            drawVdW(radiusScaling_);
            break;
    }

    utilgl::deactivateCurrentTarget();
}

void MolecularRenderer::configureVdWShader(Shader& shader) {
    utilgl::addDefines(shader, lighting_);

    shader[ShaderType::Vertex]->setShaderDefine("FORCE_RADIUS", forceRadius_);

    shader.build();
}

void MolecularRenderer::configureLicoriceShader(Shader& shader) {
    utilgl::addDefines(shader, lighting_);

    shader.getFragmentShaderObject()->addShaderExtension("GL_ARB_conservative_depth", true);

    ShaderObject::OutDeclaration outdecl;
    outdecl.decl = "layout (depth_greater) out float gl_FragDepth;";
    shader.getFragmentShaderObject()->addOutDeclaration(outdecl);

    shader.build();
}

}  // namespace inviwo
