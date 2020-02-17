/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2017-2020 Inviwo Foundation
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

#include <inviwo/tensorvisbase/processors/tensorfield2dsubset.h>
#include <inviwo/tensorvisbase/tensorvisbasemodule.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorField2DSubset::processorInfo_{
    "org.inviwo.TensorField2DSubset",  // Class identifier
    "Tensor Field 2D Subset",          // Display name
    "Tensor visualization",            // Category
    CodeState::Stable,                 // Code state
    tag::OpenTensorVis | Tag::CPU,     // Tags
};
const ProcessorInfo TensorField2DSubset::getProcessorInfo() const { return processorInfo_; }

TensorField2DSubset::TensorField2DSubset()
    : Processor()
    , inport_("inport")
    , outport_("outport")
    , origin_("origin", "Origin", size2_t{0})
    , offset_("offset", "Offset", size2_t{0}, size2_t{0}) {
    addPort(inport_);
    addPort(outport_);

    offset_.setCurrentStateAsDefault();

    addProperty(origin_);
    addProperty(offset_);

    inport_.onChange([&]() {
        auto dimensions = inport_.getData()->getDimensions();
        origin_.setMaxValue(dimensions - size2_t{1});
        origin_.setMinValue(size2_t{0});

        auto origin = origin_.get();
        auto offset = offset_.get();
        auto newOffset = offset_.get();

        offset_.setMaxValue(dimensions - origin - size2_t{1});

        auto maxOffset = offset_.getMaxValue();

        if (offset.x > maxOffset.x) newOffset.x = maxOffset.x;
        if (offset.y > maxOffset.y) newOffset.y = maxOffset.y;

        offset_.set(newOffset);
    });

    origin_.onChange([this]() {
        if (!inport_.hasData()) return;
        auto origin = origin_.get();

        offset_.setMaxValue(inport_.getData()->getDimensions() - origin - size2_t{1});
    });
}

void TensorField2DSubset::process() {
    auto tensorField = inport_.getData();

    auto dimensions = size2_t(offset_.get() + size2_t{1});

    std::vector<TensorField2D::matN> rawData;

    for (size_t y = origin_.get().y; y <= origin_.get().y + offset_.get().y; ++y) {
        for (size_t x = origin_.get().x; x <= origin_.get().x + offset_.get().x; ++x) {
            rawData.push_back(tensorField->at(size2_t(x, y)));
        }
    }

    outport_.setData(std::make_shared<TensorField2D>(dimensions, rawData));
}

}  // namespace inviwo
