/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2021-2026 Inviwo Foundation
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
#include <inviwo/core/interaction/events/pickingevent.h>
#include <inviwo/core/util/document.h>

#include <inviwo/molvisbase/util/molvisutils.h>
#include <inviwo/molvisbase/util/chain.h>
#include <inviwo/molvisbase/algorithm/boundingbox.h>
#include <inviwo/molvisgl/util/molvisglutils.h>

#include <modules/oit/rendering/fragmentlistrenderer.h>
#include <modules/oit/datastructures/transformedrasterization.h>

#include <fmt/format.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo MolecularRasterizer::processorInfo_{
    "org.inviwo.MolecularRasterizer",  // Class identifier
    "Molecular Rasterizer",            // Display name
    "MolVis",                          // Category
    CodeState::Experimental,           // Code state
    "GL, MolVis, Rendering",           // Tags
    R"(
        Create a rasterization object to render one or more molecular datastructure objects in a
        molecular representation. The molecular data is colored using standard color maps. If residue or
        chain information is present, the data can also be colored according to residues and chains.

        The result is depending on the chosen molecular representation.
        + VDW (van der Waals): considers only atoms
        + Licorice:            considers both atoms and bonds
        + Ball & Stick:        considers both atoms and bonds
    )"_unindentHelp};

const ProcessorInfo& MolecularRasterizer::getProcessorInfo() const { return processorInfo_; }

