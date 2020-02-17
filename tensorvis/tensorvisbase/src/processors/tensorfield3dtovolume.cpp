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

#include <inviwo/tensorvisbase/processors/tensorfield3dtovolume.h>
#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/core/datastructures/volume/volumeramprecision.h>
#include <inviwo/core/util/volumeramutils.h>
#include <modules/base/algorithm/dataminmax.h>
#include <inviwo/tensorvisbase/util/tensorutil.h>
#include <inviwo/core/util/constexprhash.h>
#include <glm/gtx/component_wise.hpp>
#include <inviwo/core/network/networklock.h>
#include <inviwo/tensorvisbase/tensorvisbasemodule.h>

namespace inviwo {
namespace {

struct AddOptions {
    AddOptions() = delete;
    AddOptions(std::vector<OptionPropertyOption<size_t>>& options,
               std::shared_ptr<const TensorField3D> tensorField)
        : options_(options), tensorField_(tensorField) {}

    template <typename T>
    void operator()() {
        if (!tensorField_->hasMetaData<T>()) return;

        const auto name = std::string(T::identifier);
        auto identifier = name;
        const auto id = util::constexpr_hash(T::identifier);
        replaceInString(identifier, " ", "");

        options_.emplace_back(identifier, name, id);
    }

private:
    std::vector<OptionPropertyOption<size_t>>& options_;
    std::shared_ptr<const TensorField3D> tensorField_;
};

struct IsVec {
    IsVec() = delete;
    IsVec(Processor* p, size_t id) : p_(p), id_(id) {}

    template <typename T>
    void operator()() {
        if (id_ == util::constexpr_hash(T::identifier)) {
            if constexpr (util::extent<T>::value == 1) {
                p_->getPropertiesByType<BoolProperty>().front()->setVisible(false);
            } else {
                p_->getPropertiesByType<BoolProperty>().front()->setVisible(true);
            }
        }
    }

private:
    Processor* p_;
    size_t id_;
};

struct CreateVolume {
    CreateVolume() = delete;
    CreateVolume(size_t id, std::shared_ptr<const TensorField3D> tensorField, Processor* p)
        : id_(id), tensorField_(tensorField), p_(p) {}

    template <typename T>
    void operator()() {
        using type = typename T::value_type;

        if (id_ == util::constexpr_hash(T::identifier)) {
            auto dataFormat = new DataFormat<type>();
            auto dimensions = tensorField_->getDimensions();
            volume_ = std::make_shared<Volume>(dimensions, dataFormat);
            auto data = static_cast<VolumeRAMPrecision<type>*>(
                            volume_->getEditableRepresentation<VolumeRAM>())
                            ->getDataTyped();

            auto metaData = tensorField_->metaData();
            auto col_data = std::dynamic_pointer_cast<const TemplateColumn<type>>(
                                metaData->getColumn(std::string(T::identifier)))
                                ->getTypedBuffer()
                                ->getRAMRepresentation()
                                ->getDataContainer();

            std::memcpy(data, col_data.data(), sizeof(type) * glm::compMul(dimensions));

            if constexpr (util::extent<type>::value == 1) {
                auto [min, max] =
                    std::minmax_element(col_data.begin(), col_data.end(),
                                        [](const auto a, const auto b) -> bool { return a < b; });

                volume_->dataMap_.dataRange = volume_->dataMap_.valueRange = dvec2{*min, *max};
            } else {
                auto [min, max] = std::minmax_element(col_data.begin(), col_data.end(),
                                                      [](const auto& a, const auto& b) -> bool {
                                                          return glm::compMax(a) < glm::compMax(b);
                                                      });

                volume_->dataMap_.dataRange = volume_->dataMap_.valueRange =
                    dvec2{glm::compMax(*min), glm::compMax(*max)};

                if (p_->getPropertiesByType<BoolProperty>().front()->get()) {
                    for (size_t i{0}; i < glm::compMul(dimensions); ++i) {
                        data[i] = glm::normalize(data[i]);
                    }
                }
            }

            volume_->setBasis(tensorField_->getBasis());
            volume_->setOffset(tensorField_->getOffset());
        }
    }

    std::shared_ptr<Volume> volume_;

private:
    size_t id_;
    std::shared_ptr<const TensorField3D> tensorField_;
    Processor* p_;
};
}  // namespace

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorField3DToVolume::processorInfo_{
    "org.inviwo.TensorField3DToVolume",  // Class identifier
    "Tensor Field 3D To Volume",         // Display name
    "Conversion",                        // Category
    CodeState::Stable,                   // Code state
    Tags::CPU | tag::OpenTensorVis,      // Tags
};
const ProcessorInfo TensorField3DToVolume::getProcessorInfo() const { return processorInfo_; }

TensorField3DToVolume::TensorField3DToVolume()
    : Processor()
    , inport_("inport")
    , outport_("outport")
    , volumeContent_("volumeContent", "Volume content")
    , normalizeVectors_("normalize", "Normalize eigenvectors") {
    addPort(inport_);
    addPort(outport_);

    addProperties(volumeContent_, normalizeVectors_);

    inport_.onChange([this]() {
        if (!inport_.hasData()) return;

        std::vector<OptionPropertyOption<size_t>> options{};

        util::for_each_type<attributes::types3D>{}(AddOptions{options, inport_.getData()});

        // NetworkLock l; --> As far as I can see this is done internally in replaceOptions
        volumeContent_.replaceOptions(options);
    });

    inport_.onDisconnect([this]() { volumeContent_.clearOptions(); });

    volumeContent_.onChange([this]() {
        NetworkLock l;
        util::for_each_type<attributes::types3D>{}(IsVec{this, volumeContent_.get()});
    });
}

void TensorField3DToVolume::process() {
    outport_.setData(util::for_each_type<attributes::types3D>{}(
                         CreateVolume{volumeContent_.get(), inport_.getData(), this})
                         .volume_);
}

}  // namespace inviwo
