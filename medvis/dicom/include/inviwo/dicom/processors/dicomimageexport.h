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

#pragma once

#include <inviwo/dicom/dicommoduledefine.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/ports/imageport.h>

#include <vector>
#include <thread>
#include <mutex>
#include <deque>
#include <condition_variable>

namespace inviwo {

/** \docpage{org.inviwo.dicomimageexport, dicomimageexport}
 * ![](org.inviwo.dicomimageexport.png?classIdentifier=org.inviwo.dicomimageexport)
 * Explanation of how to use the processor.
 *
 * ### Inports
 *   * __<Inport1>__ <description>.
 *
 * ### Outports
 *   * __<Outport1>__ <description>.
 *
 * ### Properties
 *   * __<Prop1>__ <description>.
 *   * __<Prop2>__ <description>
 */
class IVW_MODULE_DICOM_API DICOMImageExport : public Processor {
public:
    DICOMImageExport();
    virtual ~DICOMImageExport();

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

    struct DataFrame {
        std::shared_ptr<const Image> img;
        std::string filename;

        DataFrame() = delete;
        DataFrame(std::shared_ptr<const Image> img, const std::string& filename)
            : img(img), filename(filename) {}
    };
    /*
    gdcm::Attribute<0x0028, 0x0010> attr_image_columns = {};
    gdcm::Attribute<0x0028, 0x0011> attr_image_rows = {};
    gdcm::Attribute<0x0018, 0x1510> attr_primary_angulation = {};
    gdcm::Attribute<0x0018, 0x1511> attr_secondary_angulation = {};
    gdcm::Attribute<0x2003, 0x102E> attr_table_pos_vertical = {};      // subtag: 0x300A, 0x0128
    gdcm::Attribute<0x2003, 0x102E> attr_table_pos_longitudinal = {};  // subtag: 0x300A, 0x0129
    gdcm::Attribute<0x2003, 0x102E> attr_table_pos_lateral = {};       // subtag: 0x300A, 0x012A
    gdcm::Attribute<0x0018, 0x1110> attr_source_to_image_distance = {};
    gdcm::Attribute<0x2003, 0x1003> attr_zoom_field = {};  // or: 0x2003, 0x1010
    gdcm::Attribute<0x0018, 0x1164> attr_pixel_spacing = {};
    gdcm::Attribute<0x0018, 0x1111> attr_source_to_patient_distance = {};
    gdcm::Attribute<0x0018, 0x0040> attr_cine_frame_rate = {};
    gdcm::Attribute<0x5000, 0x0114> attr_ecg_frame_rate = {};
    gdcm::Attribute<0x5000, 0x0010> attr_ecg_number_of_points = {};
    gdcm::Attribute<0x5000, 0x3000> attr_ecg_signal = {};
    */


private:
    ImageInport inport_;

    std::vector<std::thread> workers_;
    std::mutex dequeMutex_;
    std::mutex consoleMutex_;
    std::condition_variable condVar_;
    std::deque<DataFrame> dataElements_;
    bool noNewData_;

    size_t fileID_;
};

}  // namespace inviwo
