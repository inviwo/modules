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

#include <modules/tensorvisio/processors/vtkdatasetinformation.h>
#include <inviwo/core/network/networklock.h>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkInformation.h>
#include <vtkDataSet.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <warn/pop>

namespace inviwo {
// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VTKDataSetInformation::processorInfo_{
    "org.inviwo.VTKDataSetInformation",  // Class identifier
    "VTK Data Set Information",          // Display name
    "VTK",                               // Category
    CodeState::Experimental,             // Code state
    Tags::CPU,                           // Tags
};
const ProcessorInfo VTKDataSetInformation::getProcessorInfo() const { return processorInfo_; }

VTKDataSetInformation::VTKDataSetInformation()
    : Processor()
    , inport_("inport")
    , className_("className", "Class name", "", InvalidationLevel::Valid)
    , pointDataArrays_("pointDataArrays", "Point data arrays")
    , cellDataArrays_("cellDataArrays", "Cell data arrays") {
    addPort(inport_);

    className_.setReadOnly(true);

    addProperty(className_);
    addProperty(pointDataArrays_);
}

void VTKDataSetInformation::process() {
    auto dataObject = *inport_.getData().get();
    auto dataSet = static_cast<vtkDataSet*>(dataObject);

    className_.set(std::string{dataObject->GetClassName()});

    auto pointData = dataSet->GetPointData();

    NetworkLock lock;
    for (int i{0}; i < pointData->GetNumberOfArrays(); ++i) {
        auto array = pointData->GetArray(i);
        pointDataArrays_.addProperty(new ArrayInformationProperty{
            std::string{array->GetName()}, std::string{array->GetDataTypeAsString()},
            std::to_string(array->GetNumberOfComponents())});
    }
    lock.~NetworkLock();
}

}  // namespace inviwo
