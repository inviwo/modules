/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2021-2025 Inviwo Foundation
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

#include <inviwo/computeutils/processors/volumeminmaxglprocessor.h>
#include <inviwo/core/network/networklock.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VolumeMinMaxGLProcessor::processorInfo_{
    "org.inviwo.VolumeMinMaxGLProcessor",  // Class identifier
    "Volume Min Max",                      // Display name
    "Volume Operation",                    // Category
    CodeState::Experimental,               // Code state
    "range, minmax",                       // Tags
};
const ProcessorInfo VolumeMinMaxGLProcessor::getProcessorInfo() const { return processorInfo_; }

VolumeMinMaxGLProcessor::VolumeMinMaxGLProcessor()
    : Processor()
    , volumeInport_("volumeInport")
    , volumeOutport_("volumeOutport")
    , disregardingStatus_(
          "disregardingStatus", "Disregarding status",
          {{"off", "Off", DisregardingStatus::Off}, {"on", "On", DisregardingStatus::On}})
    , disregardingRange_("range", "Range") {

    addPorts(volumeInport_, volumeOutport_);

    disregardingRange_.visibilityDependsOn(disregardingStatus_, [](Property& prop) {
        const auto status = dynamic_cast<TemplateOptionProperty<DisregardingStatus>&>(prop).get();

        return status == DisregardingStatus::On;
    });

    addProperties(disregardingStatus_, disregardingRange_);

    volumeInport_.onChange([this]() {
        /*
         * Here we call the volume reduction without any disregarding so we get the real min and max
         * values.
         */
        const auto range = vec2{volumeMinMaxGl_.minmax(volumeInport_.getData())};

        NetworkLock l;

        disregardingRange_.setRangeMin(range.x);
        disregardingRange_.setRangeMax(range.y);
        disregardingRange_.set(range);
    });
}

void VolumeMinMaxGLProcessor::process() {
    auto outVolume = std::shared_ptr<Volume>(volumeInport_.getData()->clone());
    outVolume->dataMap_.dataRange = outVolume->dataMap_.valueRange = volumeMinMaxGl_.minmax(
        volumeInport_.getData(), disregardingStatus_.get(), disregardingRange_.get());

    volumeOutport_.setData(outVolume);
}

}  // namespace inviwo
