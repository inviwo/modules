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

#include <inviwo/tensorvisbase/processors/tensorfield3dsubsample.h>
#include <inviwo/tensorvisbase/tensorvisbasemodule.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorField3DSubsample::processorInfo_{
    "org.inviwo.TensorField3DSubsample",  // Class identifier
    "Tensor Field 3D Subsample",          // Display name
    "Tensor Visualization",               // Category
    CodeState::Experimental,              // Code state
    tag::OpenTensorVis | Tag::CPU,        // Tags
};

const ProcessorInfo TensorField3DSubsample::getProcessorInfo() const { return processorInfo_; }

TensorField3DSubsample::TensorField3DSubsample()
    : Processor()
    , inport_("inport")
    , outport_("outport")
    , resolutionMultiplier_("resolutionMultiplier", "Resolution multiplier", 1.0f, 0.1f, 10.0f,
                            0.1f, InvalidationLevel::Valid)
    , interpolationMethod_(
          "interpolationMethod", "Interpolation method",
          {{"linear", "Linear", tensorutil::InterpolationMethod::Linear},
           {"nearest", "Nearest neighbour", tensorutil::InterpolationMethod::Nearest}},
          0, InvalidationLevel::Valid)
    , tf_(nullptr) {
    addPort(inport_);
    addPort(outport_);

    addProperty(resolutionMultiplier_);

    addProperty(interpolationMethod_);

    inport_.onChange([this]() { subsample(); });
    resolutionMultiplier_.onChange([this]() { subsample(); });
    interpolationMethod_.onChange([this]() { subsample(); });
}

void TensorField3DSubsample::initializeResources() {}

void TensorField3DSubsample::process() {
    if (tf_) outport_.setData(tf_);
}

void TensorField3DSubsample::subsample() {
    if (inport_.hasData()) {
        if (!isRunning_) {
            dispatchPool([this, &bar = progressBar_]() {
                auto on_progress = [&bar](float progress) { bar.updateProgress(progress); };
                isRunning_ = true;

                const auto resolutionMultiplier = resolutionMultiplier_.get();
                const auto interpolationMethod = interpolationMethod_.get();

                do {
                    bar.show();
                    bar.updateProgress(0.f);

                    tf_ = tensorutil::subsample3D(
                        inport_.getData(),
                        size3_t(glm::round(vec3(inport_.getData()->getDimensions()) *
                                           resolutionMultiplier)),
                        interpolationMethod, on_progress);

                    on_progress(1.f);

                    bar.hide();
                } while (resolutionMultiplier != resolutionMultiplier_.get() ||
                         interpolationMethod != interpolationMethod_.get());

                dispatchFront([this]() { invalidate(InvalidationLevel::InvalidOutput); });

                isRunning_ = false;
            });
        }
    }
}

}  // namespace inviwo
