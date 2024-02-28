/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2022-2024 Inviwo Foundation
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

#include <inviwo/vtk/processors/vtkdatasettovtkimagedata.h>
#include <inviwo/core/datastructures/volume/volumeramprecision.h>

#include <vtkImageData.h>

#include <vtkType.h>
#include <vtkDataObjectTypes.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VTKDowncastData::processorInfo_{
    "org.inviwo.VTKDowncastData",  // Class identifier
    "VTK Downcast Data",           // Display name
    "VTK",                         // Category
    CodeState::Experimental,       // Code state
    Tag::CPU | Tag{"VTK"},         // Tags
};
const ProcessorInfo VTKDowncastData::getProcessorInfo() const { return processorInfo_; }

VTKDowncastData::VTKDowncastData()
    : Processor()
    , outportType_{"outportType", "Derived Type",
                   []() {
                       OptionPropertyState<int> state;
                       for (int typeId = VTK_POLY_DATA; typeId <= VTK_IMAGE_STENCIL_DATA;
                            ++typeId) {

                           if (vtkDataObjectTypes::TypeIdIsA(typeId, VTK_DATA_SET)) {
                               if (typeId == VTK_IMAGE_DATA) {
                                   state.selectedIndex = state.options.size();
                               }

                               auto name = vtkDataObjectTypes::GetClassNameFromTypeId(typeId);
                               state.options.emplace_back(name, name, typeId);
                           }
                       }
                       return state;
                   }()

      }
    , inport_{"inport", VTK_DATA_SET}
    , outport_{"outport", outportType_.getSelectedValue()} {

    addPorts(inport_, outport_);
    addProperties(outportType_);

    outportType_.onChange([this]() { outport_.setTypeId(outportType_.getSelectedValue()); });
}

void VTKDowncastData::process() {
    if (auto vtkDataSet = inport_.getData()) {
        if (!vtkDataSet->IsA(vtkDataObjectTypes::GetClassNameFromTypeId(outport_.getTypeId()))) {
            outport_.setData(nullptr);
            throw Exception(IVW_CONTEXT, "Data is not of type '{}' got '{}'",
                            vtkDataObjectTypes::GetClassNameFromTypeId(outport_.getTypeId()),
                            vtkDataSet->GetClassName());
        }
        outport_.setData(vtkDataSet);
    }
}
}  // namespace inviwo
