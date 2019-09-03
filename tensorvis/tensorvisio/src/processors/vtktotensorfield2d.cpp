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

#include <modules/tensorvisio/processors/vtktotensorfield2d.h>

#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>

#include <modules/tensorvisbase/datastructures/tensorfield2d.h>
#include <modules/tensorvisbase/util/misc.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VTKToTensorField2D::processorInfo_{
    "org.inviwo.VTKToTensorField2D",  // Class identifier
    "VTK To Tensor Field 2D",         // Display name
    "VTK",                            // Category
    CodeState::Experimental,          // Code state
    "VTK",                            // Tags
};
const ProcessorInfo VTKToTensorField2D::getProcessorInfo() const { return processorInfo_; }

VTKToTensorField2D::VTKToTensorField2D()
    : Processor()
    , vtkDataSetInport_("vtkDataSetInport")
    , tensorField2DOutport_("tensorField2DOutport") {
    addPort(vtkDataSetInport_);
    addPort(tensorField2DOutport_);
}

void VTKToTensorField2D::process() {
    const auto inviwoDataSet = vtkDataSetInport_.getData();
    const auto& vtkDataSet = *vtkDataSetInport_.getData();

    size3_t dimensions{};
    if (auto d = inviwoDataSet->getDimensions())
        dimensions = *d;
    else {
        LogError("VTK data set type not supported.");
        return;
    }
    if (dimensions.z != 1) {
        LogError("Z dimension != 1. Aborting.");
        return;
    }

    const auto numberOfElements = glm::compMul(dimensions);

    const auto pointData = vtkDataSet->GetPointData();
    const auto tensors = pointData->GetTensors();

    if (tensors->GetDataType() != VTK_FLOAT && tensors->GetDataType() != VTK_DOUBLE) {
        LogError("Data type is neither float nor double. Aborting.");
        return;
    }

    std::vector<dmat2> tensorVector2D;
    tensorVector2D.resize(numberOfElements);

    // Check to see if we need to project the tensor
    // Assume that we project along Z axis
    if (tensors->GetNumberOfComponents() == 9) {
        std::vector<dmat3> tensorVector3D;
        tensorVector3D.resize(numberOfElements);
        auto tensorVector3DRawData = reinterpret_cast<double*>(tensorVector3D.data());

        if (tensors->GetDataType() == VTK_DOUBLE) {
            const double* raw = vtkDoubleArray::SafeDownCast(tensors)->GetPointer(0);
            std::memcpy(tensorVector3DRawData, raw, 9 * sizeof(double) * tensorVector3D.size());
        } else if (tensors->GetDataType() == VTK_FLOAT) {
            const float* raw = vtkFloatArray::SafeDownCast(tensors)->GetPointer(0);
            for (size_t i{0}; i < tensorVector3D.size(); ++i) {
                tensorVector3DRawData[i] = static_cast<double>(raw[i]);
            }
        }

        std::transform(tensorVector3D.begin(), tensorVector3D.end(), tensorVector2D.begin(),
                       [](const dmat3& tensor) {
                           return tensorutil::getProjectedTensor(tensor,
                                                                 CartesianCoordinateAxis::Z);
                       });
    } else {
        auto tensorVector2DRawData = reinterpret_cast<double*>(tensorVector2D.data());
        if (tensors->GetDataType() == VTK_DOUBLE) {
            const double* raw = vtkDoubleArray::SafeDownCast(tensors)->GetPointer(0);
            std::memcpy(tensorVector2DRawData, raw, 4 * sizeof(double) * tensorVector2D.size());
        } else if (tensors->GetDataType() == VTK_FLOAT) {
            const float* raw = vtkFloatArray::SafeDownCast(tensors)->GetPointer(0);
            for (size_t i{0}; i < tensorVector2D.size(); ++i) {
                tensorVector2DRawData[i] = static_cast<double>(raw[i]);
            }
        }
    }

    tensorField2DOutport_.setData(
        std::make_shared<TensorField2D>(size2_t{dimensions}, tensorVector2D));
}

}  // namespace inviwo
