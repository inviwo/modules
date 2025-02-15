#ifdef _MSC_VER
#pragma optimize("", off)
#elif ((__GNUC__ > 3) && (__GNUC_MINOR__ > 3))
#pragma GCC push_options
#pragma GCC optimize("O0")
#endif

/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2025 Inviwo Foundation
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

#include <inviwo/tensorvisbase/processors/invariantspacecombine.h>
#include <inviwo/tensorvisbase/util/misc.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo InvariantSpaceCombine::processorInfo_{
    "org.inviwo.InvariantSpaceCombine",  // Class identifier
    "Invariant Space Combine",           // Display name
    "Tensor",                            // Category
    CodeState::Experimental,             // Code state
    Tags::CPU,                           // Tags
};
const ProcessorInfo& InvariantSpaceCombine::getProcessorInfo() const { return processorInfo_; }

InvariantSpaceCombine::InvariantSpaceCombine()
    : Processor()
    , invariantSpaceInport1_("invariantSpaceInport1")
    , invariantSpaceInport2_("invariantSpaceInport2")
    , outport_("outport") {
    addPort(invariantSpaceInport1_);
    addPort(invariantSpaceInport2_);
    addPort(outport_);
}

void InvariantSpaceCombine::process() {
    auto iv1 = invariantSpaceInport1_.getData();
    auto iv2 = invariantSpaceInport2_.getData();

    auto ivOut = std::make_shared<InvariantSpace>();

    ivOut->addAxes(iv1);
    ivOut->addAxes(iv2);

    outport_.setData(ivOut);
}

}  // namespace inviwo

#ifdef _MSC_VER
#pragma optimize("", on)
#elif ((__GNUC__ > 3) && (__GNUC_MINOR__ > 3))
#pragma GCC pop_options
#endif
