/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2021 Inviwo Foundation
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

#include <inviwo/molvisgl/processors/molecularrasterizer.h>

#include <modules/opengl/rendering/meshdrawergl.h>
#include <modules/opengl/shader/shaderutils.h>
#include <modules/opengl/shader/shadertype.h>
#include <modules/opengl/openglutils.h>
#include <modules/opengl/openglcapabilities.h>
#include <inviwo/core/datastructures/geometry/mesh.h>
#include <inviwo/core/util/document.h>

#include <inviwo/molvisbase/util/molvisutils.h>
#include <inviwo/molvisbase/util/chain.h>
#include <inviwo/molvisbase/algorithm/boundingbox.h>

#include <modules/meshrenderinggl/rendering/fragmentlistrenderer.h>
#include <modules/meshrenderinggl/datastructures/transformedrasterization.h>

#include <fmt/format.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo MolecularRasterizer::processorInfo_{
    "org.inviwo.MolecularRasterizer",  // Class identifier
    "Molecular Rasterizer",            // Display name
    "MolVis",                          // Category
    CodeState::Experimental,           // Code state
    "GL, MolVis, Rendering",           // Tags
};
const ProcessorInfo MolecularRasterizer::getProcessorInfo() const { return processorInfo_; }

MolecularRasterizer::MolecularRasterizer()
    : Processor()
    , inport_("inport")
    , outport_("rasterization")

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
    , forceOpaque_("forceOpaque", "Shade Opaque", false, InvalidationLevel::InvalidResources)
    , useUniformAlpha_("useUniformAlpha", "Uniform Alpha", false,
                       InvalidationLevel::InvalidResources)
    , uniformAlpha_("alphaValue", "Alpha", 0.7f, 0, 1, 0.1f, InvalidationLevel::InvalidOutput)
    , radiusScaling_("radiusScaling", "Radius Scaling", 1.0f, 0.0f, 2.0f)
    , forceRadius_("forceRadius", "Force Radius", false, InvalidationLevel::InvalidResources)
    , defaultRadius_("defaultRadius", "Default Radius", 0.15f, 0.00001f, 2.0f, 0.01f)
    , enableTooltips_("enableTooltips", "Enable Tooltips", true)
    , camera_("camera", "Camera", molvis::boundingBox(inport_))
    , lighting_("lighting", "Lighting", &camera_)

    , vdwShaders_{new MeshShaderCache(
          {{ShaderType::Vertex, std::string{"vdw.vert"}},
           {ShaderType::Geometry, std::string{"vdw.geom"}},
           {ShaderType::Fragment, std::string{"vdw-oit.frag"}}},
          {{BufferType::PositionAttrib, MeshShaderCache::Mandatory, "vec3"},
           {BufferType::ColorAttrib, MeshShaderCache::Optional, "vec4"},
           {BufferType::RadiiAttrib, MeshShaderCache::Optional, "float"},
           {BufferType::ScalarMetaAttrib, MeshShaderCache::Optional, "float"}},

          [&](Shader& shader) -> void {
              shader.onReload([this]() { invalidate(InvalidationLevel::InvalidResources); });
              configureVdWShader(shader);
          })}
    , licoriceShaders_{new MeshShaderCache(
          {{ShaderType::Vertex, std::string{"licorice.vert"}},
           {ShaderType::Geometry, std::string{"licorice.geom"}},
           {ShaderType::Fragment, std::string{"licorice-oit.frag"}}},
          {{BufferType::PositionAttrib, MeshShaderCache::Mandatory, "vec3"},
           {BufferType::ColorAttrib, MeshShaderCache::Optional, "vec4"},
           {BufferType::ScalarMetaAttrib, MeshShaderCache::Optional, "float"}},

          [&](Shader& shader) -> void {
              shader.onReload([this]() { invalidate(InvalidationLevel::InvalidResources); });
              configureLicoriceShader(shader);
          })} {

    addPort(inport_);
    addPort(outport_);

    fixedColor_.visibilityDependsOn(
        coloring_, [](auto& prop) { return prop.getSelectedValue() == Coloring::Fixed; });
    atomColormap_.visibilityDependsOn(
        coloring_, [](auto& prop) { return prop.getSelectedValue() == Coloring::Atoms; });
    aminoColormap_.visibilityDependsOn(
        coloring_, [](auto& prop) { return prop.getSelectedValue() == Coloring::Residues; });

    addProperties(representation_, coloring_, fixedColor_, atomColormap_, aminoColormap_,
                  forceOpaque_, useUniformAlpha_, uniformAlpha_, radiusScaling_, forceRadius_,
                  defaultRadius_, enableTooltips_, camera_, lighting_);

    camera_.setCollapsed(true);

    lighting_.lightPosition_.set(vec3(550.0f, 680.0f, 1000.0f));
    lighting_.ambientColor_.set(vec3(0.515f));
    lighting_.diffuseColor_.set(vec3(0.48f));
    lighting_.specularColor_.set(vec3(0.09f));
    lighting_.specularExponent_.set(1.9f);
    lighting_.setCollapsed(true);

    lighting_.setCurrentStateAsDefault();
}

