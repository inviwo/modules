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

#include <modules/tensorvisio/processors/vtkdataarraytotensorfield3d.h>
#include <modules/tensorvisbase/util/misc.h>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkRectilinearGrid.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredPoints.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <warn/pop>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VTKDataArrayToTensorField3D::processorInfo_{
    "org.inviwo.VTKDataArrayToTensorField3D",  // Class identifier
    "VTK Data Array To Tensor Field 3D",       // Display name
    "VTK",                                     // Category
    CodeState::Experimental,                   // Code state
    Tags::CPU,                                 // Tags
};
const ProcessorInfo VTKDataArrayToTensorField3D::getProcessorInfo() const { return processorInfo_; }

VTKDataArrayToTensorField3D::VTKDataArrayToTensorField3D()
    : Processor()
    , dataArrayInport_("dataArrayInport")
    , dataSetInport_("dataSetInport")
    , tensorField3DOutport_("tensorField3DOutport")
    , normalizeExtents_("normalizeExtents", "Normalize extents", false) {
    addPort(dataSetInport_);
    addPort(dataArrayInport_);
    addPort(tensorField3DOutport_);

    addProperty(normalizeExtents_);
}

void VTKDataArrayToTensorField3D::process() {
    if (!dataArrayInport_.hasData()) return;
    if (!dataSetInport_.hasData()) return;

    auto dataArray = *dataArrayInport_.getData().get();

    if (dataArray->GetNumberOfComponents() != 9) {
        LogProcessorError("No tensor data provided.");
        return;
    }

    auto dataSet = *dataSetInport_.getData().get();

    // As far as I can see, I have to do this because there is no superclass for all those data set
    // types that have the method "GetDimensions()".
    int* dimsptr{nullptr};

    switch (dataSet->GetDataObjectType()) {
        case VTK_RECTILINEAR_GRID:
            dimsptr = dynamic_cast<vtkRectilinearGrid*>(dataSet)->GetDimensions();
            break;
        case VTK_STRUCTURED_GRID:
            dimsptr = dynamic_cast<vtkStructuredGrid*>(dataSet)->GetDimensions();
            break;
        case VTK_STRUCTURED_POINTS:
            dimsptr = dynamic_cast<vtkStructuredPoints*>(dataSet)->GetDimensions();
            break;
        default:
            LogProcessorWarn("Data set type not supported.");
            return;
    }

    size3_t dimensions{dimsptr[0], dimsptr[1], dimsptr[2]};

    LogProcessorInfo("Attempting to generate tensor field from array \""
                     << std::string{dataArray->GetName()} << "\"");

    const auto bounds = dataSet->GetBounds();
    auto extent = tensorutil::vtk_ExtentFromBounds(bounds);
    const auto offset = tensorutil::vtk_OffsetFromBounds(bounds);

    if (normalizeExtents_.get()) {
        extent /= std::max(std::max(extent.x, extent.y), extent.z);
    }

    if (dataArray->GetDataType() == VTK_DOUBLE) {
        auto tensorField = std::make_shared<TensorField3D>(
            dimensions, dynamic_cast<vtkDoubleArray*>(dataArray)->GetPointer(0), extent);
        tensorField->setOffset(offset);
        tensorField3DOutport_.setData(tensorField);
    } else if (dataArray->GetDataType() == VTK_FLOAT) {
        auto tensorField = std::make_shared<TensorField3D>(
            dimensions, dynamic_cast<vtkFloatArray*>(dataArray)->GetPointer(0), extent);
        tensorField->setOffset(offset);
        tensorField3DOutport_.setData(tensorField);
    } else {
        LogProcessorError("Failed to generate tensor field from array \""
                          << std::string{dataArray->GetName()} << "\" because data type is "
                          << std::string{dataArray->GetDataTypeAsString()} << ".");
        return;
    }
}

}  // namespace inviwo
