/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2018 Inviwo Foundation
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

#include <modules/tensorvisbase/processors/tensorfieldslice.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorFieldSlice::processorInfo_{
    "org.inviwo.TensorFieldSlice",  // Class identifier
    "Tensor Field Slice",           // Display name
    "Tensor visualization",         // Category
    CodeState::Experimental,        // Code state
    Tags::CPU,                      // Tags
};

const ProcessorInfo TensorFieldSlice::getProcessorInfo() const { return processorInfo_; }

TensorFieldSlice::TensorFieldSlice()
    : Processor()
    , inport_("inport")
    , outport2D_("outport2D")
    , outport3D_("outport3D")
    , meshOutport_("meshOutport")
    , planeOutport_("planeOutport")
    , offsetOutport_("offsetOutport")
    , sliceAlongAxis_("sliceAxis", "Slice along axis",
                      {{"x", "X axis", CartesianCoordinateAxis::X},
                       {"y", "Y axis", CartesianCoordinateAxis::Y},
                       {"z", "Z axis", CartesianCoordinateAxis::Z}},
                      0)
    , sliceNr_("sliceNr", "Slice number", 0, 0, 2, 1)
    , color_("color", "Color", vec4(1.0f), vec4(0.0f), vec4(1.0f)) {
    addPort(inport_);
    addPort(outport2D_);
    addPort(outport3D_);
    addPort(meshOutport_);
    addPort(planeOutport_);
    addPort(offsetOutport_);

    offsetOutport_.setData(std::make_shared<size_t>(0));

    addProperty(sliceAlongAxis_);
    addProperty(sliceNr_);

    color_.setSemantics(PropertySemantics::Color);
    addProperty(color_);

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

void TensorFieldSlice::process() {
    uvec3 axis{};
    auto tensorField = inport_.getData();
    auto dimensions = tensorField->getDimensions();
    auto bounds = tensorField->getBounds<float>();
    auto extends = tensorField->getExtends<float>();

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

    outport2D_.setData(
        tensorutil::getSlice2D(inport_.getData(), sliceAlongAxis_.get(), sliceNr_.get()));
    outport3D_.setData(
        tensorutil::getSlice3D(inport_.getData(), sliceAlongAxis_.get(), sliceNr_.get()));

    auto mesh = tensorutil::generateSliceLevelGeometryForTensorField(
        inport_.getData(), color_.get(), sliceAlongAxis_.get(), sliceNr_.get());

    meshOutport_.setData(mesh);

    auto plane = std::make_shared<BasicMesh>();
    plane->setModelMatrix(mat4(1.f));
    auto inds = plane->addIndexBuffer(DrawType::Triangles, ConnectivityType::None);

    switch (sliceAlongAxis_.get()) {
        case CartesianCoordinateAxis::X: {
            auto xFrac = extends.x / bounds.x;
            xFrac *= sliceNr_.get();

            plane->addVertex(vec3(xFrac, 0.f, 1.f), vec3(1.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f),
                             color_.get());
            plane->addVertex(vec3(xFrac, 0.f, 0.f), vec3(1.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f),
                             color_.get());
            plane->addVertex(vec3(xFrac, 1.f, 1.f), vec3(1.f, 0.f, 0.f), vec3(1.f, 1.f, 0.f),
                             color_.get());
            plane->addVertex(vec3(xFrac, 1.f, 0.f), vec3(1.f, 0.f, 0.f), vec3(1.f, 0.f, 0.f),
                             color_.get());

        } break;
        case CartesianCoordinateAxis::Y: {
            auto yFrac = extends.y / bounds.y;
            yFrac *= sliceNr_.get();

            plane->addVertex(vec3(0.f, yFrac, 0.f), vec3(1.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f),
                             color_.get());
            plane->addVertex(vec3(0.f, yFrac, 1.f), vec3(1.f, 0.f, 0.f), vec3(0.f, 1.f, 0.f),
                             color_.get());
            plane->addVertex(vec3(1.f, yFrac, 0.f), vec3(1.f, 0.f, 0.f), vec3(1.f, 0.f, 0.f),
                             color_.get());
            plane->addVertex(vec3(1.f, yFrac, 1.f), vec3(1.f, 0.f, 0.f), vec3(1.f, 1.f, 0.f),
                             color_.get());
        } break;
        case CartesianCoordinateAxis::Z: {
            auto zFrac = extends.z / bounds.z;
            zFrac *= sliceNr_.get();

            plane->addVertex(vec3(0.f, 0.f, zFrac), vec3(1.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f),
                             color_.get());
            plane->addVertex(vec3(0.f, 1.f, zFrac), vec3(1.f, 0.f, 0.f), vec3(0.f, 1.f, 0.f),
                             color_.get());
            plane->addVertex(vec3(1.f, 0.f, zFrac), vec3(1.f, 0.f, 0.f), vec3(1.f, 0.f, 0.f),
                             color_.get());
            plane->addVertex(vec3(1.f, 1.f, zFrac), vec3(1.f, 0.f, 0.f), vec3(1.f, 1.f, 0.f),
                             color_.get());

        } break;
    }

    inds->add(0);
    inds->add(1);
    inds->add(2);

    inds->add(1);
    inds->add(2);
    inds->add(3);

    planeOutport_.setData(plane);
}

}  // namespace inviwo
