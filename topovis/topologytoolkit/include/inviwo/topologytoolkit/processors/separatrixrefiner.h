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

#include <inviwo/topologytoolkit/topologytoolkitmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/topologytoolkit/ports/morsesmalecomplexport.h>
#include <inviwo/core/ports/meshport.h>

#include <inviwo/springsystem/datastructures/springsystem.h>
#include <inviwo/core/util/spatialsampler.h>

#include <inviwo/topologytoolkit/properties/topologycolorsproperty.h>
#include <inviwo/topologytoolkit/properties/topologyfilterproperty.h>

namespace inviwo {

/** \docpage{org.inviwo.SeparatrixRefiner, Separatrix Refiner}
 * ![](org.inviwo.SeparatrixRefiner.png?classIdentifier=org.inviwo.SeparatrixRefiner)
 */

class IVW_MODULE_TOPOLOGYTOOLKIT_API SeparatrixRefiner : public Processor {
public:
    SeparatrixRefiner();
    virtual ~SeparatrixRefiner() = default;

    virtual void process() override;

    virtual const ProcessorInfo& getProcessorInfo() const override;

    static const ProcessorInfo processorInfo_;

    topology::MorseSmaleComplexInport inport_;
    DataInport<SpatialSampler<dvec3>> sampler_;
    MeshOutport outport_;

private:
    TopologyColorsProperty colors_;
    TopologyFilterProperty filters_;
    FloatProperty sphereRadius_;
    FloatProperty lineThickness_;
    BoolProperty fillPBC_;

    CompositeProperty springSys_;
    IntSizeTProperty timesteps_;
    FloatProperty timestep_;
    FloatProperty springLength_;
    FloatProperty springLinearConstant_;
    FloatProperty springSquareConstant_;
    FloatProperty springDamping_;
    FloatProperty gradientScale_;
};

}  // namespace inviwo
