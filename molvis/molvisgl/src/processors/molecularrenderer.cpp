/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020-2021 Inviwo Foundation
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
#include <inviwo/core/datastructures/geometry/mesh.h>
#include <inviwo/core/interaction/events/pickingevent.h>

#include <inviwo/molvisbase/util/molvisutils.h>
#include <inviwo/molvisbase/util/chain.h>
#include <inviwo/molvisbase/algorithm/boundingbox.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo MolecularRenderer::processorInfo_{
    "org.inviwo.molvis.MolecularRenderer",  // Class identifier
    "Molecular Renderer",                   // Display name
    "MolVis",                               // Category
    CodeState::Stable,                      // Code state
    "GL, MolVis",                           // Tags
};
const ProcessorInfo MolecularRenderer::getProcessorInfo() const { return processorInfo_; }

MolecularRenderer::MolecularRenderer()
    : Processor()
    , inport_("inport")
    , imageInport_("imageInport")
    , outport_("outport")
    , representation_("representation", "Representation",
                      {{"vdw", "VDW", Representation::VDW},
                       {"licorice", "Licorice", Representation::Licorice},
                       {"ballAndStick", "Ball & Stick", Representation::BallAndStick}},
                      0)
    , coloring_{"coloring",
                "Coloring",
                {{"fixed", "Fixed", Coloring::Fixed},
                 {"atoms", "Atoms", Coloring::Atoms},
                 {"residues", "Residues", Coloring::Residues},
                 {"chains", "Chains", Coloring::Chains}},
                1}
    , atomColormap_{"atomColormap",
                    "Colormap",
                    {{"cpk", "Rasmol CPK", molvis::element::Colormap::RasmolCPK},
                     {"cpknew", "Rasmol CPK new", molvis::element::Colormap::RasmolCPKnew}},
                    1}
    , aminoColormap_{"aminoColormap",
                     "Colormap",
                     {{"amino", "Amino", molvis::aminoacid::Colormap::Amino},
                      {"shapely", "Shapely", molvis::aminoacid::Colormap::Shapely},
                      {"ugene", "Ugene (multiple alignment)", molvis::aminoacid::Colormap::Ugene}}}
    , fixedColor_("fixedColor", "Color", util::ordinalColor(0.8f, 0.8f, 0.8f, 1.0f))
    , radiusScaling_("radiusScaling", "Radius Scaling", 1.0f, 0.0f, 2.0f)
    , forceRadius_("forceRadius", "Force Radius", false, InvalidationLevel::InvalidResources)
    , defaultRadius_("defaultRadius", "Default Radius", 0.15f, 0.00001f, 2.0f, 0.01f)
    , enableTooltips_("enableTooltips", "Enable Tooltips", true)
    , camera_("camera", "Camera", molvis::boundingBox(inport_))
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
    , licoriceShaders_{{{ShaderType::Vertex, std::string{"licorice.vert"}},
                        {ShaderType::Geometry, std::string{"licorice.geom"}},
                        {ShaderType::Fragment, std::string{"licorice.frag"}}},
                       {{BufferType::PositionAttrib, MeshShaderCache::Mandatory, "vec3"},
                        {BufferType::ColorAttrib, MeshShaderCache::Optional, "vec4"},
                        {BufferType::PickingAttrib, MeshShaderCache::Optional, "uint"},
                        {BufferType::ScalarMetaAttrib, MeshShaderCache::Optional, "float"}},

                       [&](Shader& shader) -> void {
                           shader.onReload(
                               [this]() { invalidate(InvalidationLevel::InvalidResources); });
                           configureLicoriceShader(shader);
                       }}
    , atomPicking_(this, 1, [this](PickingEvent* e) { handlePicking(e); }) {

    addPort(inport_);
    addPort(imageInport_);
    addPort(outport_);

    imageInport_.setOptional(true);

    fixedColor_.visibilityDependsOn(
        coloring_, [](auto& prop) { return prop.getSelectedValue() == Coloring::Fixed; });
    atomColormap_.visibilityDependsOn(
        coloring_, [](auto& prop) { return prop.getSelectedValue() == Coloring::Atoms; });
    aminoColormap_.visibilityDependsOn(
        coloring_, [](auto& prop) { return prop.getSelectedValue() == Coloring::Residues; });

    addProperties(representation_, coloring_, fixedColor_, atomColormap_, aminoColormap_,
                  radiusScaling_, forceRadius_, defaultRadius_, enableTooltips_, camera_, lighting_,
                  trackball_);

    lighting_.lightPosition_.set(vec3(550.0f, 680.0f, 1000.0f));
    lighting_.ambientColor_.set(vec3(0.515f));
    lighting_.diffuseColor_.set(vec3(0.48f));
    lighting_.specularColor_.set(vec3(0.09f));
    lighting_.specularExponent_.set(1.9f);
    lighting_.setCollapsed(true);

    lighting_.setCurrentStateAsDefault();
}

