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

#include <modules/tensorvisio/processors/vtkdataarrayselection.h>
#include <inviwo/core/network/networklock.h>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <warn/pop>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VTKDataArraySelection::processorInfo_{
    "org.inviwo.VTKDataArraySelection",  // Class identifier
    "VTK Data Array Selection",          // Display name
    "VTK",                               // Category
    CodeState::Experimental,             // Code state
    Tags::CPU,                           // Tags
};
const ProcessorInfo VTKDataArraySelection::getProcessorInfo() const { return processorInfo_; }

VTKDataArraySelection::VTKDataArraySelection()
    : Processor(), inport_("inport"), outport_("outport"), arrays_("arrays", "arrays") {
    addPort(inport_);
    addPort(outport_);

    addProperty(arrays_);

    inport_.onChange([this]() { initializeResources(); });
}

void VTKDataArraySelection::initializeResources() {
    if (!inport_.hasData()) return;

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

    auto dataSet = *inport_.getData().get();
    auto pointData = dataSet->GetPointData();
    auto cellData = dataSet->GetCellData();

    int i{0};
    for (; i < pointData->GetNumberOfArrays(); ++i) {
        arrays_.addOption(
            stringReplace(stringReplace(pointData->GetArray(i)->GetName(), ".", ""), " ", ""),
            {pointData->GetArray(i)->GetName()}, i);
    }
    offset_ = i;
    i = 0;
    for (; i < cellData->GetNumberOfArrays(); ++i) {
        arrays_.addOption(
            stringReplace(stringReplace(cellData->GetArray(i)->GetName(), ".", ""), " ", ""),
            {cellData->GetArray(i)->GetName()}, i + offset_);
    }

    arrays_.setSelectedIndex(0);
}

void VTKDataArraySelection::process() {
    if (!inport_.hasData()) return;
    if (!arrays_.getOptions().size()) return;

    auto selectedOption = arrays_.get();
    auto dataSet = *inport_.getData().get();
    // point array
    if (selectedOption < offset_) {
        auto pointData = dataSet->GetPointData();
        outport_.setData(std::make_shared<vtkDataArray*>(pointData->GetArray(selectedOption)));
    } else {
        auto cellData = dataSet->GetCellData();
        outport_.setData(
            std::make_shared<vtkDataArray*>(cellData->GetArray(selectedOption - offset_)));
    }
}

}  // namespace inviwo
