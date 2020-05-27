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

#include <inviwo/dicom/processors/dicomimageexport.h>
#include <inviwo/core/datastructures/image/layerram.h>

#include <string>

#include "gdcmWriter.h"
#include "gdcmDataElement.h"
#include "gdcmDataSet.h"
#include "gdcmTag.h"
#include "gdcmFile.h"
#include "gdcmFileMetaInformation.h"
#include "gdcmString.h"
#include "gdcmVL.h"
#include "gdcmVR.h"
#include "gdcmValue.h"

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo DICOMImageExport::processorInfo_{
    "org.inviwo.dicomimageexport",  // Class identifier
    "DICOM Image Export",           // Display name
    "Data Output",                  // Category
    CodeState::Experimental,        // Code state
    Tags::None,                     // Tags
};
const ProcessorInfo DICOMImageExport::getProcessorInfo() const { return processorInfo_; }

DICOMImageExport::DICOMImageExport()
    : Processor(), inport_("inport", true), prop_("prop", "Prop", 0.0f, -100.0f, 100.0f) {

    addPort(inport_);
    addProperties(prop_);
}

void DICOMImageExport::process() {
    const auto& img = inport_.getData();
    const auto& color_layer_ram = img->getColorLayer(0)->getRepresentation<LayerRAM>();
    const auto color_data = static_cast<const char*>(color_layer_ram->getData());
    const auto dims = ivec2(img->getDimensions());
    const auto num_elements = glm::compMul(dims);
    const auto element_size = color_layer_ram->getDataFormat()->getSize();

    LogInfo("dimensions:   " << dims);
    LogInfo("num_elements: " << num_elements);
    LogInfo("element_size: " << element_size);

    gdcm::String filename{"D:/tmp_img.dcm"};
    gdcm::Writer writer;
    gdcm::File file;
    gdcm::DataSet data_set;
    gdcm::FileMetaInformation meta_info;

    const std::string sop_instance_uid_str{"1.2.3.4.5.6.7.8.9.0"};
    gdcm::DataElement sop_instance_uid(gdcm::Tag(0x0008, 0x0018));
    sop_instance_uid.SetVR(gdcm::VR::UI);
    sop_instance_uid.SetByteValue(sop_instance_uid_str.c_str(), sop_instance_uid_str.length());

    gdcm::DataElement pixel_data(gdcm::Tag(0x7fe0, 0x0010));
    pixel_data.SetByteValue(color_data, num_elements);
    pixel_data.SetVR(gdcm::VR::OB_OW);

    gdcm::DataElement image_columns(gdcm::Tag(0x0028, 0x0010));
    image_columns.SetByteValue(reinterpret_cast<const char*>(&dims.x), gdcm::VL(sizeof(dims.x)));
    image_columns.SetVR(gdcm::VR::US);

    gdcm::DataElement image_rows(gdcm::Tag(0x0028, 0x0011));
    image_rows.SetByteValue(reinterpret_cast<const char*>(&dims.y), gdcm::VL(sizeof(dims.y)));
    image_rows.SetVR(gdcm::VR::US);

    data_set.Insert(pixel_data);
    data_set.Insert(image_columns);
    data_set.Insert(image_rows);
    data_set.Insert(sop_instance_uid);

    meta_info.SetDataSetTransferSyntax(gdcm::TransferSyntax::ImplicitVRLittleEndian);
    file.SetHeader(meta_info);
    file.SetDataSet(data_set);

    writer.SetFileName(filename);
    writer.SetFile(file);
    writer.CheckFileMetaInformationOff();
    writer.Write();
}

}  // namespace inviwo