void MolecularRenderer::process() {
    atomPicking_.resize(inport_.getData()->atoms().positions.size());

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

    auto drawVdW = [&](std::shared_ptr<const Mesh> mesh, MeshDrawerGL::DrawObject& drawer,
                       float radius) {
        auto& shader = vdwShaders_.getShader(*mesh);

        shader.activate();
        utilgl::setUniforms(shader, camera_, lighting_, defaultRadius_);
        shader.setUniform("viewport", vec4(0.0f, 0.0f, 2.0f / outport_.getDimensions().x,
                                           2.0f / outport_.getDimensions().y));
        shader.setUniform("radiusScaling_", radius);
        utilgl::setShaderUniforms(shader, *mesh, "geometry");
        drawMesh(mesh, drawer, DrawType::Points);
        shader.deactivate();
    };
    auto drawLicorice = [&](std::shared_ptr<const Mesh> mesh, MeshDrawerGL::DrawObject& drawer,
                            float radius) {
        auto& shader = licoriceShaders_.getShader(*mesh);
        shader.activate();
        utilgl::setUniforms(shader, camera_, lighting_, defaultRadius_);
        shader.setUniform("radius_", 0.25f * radius);
        utilgl::setShaderUniforms(shader, *mesh, "geometry");
        drawMesh(mesh, drawer, DrawType::Lines);
        shader.deactivate();
    };

    for (auto structure : inport_) {
        auto mesh = createMesh(*structure);
        MeshDrawerGL::DrawObject drawer{mesh->getRepresentation<MeshGL>(),
                                        mesh->getDefaultMeshInfo()};
        switch (representation_) {
            case Representation::VDW:
                drawVdW(mesh, drawer, radiusScaling_);
                break;
            case Representation::Licorice:
                drawLicorice(mesh, drawer, radiusScaling_);
                break;
            case Representation::BallAndStick:
                drawVdW(mesh, drawer, radiusScaling_ * BallAndStickVDWScale);
                drawLicorice(mesh, drawer, radiusScaling_ * BallAndStickLicoriceScale);
                break;
            case Representation::Ribbon:
                throw Exception("Unsupported representation: 'Ribbon'", IVW_CONTEXT);
                break;
            case Representation::Cartoon:
                throw Exception("Unsupported representation: 'Cartoon'", IVW_CONTEXT);
                break;
            default:
                drawVdW(mesh, drawer, radiusScaling_);
                break;
        }
    }

    utilgl::deactivateCurrentTarget();
}

