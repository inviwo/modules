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

#include <inviwo/computeutils/computeutilsmoduledefine.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/compositeproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/ports/volumeport.h>
#include <inviwo/computeutils/algorithm/volumeshrinktonormalrangegl.h>

namespace inviwo {

/** \docpage{org.inviwo.VolumeShrinkToNormalRangeGLProcessor, Volume Shrink To Normal Range
 * Processor}
 * ![](org.inviwo.VolumeShrinkToNormalRangeGLProcessor.png?classIdentifier=org.inviwo.VolumeShrinkToNormalRangeGLProcessor)
 *
 * Shrinks the selected channels of the input volume to range [0 + offset, 1 + offset] where offset
 * is the percentual deviaton of the minimum from 0. For example, range [-0.5, 1.0] will be shrunk
 * to [-0.33, 0.66].
 *
 * ### Inputs
 *   * __Volume inport__ Input Volume
 *
 * ### Outports
 *   * __Volume outport__ Shrunk volume (if so selected)
 *
 * ### Properties
 *   * __Channels__ Check the boxes for those channels you wish to shrink to range [0 + offset, 1 +
 * offset]
 */

/**
 * \class VolumeShrinkToNormalRangeGLProcessor
 *
 * Enables the usage of the %VolumeNormalization algorithm. For details about the algorithm,
 * please see VolumeNormalization.
 * Note that this algorithm normalizes channels independently, it does not normalize a multi-channel
 * volume in terms of vector norms!
 */
class IVW_MODULE_COMPUTEUTILS_API VolumeShrinkToNormalRangeGLProcessor : public Processor {
public:
    VolumeShrinkToNormalRangeGLProcessor();
    virtual ~VolumeShrinkToNormalRangeGLProcessor() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    VolumeInport volumeInport_;
    VolumeOutport volumeOutport_;

    CompositeProperty channels_;
    BoolProperty shrinkChannel0_;
    BoolProperty shrinkChannel1_;
    BoolProperty shrinkChannel2_;
    BoolProperty shrinkChannel3_;

    VolumeShrinkToNormalRangeGL volumeShrinkToNormalRangeGl_;
};

}  // namespace inviwo
