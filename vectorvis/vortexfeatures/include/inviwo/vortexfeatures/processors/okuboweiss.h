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
#include <inviwo/vortexfeatures/algorithms/calculatemeasure.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/ports/imageport.h>

namespace inviwo {

/** \docpage{org.inviwo.OkuboWeiss, Okubo Weiss}
 * ![](org.inviwo.OkuboWeiss.png?classIdentifier=org.inviwo.OkuboWeiss)
 * Calculate the Okubo-Weiss criterion for eddy detection to a volume or layer/image.
 * 0s will be mapped to the new fields minimum for convenience.
 */
template <unsigned SpatialDims>
class IVW_MODULE_VORTEXFEATURES_API OkuboWeissProcessor : public MeasureProcessor<SpatialDims> {
public:
    // Should give warning if not orthogonal!!!
    OkuboWeissProcessor() {}
    virtual ~OkuboWeissProcessor() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override { return processorInfo_; }
    static const ProcessorInfo processorInfo_;
};

template <unsigned SpatialDims>
void OkuboWeissProcessor<SpatialDims>::process() {
    LogWarn("Processing OK!");
    this->template dispatchMeasure<dispatching::filter::Vecs>(
        [](auto velocity, auto jacobian) -> float {
            auto shear = jacobian[0][0] * jacobian[1][1];
            auto vort = jacobian[1][0] * jacobian[0][1];
            return float(shear * shear - vort * vort);
        },
        0.0);
}

template <unsigned SpatialDims>
const ProcessorInfo OkuboWeissProcessor<SpatialDims>::processorInfo_{
    fmt::format("org.inviwo.OkuboWeiss{0}D", SpatialDims),  // Class identifier
    fmt::format("Okubo Weiss {0}D", SpatialDims),           // Display name
    "Vortex Features",                                      // Category
    CodeState::Experimental,                                // Code state
    Tags::None,                                             // Tags
};

// using OkuboWeiss2D = OkuboWeissProcessor<2>;
using OkuboWeiss3D = OkuboWeissProcessor<3>;

}  // namespace inviwo
