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

#include <modules/tensorvisio/processors/vtkwriter.h>
#include <inviwo/core/util/filesystem.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VTKWriter::processorInfo_{
    "org.inviwo.VTKWriter",   // Class identifier
    "VTK Writer",             // Display name
    "VTK",                    // Category
    CodeState::Experimental,  // Code state
    Tags::CPU,                // Tags
};
const ProcessorInfo VTKWriter::getProcessorInfo() const { return processorInfo_; }

VTKWriter::VTKWriter()
    : Processor()
    , inport_("inport")
    , writer_(vtkSmartPointer<vtkXMLDataSetWriter>::New())
    , file_("file", "File name", "", "vtk")
    , button_("button", "Export") {
    addPort(inport_);

    addProperty(file_);
    file_.setAcceptMode(AcceptMode::Save);

    addProperty(button_);
    button_.onChange([this]() { this->export_isacppkeyword(); });
}

void VTKWriter::process() {}

void VTKWriter::export_isacppkeyword() {
    if (!inport_.hasData()) return;

    auto dataSet = *inport_.getData();

    if (file_.get().empty()) file_.requestFile();

    std::string ending{};

    if (!file_.get().empty()) {
        switch (dataSet->GetDataObjectType()) {
            case VTK_STRUCTURED_GRID:
                ending = "vts";
                break;
            case VTK_IMAGE_DATA:
                ending = "vti";
                break;
            case VTK_POLY_DATA:
                ending = "vtp";
                break;
            case VTK_RECTILINEAR_GRID:
                ending = "vtr";
                break;
            case VTK_UNSTRUCTURED_GRID:
                ending = "vtu";
                break;
            default:
                ending = "vtk";
                break;
        }

        std::string vtkConvention = filesystem::replaceFileExtension(file_.get(), ending);
        // write
        writer_->SetInputData(*dataSet);
        writer_->SetFileName(vtkConvention.c_str());
        writer_->Write();
        LogProcessorInfo("File written to " << vtkConvention);
    } else {
        LogProcessorWarn("Error: Please specify a file to write to");
    }
}

}  // namespace inviwo
