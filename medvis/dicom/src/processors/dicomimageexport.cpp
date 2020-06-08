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
#include <inviwo/core/util/rendercontext.h>
#include <modules/opengl/texture/texture2d.h>

#include <string>
#include <memory>

#include "gdcmWriter.h"
#include "gdcmReader.h"
#include "gdcmDataElement.h"
#include "gdcmDataSet.h"
#include "gdcmTag.h"
#include "gdcmFile.h"
#include "gdcmFileMetaInformation.h"
#include "gdcmString.h"
#include "gdcmVL.h"
#include "gdcmVR.h"
#include "gdcmValue.h"
#include "gdcmAttribute.h"
#include "gdcmUIDs.h"
#include "MediaStorageAndFileFormat/gdcmUIDGenerator.h"

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo DICOMImageExport::processorInfo_{
    "org.inviwo.dicomimageexport",  // Class identifier
    "DICOM Image Export",           // Display name
    "Data Output",                  // Category
    CodeState::Experimental,        // Code state
    Tags::CPU,                      // Tags
};
const ProcessorInfo DICOMImageExport::getProcessorInfo() const { return processorInfo_; }

void WriteDataFrame(std::mutex& queue_mutex, std::mutex& console_mutex,
                    std::condition_variable& cond_var,
                    std::deque<DICOMImageExport::DataFrame>& data_elements, bool& no_new_data) {

    // make sure to share OpenGL resources across multiple contexts
    RenderContext::getPtr()->activateLocalRenderContext();

    while (true) {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (cond_var.wait_for(lock, std::chrono::milliseconds(100),
                              [&data_elements] { return !data_elements.empty(); })) {
            // fetch data
            const auto blob = data_elements.front();
            data_elements.pop_front();
            // release after data blob is fetched, so main thread can continue to push data
            lock.unlock();

            // work on data
            const auto filename = blob.filename.c_str();
            const auto& layer = blob.layer;
            const auto& tex = layer->getTexture();
            //const auto& pixel_data = static_cast<const char*>(layer_ram->getData());
            const auto dims = blob.layer->getDimensions();
            const auto num_elements = glm::compMul(dims);
            const auto data_format = layer->getDataFormat();
            const auto num_channels = data_format->getComponents();
            const auto bpp = data_format->getSize();
            const auto bpc = bpp / num_channels;
            const bool flip_pixel_data_x{false};
            const bool flip_pixel_data_y{true};

            gdcm::Writer writer;
            writer.SetFileName(filename);
            auto& file = writer.GetFile();
            auto& data_set = file.GetDataSet();
            auto& meta_info = file.GetHeader();
            meta_info.SetDataSetTransferSyntax(gdcm::TransferSyntax::ImplicitVRLittleEndian);

            // generate UIDs
            gdcm::UIDGenerator uid_generator;
            const auto sop_instance_uid = uid_generator.Generate();
            const auto sop_class_uid =
                gdcm::UIDs::GetUIDString(gdcm::UIDs::XRayAngiographicImageStorage);
            const auto study_instance_uid = uid_generator.Generate();
            const auto series_instance_uid = uid_generator.Generate();

            // extract smallest and largest pixel values
            const uint16_t smallest_image_pixel_value{0};
            const uint16_t largest_image_pixel_value{255};
            const uint16_t smallest_pixel_value_in_series{0};
            const uint16_t largest_pixel_value_in_series{255};

            // generate study date and time
            const auto t = std::time(nullptr);
            const auto tm = *std::localtime(&t);
            std::stringstream oss;
            oss << std::put_time(&tm, "%Y%m%d");
            const auto study_date = oss.str();
            oss.str("");
            oss << std::put_time(&tm, "%H%M%S");
            const auto study_time = oss.str();

            // ToDo: check for dates and times to be set, also how precise should it be
            const auto instance_creation_date = study_date;
            const auto instance_creation_time = study_time;

            // meta data
            gdcm::Attribute<0x0002, 0x0002> attr_media_storage_sop_class_uid = {sop_class_uid};
            gdcm::Attribute<0x0002, 0x0003> attr_media_storage_sop_instance_uid = {
                sop_instance_uid};

            // data set
            // general attributes
            // ToDo: check why direct assignment does not work for "image type"
            gdcm::Attribute<0x0008, 0x0008> attr_image_type;
            attr_image_type.SetNumberOfValues(3);
            attr_image_type.SetValue(0, gdcm::CSComp{"DERIVED"});
            attr_image_type.SetValue(1, gdcm::CSComp{"SECONDARY"});
            attr_image_type.SetValue(2, gdcm::CSComp{"OTHER"});
            gdcm::Attribute<0x0008, 0x0016> attr_sop_class_uid = {sop_class_uid};
            gdcm::Attribute<0x0008, 0x0018> attr_sop_instance_uid = {sop_instance_uid};
            gdcm::Attribute<0x0008, 0x0012> attr_instance_creation_date = {instance_creation_date};
            gdcm::Attribute<0x0008, 0x0013> attr_instance_creation_time = {instance_creation_time};
            gdcm::Attribute<0x0008, 0x0020> attr_study_date = {study_date};
            gdcm::Attribute<0x0008, 0x0030> attr_study_time = {study_time};
            gdcm::Attribute<0x0018, 0x1510> attr_primary_angulation = {0.0};
            gdcm::Attribute<0x0018, 0x1511> attr_secondary_angulation = {0.0};
            gdcm::Attribute<0x0008, 0x0050> attr_accession_number = {""};
            gdcm::Attribute<0x0008, 0x0060> attr_modality = {"XA"};
            gdcm::Attribute<0x0008, 0x0070> attr_manufacturer = {""};
            gdcm::Attribute<0x0008, 0x0090> attr_ref_phys_name = {""};
            gdcm::Attribute<0x0010, 0x0010> attr_patient_name = {""};
            gdcm::Attribute<0x0010, 0x0020> attr_patient_id = {""};
            gdcm::Attribute<0x0010, 0x0030> attr_patient_dob = {""};
            gdcm::Attribute<0x0010, 0x0040> attr_patient_sex = {""};
            gdcm::Attribute<0x0018, 0x0060> attr_kvp = {0.0};
            gdcm::Attribute<0x0018, 0x1110> attr_distance_source_to_detector = {0.0};
            gdcm::Attribute<0x0018, 0x1111> attr_distance_source_to_patient = {0.0};
            gdcm::Attribute<0x0020, 0x000d> attr_study_instance_uid = {study_instance_uid};
            gdcm::Attribute<0x0020, 0x000e> attr_series_instance_uid = {series_instance_uid};
            gdcm::Attribute<0x0020, 0x0010> attr_study_id = {"0"};
            gdcm::Attribute<0x0020, 0x0011> attr_series_number = {static_cast<int32_t>(0)};
            gdcm::Attribute<0x0020, 0x0012> attr_acquisition_number = {static_cast<int32_t>(0)};
            gdcm::Attribute<0x0020, 0x1040> attr_position_reference_indicator = {"0"};
            // ToDo: assert num_channels == 1 for XA images
            gdcm::Attribute<0x0028, 0x0002> attr_samples_per_pixel = {
                static_cast<uint16_t>(num_channels)};
            gdcm::Attribute<0x0028, 0x0004> attr_photometric_interpretation = {"MONOCHROME2"};
            gdcm::Attribute<0x0028, 0x0010> attr_rows = {static_cast<uint16_t>(dims.y)};
            gdcm::Attribute<0x0028, 0x0011> attr_columns = {static_cast<uint16_t>(dims.x)};
            gdcm::Attribute<0x0028, 0x0100> attr_bits_allocated = {static_cast<uint16_t>(bpp * 8)};
            gdcm::Attribute<0x0028, 0x0101> attr_bits_stored = {static_cast<uint16_t>(bpp * 8)};
            gdcm::Attribute<0x0028, 0x0102> attr_high_bit = {static_cast<uint16_t>(bpp * 8 - 1)};
            gdcm::Attribute<0x0028, 0x0103> attr_pixel_representation = {static_cast<uint16_t>(0)};
            gdcm::Attribute<0x0028, 0x1052> attr_rescale_intercept = {0.0};
            gdcm::Attribute<0x0028, 0x1053> attr_rescale_slope = {1.0};
            /*gdcm::Attribute<0x0028, 0x0034> attr_pixel_aspect_ratio = {static_cast<uint16_t>(1),
            static_cast<uint16_t>(1)};*/

            // manually build misc. data elements
            gdcm::DataElement datel_smallest_image_pixel_value(gdcm::Tag(0x0028, 0x0106));
            datel_smallest_image_pixel_value.SetVR(gdcm::VR::US);
            datel_smallest_image_pixel_value.SetByteValue(
                reinterpret_cast<const char*>(&smallest_image_pixel_value),
                sizeof(decltype(smallest_image_pixel_value)));

            gdcm::DataElement datel_largest_image_pixel_value(gdcm::Tag(0x0028, 0x0107));
            datel_largest_image_pixel_value.SetVR(gdcm::VR::US);
            datel_largest_image_pixel_value.SetByteValue(
                reinterpret_cast<const char*>(&largest_image_pixel_value),
                sizeof(decltype(largest_image_pixel_value)));

            gdcm::DataElement datel_smallest_pixel_value_in_series(gdcm::Tag(0x0028, 0x0108));
            datel_smallest_pixel_value_in_series.SetVR(gdcm::VR::US);
            datel_smallest_pixel_value_in_series.SetByteValue(
                reinterpret_cast<const char*>(&smallest_pixel_value_in_series),
                sizeof(decltype(smallest_pixel_value_in_series)));

            gdcm::DataElement datel_largest_pixel_value_in_series(gdcm::Tag(0x0028, 0x0109));
            datel_largest_pixel_value_in_series.SetVR(gdcm::VR::US);
            datel_largest_pixel_value_in_series.SetByteValue(
                reinterpret_cast<const char*>(&largest_pixel_value_in_series),
                sizeof(decltype(largest_pixel_value_in_series)));

            // setup pixel data
            // download texture
            std::vector<char> pixel_data(num_elements * bpp);
            tex->download(pixel_data.data());
            // flip pixel data around y-axis
            std::vector<char> flipped_pixel_data(pixel_data.size());
            for (size_t y = 0; y < dims.y; ++y) {
                for (size_t x = 0; x < dims.x; ++x) {
                    const auto x_from = flip_pixel_data_x ? dims.x - x - 1 : x;
                    const auto y_from = flip_pixel_data_y ? dims.y - y - 1 : y;
                    // multiply with bpp to ensure all channels are copied
                    const auto idx_from = (x_from + y_from * dims.x) * bpp;
                    const auto idx_to = (x + y * dims.x) * bpp;
                    // copy all bytes from each pixel
                    for (size_t i = 0; i < bpp; ++i) {
                        flipped_pixel_data[idx_to + i] = pixel_data[idx_from + i];
                    }
                }
            }
            gdcm::DataElement datel_pixel_data(gdcm::Tag(0x7fe0, 0x0010));
            datel_pixel_data.SetByteValue(flipped_pixel_data.data(),
                                          gdcm::VL(static_cast<uint32_t>(num_elements)));
            datel_pixel_data.SetVR(gdcm::VR::OB);

            // add all attributes to meta info
            meta_info.Insert(attr_media_storage_sop_class_uid.GetAsDataElement());
            meta_info.Insert(attr_media_storage_sop_instance_uid.GetAsDataElement());

            // add all attributes to data set
            data_set.Insert(attr_image_type.GetAsDataElement());
            data_set.Insert(attr_sop_class_uid.GetAsDataElement());
            data_set.Insert(attr_sop_instance_uid.GetAsDataElement());
            data_set.Insert(attr_instance_creation_date.GetAsDataElement());
            data_set.Insert(attr_instance_creation_time.GetAsDataElement());
            data_set.Insert(attr_study_date.GetAsDataElement());
            data_set.Insert(attr_study_time.GetAsDataElement());
            data_set.Insert(attr_primary_angulation.GetAsDataElement());
            data_set.Insert(attr_secondary_angulation.GetAsDataElement());
            data_set.Insert(attr_accession_number.GetAsDataElement());
            data_set.Insert(attr_modality.GetAsDataElement());
            data_set.Insert(attr_manufacturer.GetAsDataElement());
            data_set.Insert(attr_ref_phys_name.GetAsDataElement());
            data_set.Insert(attr_patient_name.GetAsDataElement());
            data_set.Insert(attr_patient_id.GetAsDataElement());
            data_set.Insert(attr_patient_dob.GetAsDataElement());
            data_set.Insert(attr_patient_sex.GetAsDataElement());
            data_set.Insert(attr_kvp.GetAsDataElement());
            data_set.Insert(attr_distance_source_to_detector.GetAsDataElement());
            data_set.Insert(attr_distance_source_to_patient.GetAsDataElement());
            data_set.Insert(attr_study_instance_uid.GetAsDataElement());
            data_set.Insert(attr_series_instance_uid.GetAsDataElement());
            data_set.Insert(attr_study_id.GetAsDataElement());
            data_set.Insert(attr_series_number.GetAsDataElement());
            data_set.Insert(attr_acquisition_number.GetAsDataElement());
            data_set.Insert(attr_position_reference_indicator.GetAsDataElement());
            data_set.Insert(attr_samples_per_pixel.GetAsDataElement());
            data_set.Insert(attr_photometric_interpretation.GetAsDataElement());
            data_set.Insert(attr_rows.GetAsDataElement());
            data_set.Insert(attr_columns.GetAsDataElement());
            data_set.Insert(attr_bits_allocated.GetAsDataElement());
            data_set.Insert(attr_bits_stored.GetAsDataElement());
            data_set.Insert(attr_high_bit.GetAsDataElement());
            data_set.Insert(attr_pixel_representation.GetAsDataElement());
            data_set.Insert(attr_rescale_intercept.GetAsDataElement());
            data_set.Insert(attr_rescale_slope.GetAsDataElement());
            // add all data elements to data set
            data_set.Insert(datel_smallest_image_pixel_value);
            data_set.Insert(datel_largest_image_pixel_value);
            data_set.Insert(datel_smallest_pixel_value_in_series);
            data_set.Insert(datel_largest_pixel_value_in_series);
            data_set.Insert(datel_pixel_data);

            // print data set and meta info
            /*if (printDataSetInfo_) {
                std::cout << "meta info:" << std::endl;
                meta_info.Print(std::cout, " ");
                std::cout << "data set:" << std::endl;
                data_set.Print(std::cout, " ");
            }*/

            // write file
            // writer.CheckFileMetaInformationOff();
            if (!writer.Write()) {
                std::lock_guard<std::mutex> guard(console_mutex);
                std::cout << "could not write: " << filename << '\n';
            }

        } else {
            // check if thread can be closed
            if (no_new_data && data_elements.empty()) {
                std::lock_guard<std::mutex> guard(console_mutex);
                std::cout << "returning worker: " << std::this_thread::get_id() << '\n';
                break;
            }
        }
    }
}

