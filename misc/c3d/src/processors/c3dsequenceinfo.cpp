/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2026 Inviwo Foundation
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

#include <inviwo/c3d/processors/c3dsequenceinfo.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo C3DSequenceInfo::processorInfo_{
    "org.inviwo.C3DSequenceInfo",              // Class identifier
    "C3D Sequence Info",                       // Display name
    "Motion Tracking",                         // Category
    CodeState::Experimental,                   // Code state
    Tags::CPU | Tag{"C3D"} | Tag{"Sequence"},  // Tags
    R"(Shows some basic information of the input C3D motion tracking data.)"_unindentHelp,
};

const ProcessorInfo& C3DSequenceInfo::getProcessorInfo() const { return processorInfo_; }

C3DSequenceInfo::C3DSequenceInfo()
    : Processor{}
    , inport_{"inport", "C3D sequence data to be cropped"_help}
    , outport_{"outport", "Forwarded C3D input"_help}
    , sequenceSize_{"sequenceSize", "Sequence Size",
                    util::ordinalCount(0)
                        .set("Number of elements in the input sequence"_help)
                        .set(ReadOnly::Yes)}
    , maxSequenceSize_{
          "maxSequenceSize", "Max Sequence Size",
          util::ordinalOptCount(std::optional<int>{std::nullopt}, 100).set(ReadOnly::Yes)} {

    addPorts(inport_, outport_);
    addProperties(sequenceSize_, maxSequenceSize_);

    isSink_.setUpdate([]() { return true; });
}

void C3DSequenceInfo::process() {
    outport_.setData(inport_.getData());

    const auto& sequence = *inport_.getData();

    sequenceSize_.set(static_cast<int>(sequence.size()));
    if (!sequence.empty() && sequence.front()->hasMetaData("sequenceIndex")) {
        maxSequenceSize_.set(
            static_cast<int>(sequence.front()->getMetaData<SizeMetaData>("sequenceSize", 0uz)));
    } else {
        maxSequenceSize_.clear();
    }
}

}  // namespace inviwo
