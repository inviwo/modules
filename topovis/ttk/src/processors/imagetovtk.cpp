/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2023 Inviwo Foundation
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

#include <inviwo/ttk/processors/imagetovtk.h>

#include <inviwo/core/datastructures/image/image.h>
#include <inviwo/core/datastructures/image/imageram.h>
#include <inviwo/core/datastructures/image/layer.h>
#include <inviwo/core/datastructures/image/layerram.h>

#include <inviwo/core/util/glm.h>

#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkInformation.h>

namespace inviwo {

namespace {

template <typename T = double, typename U = T>
OrdinalPropertyState<T> transformState(T val = T{1}) {
    return {val,
            util::filled<T>(static_cast<util::value_type_t<T>>(-100)),
            ConstraintBehavior::Ignore,
            util::filled<T>(static_cast<util::value_type_t<T>>(100)),
            ConstraintBehavior::Ignore,
            util::filled<T>(static_cast<util::value_type_t<T>>(0.1)),
            InvalidationLevel::InvalidOutput,
            PropertySemantics::Text};
}

}  // namespace

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo ImageToVTK::processorInfo_{
    "org.inviwo.ImageToVTK",               // Class identifier
    "Image To VTK",                        // Display name
    "VTK",                                 // Category
    CodeState::Experimental,               // Code state
    Tag::CPU | Tag{"VTK"} | Tag{"Image"},  // Tags
    R"(Converts an inviwo Image to a VTKImageData )"_unindentHelp};

const ProcessorInfo ImageToVTK::getProcessorInfo() const { return processorInfo_; }

ImageToVTK::ImageToVTK()
    : Processor{}
    , inport_{"inport"}
    , outport_{"outport"}
    , layer_{"layer",
             "Layer",
             "Select which image layer that should be shown in the canvas. defaults to "
             "the first color layer"_help,
             {{"color", "Color layer", LayerType::Color},
              {"depth", "Depth layer", LayerType::Depth},
              {"picking", "Picking layer", LayerType::Picking}},
             0}
    , layerIndex_{"layerIndex",
                  "Color Layer ID",
                  "Index of the color layer to show in the canvas"_help,
                  0,
                  {0, ConstraintBehavior::Immutable},
                  {0, ConstraintBehavior::Mutable}

      }
    , transform_{"transform", "Transform", transformState<dmat4>()} {

    addPorts(inport_, outport_);
    addProperties(layer_, layerIndex_);

    inport_.onChange([&]() {
        int layers = static_cast<int>(inport_.getData()->getNumberOfColorLayers());
        layerIndex_.setMaxValue(layers - 1);
    });
}

void ImageToVTK::process() {
    auto image = inport_.getData();

    auto layer = image->getLayer(layer_.getSelectedValue(), layerIndex_.get());

    const auto dim = static_cast<ivec2>(layer->getDimensions());

    data_ = vtkSmartPointer<vtkImageData>::New();

    const auto basis = dmat3{transform_.get()};
    const auto offset = dvec3{transform_.get()[3]};

    const dvec3 ddim{dim, 1.0};
    const dvec3 length{glm::length(basis[0]), glm::length(basis[1]), glm::length(basis[2])};
    const dvec3 spacing{length / ddim};
    const dmat3 direction{basis[0] / length[0], basis[1] / length[1], basis[2] / length[2]};

    data_ = vtkSmartPointer<vtkImageData>::New();
    data_->SetDimensions(dim.x, dim.y, 1);
    data_->SetSpacing(spacing.x, spacing.y, spacing.z);
    data_->SetOrigin(glm::value_ptr(offset));
    data_->SetDirectionMatrix(glm::value_ptr(direction));

    const auto vtkType = [&]() {
        switch (layer->getDataFormat()->getId()) {
            case DataFormatId::Int8:
            case DataFormatId::Vec2Int8:
            case DataFormatId::Vec3Int8:
            case DataFormatId::Vec4Int8: {
                return VTK_CHAR;
            }
            case DataFormatId::UInt8:
                [[fallthrough]];
            case DataFormatId::Vec2UInt8:
                [[fallthrough]];
            case DataFormatId::Vec3UInt8:
                [[fallthrough]];
            case DataFormatId::Vec4UInt8: {
                return VTK_UNSIGNED_CHAR;
            }
            case DataFormatId::Int16:
                [[fallthrough]];
            case DataFormatId::Vec2Int16:
                [[fallthrough]];
            case DataFormatId::Vec3Int16:
                [[fallthrough]];
            case DataFormatId::Vec4Int16: {
                return VTK_SHORT;
            }
            case DataFormatId::UInt16:
                [[fallthrough]];
            case DataFormatId::Vec2UInt16:
                [[fallthrough]];
            case DataFormatId::Vec3UInt16:
                [[fallthrough]];
            case DataFormatId::Vec4UInt16: {
                return VTK_UNSIGNED_SHORT;
            }
            case DataFormatId::Int32:
                [[fallthrough]];
            case DataFormatId::Vec2Int32:
                [[fallthrough]];
            case DataFormatId::Vec3Int32:
                [[fallthrough]];
            case DataFormatId::Vec4Int32: {
                return VTK_INT;
            }
            case DataFormatId::UInt32:
                [[fallthrough]];
            case DataFormatId::Vec2UInt32:
                [[fallthrough]];
            case DataFormatId::Vec3UInt32:
                [[fallthrough]];
            case DataFormatId::Vec4UInt32: {
                return VTK_UNSIGNED_INT;
            }
            case DataFormatId::Int64:
                [[fallthrough]];
            case DataFormatId::Vec2Int64:
                [[fallthrough]];
            case DataFormatId::Vec3Int64:
                [[fallthrough]];
            case DataFormatId::Vec4Int64: {
                return VTK_LONG;
            }
            case DataFormatId::UInt64:
                [[fallthrough]];
            case DataFormatId::Vec2UInt64:
                [[fallthrough]];
            case DataFormatId::Vec3UInt64:
                [[fallthrough]];
            case DataFormatId::Vec4UInt64: {
                return VTK_UNSIGNED_LONG;
            }
            case DataFormatId::Float32:
                [[fallthrough]];
            case DataFormatId::Vec2Float32:
                [[fallthrough]];
            case DataFormatId::Vec3Float32:
                [[fallthrough]];
            case DataFormatId::Vec4Float32: {
                return VTK_FLOAT;
            }
            case DataFormatId::Float64:
                [[fallthrough]];
            case DataFormatId::Vec2Float64:
                [[fallthrough]];
            case DataFormatId::Vec3Float64:
                [[fallthrough]];
            case DataFormatId::Vec4Float64: {
                return VTK_DOUBLE;
            }
            default: {
                throw Exception("Cannot map type to VTK.", IVW_CONTEXT_CUSTOM("VolumeToVtk"));
            }
        }
    }();

    const auto nComp = layer->getDataFormat()->getComponents();
    data_->AllocateScalars(vtkType, static_cast<int>(nComp));
    void* dst = data_->GetScalarPointer();
    const void* src = layer->getRepresentation<LayerRAM>()->getData();
    std::memcpy(dst, src, glm::compMul(dim) * layer->getDataFormat()->getSize());

    outport_.setData(data_);
}

}  // namespace inviwo