MolecularRasterizer::MolecularRasterizer()
    : Rasterizer()
    , inport_("inport", "Molecular datastructures"_help)
    , brushing_("brushing", "Brushing & Linking"_help)

    , representation_("representation", "Representation", R"(
                        Determines the molecular representation
                        + *VDW (van der Waals)*: considers only atoms
                        + *Licorice*:            considers both atoms and bonds
                        + *Ball & Stick*:        considers both atoms and bonds
                    )"_unindentHelp,
                      {{"vdw", "VDW", Representation::VDW},
                       {"licorice", "Licorice", Representation::Licorice},
                       {"ballAndStick", "Ball & Stick", Representation::BallAndStick}},
                      0)
    , coloring_{"coloring",
                "Coloring",
                R"(
                    Coloring scheme applied to the molecular structure.
                    + *Fixed*: use a fixed color for the entire structure
                    + *Atoms*: applies a colormap (Rasmol CPK, Rasmol CPK new) based on atomic elements
                    + *Residues*: applies a colormap (Amino, Shapely, Ugene) based on amino acids
                    + *Chains*: chain IDs are mapped to colors
                    + *B-Factor*: B-Factors, that is the Debye-Waller factor (DWF) or temperature factor, are mapped to colors
                )"_unindentHelp,
                {{"fixed", "Fixed", Coloring::Fixed},
                 {"atoms", "Atoms", Coloring::Atoms},
                 {"residues", "Residues", Coloring::Residues},
                 {"chains", "Chains", Coloring::Chains},
                 {"bFactor", "B-Factor", Coloring::BFactor}},
                1}
    , atomColormap_{"atomColormap",
                    "Colormap",
                    "Colormap for coloring atomic elements"_help,
                    {{"cpk", "Rasmol CPK", molvis::element::Colormap::RasmolCPK},
                     {"cpknew", "Rasmol CPK new", molvis::element::Colormap::RasmolCPKnew},
                     {"jmol", "Jmol", molvis::element::Colormap::Jmol}},
                    1}
    , aminoColormap_{"aminoColormap",
                     "Colormap",
                     "Colormap for coloring amino acids/residues"_help,
                     {{"amino", "Amino", molvis::aminoacid::Colormap::Amino},
                      {"shapely", "Shapely", molvis::aminoacid::Colormap::Shapely},
                      {"ugene", "Ugene (multiple alignment)", molvis::aminoacid::Colormap::Ugene}}}
    , fixedColor_("fixedColor", "Color",
                  util::ordinalColor(0.8f, 0.8f, 0.8f, 1.0f)
                      .set("Fixed color for the entire structure"_help))
    , bFactorColormap_{"bFactorColormap", "Colormap", "Colormap for coloring B-Factors"_help,
                       TransferFunction{{{.pos = 0.0, .color = vec4{0.0f, 0.0f, 0.0f, 1.0f}},
                                         {.pos = 1.0, .color = vec4{1.0f, 1.0f, 1.0f, 1.0f}}}}}
    , showHighlighted_("showHighlighted", "Show Highlighted",
                       "Parameters for color overlay of highlighted data"_help, true,
                       vec3(0.35f, 0.75f, 0.93f))
    , showSelected_("showSelected", "Show Selected",
                    "Parameters for color overlay of a selection"_help, true,
                    vec3(1.0f, 0.84f, 0.0f))
    , showFiltered_("showFiltered", "Show Filtered",
                    "Parameters for color overlay of filtered data"_help, false,
                    vec3(0.5f, 0.5f, 0.5f))
    , forceOpaque_("forceOpaque", "Shade Opaque", false, InvalidationLevel::InvalidResources)
    , useUniformAlpha_("useUniformAlpha", "Uniform Alpha", false,
                       InvalidationLevel::InvalidResources)
    , uniformAlpha_("alphaValue", "Alpha", 0.7f, 0, 1, 0.1f, InvalidationLevel::InvalidOutput)
    , radiusScaling_("radiusScaling", "Radius Scaling",
                     "Scaling factor for the van der Waals representation and bonds"_help, 1.0f,
                     {0.0f, ConstraintBehavior::Immutable}, {2.0f, ConstraintBehavior::Editable})
    , forceRadius_("forceRadius", "Force Radius", "Use a uniform default radius for all atoms"_help,
                   false, InvalidationLevel::InvalidResources)
    , defaultRadius_("defaultRadius", "Default Radius",
                     "Default radius for all atoms if *Force Radius* is enabled"_help, 0.15f,
                     {0.00001f, ConstraintBehavior::Immutable},
                     {2.0f, ConstraintBehavior::Editable}, 0.01f)
    , enableTooltips_("enableTooltips", "Enable Tooltips",
                      "Show tooltips with detailed information for atoms"_help, true)
    , camera_("camera", "Camera", molvis::boundingBox(inport_))

    , vdwShaders_{{{ShaderType::Vertex, std::string{"vdw.vert"}},
                   {ShaderType::Geometry, std::string{"vdw.geom"}},
                   {ShaderType::Fragment, std::string{"vdw-oit.frag"}}},
                  {{BufferType::PositionAttrib, MeshShaderCache::Mandatory, "vec3"},
                   {BufferType::ColorAttrib, MeshShaderCache::Optional, "vec4"},
                   {BufferType::RadiiAttrib, MeshShaderCache::Optional, "float"},
                   {BufferType::PickingAttrib, MeshShaderCache::Optional, "uint"},
                   {BufferType::TexCoordAttrib, MeshShaderCache::Optional, "uint"}},

                  [&](Shader& shader) -> void {
                      shader.onReload(
                          [this]() { invalidate(InvalidationLevel::InvalidResources); });
                      configureVdWShader(shader);
                  }}
    , licoriceShaders_{{{ShaderType::Vertex, std::string{"licorice.vert"}},
                        {ShaderType::Geometry, std::string{"licorice.geom"}},
                        {ShaderType::Fragment, std::string{"licorice-oit.frag"}}},
                       {{BufferType::PositionAttrib, MeshShaderCache::Mandatory, "vec3"},
                        {BufferType::ColorAttrib, MeshShaderCache::Optional, "vec4"},
                        {BufferType::PickingAttrib, MeshShaderCache::Optional, "uint"},
                        {BufferType::TexCoordAttrib, MeshShaderCache::Optional, "uint"}},

                       [&](Shader& shader) -> void {
                           shader.onReload(
                               [this]() { invalidate(InvalidationLevel::InvalidResources); });
                           configureLicoriceShader(shader);
                       }}
    , atomPicking_(this, 1, [this](PickingEvent* e) { handlePicking(e); }) {

    addPort(inport_);
    addPort(brushing_);

    brushing_.setOptional(true);

    fixedColor_.visibilityDependsOn(
        coloring_, [](auto& prop) { return prop.getSelectedValue() == Coloring::Fixed; });
    atomColormap_.visibilityDependsOn(
        coloring_, [](auto& prop) { return prop.getSelectedValue() == Coloring::Atoms; });
    aminoColormap_.visibilityDependsOn(
        coloring_, [](auto& prop) { return prop.getSelectedValue() == Coloring::Residues; });
    bFactorColormap_.visibilityDependsOn(
        coloring_, [](auto& prop) { return prop.getSelectedValue() == Coloring::BFactor; });

    addProperties(representation_, coloring_, fixedColor_, atomColormap_, aminoColormap_,
                  bFactorColormap_, forceOpaque_, useUniformAlpha_, uniformAlpha_, radiusScaling_,
                  forceRadius_, defaultRadius_, showHighlighted_, showSelected_, showFiltered_,
                  enableTooltips_, camera_);

    camera_.setCollapsed(true);
    showHighlighted_.setCollapsed(true);
    showSelected_.setCollapsed(true);
    showFiltered_.setCollapsed(true);
}

