/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019 Inviwo Foundation
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

#include <modules/tensorvisbase/processors/invariantspacesubampling.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo InvariantSpaceSubampling::processorInfo_{
    "org.inviwo.InvariantSpaceSubampling",  // Class identifier
    "Invariant Space Subampling",           // Display name
    "Undefined",                            // Category
    CodeState::Experimental,                // Code state
    Tags::None,                             // Tags
};
const ProcessorInfo InvariantSpaceSubampling::getProcessorInfo() const { return processorInfo_; }

InvariantSpaceSubampling::InvariantSpaceSubampling()
    : Processor()
    , invariantSpaceInport_("invariantSpaceInport")
    , invariantSpaceOutport_("invariantSpaceOutport")
    , percentage_("percentage", "Percentage", 1, 1, 10, 1) {

    addPort(invariantSpaceInport_);
    addPort(invariantSpaceOutport_);

    addProperty(percentage_);
}

void InvariantSpaceSubampling::process() {
    auto invariantSpace = invariantSpaceInport_.getData();

    const auto percentage = percentage_.get();

    auto iv = std::make_shared<InvariantSpace>(invariantSpace->getNumberOfDimensions(),
                                               invariantSpace->getIdentifiers(),
                                               invariantSpace->getMetaDataTypes());

    const auto numberOfElements = invariantSpace->getNumElements();

    for (size_t i{0}; i < numberOfElements; i += percentage) {
        iv->addPoint(invariantSpace->getPoint(i));
    }

    invariantSpaceOutport_.setData(iv);
}

}  // namespace inviwo
