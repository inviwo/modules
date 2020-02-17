/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2020 Inviwo Foundation
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

#include <inviwo/tensorvisbase/processors/tensorfield2dsubsample.h>
#include <inviwo/tensorvisbase/tensorvisbasemodule.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorField2DSubsample::processorInfo_{
    "org.inviwo.TensorField2DSubsample",  // Class identifier
    "Tensor Field 2D Subsample",          // Display name
    "Tensor visualization",               // Category
    CodeState::Experimental,              // Code state
    tag::OpenTensorVis | Tag::CPU,        // Tags
};

const ProcessorInfo TensorField2DSubsample::getProcessorInfo() const { return processorInfo_; }

TensorField2DSubsample::TensorField2DSubsample()
    : Processor()
    , inport_("inport")
    , outport_("outport")
    , resolutionMultiplier_("resolutionMultiplier", "Resolution multiplier", 1, 1, 10, 1)
    , interpolationMethod_(
          "interpolationMethod", "Interpolation method",
          {{"linear", "Linear", tensorutil::InterpolationMethod::Linear},
           {"nearest", "Nearest neighbour", tensorutil::InterpolationMethod::Nearest},
           {"barycentric", "Barycentric", tensorutil::InterpolationMethod::Barycentric}},
          0) {
    addPort(inport_);
    addPort(outport_);

    addProperty(resolutionMultiplier_);

    addProperty(interpolationMethod_);
}

void TensorField2DSubsample::initializeResources() {}

void TensorField2DSubsample::process() {
    auto subsampled = tensorutil::subsample2D(
        inport_.getData(), inport_.getData()->getDimensions() * resolutionMultiplier_.get(),
        interpolationMethod_.get());

    outport_.setData(subsampled);
}

}  // namespace inviwo
