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

#include <modules/tensorvisio/processors/vtkdatasettotensorfield3d.h>
#include <modules/tensorvisbase/util/misc.h>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkRectilinearGrid.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredPoints.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <warn/pop>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VTKDataSetToTensorField3D::processorInfo_{
    "org.inviwo.VTKDataSetToTensorField3D",  // Class identifier
    "VTK Data Set To Tensor Field 3D",       // Display name
    "VTK",                                   // Category
    CodeState::Experimental,                 // Code state
    Tags::CPU,                               // Tags
};
const ProcessorInfo VTKDataSetToTensorField3D::getProcessorInfo() const { return processorInfo_; }

VTKDataSetToTensorField3D::VTKDataSetToTensorField3D()
    : Processor()
    , dataSetInport_("dataSetInport")
    , tensorField3DOutport_("tensorField3DOutport")
    , normalizeExtents_("normalizeExtents", "Normalize extents", false)
    , arrays_("arrays", "arrays")
    , offset_{0}
    , previouslySelectedArrayName_{} {
    addPort(dataSetInport_);
    addPort(tensorField3DOutport_);

    addProperty(normalizeExtents_);

    addProperty(arrays_);

    dataSetInport_.onChange([this]() { initializeResources(); });
}

void VTKDataSetToTensorField3D::initializeResources() {
    if (!dataSetInport_.hasData()) return;

    const auto& dataSet = *dataSetInport_.getData();
    auto pointData = dataSet->GetPointData();

    auto stringReplace = [](std::string str, const std::string& from,
                            const std::string& to) -> const std::string {
        size_t start_pos{0};
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
        return str;
    };

    arrays_.clearOptions();

    for (int i{0}; i < pointData->GetNumberOfArrays(); ++i) {
        auto array = pointData->GetArray(i);

        if (array->GetNumberOfComponents() != 9) continue;

        arrays_.addOption(stringReplace(stringReplace(array->GetName(), ".", ""), " ", ""),
                          {array->GetName()}, {array->GetName()});
    }

    // If the data set contains an array with the same name as the previous data set, we set the
    // selected option to that array. This is mostly for serialization purposes.
    for (const auto& option : arrays_.getOptions()) {
        if (option.id_ == previouslySelectedArrayName_) {
            arrays_.setSelectedValue(option.value_);
            return;
        }
    }
    arrays_.setSelectedIndex(0);
    previouslySelectedArrayName_ = arrays_.getOptionDisplayName(0);
}

void VTKDataSetToTensorField3D::process() {
    if (!dataSetInport_.hasData()) return;

    const auto& dataSet = *dataSetInport_.getData();

    auto dataArray = dataSet->GetPointData()->GetArray(arrays_.get().c_str());

    const auto dimensions = dataSet.getDimensions();

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
