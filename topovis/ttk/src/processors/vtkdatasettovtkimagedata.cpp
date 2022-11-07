/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2022 Inviwo Foundation
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

#include <inviwo/ttk/processors/vtkdatasettovtkimagedata.h>
#include <inviwo/core/datastructures/volume/volumeramprecision.h>

#include <vtkImageData.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VTKDataSetToVTKImageData::processorInfo_{
    "org.inviwo.VTKDataSetToVTKImageData",  // Class identifier
    "vtkDataSet To vtkImageData",           // Display name
    "VTK",                                  // Category
    CodeState::Experimental,                // Code state
    Tags::CPU,                              // Tags
};
const ProcessorInfo VTKDataSetToVTKImageData::getProcessorInfo() const { return processorInfo_; }

VTKDataSetToVTKImageData::VTKDataSetToVTKImageData()
    : Processor(), inport_("inport", "vtkDataSet"), outport_("outport", "vtkImageData") {

    addPorts(inport_, outport_);
}

void VTKDataSetToVTKImageData::process() {
    auto vtkDataSet = inport_.getData();

    if (vtkDataSet != nullptr) {

        if (!vtkDataSet->IsA("vtkImageData")) {
            throw Exception("Data is not of type vtkImageData.",
                            IVW_CONTEXT_CUSTOM("VTKDataSetToVTKImageData"));
        }

        outport_.setData(vtkDataSet);
    }
}
}  // namespace inviwo
