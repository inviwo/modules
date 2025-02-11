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
#include <inviwo/computeutils/processors/volumeshrinktonormalrangeglprocessor.h>
#include <inviwo/core/network/networklock.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VolumeShrinkToNormalRangeGLProcessor::processorInfo_{
    "org.inviwo.VolumeShrinkToNormalRangeGLProcessor",  // Class identifier
    "Volume Shrink to Normal Range",                    // Display name
    "Volume Operation",                                 // Category
    CodeState::Stable,                                  // Code state
    "range, normal, shrink, feature",                   // Tags
};
const ProcessorInfo VolumeShrinkToNormalRangeGLProcessor::getProcessorInfo() const {
    return processorInfo_;
}

VolumeShrinkToNormalRangeGLProcessor::VolumeShrinkToNormalRangeGLProcessor()
    : Processor()
    , volumeInport_("volumeInport")
    , volumeOutport_("volumeOutport")
    , channels_("channels", "Channels")
    , shrinkChannel0_("shrinkChannel0", "Channel 1", true)
    , shrinkChannel1_("shrinkChannel1", "Channel 2", false)
    , shrinkChannel2_("shrinkChannel2", "Channel 3", false)
    , shrinkChannel3_("shrinkChannel3", "Channel 4", false)
    , volumeShrinkToNormalRangeGl_([&]() { this->invalidate(InvalidationLevel::InvalidOutput); }) {

    addPorts(volumeInport_, volumeOutport_);

    shrinkChannel1_.setVisible(false);
    shrinkChannel2_.setVisible(false);
    shrinkChannel3_.setVisible(false);

    channels_.addProperties(shrinkChannel0_, shrinkChannel1_, shrinkChannel2_, shrinkChannel3_);

    addProperties(channels_);

    volumeInport_.onChange([this]() {
        if (volumeInport_.hasData()) {
            auto volume = volumeInport_.getData();

            const auto channels = static_cast<int>(volume->getDataFormat()->getComponents());
            if (channels == static_cast<int>(channels_.getProperties().size())) return;

            auto properties = channels_.getProperties();

            dynamic_cast<BoolProperty*>(properties[0])->set(true);

            for (int i = 1; i < 4; i++) {
                auto boolProp = dynamic_cast<BoolProperty*>(properties[i]);
                boolProp->set(i < channels);
                boolProp->setVisible(i < channels);
            }

            volumeShrinkToNormalRangeGl_.reset();
        }
    });

    shrinkChannel0_.onChange(
        [this]() { volumeShrinkToNormalRangeGl_.setShrinkChannel(0, shrinkChannel0_.get()); });
    shrinkChannel1_.onChange(
        [this]() { volumeShrinkToNormalRangeGl_.setShrinkChannel(1, shrinkChannel1_.get()); });
    shrinkChannel2_.onChange(
        [this]() { volumeShrinkToNormalRangeGl_.setShrinkChannel(2, shrinkChannel2_.get()); });
    shrinkChannel3_.onChange(
        [this]() { volumeShrinkToNormalRangeGl_.setShrinkChannel(3, shrinkChannel3_.get()); });
}

void VolumeShrinkToNormalRangeGLProcessor::process() {
    auto inputVolume = volumeInport_.getData();
    auto channelProperties = channels_.getProperties();

    bool apply = false;
    for (size_t i{0}; i < channelProperties.size(); ++i) {
        apply = apply || dynamic_cast<BoolProperty*>(channelProperties[i])->get();
    }
    if (inputVolume->getDataFormat()->getNumericType() != NumericType::Float) {
        LogWarn("Numeric type of input volume is not floating point.");
    }

    if (!apply) {
        volumeOutport_.setData(inputVolume);
    } else {
        volumeOutport_.setData(volumeShrinkToNormalRangeGl_.shrink(*inputVolume));
    }
}

}  // namespace inviwo
