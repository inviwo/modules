/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2020 Inviwo Foundation
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

#include <inviwo/tensorvisio/processors/tensorfield2dtovtk.h>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkStructuredGrid.h>
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkDoubleArray.h>
#include <warn/pop>

#include <inviwo/tensorvisbase/util/misc.h>
#include <inviwo/tensorvisbase/tensorvisbasemodule.h>
#include <inviwo/vtk/vtkmodule.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorField2DToVTK::processorInfo_{
    "org.inviwo.TensorField2DToVTK",             // Class identifier
    "Tensor Field 2D To VTK",                    // Display name
    "VTK",                                       // Category
    CodeState::Experimental,                     // Code state
    tag::OpenTensorVis | Tag::CPU | Tag("VTK"),  // Tags
};
const ProcessorInfo TensorField2DToVTK::getProcessorInfo() const { return processorInfo_; }

TensorField2DToVTK::TensorField2DToVTK()
    : Processor()
    , tensorFieldInport_("tensorFieldInport")
    , vtkDataSetOutport_("vtkDataSetOutport") {

    addPort(tensorFieldInport_);
    addPort(vtkDataSetOutport_);
}

void TensorField2DToVTK::process() {
    const auto tensorField = tensorFieldInport_.getData();
    const auto dimensions = ivec3{tensorField->getDimensionsAs<int>(), 1};
    const auto &tensors = *tensorField->tensors();

    auto structuredGrid = vtkSmartPointer<vtkStructuredGrid>::New();
    auto points = vtkSmartPointer<vtkPoints>::New();

    for (int y{0}; y < dimensions[1]; ++y) {
        for (int x{0}; x < dimensions[0]; ++x) {
            points->InsertNextPoint(x, y, 0);
        }
    }

    structuredGrid->SetDimensions(glm::value_ptr(dimensions));
    structuredGrid->SetPoints(points);

    auto pointData = structuredGrid->GetPointData();

    auto tensorArray = vtkSmartPointer<vtkDoubleArray>::New();
    tensorArray->SetNumberOfComponents(4);
    tensorArray->SetNumberOfTuples(tensors.size());

    tensorArray->SetComponentName(0, "xx");
    tensorArray->SetComponentName(1, "xy");
    tensorArray->SetComponentName(2, "yx");
    tensorArray->SetComponentName(3, "yy");
    tensorArray->SetName("Tensors");

    std::memcpy(tensorArray->GetPointer(0), tensors.data(), sizeof(double) * 4 * tensors.size());

    pointData->SetTensors(tensorArray);

    vtkDataSetOutport_.setData(std::make_shared<VTKDataSet>(structuredGrid));
}

}  // namespace inviwo
