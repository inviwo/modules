/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2020 Inviwo Foundation
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

#include <inviwo/tensorvisio/processors/tensorfield3dexport.h>
#include <inviwo/tensorvisbase/ports/tensorfieldport.h>
#include <inviwo/tensorvisbase/tensorvisbasemodule.h>
#include <inviwo/tensorvisio/util/util.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorField3DExport::processorInfo_{
    "org.inviwo.TensorField3DExport",  // Class identifier
    "Tensor Field 3D Export",          // Display name
    "IO",                              // Category
    CodeState::Experimental,           // Code state
    tag::OpenTensorVis | Tag::CPU,     // Tags
};
const ProcessorInfo TensorField3DExport::getProcessorInfo() const { return processorInfo_; }

TensorField3DExport::TensorField3DExport()
    : Processor()
    , inport_("inport")
    , export_("export", "Export")
    , exportFile_("exportFile", "Export to", "")
    , exportButton_("exportButton", "Export")
    , includeMetaData_("includeMetaData", "Include meta data", true) {
    export_.addProperty(exportFile_);
    export_.addProperty(exportButton_);
    export_.addProperty(includeMetaData_);
    addProperty(export_);

    exportFile_.setFileMode(FileMode::AnyFile);
    exportFile_.setAcceptMode(AcceptMode::Save);
    exportFile_.clearNameFilters();
    exportFile_.addNameFilter("Tensor field binary (*.tfb)");

    exportFile_.setCurrentStateAsDefault();

    exportButton_.onChange([&]() { exportBinary(); });

    addPort(inport_);
}

void TensorField3DExport::process() {}

void TensorField3DExport::exportBinary() const {
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

    size_t version = TFB_CURRENT_VERSION;
    outFile.write(reinterpret_cast<const char *>(&version), sizeof(size_t));

    auto hasMetaData = glm::uint8(includeMetaData_.get());
    outFile.write(reinterpret_cast<const char *>(&hasMetaData), sizeof(glm::uint8));

    auto dimensions = tensorField->getDimensions();
    outFile.write(reinterpret_cast<const char *>(&dimensions), sizeof(size_t) * 3);

    auto extents = tensorField->getExtents();
    outFile.write(reinterpret_cast<const char *>(&extents), sizeof(float) * 3);

    auto offset = tensorField->getOffset();
    outFile.write(reinterpret_cast<const char *>(&offset), sizeof(float) * 3);

    auto &eigenValueDataMaps = tensorField->dataMapEigenValues_;
    outFile.write(reinterpret_cast<const char *>(&eigenValueDataMaps[0].dataRange),
                  sizeof(double) * 2);
    outFile.write(reinterpret_cast<const char *>(&eigenValueDataMaps[1].dataRange),
                  sizeof(double) * 2);
    outFile.write(reinterpret_cast<const char *>(&eigenValueDataMaps[2].dataRange),
                  sizeof(double) * 2);

    auto &eigenVectorDataMaps = tensorField->dataMapEigenVectors_;
    outFile.write(reinterpret_cast<const char *>(&eigenVectorDataMaps[0].dataRange),
                  sizeof(double) * 2);
    outFile.write(reinterpret_cast<const char *>(&eigenVectorDataMaps[1].dataRange),
                  sizeof(double) * 2);
    outFile.write(reinterpret_cast<const char *>(&eigenVectorDataMaps[2].dataRange),
                  sizeof(double) * 2);

    const auto &data = *tensorField->tensors();

    for (const auto &val : data) {
        outFile.write(reinterpret_cast<const char *>(glm::value_ptr(val)),
                      sizeof(TensorField3D::value_type) * 9);
    }

    auto hasMask = glm::uint8(tensorField->hasMask());
    outFile.write(reinterpret_cast<char *>(&hasMask), sizeof(glm::uint8));

    if (hasMask) {
        auto &mask = tensorField->getMask();
        auto maskData = mask.data();
        outFile.write(reinterpret_cast<const char *>(maskData), sizeof(glm::uint8) * mask.size());
    }

    if (includeMetaData_.get()) {
        const size_t numMetaDataEntries =
            tensorField->metaData()->getNumberOfColumns() - 1;  // omit index buffer
        outFile.write(reinterpret_cast<const char *>(&numMetaDataEntries), sizeof(size_t));
        util::serializeDataFrame(tensorField->metaData(), outFile);
    }

    std::string str("EOFreached");
    size = str.size();
    outFile.write(reinterpret_cast<const char *>(&size), sizeof(size_t));
    outFile.write(&str[0], size);

    outFile.close();

    LogInfo(exportFile_.get() << " successfully exported. (roughly "
                              << util::getFileSizeAsString(exportFile_.get(),
                                                           util::FileSizeOrder::GiB));
}
}  // namespace inviwo