/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2023 Inviwo Foundation
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

#include <inviwo/molvisbase/processors/molecularstructuretomesh.h>

#include <inviwo/core/interaction/events/pickingevent.h>

#include <inviwo/molvisbase/util/molvisutils.h>
#include <inviwo/molvisbase/util/aminoacid.h>
#include <inviwo/molvisbase/util/atomicelement.h>
#include <inviwo/molvisbase/util/chain.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo MolecularStructureToMesh::processorInfo_{
    "org.inviwo.molvis.MolecularStructureToMesh",  // Class identifier
    "Molecular Structure To Mesh",                 // Display name
    "MolVis",                                      // Category
    CodeState::Stable,                             // Code state
    "CPU, MolVis, Mesh",                           // Tags
};
const ProcessorInfo MolecularStructureToMesh::getProcessorInfo() const { return processorInfo_; }

MolecularStructureToMesh::MolecularStructureToMesh()
    : Processor()
    , inport_("inport")
    , outport_("outport")
    , coloring_{"coloring",
                "Coloring",
                {{"default", "Default (atoms)", Coloring::Default},
                 {"fixed", "Fixed", Coloring::Fixed},
                 {"atoms", "Atoms", Coloring::Atoms},
                 {"residues", "Residues", Coloring::Residues},
                 {"chains", "Chains", Coloring::Chains}},
                0}
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
    , enableTooltips_("enableTooltips", "Enable Tooltips", true)
    , atomPicking_(this, 1, [this](PickingEvent* e) { handlePicking(e); }) {

    addPort(inport_);
    addPort(outport_);

    fixedColor_.visibilityDependsOn(
        coloring_, [](auto& prop) { return prop.getSelectedValue() == Coloring::Fixed; });
    atomColormap_.visibilityDependsOn(
        coloring_, [](auto& prop) { return prop.getSelectedValue() == Coloring::Atoms; });
    aminoColormap_.visibilityDependsOn(
        coloring_, [](auto& prop) { return prop.getSelectedValue() == Coloring::Residues; });

    addProperties(coloring_, fixedColor_, atomColormap_, aminoColormap_, enableTooltips_);
}

void MolecularStructureToMesh::process() {
    atomPicking_.resize(std::max<size_t>(inport_.getData()->atoms().positions.size(), 1));

    auto mesh = molvis::createMesh(*inport_.getData(), enableTooltips_,
                                   static_cast<uint32_t>(atomPicking_.getPickingId(0)));

    if (coloring_ != Coloring::Default) {
        if (auto buf = mesh->findBuffer(BufferType::ColorAttrib); buf.first) {
            mesh->removeBuffer(buf.first);
        }
        mesh->addBuffer(BufferType::ColorAttrib, util::makeBuffer(colors()));
    }

    outport_.setData(mesh);
}

void MolecularStructureToMesh::handlePicking(PickingEvent* p) {
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

std::vector<vec4> MolecularStructureToMesh::colors() const {
    auto s = inport_.getData();
    const size_t atomCount = s->atoms().positions.size();

    using namespace molvis;

    switch (coloring_) {
        case Coloring::Atoms:
            if (!s->atoms().atomicNumbers.empty()) {
                std::vector<vec4> colors;
                for (auto elem : s->atoms().atomicNumbers) {
                    colors.emplace_back(element::color(elem, atomColormap_));
                }
                return colors;
            } else {
                return std::vector<vec4>(atomCount,
                                         element::color(Element::Unknown, atomColormap_));
            }
        case Coloring::Residues:
            if (s->hasResidues()) {
                std::vector<vec4> colors;
                const auto& residues = s->residues();
                for (auto resIndex : s->getResidueIndices()) {
                    colors.emplace_back(
                        aminoacid::color(residues[resIndex].aminoacid, aminoColormap_));
                }
                return colors;
            } else {
                return std::vector<vec4>(atomCount,
                                         aminoacid::color(AminoAcid::Unknown, aminoColormap_));
            }
        case Coloring::Chains:
            if (s->hasChains()) {
                std::vector<vec4> colors;
                for (auto chainId : s->atoms().chainIds) {
                    colors.emplace_back(chain::color(chainId));
                }
                return colors;
            } else {
                return std::vector<vec4>(atomCount, chain::color(ChainId::Unknown));
            }
        case Coloring::Fixed:
        case Coloring::Default:
        default:
            return std::vector<vec4>(atomCount, fixedColor_);
    }
}

}  // namespace inviwo
