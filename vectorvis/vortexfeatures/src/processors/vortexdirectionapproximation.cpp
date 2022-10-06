/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2022 Inviwo Foundation
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

#include <inviwo/vortexfeatures/processors/vortexdirectionapproximation.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VortexDirectionApproximation::processorInfo_{
    "org.inviwo.VortexDirectionApproximation",  // Class identifier
    "Vortex Direction Approximation",           // Display name
    "Undefined",                                // Category
    CodeState::Experimental,                    // Code state
    Tags::None,                                 // Tags
};
const ProcessorInfo VortexDirectionApproximation::getProcessorInfo() const {
    return processorInfo_;
}

VortexDirectionApproximation::VortexDirectionApproximation()
    : Processor()
    , vorticesIn_("vorticesIn")
    , vorticesOut_("vorticesOut")
    , dirsOut_("dirsOut")
    , offset_("offset", "Offset", 0.1, {0, ConstraintBehavior::Immutable},
              {10, ConstraintBehavior::Ignore}) {

    addPorts(vorticesIn_, vorticesOut_, dirsOut_);
    addProperty(offset_);
}

void VortexDirectionApproximation::process() {
    if (!vorticesIn_.hasData()) {
        vorticesOut_.clear();
        return;
    }

    auto eddies = vorticesIn_.getData();
    auto vortsOut = std::make_shared<VortexSet>(eddies->getModelMatrix(), eddies->getWorldMatrix());

    // std::vector<Vortex> vorts;
    for (size_t g = 0; g < eddies->numGroups(); ++g) {
        Vortex eddy = *eddies->beginGroup(g);
        eddy.score = 0;

        Vortex halo;
        halo.rotation = Vortex::Turning(3 - int(eddy.rotation));
        halo.score = 1;
        halo.center = eddy.center;
        halo.timeSlice = eddy.timeSlice;
        halo.heightSlice = eddy.heightSlice;

        // Fill halo points.
        for (size_t v = 0; v < eddy.size(); ++v) {
            // dvec2 grad = eddy.boundary[(v + 1) % eddy.size()] -
            //              eddy.boundary[(v + eddy.size() - 1) % eddy.size()];
            // dvec2 outVec(grad.y, grad.x);
            dvec2 outVec = eddy.boundary[v] - eddy.center;
            // if (eddy.rotation == Vortex::Turning::Clockwise) {
            //     // outVec = -outVec;
            // }
            outVec = glm::normalize(outVec) * double(offset_.get());
            halo.boundary.push_back(eddy.boundary[v] + outVec);
        }
        vortsOut->push_back(eddy);
        vortsOut->startNewGroup();
        vortsOut->push_back(halo);
        // vortsOut->push_back(eddy);
        // vortsOut->push_back(halo);
        // vortsOut->push_back(eddy);
        // vortsOut->push_back(halo);
        vortsOut->startNewGroup();
    }
    // vortsOut->append(vorts);
    vorticesOut_.setData(vortsOut);
}

}  // namespace inviwo
