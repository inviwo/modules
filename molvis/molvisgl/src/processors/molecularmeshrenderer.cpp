/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020-2026 Inviwo Foundation
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

#include <inviwo/molvisgl/processors/molecularmeshrenderer.h>

#include <modules/opengl/rendering/meshdrawergl.h>
#include <modules/opengl/shader/shaderutils.h>
#include <modules/opengl/shader/shadertype.h>
#include <modules/opengl/openglutils.h>
#include <modules/opengl/openglcapabilities.h>
#include <inviwo/core/algorithm/boundingbox.h>

#include <fmt/format.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo MolecularMeshRenderer::processorInfo_{
    "org.inviwo.molvis.MolecularMeshRenderer",  // Class identifier
    "Molecular Mesh Renderer",                  // Display name
    "MolVis",                                   // Category
    CodeState::Stable,                          // Code state
    "GL, MolVis",                               // Tags
    R"(Renders the input mesh in a molecular representation. The input mesh is expected to contain
    molecular data, that is points representing individual atoms (including their radius) and lines
    indicating bonds.
    
    The result is depending on the chosen molecular representation.
      - VDW (van der Waals): considers only points
      - Licorice:            considers both points and lines
     - Ball & Stick:        considers both points and lines
    If the mesh does not contain points or lines, the resulting rendering will lack either
    atoms/spheres or bonds.)"_unindentHelp,
};
const ProcessorInfo& MolecularMeshRenderer::getProcessorInfo() const { return processorInfo_; }

MolecularMeshRenderer::MolecularMeshRenderer()
    : Processor()
    , inport_("geometry", "input meshes containing molecular data"_help)
    , imageInport_("imageInport", "Optional background image"_help)
    , outport_("outport", "output image containing the moleculare rendering of the input"_help)
    , representation_("representation", "Representation",
                      {{"vdw", "VDW", Representation::VDW},
                       {"licorice", "Licorice", Representation::Licorice},
                       {"ballAndStick", "Ball & Stick", Representation::BallAndStick}},
                      0)
    , radiusScaling_("radiusScaling", "Radius Scaling", 1.0f, 0.0f, 2.0f)
    , forceRadius_("forceRadius", "Force Radius", false, InvalidationLevel::InvalidResources)
    , defaultRadius_("defaultRadius", "Default Radius", 0.15f, 0.00001f, 2.0f, 0.01f)
    , camera_("camera", "Camera", util::boundingBox(inport_))
    , lighting_("lighting", "Lighting", "Light settings for illuminating the molecular mesh"_help,
                LightingConfig{
                    .ambient = vec3{0.515f},
                    .diffuse = vec3{0.48f},
                    .specular = vec3{0.09f},
                    .specularExponent = 1.9f,
                },
                &camera_)
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
           {BufferType::PickingAttrib, MeshShaderCache::Optional, "uint"},
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

    lighting_.setCollapsed(true);
}

void MolecularMeshRenderer::process() {
    utilgl::activateTargetAndClearOrCopySource(outport_, imageInport_);

    auto drawMesh = [](std::shared_ptr<const Mesh> mesh, MeshDrawerGL::DrawObject& drawer,
                       DrawType dt) {
        if ((mesh->getNumberOfIndices() == 0) && (mesh->getDefaultMeshInfo().dt == dt)) {
            drawer.draw();
        } else {
            for (size_t i = 0; i < mesh->getNumberOfIndices(); ++i) {
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
            throw Exception("Unsupported representation: 'Ribbon'");
            break;
        case Representation::Cartoon:
            throw Exception("Unsupported representation: 'Cartoon'");
            break;
        default:
            drawVdW(radiusScaling_);
            break;
    }

    utilgl::deactivateCurrentTarget();
}

void MolecularMeshRenderer::initializeResources() {
    for (auto& item : vdwShaders_.getShaders()) {
        configureVdWShader(item.second);
    }
    for (auto& item : licoriceShaders_.getShaders()) {
        configureLicoriceShader(item.second);
    }
}

void MolecularMeshRenderer::configureVdWShader(Shader& shader) {
    utilgl::addDefines(shader, lighting_);

    shader[ShaderType::Vertex]->setShaderDefine("FORCE_RADIUS", forceRadius_);

    shader.build();
}

void MolecularMeshRenderer::configureLicoriceShader(Shader& shader) {
    utilgl::addDefines(shader, lighting_);

    const bool arbExt = OpenGLCapabilities::isExtensionSupported("GL_ARB_conservative_depth");
    const bool extExt = OpenGLCapabilities::isExtensionSupported("GL_EXT_conservative_depth");
    if (arbExt || extExt) {
        using namespace std::literals;
        const std::string_view ext =
            arbExt ? "GL_ARB_conservative_depth"sv : "GL_EXT_conservative_depth"sv;
        shader.getFragmentShaderObject()->addShaderExtension(ext, true);

        ShaderObject::OutDeclaration outdecl;
        // even though the driver might report that the extension is supported, it might not be
        // available in the shader. Therefore we need to wrap it with an ifdef block.
        outdecl.decl =
            fmt::format("#ifdef {}\nlayout (depth_greater) out float gl_FragDepth;\n#endif", ext);
        shader.getFragmentShaderObject()->addOutDeclaration(outdecl);
    }

    shader.build();
}

}  // namespace inviwo