void MolecularRasterizer::process() {
    atomPicking_.resize(std::max<size_t>(
        std::accumulate(inport_.begin(), inport_.end(), size_t{0u},
                        [](size_t val, auto s) { return val + s->atoms().positions.size(); }),
        1));

    const bool updateColorMap = [&]() {
        if (coloring_.isModified()) return true;
        switch (coloring_) {
            case Coloring::Atoms:
                return atomColormap_.isModified();
            case Coloring::Residues:
                return aminoColormap_.isModified();
            case Coloring::Fixed:
                return fixedColor_.isModified();
            case Coloring::BFactor:
                return bFactorColormap_.isModified();
            case Coloring::Chains:
            default:
                return false;
        }
    }();

    bool meshCreated = false;
    if (meshes_.empty() || inport_.isChanged() || updateColorMap) {
        meshes_.clear();
        const size_t pickingId = atomPicking_.getPickingId(0);
        size_t offset = 0;
        for (const auto& structure : inport_) {
            meshes_.push_back(createMesh(*structure,
                                         {.coloring = coloring_,
                                          .atoms = atomColormap_,
                                          .aminoacids = aminoColormap_,
                                          .bFactor = bFactorColormap_.get(),
                                          .fixedColor = fixedColor_},
                                         pickingId + offset));
            offset += structure->atoms().positions.size();
        }
        meshCreated = true;
    }
    if (brushing_.isChanged() || meshCreated) {
        size_t offset = 0;
        for (const auto& mesh : meshes_) {
            if (mesh->getBuffers().empty()) continue;
            molvis::updateBrushing(brushing_.getManager(), *mesh, offset, showSelected_,
                                   showHighlighted_);
            offset += mesh->getIndices(0)->getSize();
        }
    }

    Rasterizer::process();
}

void MolecularRasterizer::initializeResources() {
    for (auto& [state, shader] : vdwShaders_.getShaders()) {
        configureVdWShader(shader);
    }
    for (auto& [state, shader] : licoriceShaders_.getShaders()) {
        configureLicoriceShader(shader);
    }
}

void MolecularRasterizer::configureVdWShader(Shader& shader) {
    Rasterizer::configureShader(shader);

    shader[ShaderType::Vertex]->setShaderDefine("FORCE_RADIUS", forceRadius_);
    shader.build();
}

