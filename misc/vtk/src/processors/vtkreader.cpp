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

#include <inviwo/vtk/processors/vtkreader.h>
#include <inviwo/core/util/filesystem.h>
#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/util/clock.h>

#include <warn/push>
#include <warn/ignore/all>

#include <warn/pop>

#include <fstream>

#include <fmt/format.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VTKReader::processorInfo_{
    "org.inviwo.VTKReader",   // Class identifier
    "VTK Reader",             // Display name
    "VTK",                    // Category
    CodeState::Experimental,  // Code state
    "CPU, Data Input, VTK",   // Tags
};
const ProcessorInfo VTKReader::getProcessorInfo() const { return processorInfo_; }

VTKReader::VTKReader()
    : Processor()
    , file_("vtkFile", "VTK file", "", "VTK")
    , reloadButton_("reload", "Reload Data")
    , outport_("VTKDataObjectOutport")
    , data_(nullptr) {

    file_.addNameFilter(FileExtension("vti", "VTK ImageData (structured)"));
    file_.addNameFilter(FileExtension("vtp", "VTK PolyData (unstructured)"));
    file_.addNameFilter(FileExtension("vtr", "VTK RectilinearGrid (structured)"));
    file_.addNameFilter(FileExtension("vts", "VTK StructuredGrid (structured)"));
    file_.addNameFilter(FileExtension("vtu", "VTK UnstructuredGrid (unstructured)"));
    file_.addNameFilter(FileExtension("vtk", "Legacy VTK Format"));
    file_.addNameFilter(FileExtension("pvti", "Parallel vtkImageData (structured)"));
    file_.addNameFilter(FileExtension("pvtp", "Parallel vtkPolyData (unstructured)"));
    file_.addNameFilter(FileExtension("pvtr", "Parallel vtkRectilinearGrid (structured)"));
    file_.addNameFilter(FileExtension("pvts", "Parallel vtkStructuredGrid (structured)"));
    file_.addNameFilter(FileExtension("pvtu", "Parallel vtkUnstructuredGrid (unstructured)"));

    addProperty(file_);

    addPort(outport_);

    progressBar_.hide();
}

void VTKReader::process() {
    if (file_.isModified()) {
        const auto fileName = file_.get();

        if (!filesystem::fileExists(fileName)) {
            LogError("File " << fileName << "not found.");
            return;
        }

        const auto fileType = determineFileType(fileName);

        switch (fileType) {
            case VTKFileType::Legacy:
                read<VTKFileType::Legacy>();
                break;
            case VTKFileType::XML:
                read<VTKFileType::XML>();
                break;
            default:
                break;
        }
    }
}

VTKReader::VTKFileType VTKReader::determineFileType(const std::string& fileName) const {
    std::string line{};
    std::ifstream infile = filesystem::ifstream(fileName);
    if (!infile.is_open()) {
        throw inviwo::Exception(fmt::format("Could not open file '{}'", fileName), IVW_CONTEXT);
    }

    // VTK file header is always ASCII so we can just read the first line.
    std::getline(infile, line);

    line = toLower(line);

    // This is what should be in the header according to documentation.
    if (line.find("xml") != std::string::npos) {
        return VTKFileType::XML;
    }
    // This is what Paraview puts into their header...
    if (line.find("vtkfile") != std::string::npos) {
        return VTKFileType::XML;
    }
    if (line.find("vtk") != std::string::npos) {
        return VTKFileType::Legacy;
    }
    LogInfo("File type could not be determined.") { return VTKFileType::Unknown; }
}
}  // namespace inviwo
