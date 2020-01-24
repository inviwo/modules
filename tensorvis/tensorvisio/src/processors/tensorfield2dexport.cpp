/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2017-2018 Inviwo Foundation
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

#include <inviwo/tensorvisio/processors/tensorfield2dexport.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorField2DExport::processorInfo_{
    "org.inviwo.TensorField2DExport",  // Class identifier
    "Tensor Field 2D Export",          // Display name
    "Tensor Field IO",                 // Category
    CodeState::Experimental,           // Code state
    Tags::CPU,                         // Tags
};
const ProcessorInfo TensorField2DExport::getProcessorInfo() const { return processorInfo_; }

TensorField2DExport::TensorField2DExport()
    : Processor()
    , inport_("inport")
    , export_("export", "Export")
    , exportFile_("exportFile", "Export to", "")
    , exportButton_("exportButton", "Export")
    , includeEigenInfo_("includeEigenInfo", "Include eigen info", true) {
    export_.addProperty(exportFile_);
    export_.addProperty(exportButton_);
    addProperty(export_);

    exportFile_.setFileMode(FileMode::AnyFile);
    exportFile_.setAcceptMode(AcceptMode::Save);
    exportFile_.clearNameFilters();
    exportFile_.addNameFilter("Tensor field binary (*.tfb)");

    exportFile_.setCurrentStateAsDefault();

    exportButton_.onChange([&]() { exportBinary(); });

    addPort(inport_);
}

void TensorField2DExport::process() {}

void TensorField2DExport::exportBinary() const {
    if (!inport_.hasData() || !inport_.getData().get()) {
        LogWarn("Inport has no data");
        return;
    }

    LogInfo("Exporting...");

    auto tensorField = inport_.getData();

    std::ofstream outFile;
    outFile.open(exportFile_.get(), std::ios::out | std::ios::binary);

    std::string versionStr("TFBVersion:");
    size_t size = versionStr.size();
    outFile.write(reinterpret_cast<const char *>(&size), sizeof(size_t));
    outFile.write(&versionStr[0], size);

    size_t version = 2;
    outFile.write(reinterpret_cast<const char *>(&version), sizeof(size_t));

    bool hasEigenInfo = includeEigenInfo_.get();
    outFile.write(reinterpret_cast<const char *>(&hasEigenInfo), sizeof(bool));

    auto dimensions = tensorField->getDimensions();
    outFile.write(reinterpret_cast<const char *>(&dimensions), sizeof(size_t) * 2);

    auto extents = tensorField->getExtents();
    outFile.write(reinterpret_cast<const char *>(&extents), sizeof(double) * 2);

    const auto &data = tensorField->tensors();

    for (const auto &val : *data) {
        // Diagonal
        outFile.write(reinterpret_cast<const char *>(&val[0][0]), sizeof(double));
        outFile.write(reinterpret_cast<const char *>(&val[1][1]), sizeof(double));

        // Symmetric part
        outFile.write(reinterpret_cast<const char *>(&val[1][0]), sizeof(double));
    }

    if (includeEigenInfo_.get()) {
        // Create tensor field here so eigen properties get calculated
        auto majorEigenValues = tensorField->majorEigenValues().data();
        auto minorEigenValues = tensorField->minorEigenValues().data();

        auto majorEigenVectors = tensorField->majorEigenVectors().data();
        auto minorEigenVectors = tensorField->minorEigenVectors().data();

        auto numItems = tensorField->getSize();

        outFile.write(reinterpret_cast<const char *>(majorEigenValues), sizeof(double) * numItems);

        outFile.write(reinterpret_cast<const char *>(minorEigenValues), sizeof(double) * numItems);

        outFile.write(reinterpret_cast<const char *>(majorEigenVectors),
                      sizeof(double) * numItems * 2);

        outFile.write(reinterpret_cast<const char *>(minorEigenVectors),
                      sizeof(double) * numItems * 2);
    }

    std::string str("EOFreached");
    size = str.size();
    outFile.write(reinterpret_cast<const char *>(&size), sizeof(size_t));
    outFile.write(&str[0], size);

    outFile.close();

    LogInfo("Exporting done.");
}

}  // namespace inviwo
