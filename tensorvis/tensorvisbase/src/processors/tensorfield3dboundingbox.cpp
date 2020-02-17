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

#include <inviwo/tensorvisbase/processors/tensorfield3dboundingbox.h>
#include <inviwo/core/datastructures/geometry/basicmesh.h>
#include <inviwo/tensorvisbase/util/tensorfieldutil.h>
#include <inviwo/tensorvisbase/tensorvisbasemodule.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorField3DBoundingBox::processorInfo_{
    "org.inviwo.TensorField3DBoundingBox",  // Class identifier
    "Tensor Field Bounding Box",            // Display name
    "Mesh Creation",                        // Category
    CodeState::Stable,                      // Code state
    tag::OpenTensorVis | Tag::CPU,          // Tags
};
const ProcessorInfo TensorField3DBoundingBox::getProcessorInfo() const { return processorInfo_; }

TensorField3DBoundingBox::TensorField3DBoundingBox()
    : Processor()
    , volume_("volume")
    , linemesh_("linemesh")
    , boxmesh_("boxmesh")
    , color_("color", "Color", vec4(1.0f), vec4(0.0f), vec4(1.0f)) {
    addPort(volume_);
    addPort(linemesh_);
    addPort(boxmesh_);

    color_.setSemantics(PropertySemantics::Color);
    addProperty(color_);
}

void TensorField3DBoundingBox::process() {
    linemesh_.setData(
        tensorutil::generateBoundingBoxAdjacencyForTensorField(volume_.getData(), color_.get()));
    boxmesh_.setData(
        tensorutil::generateBoundingBoxForTensorField(volume_.getData(), color_.get()));
}

}  // namespace inviwo
