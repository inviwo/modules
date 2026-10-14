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

#include <inviwo/c3d/processors/c3dsequencecrop.h>

#include <inviwo/dataframe/util/dataframeutil.h>
#include <inviwo/core/util/exception.h>

#include <ranges>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo C3DSequenceCrop::processorInfo_{
    "org.inviwo.C3DSequenceCrop",              // Class identifier
    "C3D Sequence Crop",                       // Display name
    "Motion Tracking",                         // Category
    CodeState::Experimental,                   // Code state
    Tags::CPU | Tag{"C3D"} | Tag{"Sequence"},  // Tags
    R"(
    This processor crops a sequence of C3D motion tracking data to a frame range [start, stop] 
    provided by a DataFrame. The sequence ID, stored in the metadata of the C3D data, is used to 
    locate the corresponding row in the dataframe and lookup start and stop frames for the 
    individual sequence data.
    )"_unindentHelp,
};

const ProcessorInfo& C3DSequenceCrop::getProcessorInfo() const { return processorInfo_; }

C3DSequenceCrop::C3DSequenceCrop()
    : PoolProcessor{}
    , inport_{"c3dsequnence", "C3D sequence data to be cropped"_help}
    , outport_{"outport", "Resulting cropped frame subset of the C3D input"_help}
    , dataframe_{"dataframe",
                 "DataFrame providing start and stop frames along with sequence IDs"_help}
    , enabled_{"enabled", "Enabled", true}
    , sequenceIndex_{"sequenceIndex", "Sequence Index Column", dataframe_,
                     ColumnOptionProperty::AddNoneOption::No, 1}
    , firstFrame_{"firstFrame", "First Frame Column", dataframe_,
                  ColumnOptionProperty::AddNoneOption::No, 0}
    , lastFrame_{"lastFrame", "Last Frame Column", dataframe_,
                 ColumnOptionProperty::AddNoneOption::No, 0} {

    addPorts(inport_, dataframe_, outport_);
    addProperties(enabled_, sequenceIndex_, firstFrame_, lastFrame_);
}

namespace {

struct DataFrameLookUp {
    std::shared_ptr<const Column> sequenceColumn;
    std::shared_ptr<const Column> firstFrameColumn;
    std::shared_ptr<const Column> lastFrameColumn;
};

DataSequence<C3D> crop(const std::shared_ptr<const C3D>& c3d, const DataFrameLookUp& lookup,
                       size_t index) {
    index = c3d->getMetaData<SizeMetaData>("sequenceIndex", index);

    const std::vector<dataframefilters::ItemFilter> filters{
        dataframefilters::intMatch(0, filters::NumberComp::Equal, static_cast<int>(index))};
    const auto rows = dataframe::selectRows(*lookup.sequenceColumn, filters);
    if (rows.empty()) {
        log::warn("Could not find row for sequence index = {}", index);
        DataSequence<C3D> sequence{};
        sequence.push_back(c3d);
        return sequence;
    }

    auto firstFrame = static_cast<size_t>(lookup.firstFrameColumn->getAsDouble(rows.front()));
    auto lastFrame = static_cast<size_t>(lookup.lastFrameColumn->getAsDouble(rows.front()));
    if (lastFrame < firstFrame) {
        throw Exception{SourceContext{}, "Invalid frame range first={}, last={}", firstFrame,
                        lastFrame};
    }

    DataSequence<C3D> sequence{};
    sequence.push_back(std::make_shared<C3D>(*c3d, firstFrame, lastFrame));
    return sequence;
}

}  // namespace

void C3DSequenceCrop::process() {
    if (!enabled_) {
        outport_.setData(inport_.getData());
        return;
    }

    auto data = inport_.getData();
    data->front();

    const auto& dataFrame = *dataframe_.getData();

    auto getColumn = [&](int index) {
        auto col = dataFrame.getColumn(static_cast<size_t>(index));
        if (!col) {
            throw Exception{SourceContext{}, "Could not access column {} in DataFrame", index};
        }
        return col;
    };
    const DataFrameLookUp lookup{.sequenceColumn = getColumn(sequenceIndex_),
                                 .firstFrameColumn = getColumn(firstFrame_),
                                 .lastFrameColumn = getColumn(lastFrame_)};

    const auto croppers =
        std::views::zip(std::views::iota(0uz), *inport_.getData()) |
        std::views::transform(
            [dataFrame = dataframe_.getData(), &lookup](
                auto&& item) -> std::function<DataSequence<C3D>(std::stop_token, pool::Progress)> {
                return [dataFrame, lookup, c3d = std::get<1>(item), index = std::get<0>(item)](
                           std::stop_token stop, pool::Progress progress) -> DataSequence<C3D> {
                    if (stop.stop_requested()) return {};
                    progress(0.0);
                    const util::OnScopeExit done{[&]() { progress(1.0); }};

                    return crop(c3d, lookup, index);
                };
            }) |
        std::ranges::to<std::vector>();

    dispatchMany(croppers, [this](std::vector<DataSequence<C3D>> sequences) {
        auto result = std::make_shared<DataSequence<C3D>>(sequences | std::views::join);
        outport_.setData(result);
        newResults();
    });
}

}  // namespace inviwo
