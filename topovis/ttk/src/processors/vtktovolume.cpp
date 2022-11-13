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

#include <inviwo/ttk/processors/vtktovolume.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/core/network/processornetwork.h>
#include <inviwo/core/util/glm.h>
#include <inviwo/core/util/formats.h>
#include <inviwo/core/util/utilities.h>

#include <inviwo/ttk/arrayutils.h>

#include <regex>

#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkInformation.h>
#include <vtkMatrix3x3.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VTKToVolume::processorInfo_{
    "org.inviwo.VTKToVolume",  // Class identifier
    "VTK To Volume",           // Display name
    "VTK",                     // Category
    CodeState::Experimental,   // Code state
    Tags::CPU,                 // Tags
};
const ProcessorInfo VTKToVolume::getProcessorInfo() const { return processorInfo_; }

VTKToVolume::VTKToVolume()
    : Processor()
    , inport_("inport", "vtkImageData")
    , outport_("outport")
    , source_{"source", "Source"}
    , precision_{"precision",
                 "Output Precision",
                 {{"input", "Same As Input", 0},
                  {"low", "8 bit", 8},
                  {"medium", "16 bit", 16},
                  {"heigh", "32 bit", 32},
                  {"full", "64 bit", 64}},
                 0}
    , information_("Information", "Data information") {
    addPorts(inport_, outport_);

    addProperties(source_, precision_, information_);
}

void VTKToVolume::updateSources(vtkDataSet* data) {
    std::vector<OptionPropertyOption<int>> opts;

    if (data) {
        auto arrays = data->GetPointData();
        const auto nArrays = arrays->GetNumberOfArrays();
        for (int i = 0; i < nArrays; ++i) {
            auto array = arrays->GetAbstractArray(i);
            opts.emplace_back(
                util::stripIdentifier(array->GetName()),
                fmt::format("{} ({}D {})", array->GetName(), array->GetNumberOfComponents(),
                            array->GetDataTypeAsString()),
                i);
        }
    }

    source_.replaceOptions(opts);
}

template <bool FloatingPoint, bool Signed, size_t Precision>
constexpr auto typemap() {
    if constexpr (FloatingPoint) {
        if constexpr (Precision == 64) {
            return double{};
        } else {
            return float();
        }
    } else {
        if constexpr (Signed) {
            if constexpr (Precision == 8) {
                return char{};
            } else if constexpr (Precision == 16) {
                return short{};
            } else if constexpr (Precision == 32) {
                return int{};
            } else if constexpr (Precision == 64) {
                long long val{};
                return val;
            }
        } else {
            return std::make_unsigned_t<decltype(typemap<false, true, Precision>())>();
        }
    }
}

template <size_t Precision, typename Src>
constexpr auto changePrecision(Src) {
    using SrcE = util::glmtype_t<Src>;
    using DstE =
        decltype(typemap<std::is_floating_point_v<SrcE>, std::is_signed_v<SrcE>, Precision>());
    using Dst = util::same_extent_t<Src, DstE>;
    return Dst{};
}

void VTKToVolume::process() {
    auto data = inport_.getData();
    auto vtkImg = vtkImageData::SafeDownCast(data);

    if (inport_.isChanged()) {
        updateSources(vtkImg);
    }

    const ivec3 dim = glm::make_vec3(vtkImg->GetDimensions());

    if (source_.size() == 0) {
        outport_.detachData();
        return;
    }

    if (inport_.isChanged() || source_.isModified() || precision_.isModified()) {

        auto array = vtkImg->GetPointData()->GetArray(source_.getSelectedValue());
        auto ram = [&]() {
            if (precision_.get() == 8) {
                return vtk::arrayToVolumeRAM(static_cast<size3_t>(dim), array, [](auto item) {
                    using Dst = decltype(changePrecision<8>(item));
                    return static_cast<Dst>(item);
                });
            } else if (precision_.get() == 16) {
                return vtk::arrayToVolumeRAM(static_cast<size3_t>(dim), array, [](auto item) {
                    using Dst = decltype(changePrecision<16>(item));
                    return static_cast<Dst>(item);
                });
            } else if (precision_.get() == 32) {
                return vtk::arrayToVolumeRAM(static_cast<size3_t>(dim), array, [](auto item) {
                    using Dst = decltype(changePrecision<32>(item));
                    return static_cast<Dst>(item);
                });
            } else if (precision_.get() == 64) {
                return vtk::arrayToVolumeRAM(static_cast<size3_t>(dim), array, [](auto item) {
                    using Dst = decltype(changePrecision<64>(item));
                    return static_cast<Dst>(item);
                });
            } else {
                return vtk::arrayToVolumeRAM(static_cast<size3_t>(dim), array,
                                             [](auto item) { return item; });
            }
        }();

        if (ram->getDataFormatId() == DataFormatId::NotSpecialized) {
            throw Exception(IVW_CONTEXT,
                            "No valid inviwo DataFormat found for '{}' with '{}' components",
                            array->GetDataTypeAsString(), array->GetNumberOfComponents());
        }

        volume_ = std::make_shared<Volume>(ram);

        const dvec3 offset = glm::make_vec3(vtkImg->GetOrigin());
        const dmat3 basis = glm::make_mat3x3(vtkImg->GetDirectionMatrix()->GetData());

        volume_->setBasis(static_cast<mat3>(basis));
        volume_->setOffset(static_cast<vec3>(offset));

        vtkInformation* info = array->GetInformation();
        if (info->Has(vtkDataArray::UNITS_LABEL())) {
            std::regex re{R"((.*)\[(.*)\])"};
            std::smatch m;
            auto label = std::string{info->Get(vtkDataArray::UNITS_LABEL())};
            if (std::regex_match(label, m, re)) {
                volume_->dataMap_.valueAxis.name = util::trim(m.str(1));
                volume_->dataMap_.valueAxis.unit = units::unit_from_string(m.str(2));
            }
        }
        if (info->Has(vtkDataArray::COMPONENT_RANGE())) {
            auto* range = info->Get(vtkDataArray::COMPONENT_RANGE());
            volume_->dataMap_.dataRange[0] = range[0];
            volume_->dataMap_.dataRange[1] = range[1];
            volume_->dataMap_.valueRange = volume_->dataMap_.dataRange;
        } else {
            auto* range = array->GetRange();
            volume_->dataMap_.dataRange[0] = range[0];
            volume_->dataMap_.dataRange[1] = range[1];
            volume_->dataMap_.valueRange = volume_->dataMap_.dataRange;
        }

        information_.updateForNewVolume(*volume_, getNetwork()->isDeserializing()
                                                      ? util::OverwriteState::Yes
                                                      : util::OverwriteState::No);
    }

    information_.updateVolume(*volume_);

    outport_.setData(volume_);
}

}  // namespace inviwo
