/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2017-2020 Inviwo Foundation
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

#include <inviwo/tensorvisio/processors/nrrdreader.h>
#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/core/util/filesystem.h>
#include <inviwo/tensorvisbase/tensorvisbasemodule.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo NRRDReader::processorInfo_{
    "org.inviwo.NRRDReader",        // Class identifier
    "NRRDReader",                   // Display name
    "Tensor Field IO",              // Category
    CodeState::Experimental,        // Code state
    tag::OpenTensorVis | Tag::CPU,  // Tags
};
const ProcessorInfo NRRDReader::getProcessorInfo() const { return processorInfo_; }

NRRDReader::NRRDReader()
    : Processor()
    , inFile_("inFile", "File", "")
    , outport3D_("outport3D")
    , volumeOutport_("volumeOutport") {
    addPort(outport3D_);
    addPort(volumeOutport_);
    addProperty(inFile_);
}

void NRRDReader::process() {
    std::ifstream inFile(inFile_.get());

    if (!inFile) {
        LogError("Couldn't open file");
        return;
    }

    auto extension = filesystem::getFileExtension(inFile_.get());
    if (extension != "nhdr") {
        LogError("Not a NHDR file");
        return;
    }

    std::map<std::string, std::string> values;

    // Get first line
    std::string version;
    std::getline(inFile, version);

    std::string line;
    while (std::getline(inFile, line)) {
        std::istringstream iss(line);
        std::string component;
        std::string value;

        std::getline(iss, component, ':');
        std::getline(iss, value, ':');

        if (component[0] == '#') continue;

        value.erase(0, 1);

        value.erase(std::remove(value.begin(), value.end(), '"'), value.end());

        values.insert(std::pair<std::string, std::string>(component, value));
    }

    size3_t dimensions{0};
    std::string rawFileName;
    {
        auto it = values.find("sizes");
        if (it != values.end()) {
            auto dimensionString = (*it).second;
            std::istringstream iss(dimensionString);

            std::string numberOfItemsPerTensorString;
            std::getline(iss, numberOfItemsPerTensorString, ' ');

            std::string dimXString;
            std::getline(iss, dimXString, ' ');
            dimensions.x = std::stoull(dimXString);

            std::string dimYString;
            std::getline(iss, dimYString, ' ');
            dimensions.y = std::stoull(dimYString);

            std::string dimZString;
            std::getline(iss, dimZString, ' ');
            dimensions.z = std::stoull(dimZString);
        }
    }

    {
        auto it = values.find("data file");
        if (it != values.end()) {
            auto path = filesystem::getFileDirectory(inFile_.get());
            rawFileName = path + '/' + (*it).second;
        }
    }

    if (!filesystem::fileExists(rawFileName)) {
        LogError("Raw file does not exist");
        return;
    }
    std::ifstream rawFile(rawFileName, std::ios::in | std::ios::binary);

    if (!rawFile) {
        LogError("Couldn't open raw file");
        return;
    }

    auto numberOfTensors = dimensions.x * dimensions.y * dimensions.z;

    std::vector<mat3> dataForTensorField;

    dataForTensorField.resize(numberOfTensors);

    util::IndexMapper3D indexMapper(dimensions);

    auto vol = std::make_shared<Volume>(dimensions, DataFloat32::get());
    auto volRam = vol->getEditableRepresentation<VolumeRAM>();
    auto volRamData = static_cast<float*>(volRam->getData());

    auto toLittleEndian = [](const float val) -> float {
        float read = val;
        float write;
        auto r_ptr = reinterpret_cast<char*>(&read);
        auto w_ptr = reinterpret_cast<char*>(&write);
        w_ptr[0] = r_ptr[3];
        w_ptr[1] = r_ptr[2];
        w_ptr[2] = r_ptr[1];
        w_ptr[3] = r_ptr[0];
        return write;
    };

    for (size_t z = 0; z < dimensions.z; z++) {
        for (size_t y = 0; y < dimensions.y; y++) {
            for (size_t x = 0; x < dimensions.x; x++) {
                auto index = indexMapper(size3_t(x, y, z));
                float confidence;
                float xx;
                float yy;
                float zz;
                float xy;
                float xz;
                float yz;

                rawFile.read(reinterpret_cast<char*>(&confidence), sizeof(float));

                rawFile.read(reinterpret_cast<char*>(&xx), sizeof(float));
                rawFile.read(reinterpret_cast<char*>(&xy), sizeof(float));
                rawFile.read(reinterpret_cast<char*>(&xz), sizeof(float));

                rawFile.read(reinterpret_cast<char*>(&yy), sizeof(float));
                rawFile.read(reinterpret_cast<char*>(&yz), sizeof(float));
                rawFile.read(reinterpret_cast<char*>(&zz), sizeof(float));

                confidence = toLittleEndian(confidence);
                xx = toLittleEndian(xx);
                yy = toLittleEndian(yy);
                zz = toLittleEndian(zz);
                xy = toLittleEndian(xy);
                xz = toLittleEndian(xz);
                yz = toLittleEndian(yz);

                double weight = 0.;
                if (confidence == 1.) {
                    weight = 1.;
                }

                vec3 col1(xx, xy, xz);
                vec3 col2(xy, yy, yz);
                vec3 col3(xz, yz, zz);

                dataForTensorField[index] = mat3(col1, col2, col3) * weight;
                volRamData[index] = confidence;
            }
        }
    }

    vol->setBasis(mat3(1.f));
    vol->setOffset(vec3(0.f));
    vol->dataMap_.dataRange = vec2(0, 1);
    vol->dataMap_.valueRange = vec2(0, 1);
    volumeOutport_.setData(vol);

    auto outField = std::make_shared<TensorField3D>(dimensions, dataForTensorField);
    outField->setBasis(vol->getBasis());
    outField->setOffset(vol->getOffset());
    outport3D_.setData(outField);
}

}  // namespace inviwo
