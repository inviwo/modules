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

#include <inviwo/c3d/processors/c3dcrop.h>

#include <inviwo/core/util/logcentral.h>
#include <inviwo/core/util/exception.h>
#include <inviwo/dataframe/util/dataframeutil.h>

#pragma optimize("", off)

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo C3DCrop::processorInfo_{
    "org.inviwo.C3DCrop",     // Class identifier
    "C3DCrop",                // Display name
    "Motion Tracking",        // Category
    CodeState::Experimental,  // Code state
    Tags::CPU | Tag{"C3D"},   // Tags
    R"(
    This processor crops the C3D motion tracking input to a frame range [start, stop] provided by 
    a DataFrame. The sequence ID, stored in the metadata of the C3D data, is used to locate the 
    corresponding row in the dataframe and lookup the start and stop frames.
    )"_unindentHelp,
};

const ProcessorInfo& C3DCrop::getProcessorInfo() const { return processorInfo_; }

C3DCrop::C3DCrop()
    : Processor{}
    , inport_{"inport", "C3D input data which will be cropped"_help}
    , dataframe_{"dataframe", ""_help}
    , outport_{"outport", "Resulting cropped frame subset of the C3D input"_help}
    , enabled_{"enabled", "Enabled", true}
    , sequenceIndex_{"sequenceIndex", "Sequence Index Column", dataframe_,
                     ColumnOptionProperty::AddNoneOption::No, 1}
    , firstFrame_{"firstFrame", "First Frame Column", dataframe_,
                  ColumnOptionProperty::AddNoneOption::No, 0}
    , lastFrame_{"lastFrame", "Last Frame Column", dataframe_,
                 ColumnOptionProperty::AddNoneOption::No, 0}
    , frame_{"frame",
             "Output Frame Range",
             "Range of frame indices of the resulting C3D data"_help,
             0,
             0,
             0,
             0} {

    addPorts(inport_, dataframe_, outport_);
    addProperties(enabled_, sequenceIndex_, firstFrame_, lastFrame_, frame_);
    frame_.setReadOnly(true);
}

void C3DCrop::process() {
    const auto& c3d = *inport_.getData();
    const auto& header = c3d.header();

    if (!enabled_ || header.nbFrames() == 0 || header.nb3dPoints() == 0) {
        outport_.setData(inport_.getData());
        return;
    }

    if (!c3d.hasMetaData<SizeMetaData>("sequenceIndex")) {
        throw Exception{"C3D data has no MetaData 'sequenceIndex'"};
    }
    const size_t index = c3d.getMetaData<SizeMetaData>("sequenceIndex", 0uz);

    const auto& dataFrame = *dataframe_.getData();

    const std::vector<dataframefilters::ItemFilter> filters{
        dataframefilters::intMatch(0, filters::NumberComp::Equal, static_cast<int>(index))};
    const auto rows = dataframe::selectRows(*dataFrame.getColumn(sequenceIndex_), filters);

    if (rows.empty()) {
        log::warn("Could not find row for sequence index = {}", index);
        outport_.setData(inport_.getData());
        return;
    }

    auto firstFrame =
        static_cast<size_t>(dataFrame.getColumn(firstFrame_)->getAsDouble(rows.front()));
    auto lastFrame =
        static_cast<size_t>(dataFrame.getColumn(lastFrame_)->getAsDouble(rows.front()));
    if (lastFrame < firstFrame) {
        throw Exception{SourceContext{}, "Invalid frame range first={}, last={}", firstFrame,
                        lastFrame};
    }
    frame_.set(firstFrame, lastFrame, firstFrame, lastFrame, 1, 0);

    outport_.setData(std::make_shared<C3D>(c3d, firstFrame, lastFrame));
}

}  // namespace inviwo
