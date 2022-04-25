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
#include <inviwo/core/ports/volumeport.h>
#include <inviwo/computeutils/algorithm/volumeminmaxgl.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/minmaxproperty.h>

namespace inviwo {

/** \docpage{org.inviwo.VolumeMinMaxGLProcessor, Volume Min Max GLProcessor}
 * ![](org.inviwo.VolumeMinMaxGLProcessor.png?classIdentifier=org.inviwo.VolumeMinMaxGLProcessor)
 *
 * This method calculates min and max values of a volume. If you need min/max per channel you should
 * look at VolumeReductionGL. The disregarding status indicates whether or not the calculation of
 * the min/max values should disregard a certain value range. This is for example handy for volumes
 * where special regions are marked with voxel values of INT_MAX or the like. Example: Your data
 * array consists of {0, 1, 2, 3, INT_MAX} and you would like to compute the max value. In addition,
 * you know that outliers are marked with a value of INT_MAX so you would like those values to not
 * be considered.
 *
 * ### Inputs
 *   * __Volume inport__ Input volume.
 *
 * ### Outports
 *   * __Volume outport__ Reduced volume.
 *
 * ### Properties
 *   * __Disregarding status__ Enable/disable value discarding
 *   * __Disregarding range__ Value range to be discarded when computing minmax values.
 */
class IVW_MODULE_COMPUTEUTILS_API VolumeMinMaxGLProcessor : public Processor {
public:
    VolumeMinMaxGLProcessor();
    virtual ~VolumeMinMaxGLProcessor() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    VolumeInport volumeInport_;
    VolumeOutport volumeOutport_;

    VolumeMinMaxGL volumeMinMaxGl_;

    TemplateOptionProperty<DisregardingStatus> disregardingStatus_;
    FloatMinMaxProperty disregardingRange_;
};

}  // namespace inviwo
