/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2021-2026 Inviwo Foundation
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
#include <inviwo/computeutils/processors/volumeshrinktonormalrangeglprocessor.h>
#include <inviwo/core/network/networklock.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VolumeShrinkToNormalRangeGLProcessor::processorInfo_{
    "org.inviwo.VolumeShrinkToNormalRangeGLProcessor",  // Class identifier
    "Volume Shrink to Normal Range",                    // Display name
    "Volume Operation",                                 // Category
    CodeState::Stable,                                  // Code state
    Tags::GL | Tag{"Normalization"},                    // Tags
    R"(Shrinks the selected channels of the input volume to range 
    [0 + offset, 1 + offset] where offset is the percentual deviaton of the minimum 
    from 0. For example, range [-0.5, 1.0] will be shrunk to [-0.33, 0.66].)"_unindentHelp,
};
const ProcessorInfo& VolumeShrinkToNormalRangeGLProcessor::getProcessorInfo() const {
    return processorInfo_;
}

VolumeShrinkToNormalRangeGLProcessor::VolumeShrinkToNormalRangeGLProcessor()
    : Processor()
    , volumeInport_{"volumeInport", "Input Volume"_help}
    , volumeOutport_{"volumeOutport", "Shrunk volume"_help}
    , channels_{"channels", "Channels",
                "Select those channels you wish to shrink to range [0 + offset, 1 + offset]"_help}
    , shrinkChannel_{{{"shrinkChannel0", "Channel 1", true},
                      {"shrinkChannel1", "Channel 2", false},
                      {"shrinkChannel2", "Channel 3", false},
                      {"shrinkChannel3", "Channel 4", false}}}
    , volumeShrinkToNormalRangeGl_{[this]() { invalidate(InvalidationLevel::InvalidOutput); }} {

    addPorts(volumeInport_, volumeOutport_);

    for (auto& p : shrinkChannel_) {
        channels_.addProperty(p);
    }
    addProperties(channels_);
}

void VolumeShrinkToNormalRangeGLProcessor::process() {
    if (volumeInport_.isChanged() && volumeInport_.hasData()) {
        auto volume = volumeInport_.getData();

        const auto channels = static_cast<int>(volume->getDataFormat()->getComponents());
        if (channels == static_cast<int>(channels_.getProperties().size())) return;

        for (auto&& [index, p] : shrinkChannel_ | std::views::enumerate) {
            p.set(index < channels);
        }
        volumeShrinkToNormalRangeGl_.reset();
    }
    for (auto&& [index, p] :
         shrinkChannel_ | std::views::enumerate |
             std::views::filter([](auto v) { return std::get<1>(v).isModified(); })) {
        volumeShrinkToNormalRangeGl_.setShrinkChannel(index, p);
    }


    auto inputVolume = volumeInport_.getData();
    auto channelProperties = channels_.getProperties();

    bool apply = false;
    for (size_t i{0}; i < channelProperties.size(); ++i) {
        apply = apply || dynamic_cast<BoolProperty*>(channelProperties[i])->get();
    }
    if (inputVolume->getDataFormat()->getNumericType() != NumericType::Float) {
        log::warn("Numeric type of input volume is not floating point.");
    }

    if (!apply) {
        volumeOutport_.setData(inputVolume);
    } else {
        volumeOutport_.setData(volumeShrinkToNormalRangeGl_.shrink(*inputVolume));
    }
}

}  // namespace inviwo