void MolecularRasterizer::process() {
    const bool updateColorMap = [&]() {
        if (coloring_.isModified()) return true;
        switch (coloring_) {
            case Coloring::Atoms:
                return atomColormap_.isModified();
            case Coloring::Residues:
                return aminoColormap_.isModified();
            case Coloring::Fixed:
                return fixedColor_.isModified();
            case Coloring::Chains:
            default:
                return false;
        }
    }();

    if (meshes_.empty() || inport_.isChanged() || updateColorMap) {
        meshes_.clear();
        for (auto structure : inport_) {
            meshes_.push_back(
                createMesh(*structure, {coloring_, atomColormap_, aminoColormap_, fixedColor_}, 0));
        }
    }

    std::shared_ptr<const Rasterization> rasterization =
        std::make_shared<MolecularRasterization>(*this);
    outport_.setData(rasterization);
}

void MolecularRasterizer::initializeResources() {
    for (auto& item : vdwShaders_->getShaders()) {
        configureVdWShader(item.second);
    }
    for (auto& item : licoriceShaders_->getShaders()) {
        configureLicoriceShader(item.second);
    }
}

void MolecularRasterizer::configureVdWShader(Shader& shader) {
    utilgl::addDefines(shader, lighting_);

    shader[ShaderType::Vertex]->setShaderDefine("FORCE_RADIUS", forceRadius_);

    configureOITShader(shader);
    shader.build();
}

void MolecularRasterizer::configureLicoriceShader(Shader& shader) {
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

    configureOITShader(shader);
    shader.build();
}

void MolecularRasterizer::configureOITShader(Shader& shader) {
    auto fso = shader.getFragmentShaderObject();

    fso->addShaderExtension("GL_NV_gpu_shader5", true);
    fso->addShaderExtension("GL_EXT_shader_image_load_store", true);
    fso->addShaderExtension("GL_NV_shader_buffer_load", true);
    fso->addShaderExtension("GL_EXT_bindable_uniform", true);

    fso->setShaderDefine("USE_FRAGMENT_LIST",
                         !forceOpaque_.get() && FragmentListRenderer::supportsFragmentLists());

    fso->setShaderDefine("UNIFORM_ALPHA", useUniformAlpha_.get());
}

