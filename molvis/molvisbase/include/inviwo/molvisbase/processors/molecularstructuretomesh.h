/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2025 Inviwo Foundation
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

#pragma once

#include <inviwo/molvisbase/molvisbasemoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/ports/meshport.h>
#include <inviwo/core/interaction/pickingmapper.h>

#include <inviwo/molvisbase/ports/molecularstructureport.h>
#include <inviwo/molvisbase/util/aminoacid.h>

namespace inviwo {

class PickingEvent;

/** \docpage{org.inviwo.MolecularStructureToMesh, Molecular Structure To Mesh}
 * ![](org.inviwo.MolecularStructureToMesh.png?classIdentifier=org.inviwo.MolecularStructureToMesh)
 * Converts a molecular datastructure object into a mesh consisting of point and line meshes. This
 * mesh can be rendered with a MolecularRenderer resulting in various molecular representations or,
 * alternatively, with any Mesh renderer, e.g. a SphereRenderer.
 */
class IVW_MODULE_MOLVISBASE_API MolecularStructureToMesh : public Processor {
public:
    MolecularStructureToMesh();
    virtual ~MolecularStructureToMesh() = default;

    virtual void process() override;

    virtual const ProcessorInfo& getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    void handlePicking(PickingEvent* p);
    std::vector<vec4> colors() const;

    enum class Coloring { Default, Atoms, Residues, Chains, Fixed };

    molvis::MolecularStructureInport inport_;
    MeshOutport outport_;

    OptionProperty<Coloring> coloring_;

    OptionProperty<molvis::element::Colormap> atomColormap_;
    OptionProperty<molvis::aminoacid::Colormap> aminoColormap_;
    FloatVec4Property fixedColor_;

    BoolProperty enableTooltips_;

    PickingMapper atomPicking_;
};

}  // namespace inviwo
