/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2024 Inviwo Foundation
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

#include <inviwo/tensorvisbase/processors/invariantspacefilter.h>
#include <glm/gtc/epsilon.hpp>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo InvariantSpaceFilter::processorInfo_{
    "org.inviwo.InvariantSpaceFilter",  // Class identifier
    "Invariant Space Filter",           // Display name
    "Tensor",                           // Category
    CodeState::Experimental,            // Code state
    Tags::CPU,                          // Tags
};
const ProcessorInfo& InvariantSpaceFilter::getProcessorInfo() const { return processorInfo_; }

InvariantSpaceFilter::InvariantSpaceFilter()
    : Processor()
    , invariantSpaceInport_("invariantSpaceInport")
    , tensorField3DInport_("tensorField3DInport")
    , invariantSpaceOutport_("invariantSpaceOutport") {

    addPort(tensorField3DInport_);
    addPort(invariantSpaceInport_);
    addPort(invariantSpaceOutport_);
}

void InvariantSpaceFilter::process() {
    auto tensorField = tensorField3DInport_.getData();
    auto invariantSpace = invariantSpaceInport_.getData();

    const auto& tensors = tensorField->tensors();
    auto filteredInvariantSpace = std::make_shared<InvariantSpace>(
        invariantSpace->getNumberOfDimensions(), invariantSpace->getIdentifiers(),
        invariantSpace->getMetaDataTypes());

    size_t numberOfFilteredTensors{0};
    const auto numberOfElements = invariantSpace->getNumElements();
    const auto epsilon{std::numeric_limits<double>::epsilon()};

    auto lessThanEpsilon = [&](const double* tensor) -> bool {
        if (glm::abs(tensor[0]) < epsilon && glm::abs(tensor[1]) < epsilon &&
            glm::abs(tensor[2]) < epsilon && glm::abs(tensor[3]) < epsilon &&
            glm::abs(tensor[4]) < epsilon && glm::abs(tensor[5]) < epsilon &&
            glm::abs(tensor[6]) < epsilon && glm::abs(tensor[7]) < epsilon &&
            glm::abs(tensor[8]) < epsilon) {
            return true;
        }

        return false;
    };

    for (size_t i = 0; i < numberOfElements; i++) {
        if (!lessThanEpsilon(glm::value_ptr(tensors[i]))) {
            //        if (glm::all(glm::lessThan(glm::abs(tensors[i]), epsilon))){
            filteredInvariantSpace->addPoint(invariantSpace->getPoint(i));
            numberOfFilteredTensors++;
        }
    }

    log::info("{}% filtered ({} out of {})",
              (float(numberOfFilteredTensors) / float(numberOfElements)) * 100.f,
              numberOfFilteredTensors, numberOfElements);

    invariantSpaceOutport_.setData(filteredInvariantSpace);
}

}  // namespace inviwo
