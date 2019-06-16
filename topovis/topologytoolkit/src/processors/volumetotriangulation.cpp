/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018-2019 Inviwo Foundation
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

#include <inviwo/topologytoolkit/processors/volumetotriangulation.h>
#include <inviwo/topologytoolkit/utils/ttkutils.h>

#include <inviwo/core/datastructures/buffer/bufferram.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VolumeToTriangulation::processorInfo_{
    "org.inviwo.ttk.VolumeToTriangulation",  // Class identifier
    "Volume To Triangulation",               // Display name
    "Topology",                              // Category
    CodeState::Experimental,                 // Code state
    "CPU, Topology, TTK, Triangulation",     // Tags
};
const ProcessorInfo VolumeToTriangulation::getProcessorInfo() const { return processorInfo_; }

VolumeToTriangulation::VolumeToTriangulation()
    : Processor(), volumeInport_("volume"), outport_("outport"), channel_("channel", "Channel") {

    addPort(volumeInport_);
    addPort(outport_);

    channel_.addOption("Channel 1", "Channel 1", 0);
    channel_.setSerializationMode(PropertySerializationMode::All);
    channel_.setCurrentStateAsDefault();

    addProperty(channel_);

    volumeInport_.onChange([this]() {
        if (volumeInport_.hasData()) {
            size_t channels = volumeInport_.getData()->getDataFormat()->getComponents();

            if (channels == channel_.size()) return;

            std::vector<OptionPropertyIntOption> channelOptions;
            for (size_t i = 0; i < channels; i++) {
                channelOptions.emplace_back("Channel " + toString(i + 1),
                                            "Channel " + toString(i + 1), static_cast<int>(i));
            }
            channel_.replaceOptions(channelOptions);
            channel_.setCurrentStateAsDefault();
        }
    });
}

void VolumeToTriangulation::process() {
    auto data = std::make_shared<topology::TriangulationData>(topology::volumeToTTKTriangulation(
        *volumeInport_.getData().get(), static_cast<size_t>(channel_.get())));
    outport_.setData(data);
}

}  // namespace inviwo