std::shared_ptr<Mesh> MolecularRasterizer::createMesh(const molvis::MolecularStructure& s,
                                                      ColorMapping colormap, size_t pickingId) {
    if (s.atoms().positions.empty()) {
        return std::make_shared<Mesh>(DrawType::Points, ConnectivityType::None);
    }

    using namespace molvis;

    const size_t atomCount = s.atoms().positions.size();
    std::vector<vec3> positions{
        util::transform(s.atoms().positions, [](const dvec3& p) { return glm::vec3{p}; })};

    std::vector<vec4> colors = [&]() {
        switch (colormap.coloring) {
            case Coloring::Atoms:
                if (!s.atoms().atomicNumbers.empty()) {
                    std::vector<vec4> colors;
                    for (auto elem : s.atoms().atomicNumbers) {
                        colors.emplace_back(element::color(elem, colormap.atoms));
                    }
                    return colors;
                } else {
                    return std::vector<vec4>(atomCount,
                                             element::color(Element::Unknown, colormap.atoms));
                }
            case Coloring::Residues:
                if (s.hasResidues()) {
                    std::vector<vec4> colors;
                    const auto& residues = s.residues();
                    for (auto resIndex : s.getResidueIndices()) {
                        colors.emplace_back(
                            aminoacid::color(residues[resIndex].aminoacid, colormap.aminoacids));
                    }
                    return colors;
                } else {
                    return std::vector<vec4>(
                        atomCount, aminoacid::color(AminoAcid::Unknown, colormap.aminoacids));
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
                return std::vector<vec4>(atomCount, colormap.fixedColor);
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
    std::iota(std::begin(picking), std::end(picking), static_cast<uint32_t>(pickingId));
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

MolecularRasterization::MolecularRasterization(const MolecularRasterizer& processor)
    : BallAndStickVDWScale(processor.BallAndStickVDWScale)
    , BallAndStickLicoriceScale(processor.BallAndStickLicoriceScale)
    , representation_(processor.representation_)
    , radiusScaling_(processor.radiusScaling_)
    , defaultRadius_(processor.defaultRadius_)
    , lighting_(processor.lighting_.getState())
    , uniformAlpha_(processor.uniformAlpha_)
    , forceOpaque_(processor.forceOpaque_)
    , vdwShaders_(processor.vdwShaders_)
    , licoriceShaders_(processor.licoriceShaders_)
    , meshes_(processor.meshes_) {}

void MolecularRasterization::rasterize(const ivec2& imageSize, const mat4& worldMatrixTransform,
                                       std::function<void(Shader&)> setUniforms) const {

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
        auto& shader = vdwShaders_->getShader(*mesh);

        shader.activate();
        shader.setUniform("defaultRadius", defaultRadius_);
        shader.setUniform("viewport", vec4(0.0f, 0.0f, 2.0f / imageSize.x, 2.0f / imageSize.y));
        shader.setUniform("radiusScaling_", radius);
        shader.setUniform("uniformAlpha", uniformAlpha_);
        utilgl::setShaderUniforms(shader, lighting_, "lighting");
        setUniforms(shader);
        auto transform = CompositeTransform(mesh->getModelMatrix(),
                                            mesh->getWorldMatrix() * worldMatrixTransform);
        utilgl::setShaderUniforms(shader, transform, "geometry");
        drawMesh(mesh, drawer, DrawType::Points);
        shader.deactivate();
    };
    auto drawLicorice = [&](std::shared_ptr<const Mesh> mesh, MeshDrawerGL::DrawObject& drawer,
                            float radius) {
        auto& shader = licoriceShaders_->getShader(*mesh);
        shader.activate();
        shader.setUniform("defaultRadius", defaultRadius_);
        shader.setUniform("radius_", 0.25f * radius);
        shader.setUniform("uniformAlpha", uniformAlpha_);
        utilgl::setShaderUniforms(shader, lighting_, "lighting");
        auto transform = CompositeTransform(mesh->getModelMatrix(),
                                            mesh->getWorldMatrix() * worldMatrixTransform);
        setUniforms(shader);
        utilgl::setShaderUniforms(shader, transform, "geometry");
        drawMesh(mesh, drawer, DrawType::Lines);
        shader.deactivate();
    };

    utilgl::GlBoolState depthTest(GL_DEPTH_TEST, !usesFragmentLists());

    for (auto mesh : meshes_) {
        if (mesh->getNumberOfBuffers() == 0) continue;

        MeshDrawerGL::DrawObject drawer{mesh->getRepresentation<MeshGL>(),
                                        mesh->getDefaultMeshInfo()};
        switch (representation_) {
            case MolecularRasterizer::Representation::VDW:
                drawVdW(mesh, drawer, radiusScaling_);
                break;
            case MolecularRasterizer::Representation::Licorice:
                drawLicorice(mesh, drawer, radiusScaling_);
                break;
            case MolecularRasterizer::Representation::BallAndStick:
                drawVdW(mesh, drawer, radiusScaling_ * BallAndStickVDWScale);
                drawLicorice(mesh, drawer, radiusScaling_ * BallAndStickLicoriceScale);
                break;
            case MolecularRasterizer::Representation::Ribbon:
                throw Exception("Unsupported representation: 'Ribbon'", IVW_CONTEXT);
                break;
            case MolecularRasterizer::Representation::Cartoon:
                throw Exception("Unsupported representation: 'Cartoon'", IVW_CONTEXT);
                break;
            default:
                drawVdW(mesh, drawer, radiusScaling_);
                break;
        }
    }
}

bool MolecularRasterization::usesFragmentLists() const {
    return !forceOpaque_ && FragmentListRenderer::supportsFragmentLists();
}

Document MolecularRasterization::getInfo() const {
    Document doc;
    doc.append("p", fmt::format("Molecular rasterization functor with {} molecule{}. {}.",
                                meshes_.size(), (meshes_.size() == 1) ? "" : "s",
                                usesFragmentLists() ? "Using A-buffer" : "Rendering opaque"));
    return doc;
}

Rasterization* MolecularRasterization::clone() const { return new MolecularRasterization(*this); }

}  // namespace inviwo