void MolecularRasterizer::configureLicoriceShader(Shader& shader) {
    Rasterizer::configureShader(shader);

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

void MolecularRasterizer::setUniforms(Shader& shader) {
    Rasterizer::setUniforms(shader);

    shader.setUniform("defaultRadius", defaultRadius_);
    shader.setUniform("uniformAlpha", uniformAlpha_);
    utilgl::setShaderUniforms(shader, showHighlighted_, "showHighlighted");
    utilgl::setShaderUniforms(shader, showSelected_, "showSelected");
    utilgl::setShaderUniforms(shader, showFiltered_, "showFiltered");
}

void MolecularRasterizer::rasterize(const ivec2& imageSize, const mat4& worldMatrixTransform) {

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

    auto drawVdW = [&](std::shared_ptr<const Mesh> mesh, MeshDrawerGL::DrawObject& drawer,
                       float radius) {
        auto& shader = vdwShaders_.getShader(*mesh);
        utilgl::Activate activate{&shader};

        setUniforms(shader);
        shader.setUniform("radius_", 0.25f * radius);
        shader.setUniform("radiusScaling_", radius);
        shader.setUniform("viewport", vec4(0.0f, 0.0f, 2.0f / imageSize.x, 2.0f / imageSize.y));
        auto transform = CompositeTransform(mesh->getModelMatrix(),
                                            mesh->getWorldMatrix() * worldMatrixTransform);
        utilgl::setShaderUniforms(shader, transform, "geometry");

        drawMesh(mesh, drawer, DrawType::Points);
    };
    auto drawLicorice = [&](std::shared_ptr<const Mesh> mesh, MeshDrawerGL::DrawObject& drawer,
                            float radius) {
        auto& shader = licoriceShaders_.getShader(*mesh);
        utilgl::Activate activate{&shader};

        shader.setUniform("radius_", 0.25f * radius);
        auto transform = CompositeTransform(mesh->getModelMatrix(),
                                            mesh->getWorldMatrix() * worldMatrixTransform);
        setUniforms(shader);
        utilgl::setShaderUniforms(shader, transform, "geometry");

        utilgl::CullFaceState cull(GL_BACK);
        drawMesh(mesh, drawer, DrawType::Lines);
    };

    utilgl::GlBoolState depthTest(GL_DEPTH_TEST, usesFragmentLists() == UseFragmentList::No);

    for (const auto& mesh : meshes_) {
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
                throw Exception("Unsupported representation: 'Ribbon'");
                break;
            case MolecularRasterizer::Representation::Cartoon:
                throw Exception("Unsupported representation: 'Cartoon'");
                break;
            default:
                drawVdW(mesh, drawer, radiusScaling_);
                break;
        }
    }
}

UseFragmentList MolecularRasterizer::usesFragmentLists() const {
    return !forceOpaque_.get() && FragmentListRenderer::supportsFragmentLists()
               ? UseFragmentList::Yes
               : UseFragmentList::No;
}

Document MolecularRasterizer::getInfo() const {
    Document doc;
    doc.append("p", fmt::format("Molecular rasterization functor with {} molecule{}. {}.",
                                meshes_.size(), (meshes_.size() == 1) ? "" : "s",
                                usesFragmentLists() == UseFragmentList::Yes ? "Using A-buffer"
                                                                            : "Rendering opaque"));
    return doc;
}

std::optional<mat4> MolecularRasterizer::boundingBox() const {
    return molvis::boundingBox(inport_)();
}

std::shared_ptr<Mesh> MolecularRasterizer::createMesh(const molvis::MolecularStructure& s,
                                                      const ColorMapping& colormap,
                                                      size_t pickingId) {
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
            case Coloring::BFactor:
                if (!s.atoms().bFactors.empty()) {
                    std::vector<vec4> colorVec;
                    auto [min, max] = std::ranges::minmax_element(s.atoms().bFactors);
                    const TransferFunction& tf = bFactorColormap_.get();
                    for (auto elem : s.atoms().bFactors) {
                        colorVec.emplace_back(tf.sample((elem - *min) / (*max - *min)));
                    }
                    return colorVec;
                } else {
                    return std::vector<vec4>(atomCount,
                                             element::color(Element::Unknown, atomColormap_));
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

void MolecularRasterizer::handlePicking(PickingEvent* p) {
    const uint32_t atomId = static_cast<uint32_t>(p->getPickedId());

    // Show tooltip for current item
    if (p->getPressState() == PickingPressState::None) {
        if (p->getHoverState() == PickingHoverState::Enter ||
            (p->getGlobalPickingId() != p->getPreviousGlobalPickingId())) {
            BitSet b(atomId);
            brushing_.highlight(b);
            if (enableTooltips_) {
                size_t offset = 0;
                for (const auto& structure : inport_) {
                    const auto atomCount = structure->atoms().positions.size();
                    if (atomId < offset + atomCount) {
                        p->setToolTip(
                            molvis::createToolTip(*structure, static_cast<int>(atomId - offset))
                                .str());
                        break;
                    }
                    offset += atomCount;
                }
            }
        } else if (p->getHoverState() == PickingHoverState::Exit) {
            brushing_.highlight({});
            if (enableTooltips_) {
                p->setToolTip("");
            }
        }
    }
}

}  // namespace inviwo
