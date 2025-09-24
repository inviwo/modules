/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020-2025 Inviwo Foundation
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

#include <inviwo/molvisgl/molvisglmoduledefine.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/interaction/cameratrackball.h>
#include <inviwo/core/properties/cameraproperty.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/simplelightingproperty.h>
#include <inviwo/core/properties/selectioncolorproperty.h>
#include <inviwo/core/properties/transferfunctionproperty.h>
#include <inviwo/core/ports/imageport.h>
#include <inviwo/core/interaction/pickingmapper.h>

#include <modules/basegl/datastructures/meshshadercache.h>
#include <inviwo/molvisbase/ports/molecularstructureport.h>
#include <inviwo/molvisbase/util/aminoacid.h>

#include <modules/brushingandlinking/ports/brushingandlinkingports.h>

namespace inviwo {

class Mesh;
class PickingEvent;

class IVW_MODULE_MOLVISGL_API MolecularRenderer : public Processor {
public:
    MolecularRenderer();
    virtual ~MolecularRenderer() = default;

    virtual void process() override;

    virtual void initializeResources() override;

    virtual const ProcessorInfo& getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    enum class Representation { VDW, Licorice, BallAndStick, Ribbon, Cartoon };
    enum class Coloring : std::uint8_t { Atoms, Residues, Chains, BFactor, Fixed };

    struct ColorMapping {
        Coloring coloring;
        molvis::element::Colormap atoms;
        molvis::aminoacid::Colormap aminoacids;
        const TransferFunction& bFactor;
        vec4 fixedColor;
    };

    const float BallAndStickVDWScale = 0.3f;
    const float BallAndStickLicoriceScale = 0.5f;

    void configureVdWShader(Shader& shader);
    void configureLicoriceShader(Shader& shader);
    std::shared_ptr<Mesh> createMesh(const molvis::MolecularStructure& s,
                                     const ColorMapping& colormap, size_t pickingId);
    void handlePicking(PickingEvent* p);

    molvis::MolecularStructureFlatMultiInport inport_;
    ImageInport imageInport_;
    BrushingAndLinkingInport brushing_;
    ImageOutport outport_;

    OptionProperty<Representation> representation_;
    OptionProperty<Coloring> coloring_;

    OptionProperty<molvis::element::Colormap> atomColormap_;
    OptionProperty<molvis::aminoacid::Colormap> aminoColormap_;
    FloatVec4Property fixedColor_;
    TransferFunctionProperty bFactorColormap_;

    SelectionColorProperty showHighlighted_;
    SelectionColorProperty showSelected_;
    SelectionColorProperty showFiltered_;

    FloatProperty radiusScaling_;
    BoolProperty forceRadius_;
    FloatProperty defaultRadius_;

    BoolProperty enableTooltips_;

    CameraProperty camera_;
    SimpleLightingProperty lighting_;
    CameraTrackball trackball_;

    MeshShaderCache vdwShaders_;
    MeshShaderCache licoriceShaders_;
    PickingMapper atomPicking_;

    std::vector<std::shared_ptr<Mesh>> meshes_;
};

}  // namespace inviwo
