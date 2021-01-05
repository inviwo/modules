/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2017-2021 Inviwo Foundation
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

#ifndef IVW_TENSORFIELD3DMETADATA_H
#define IVW_TENSORFIELD3DMETADATA_H

#include <inviwo/tensorvisbase/tensorvisbasemoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/tensorvisbase/ports/tensorfieldport.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/buttonproperty.h>

namespace inviwo {

/** \docpage{org.inviwo.TensorField3DMetaData, Tensor Field3DMeta Data}
 * ![](org.inviwo.TensorField3DMetaData.png?classIdentifier=org.inviwo.TensorField3DMetaData)
 * Explanation of how to use the processor.
 *
 * ### Inports
 *   * __<Inport1>__ <description>.
 *
 * ### Outports
 *   * __<Outport1>__ <description>.
 *
 * ### Properties
 *   * __<Prop1>__ <description>.
 *   * __<Prop2>__ <description>
 */

/**
 * \class TensorField3DMetaData
 * \brief VERY_BRIEFLY_DESCRIBE_THE_PROCESSOR
 * DESCRIBE_THE_PROCESSOR_FROM_A_DEVELOPER_PERSPECTIVE
 */
class IVW_MODULE_TENSORVISBASE_API TensorField3DMetaData : public Processor {
public:
    TensorField3DMetaData();
    virtual ~TensorField3DMetaData() = default;

    virtual void initializeResources() override;
    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    TensorField3DInport inport_;
    TensorField3DOutport outport_;

    BoolProperty sigma1_;
    BoolProperty sigma2_;
    BoolProperty sigma3_;
    BoolProperty ev1_;
    BoolProperty ev2_;
    BoolProperty ev3_;
    BoolProperty i1_;
    BoolProperty i2_;
    BoolProperty i3_;
    BoolProperty j1_;
    BoolProperty j2_;
    BoolProperty j3_;
    BoolProperty lodeAngle_;
    BoolProperty anisotropy_;
    BoolProperty linearAnisotropy_;
    BoolProperty planarAnisotropy_;
    BoolProperty sphericalAnisotropy_;
    BoolProperty diffusivity_;
    BoolProperty shearStress_;
    BoolProperty pureShear_;
    BoolProperty shapeFactor_;
    BoolProperty isotropicScaling_;
    BoolProperty rotation_;
    BoolProperty frobeniusNorm_;
    BoolProperty hillYieldCriterion_;

    ButtonProperty selectAll_;
    ButtonProperty deselectAll_;

    std::shared_ptr<TensorField3D> tensorFieldOut_;

    void addMetaData();
    void removeMetaData();

    void selectAll();
    void deselectAll();
};

}  // namespace inviwo

#endif  // IVW_TENSORFIELD3DMETADATA_H
