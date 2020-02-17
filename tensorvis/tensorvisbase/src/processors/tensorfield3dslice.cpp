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

#include <inviwo/tensorvisbase/processors/tensorfield3dslice.h>
#include <inviwo/tensorvisbase/algorithm/tensorfieldslicing.h>
#include <inviwo/core/datastructures/geometry/geometrytype.h>
#include <inviwo/tensorvisbase/util/tensorfieldutil.h>
#include <inviwo/tensorvisbase/tensorvisbasemodule.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorField3DSlice::processorInfo_{
    "org.inviwo.TensorField3DSlice",  // Class identifier
    "Tensor Field 3D Slice",          // Display name
    "Tensor visualization",           // Category
    CodeState::Experimental,          // Code state
    tag::OpenTensorVis | Tag::CPU,    // Tags
};

const ProcessorInfo TensorField3DSlice::getProcessorInfo() const { return processorInfo_; }

TensorField3DSlice::TensorField3DSlice()
    : Processor()
    , inport_("inport")
    , outport2D_("outport2D")
    , outport3D_("outport3D")
    , sliceOutport_("meshOutport")
    , planeOutport_("planeOutport")
    , offsetOutport_("offsetOutport")
    , sliceAlongAxis_("sliceAxis", "Slice along axis",
                      {{"x", "X axis", CartesianCoordinateAxis::X},
                       {"y", "Y axis", CartesianCoordinateAxis::Y},
                       {"z", "Z axis", CartesianCoordinateAxis::Z}},
                      0)
    , sliceNr_("sliceNr", "Slice number", 0, 0, 2, 1)
    , sliceColor_("sliceColor", "Slice color", vec4(1.0f), vec4(0.0f), vec4(1.0f))
    , planeColor_("planeColor", "Plane color", vec4(1.0f), vec4(0.0f), vec4(1.0f)) {
    addPort(inport_);
    addPort(outport2D_);
    addPort(outport3D_);
    addPort(sliceOutport_);
    addPort(planeOutport_);
    addPort(offsetOutport_);

    offsetOutport_.setData(std::make_shared<size_t>(0));

    addProperty(sliceAlongAxis_);
    addProperty(sliceNr_);

    sliceColor_.setSemantics(PropertySemantics::Color);
    addProperty(sliceColor_);

    planeColor_.setSemantics(PropertySemantics::Color);
    addProperty(planeColor_);

    inport_.onChange([&]() {
        if (!inport_.hasData() || !inport_.getData().get()) return;

        size_t numSlices{0};
        auto dimensions3D = inport_.getData()->getDimensions();

        switch (sliceAlongAxis_.get()) {
            case CartesianCoordinateAxis::X:
                numSlices = dimensions3D.x;
                break;
            case CartesianCoordinateAxis::Y:
                numSlices = dimensions3D.y;
                break;
            case CartesianCoordinateAxis::Z:
                numSlices = dimensions3D.z;
                break;
        }
        sliceNr_.setMinValue(0);
        sliceNr_.setMaxValue(numSlices - 1);
    });

    sliceAlongAxis_.onChange([&]() {
        if (!inport_.hasData() || !inport_.getData().get()) return;

        size_t numSlices{0};
        auto dimensions3D = inport_.getData()->getDimensions();

        switch (sliceAlongAxis_.get()) {
            case CartesianCoordinateAxis::X:
                numSlices = dimensions3D.x;
                break;
            case CartesianCoordinateAxis::Y:
                numSlices = dimensions3D.y;
                break;
            case CartesianCoordinateAxis::Z:
                numSlices = dimensions3D.z;
                break;
        }
        sliceNr_.setMinValue(0);
        sliceNr_.setMaxValue(numSlices - 1);
    });
}

void TensorField3DSlice::process() {
    uvec3 axis{};
    auto tensorField = inport_.getData();
    auto dimensions = tensorField->getDimensions();

    size2_t offsetDimensions{0};

    switch (sliceAlongAxis_.get()) {
        case CartesianCoordinateAxis::X:
            axis = uvec3(1, 0, 0);
            offsetDimensions.x = dimensions.y;
            offsetDimensions.y = dimensions.z;
            if (sliceNr_.get() >= dimensions.x) {
                LogError("Slice number out of bounds");
                return;
            }
            break;
        case CartesianCoordinateAxis::Y:
            axis = uvec3(0, 1, 0);
            offsetDimensions.x = dimensions.x;
            offsetDimensions.y = dimensions.z;
            if (sliceNr_.get() >= dimensions.y) {
                LogError("Slice number out of bounds");
                return;
            }
            break;
        case CartesianCoordinateAxis::Z:
            axis = uvec3(0, 0, 1);
            offsetDimensions.x = dimensions.x;
            offsetDimensions.y = dimensions.y;
            if (sliceNr_.get() >= dimensions.z) {
                LogError("Slice number out of bounds");
                return;
            }
            break;
    }

    auto offset = offsetDimensions.x * offsetDimensions.y * (sliceNr_.get());
    offsetOutport_.setData(std::make_shared<size_t>(offset));

    auto slice2D = slice<2>(inport_.getData(), sliceAlongAxis_.get(), sliceNr_.get());

    outport2D_.setData(slice2D);
    outport3D_.setData(slice<3>(inport_.getData(), sliceAlongAxis_.get(), sliceNr_.get()));
    sliceOutport_.setData(tensorutil::generateSliceLevelGeometryForTensorField(
        inport_.getData(), sliceColor_.get(), sliceAlongAxis_.get(), sliceNr_.get()));
    planeOutport_.setData(tensorutil::generateSlicePlaneGeometryForTensorField(
        tensorField, planeColor_.get(), sliceAlongAxis_.get(), sliceNr_.get()));
}

}  // namespace inviwo
