/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2025 Inviwo Foundation
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

#include <inviwo/springsystem/springsystemmoduledefine.h>
#include <inviwo/springsystem/datastructures/gravityspringsystem.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/buttonproperty.h>
#include <inviwo/core/interaction/pickingmapper.h>
#include <inviwo/core/properties/cameraproperty.h>
#include <inviwo/core/ports/meshport.h>
#include <inviwo/core/util/timer.h>

namespace inviwo {

class Mesh;
class PickingEvent;

class IVW_MODULE_SPRINGSYSTEM_API SpringSystemProcessor : public Processor {
public:
    SpringSystemProcessor();
    virtual ~SpringSystemProcessor() = default;

    virtual void process() override;

    virtual void deserialize(Deserializer& d) override;

    virtual const ProcessorInfo& getProcessorInfo() const override;

    static const ProcessorInfo processorInfo_;

private:
    void updateMesh();
    void updateSystemFromProperties();
    void handlePicking(PickingEvent* pe);
    GravitySpringSystem<2, double> getSystem();

    MeshOutport nodeOutport_;
    MeshOutport springOutport_;

    OptionPropertyInt springLayout_;
    IntProperty nodeCount_;
    DoubleProperty nodeSpacing_;

    DoubleProperty dampingCoeff_;
    DoubleProperty nodeMass_;
    DoubleProperty springConst_;
    DoubleProperty springRestLength_;

    DoubleProperty deltaT_;
    IntProperty iterationsPerStep_;
    DoubleVec2Property externalForce_;
    FloatProperty scaleFactor_;
    ButtonProperty advanceButton_;
    ButtonProperty resetButton_;
    ButtonProperty startButton_;
    ButtonProperty stopButton_;
    ButtonProperty logStatusButton_;

    CameraProperty camera_;

    bool advance_;
    bool meshDirty_;
    Timer updateTimer_;

    GravitySpringSystem<2, double> springSystem_;
    PickingMapper nodePicking_;
};

}  // namespace inviwo
