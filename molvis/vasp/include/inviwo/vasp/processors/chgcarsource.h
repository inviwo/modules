/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2024 Inviwo Foundation
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

#include <inviwo/vasp/vaspmoduledefine.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/core/processors/poolprocessor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/fileproperty.h>
#include <inviwo/core/properties/buttonproperty.h>
#include <modules/base/properties/basisproperty.h>
#include <inviwo/core/ports/volumeport.h>
#include <inviwo/core/ports/meshport.h>
#include <inviwo/core/interaction/pickingmapper.h>

#include <modules/base/properties/volumeinformationproperty.h>
#include <modules/brushingandlinking/ports/brushingandlinkingports.h>
#include <inviwo/dataframe/datastructures/dataframe.h>
#include <inviwo/molvisbase/ports/molecularstructureport.h>

#include <string_view>
#include <optional>

namespace inviwo {

namespace vasp {

struct Potential {
    std::string_view name = "";
    double valenceElectrons = 0.0;
    std::string_view valenceConfiguration = "";
};

enum struct PotentialType { Standard_LDA, Standard_PBE, GW_LDA, GW_PBE };

std::optional<Potential> findPotential(PotentialType type, std::string_view name);

}  // namespace vasp

class Chgcar;

class IVW_MODULE_VASP_API ChgcarSource : public PoolProcessor {
public:
    ChgcarSource();
    virtual ~ChgcarSource();

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

    virtual void deserialize(Deserializer& d) override;

private:
    void picking(const PickingEvent* event);

    VolumeOutport chargeOutport_;
    VolumeOutport magnetizationOutport_;
    MeshOutport atomsOutport_;
    DataFrameOutport atomInformationOutport_;
    molvis::MolecularStructureOutport moleculeOutport_;
    BrushingAndLinkingInport bnlInport_;

    FileProperty file_;
    ButtonProperty reload_;
    BoolProperty readChg_;
    BoolProperty readMag_;
    VolumeInformationProperty chgInfo_;
    VolumeInformationProperty magInfo_;
    BasisProperty basis_;

    OptionProperty<vasp::PotentialType> potential_;
    StringProperty potcars_;
    OptionProperty<molvis::element::Colormap> colormap_;
    DoubleProperty radiusScaling_;
    DoubleProperty borderMargin_;

    PickingMapper pm_;

    std::unique_ptr<Chgcar> data_;
    std::shared_ptr<VolumeRAMPrecision<float>> chg_;
    dvec2 chgDataRange_;
    std::shared_ptr<VolumeRAMPrecision<float>> mag_;
    dvec2 magDataRange_;
    bool deserialized_ = false;
};

}  // namespace inviwo
