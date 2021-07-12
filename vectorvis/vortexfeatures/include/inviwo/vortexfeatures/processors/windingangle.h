/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020 Inviwo Foundation
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

#include <inviwo/vortexfeatures/vortexfeaturesmoduledefine.h>
#include <modules/vectorfieldvisualization/vectorfieldvisualizationmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/processors/processortraits.h>
#include <inviwo/core/processors/poolprocessor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/compositeproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/ports/datainport.h>
#include <inviwo/core/ports/imageport.h>
#include <inviwo/core/ports/meshport.h>
#include <inviwo/core/util/utilities.h>
#include <inviwo/core/util/foreach.h>
#include <modules/vectorfieldvisualization/algorithms/integrallineoperations.h>
#include <modules/vectorfieldvisualization/integrallinetracer.h>
#include <modules/vectorfieldvisualization/ports/seedpointsport.h>

namespace inviwo {

/** \docpage{org.inviwo.WindingAngle, Winding Angle}
 * ![](org.inviwo.WindingAngle.png?classIdentifier=org.inviwo.WindingAngle)
 * Explanation of how to use the processor.
 */
class IVW_MODULE_VORTEXFEATURES_API WindingAngle : public PoolProcessor {
public:
    WindingAngle();
    virtual ~WindingAngle() = default;

    virtual void process() override;
    void integrateLines();

private:
    // void integrateLines(pool::Stop stop, pool::Progress progress);
    // void filterLines(pool::Stop stop, pool::Progress progress);

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    ImageInport inImage_;
    IntegralLineSetOutport outLines_;
    IntegralLineSetOutport outAllLines_;
    MeshOutport outCenters_;

    IntegralLineProperties properties_;
    BoolProperty relativeThreshold_;
    DoubleProperty relativeDistanceThreshold_;
    DoubleProperty absoluteDistanceThreshold_;
    std::shared_ptr<IntegralLineSet> allLines_;
};

}  // namespace inviwo