DICOMImageExport::DICOMImageExport()
    : Processor(), inport_("inport", true), noNewData_{false}, fileID_{0} {

    addPort(inport_);

    const size_t num_threads{4};
    workers_.reserve(num_threads);
    for (size_t idx = 0; idx < num_threads; ++idx) {
        // adding worker
        workers_.emplace_back(WriteDataFrame, std::ref(dequeMutex_), std::ref(consoleMutex_),
                              std::ref(condVar_), std::ref(dataElements_), std::ref(noNewData_));
        {
            std::lock_guard<std::mutex> guard(consoleMutex_);
            std::cout << "created worker: " << workers_.back().get_id() << '\n';
        }
    }
}

DICOMImageExport::~DICOMImageExport() {
    {
        // signal that all data has been pushed
        std::lock_guard<std::mutex> lock(dequeMutex_);
        noNewData_ = true;
    }
    // wake all workers up to finish all remaining data elements or return if done
    condVar_.notify_all();

    // join workers
    for (auto& worker : workers_) {
        const auto id = worker.get_id();
        worker.join();
        {
            std::lock_guard<std::mutex> guard(consoleMutex_);
            std::cout << "joined worker: " << id << '\n';
        }
    }
}

void DICOMImageExport::process() {
    const auto t0 = std::chrono::steady_clock::now();

    const std::string filename =
        "D:/uni/transnav/data/tmp/tmp_img_" + std::to_string(fileID_) + ".dcm";
    fileID_++;

    {
        // add new data frame
        std::lock_guard<std::mutex> guard(dequeMutex_);
        // clone layer to be able to transfer it to another thread, GPU buffers won't work here
        // ToDo: find a fix for that?
        dataElements_.emplace_back(
            std::shared_ptr<LayerGL>(
                inport_.getData()->getColorLayer(0)->getRepresentation<LayerGL>()->clone()),
            filename);
    }
    // signale all workers that new data is available
    condVar_.notify_all();

    const auto t1 = std::chrono::steady_clock::now();
    LogInfo("e: " << std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count());
}

}  // namespace inviwo
