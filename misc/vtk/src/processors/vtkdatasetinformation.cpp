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

#include <inviwo/vtk/processors/vtkdatasetinformation.h>
#include <inviwo/core/network/networklock.h>
#include <inviwo/core/util/stringconversion.h>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkInformation.h>
#include <vtkDataSet.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkDoubleArray.h>
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
    addProperty(cellDataArrays_);
}

void VTKDataSetInformation::process() {
    const auto &dataSet = *inport_.getData();

    className_.set(std::string{dataSet->GetClassName()});

    const auto pointData = dataSet->GetPointData();
    const auto cellData = dataSet->GetCellData();

    {
        NetworkLock lock;

        std::vector<std::string> identifiers{};
        for (const auto &property : pointDataArrays_.getProperties()) {
            identifiers.emplace_back(property->getIdentifier());
        }
        for (const auto &identifier : identifiers) {
            pointDataArrays_.removeProperty(identifier);
        }

        identifiers.clear();
        for (const auto &property : cellDataArrays_.getProperties()) {
            identifiers.emplace_back(property->getIdentifier());
        }
        for (const auto &identifier : identifiers) {
            cellDataArrays_.removeProperty(identifier);
        }

        for (int i{0}; i < pointData->GetNumberOfArrays(); ++i) {
            const auto array = pointData->GetArray(i);
            auto arrayName = std::string{array->GetName()};
            auto identifier = arrayName;

            replaceInString(identifier, ".", "");
            replaceInString(identifier, " ", "");

            pointDataArrays_.addProperty(new ArrayInformationProperty{
                arrayName, identifier, std::string{array->GetDataTypeAsString()},
                std::to_string(array->GetNumberOfComponents())});
        }

        for (int i{0}; i < cellData->GetNumberOfArrays(); ++i) {
            const auto array = cellData->GetArray(i);
            const auto arrayName = std::string{array->GetName()};
            auto identifier = arrayName;

            replaceInString(identifier, ".", "");
            replaceInString(identifier, " ", "");

            cellDataArrays_.addProperty(new ArrayInformationProperty{
                arrayName, identifier, std::string{array->GetDataTypeAsString()},
                std::to_string(array->GetNumberOfComponents())});
        }
    }
}

}  // namespace inviwo
