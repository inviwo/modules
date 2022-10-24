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

#pragma once

#include <inviwo/molvisgl/molvisglmoduledefine.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/interaction/cameratrackball.h>
#include <inviwo/core/properties/cameraproperty.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/selectioncolorproperty.h>
#include <inviwo/core/properties/simplelightingproperty.h>
#include <inviwo/core/datastructures/light/lightingstate.h>
#include <inviwo/core/interaction/pickingmapper.h>

#include <modules/basegl/datastructures/meshshadercache.h>
#include <inviwo/molvisbase/ports/molecularstructureport.h>
#include <inviwo/molvisbase/util/aminoacid.h>

#include <modules/meshrenderinggl/datastructures/rasterization.h>
#include <modules/meshrenderinggl/ports/rasterizationport.h>

#include <modules/brushingandlinking/ports/brushingandlinkingports.h>

namespace inviwo {

class IVW_MODULE_MOLVISGL_API MolecularRasterizer : public Processor {
    friend class MolecularRasterization;

public:
    MolecularRasterizer();
    virtual ~MolecularRasterizer() = default;

    virtual void process() override;

    virtual void initializeResources() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    enum class Representation { VDW, Licorice, BallAndStick, Ribbon, Cartoon };
    enum class Coloring { Atoms, Residues, Chains, Fixed };

    struct ColorMapping {
        Coloring coloring;
        molvis::element::Colormap atoms;
        molvis::aminoacid::Colormap aminoacids;
        vec4 fixedColor;
    };

    const float BallAndStickVDWScale = 0.3f;
    const float BallAndStickLicoriceScale = 0.5f;

    void configureVdWShader(Shader& shader);
    void configureLicoriceShader(Shader& shader);
    void configureOITShader(Shader& shader);

    std::shared_ptr<Mesh> createMesh(const molvis::MolecularStructure& s, ColorMapping colormap,
                                     size_t pickingId);
    void handlePicking(PickingEvent* p);

    molvis::MolecularStructureFlatMultiInport inport_;
    BrushingAndLinkingInport brushing_;
    RasterizationOutport outport_;

    OptionProperty<Representation> representation_;
    OptionProperty<Coloring> coloring_;

    OptionProperty<molvis::element::Colormap> atomColormap_;
    OptionProperty<molvis::aminoacid::Colormap> aminoColormap_;
    FloatVec4Property fixedColor_;

    SelectionColorProperty showHighlighted_;
    SelectionColorProperty showSelected_;
    SelectionColorProperty showFiltered_;

    BoolProperty forceOpaque_;
    BoolProperty useUniformAlpha_;
    FloatProperty uniformAlpha_;

    FloatProperty radiusScaling_;
    BoolProperty forceRadius_;
    FloatProperty defaultRadius_;

    BoolProperty enableTooltips_;

    CameraProperty camera_;
    SimpleLightingProperty lighting_;

    std::shared_ptr<MeshShaderCache> vdwShaders_;
    std::shared_ptr<MeshShaderCache> licoriceShaders_;
    PickingMapper atomPicking_;

    std::vector<std::shared_ptr<Mesh>> meshes_;
};

/**
 * \brief Functor object that will render molecular data into a fragment list.
 */
class IVW_MODULE_MOLVISGL_API MolecularRasterization : public Rasterization {
public:
    MolecularRasterization(const MolecularRasterizer& processor);
    virtual void rasterize(const ivec2& imageSize, const mat4& worldMatrixTransform,
                           std::function<void(Shader&)> setUniforms) const override;
    virtual bool usesFragmentLists() const override;
    virtual Document getInfo() const override;
    virtual Rasterization* clone() const override;

protected:
    const float BallAndStickVDWScale;
    const float BallAndStickLicoriceScale;

    MolecularRasterizer::Representation representation_;
    float radiusScaling_;
    float defaultRadius_;
    LightingState lighting_;
    SelectionColorState highlighted_;
    SelectionColorState selected_;
    SelectionColorState filtered_;
    float uniformAlpha_;
    const bool forceOpaque_;

    std::shared_ptr<MeshShaderCache> vdwShaders_;
    std::shared_ptr<MeshShaderCache> licoriceShaders_;
    std::vector<std::shared_ptr<Mesh>> meshes_;
};

}  // namespace inviwo
