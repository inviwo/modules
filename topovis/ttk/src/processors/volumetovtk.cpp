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

#include <inviwo/ttk/processors/volumetovtk.h>
#include <inviwo/core/datastructures/volume/volumeramprecision.h>

#include <vtkImageData.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VolumeToVTK::processorInfo_{
    "org.inviwo.VolumeToVTK",  // Class identifier
    "Volume To VTK",           // Display name
    "VTK",                     // Category
    CodeState::Experimental,   // Code state
    Tags::CPU,                 // Tags
};
const ProcessorInfo VolumeToVTK::getProcessorInfo() const { return processorInfo_; }

VolumeToVTK::VolumeToVTK() : Processor(), inport_("inport"), outport_("outport", "vtkImageData") {

    addPorts(inport_, outport_);
}

void VolumeToVTK::process() {
    auto volume = inport_.getData();

    // data_.reset(vtkImageData::New());

    // auto data = vtkSmartPointer<vtkImageData>::New();
    data_ = vtkSmartPointer<vtkImageData>::New();

    auto dim = volume->getDimensions();

    data_->SetDimensions(dim.x, dim.y, dim.z);

    data_->SetOrigin(0, 0, 0);
    // data_->SetSpacing(spacing); // Not needed?

    const auto vtkType = [&]() {
        switch (volume->getDataFormat()->getId()) {
            case DataFormatId::Int8: {
                return VTK_CHAR;  // Or VTK_SIGNED_CHAR??
            }
            case DataFormatId::UInt8: {
                return VTK_UNSIGNED_CHAR;
            }
            case DataFormatId::Int16: {
                return VTK_SHORT;
            }
            case DataFormatId::UInt16: {
                return VTK_UNSIGNED_SHORT;
            }
            case DataFormatId::Int32: {
                return VTK_INT;
            }
            case DataFormatId::UInt32: {
                return VTK_UNSIGNED_INT;
            }
            case DataFormatId::Int64: {
                return VTK_LONG;
            }
            case DataFormatId::UInt64: {
                return VTK_UNSIGNED_LONG;
            }
            case DataFormatId::Float32: {
                return VTK_FLOAT;
            }
            case DataFormatId::Float64: {
                return VTK_DOUBLE;
            }
            default: {
                throw Exception("Cannot map type to VTK.", IVW_CONTEXT_CUSTOM("VolumeToVtk"));
            }

			// Need to handle eg Vec3Float32 by setting extent in VTK??

                // switch (volume->getDataFormat()->getNumericType()) {
                //     case NumericType::Float: {
                //         if (volume->getDataFormat()->getPrecision() == 32) {
                //             return VTK_FLOAT;
                //         } else if (volume->getDataFormat()->getPrecision() == 64) {
                //             return VTK_DOUBLE;
                //         } else {
                //             throw Exception("Cannot map float with this precistion to VTK.",
                //                             IVW_CONTEXT_CUSTOM("VolumeToVtk"));
                //         }
                //     }
                //     case NumericType::UnsignedInteger: {
                //         auto id = volume->getDataFormat()->getId();
                //         if (id == DataFormatId::UInt8) {
                //             return VTK_UNSIGNED_CHAR;
                //         } else if (id == DataFormatId::UInt32) {
                //             return VTK_UNSIGNED_INT;
                //         } else if (id == DataFormatId::UInt64) {
                //             return VTK_UNSIGNED_LONG;
                //         } else if (id == DataFormatId::UInt64) {
                //             return VTK_UNSIGNED_LONG;
                //         }
                //     }
                //     case NumericType::SignedInteger: {
                //         auto id = volume->getDataFormat()->getId();
                //         if (id == DataFormatId::Int8) {
                //             return VTK_SIGNED_CHAR;  // Or VTK_CHAR?
                //         } else if (id == DataFormatId::Int16) {
                //             return VTK_SHORT;
                //         } else if (id == DataFormatId::Int32) {
                //             return VTK_INT;
                //         } else if (id == DataFormatId::Int64) {
                //             return VTK_LONG;
                //         }
                //     }
                //     case NumericType::NotSpecialized: {
                //         throw Exception("Cannot map NotSpecialized numeric type to VTK.",
                //                         IVW_CONTEXT_CUSTOM("VolumeToVtk"));
                //     }
                //     default:
                //         throw Exception("Cannot map type to VTK.",
                //         IVW_CONTEXT_CUSTOM("VolumeToVtk"));
        }
    }();

    data_->AllocateScalars(vtkType, volume->getDataFormat()->getComponents());
    void* dest = data_->GetScalarPointer();
    const void* src = volume->getRepresentation<VolumeRAM>()->getData();
    std::memcpy(dest, src, glm::compMul(dim) * volume->getDataFormat()->getSize());

    /*
    volume->getRepresentation<VolumeRAM>()->dispatch<void>([](auto* repr) {
        using VolumeType = util::PrecisionType<decltype(repr)>;
        using ValueType = util::PrecisionValueType<decltype(repr)>;


        // Need to go through all voxels?
        // util::forEachVoxelParallel(....)
    });
    */

    outport_.setData(data_);
}
}  // namespace inviwo
