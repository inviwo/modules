/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020 Inviwo Foundation
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

#include <inviwo/dicom/processors/dicomtagextractor.h>

#include <gdcmReader.h>
#include <gdcmTag.h>

#include <fstream>
#include <sstream>
#include <vector>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo DICOMTagExtractor::processorInfo_{
    "org.inviwo.dicomtagextractor",  // Class identifier
    "DICOM Tag Extractor",           // Display name
    "Information",                   // Category
    CodeState::Experimental,         // Code state
    Tags::CPU,                       // Tags
};
const ProcessorInfo DICOMTagExtractor::getProcessorInfo() const { return processorInfo_; }

DICOMTagExtractor::DICOMTagExtractor()
    : Processor()
    , filename_("filename_", "Filename")
    , tagGroup_("tag_group", "Tag Group", "0x0000")
    , tagElement_("tag_element", "Tag Element", "0x0000")
    , useSubTag_("use_sub_tag", "Use Sub Tag", false)
    , subTagGroup_("sub_tag_group", "Sub Tag Group", "0x0000")
    , subTagElement_("sub_tag_element", "Sub Tag Element", "0x0000") {

    subTagGroup_.setVisible(useSubTag_.get());
    subTagElement_.setVisible(useSubTag_.get());
    addProperties(filename_, tagGroup_, tagElement_, useSubTag_, subTagGroup_, subTagElement_);

    useSubTag_.onChange([this]() {
        subTagGroup_.setVisible(useSubTag_.get());
        subTagElement_.setVisible(useSubTag_.get());
    });
}

gdcm::Tag convert_hex_str_to_tag(const std::string& tag_group, const std::string& tag_element) {
    uint16_t tg, te;
    std::stringstream converter;
    converter << std::hex << tag_group;
    converter >> tg;
    converter.clear();
    converter << std::hex << tag_element;
    converter >> te;
    return {tg, te};
}

void DICOMTagExtractor::process() {
    std::ifstream f_in{filename_, std::ios::binary};
    if (!f_in.is_open()) {
        LogWarn("could no open file: " << filename_);
        return;
    }

    gdcm::Reader reader;
    reader.SetStream(f_in);
    if (!reader.Read()) {
        LogWarn(filename_ << " is not a DICOM file");
        return;
    }

    const auto& data_set = reader.GetFile().GetDataSet();

    const auto tag = convert_hex_str_to_tag(tagGroup_, tagElement_);
    LogInfo("tag: " << tag);

    if (data_set.FindDataElement(tag)) {
        const auto& data_element = data_set.GetDataElement(tag);
        if (useSubTag_) {
            const auto& sqi = data_element.GetValueAsSQ();
            if (sqi->GetNumberOfItems() == 1) {
                const auto& item = sqi->GetItem(1);
                const auto& sub_data_set = item.GetNestedDataSet();
                const auto sub_tag = convert_hex_str_to_tag(subTagGroup_, subTagElement_);
                LogInfo("sub-tag: " << sub_tag);
                if (sub_data_set.FindDataElement(sub_tag)) {
                    std::stringstream content;
                    item.GetDataElement(sub_tag).GetValue().Print(content);
                    LogInfo("value: " << content.str());
                } else {
                    LogWarn("no value found for sub-tag: " << sub_tag);
                }
            } else {
                LogWarn("number of items in sequence is not 1");
            }
        } else {
            std::stringstream content;
            data_element.GetValue().Print(content);
            LogInfo("value: " << content.str());
        }
    } else {
        LogWarn("no value found for tag: " << tag);
    }
}

}  // namespace inviwo