void MolecularRenderer::initializeResources() {
    for (auto& item : vdwShaders_.getShaders()) {
        configureVdWShader(item.second);
    }
    for (auto& item : licoriceShaders_.getShaders()) {
        configureLicoriceShader(item.second);
    }
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

std::shared_ptr<Mesh> MolecularRenderer::createMesh(const molvis::MolecularStructure& s) const {
    if (s.atoms().positions.empty()) {
        return std::make_shared<Mesh>(DrawType::Points, ConnectivityType::None);
    }

    using namespace molvis;

    const size_t atomCount = s.atoms().positions.size();
    std::vector<vec3> positions{
        util::transform(s.atoms().positions, [](const dvec3& p) { return glm::vec3{p}; })};

    std::vector<vec4> colors = [&]() {
        switch (coloring_) {
            case Coloring::Atoms:
                if (!s.atoms().atomicNumbers.empty()) {
                    std::vector<vec4> colors;
                    for (auto elem : s.atoms().atomicNumbers) {
                        colors.emplace_back(element::color(elem, atomColormap_));
                    }
                    return colors;
                } else {
                    return std::vector<vec4>(atomCount,
                                             element::color(Element::Unknown, atomColormap_));
                }
            case Coloring::Residues:
                if (s.hasResidues()) {
                    std::vector<vec4> colors;
                    const auto& residues = s.residues();
                    for (auto resIndex : s.getResidueIndices()) {
                        colors.emplace_back(
                            aminoacid::color(residues[resIndex].aminoacid, aminoColormap_));
                    }
                    return colors;
                } else {
                    return std::vector<vec4>(atomCount,
                                             aminoacid::color(AminoAcid::Unknown, aminoColormap_));
                }
            case Coloring::Chains:
                if (s.hasChains()) {
                    std::vector<vec4> colors;
                    for (auto chainId : s.atoms().chainIds) {
                        colors.emplace_back(chain::color(chainId));
                    }
                    return colors;
                } else {
                    return std::vector<vec4>(atomCount, chain::color(ChainId::Unknown));
                }
            case Coloring::Fixed:
            default:
                return std::vector<vec4>(atomCount, fixedColor_);
        }
    }();

    std::vector<float> radii;
    for (auto elem : s.atoms().atomicNumbers) {
        radii.emplace_back(static_cast<float>(element::vdwRadius(elem)));
    }
    if (s.atoms().atomicNumbers.empty()) {
        // fall-back if atomic numbers are not available
        radii.resize(atomCount, static_cast<float>(element::vdwRadius(Element::Unknown)));
    }

    auto mesh = std::make_shared<Mesh>(DrawType::Points, ConnectivityType::None);
    mesh->addBuffer(BufferType::PositionAttrib, util::makeBuffer(std::move(positions)));
    mesh->addBuffer(BufferType::ColorAttrib, util::makeBuffer(std::move(colors)));
    mesh->addBuffer(BufferType::RadiiAttrib, util::makeBuffer(std::move(radii)));

    std::vector<uint32_t> picking(atomCount);
    std::iota(std::begin(picking), std::end(picking),
              static_cast<uint32_t>(atomPicking_.getPickingId(0)));
    mesh->addBuffer(BufferType::PickingAttrib, util::makeBuffer(std::move(picking)));

    // atoms
    std::vector<uint32_t> indices(atomCount);
    std::iota(indices.begin(), indices.end(), 0);
    mesh->addIndices(Mesh::MeshInfo(DrawType::Points, ConnectivityType::None),
                     util::makeIndexBuffer(std::move(indices)));

    // bonds
    if (!s.bonds().empty()) {
        indices.clear();
        indices.reserve(s.bonds().size() * 2);
        for (const auto& bond : s.bonds()) {
            indices.push_back(static_cast<uint32_t>(bond.first));
            indices.push_back(static_cast<uint32_t>(bond.second));
        }
        mesh->addIndices(Mesh::MeshInfo(DrawType::Lines, ConnectivityType::None),
                         util::makeIndexBuffer(std::move(indices)));
    }

    return mesh;
}

void MolecularRenderer::handlePicking(PickingEvent* p) {
    const uint32_t atomId = static_cast<uint32_t>(p->getPickedId());

    // Show tooltip for current item
    if (enableTooltips_ && p->getPressState() == PickingPressState::None) {
        if (p->getHoverState() == PickingHoverState::Move ||
            p->getHoverState() == PickingHoverState::Enter) {
            p->setToolTip(molvis::createToolTip(*inport_.getData(), atomId));
        } else if (p->getHoverState() == PickingHoverState::Exit) {
            p->setToolTip("");
        }
    }
}

}  // namespace inviwo
