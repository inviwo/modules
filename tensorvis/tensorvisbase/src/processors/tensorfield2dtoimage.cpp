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

#include <inviwo/tensorvisbase/processors/tensorfield2dtoimage.h>
#include <inviwo/core/datastructures/image/layer.h>
#include <inviwo/core/datastructures/image/layerramprecision.h>
#include <inviwo/tensorvisbase/tensorvisbasemodule.h>

namespace inviwo {

namespace {

struct AddOptions {
    AddOptions() = delete;
    AddOptions(std::vector<OptionPropertyOption<size_t>>& options,
               std::shared_ptr<const TensorField2D> tensorField)
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
    std::shared_ptr<const TensorField2D> tensorField_;
};

struct CreateImage {
    CreateImage() = delete;
    CreateImage(size_t id, std::shared_ptr<const TensorField2D> tensorField, Processor* p)
        : id_(id), tensorField_(tensorField), p_(p) {}

    template <typename T>
    void operator()() {
        using type = typename T::value_type;

        if (id_ == util::constexpr_hash(T::identifier)) {
            auto dataFormat = new DataFormat<type>();
            auto dimensions = tensorField_->getDimensions();
            layer_ = std::make_shared<Layer>(dimensions, dataFormat);
            auto data =
                static_cast<LayerRAMPrecision<type>*>(layer_->getEditableRepresentation<LayerRAM>())
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

                dataMap_.dataRange = dataMap_.valueRange = dvec2{*min, *max};

                if (p_->getPropertiesByType<BoolProperty>().front()->get()) {
                    for (size_t i{0}; i < glm::compMul(dimensions); ++i) {
                        data[i] = dataMap_.mapFromValueToNormalized(data[i]);
                    }
                }
            } else {
                auto [min, max] = std::minmax_element(col_data.begin(), col_data.end(),
                                                      [](const auto& a, const auto& b) -> bool {
                                                          return glm::compMax(a) < glm::compMax(b);
                                                      });

                dataMap_.dataRange = dataMap_.valueRange =
                    dvec2{glm::compMax(*min), glm::compMax(*max)};

                if (p_->getPropertiesByType<BoolProperty>().front()->get()) {
                    for (size_t i{0}; i < glm::compMul(dimensions); ++i) {
                        data[i] = glm::normalize(data[i]);
                    }
                }
            }
        }
    }

    std::shared_ptr<Layer> layer_;
    DataMapper dataMap_;

private:
    size_t id_;
    std::shared_ptr<const TensorField2D> tensorField_;
    Processor* p_;
};
}  // namespace

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorField2DToImage::processorInfo_{
    "org.inviwo.TensorField2DToImage",  // Class identifier
    "Tensor Field 2D To Image",         // Display name
    "Conversion",                       // Category
    CodeState::Experimental,            // Code state
    tag::OpenTensorVis | Tag::CPU,      // Tags
};
const ProcessorInfo TensorField2DToImage::getProcessorInfo() const { return processorInfo_; }

TensorField2DToImage::TensorField2DToImage()
    : Processor()
    , tensorField2DInport_("tensorField2DInport")
    , imageOutport_("imageOutport")
    , imageContent_("imageContent", "Image content")
    , normalizeContent_("normalize", "Normalize data", true) {

    addPort(tensorField2DInport_);
    addPort(imageOutport_);

    addProperties(imageContent_, normalizeContent_);

    tensorField2DInport_.onChange([this]() {
        if (!tensorField2DInport_.hasData()) return;

        std::vector<OptionPropertyOption<size_t>> options{};
        options.emplace_back("rgba", "RGBA", 0);

        util::for_each_type<attributes::types2D>{}(
            AddOptions{options, tensorField2DInport_.getData()});

        // NetworkLock l; --> As far as I can see this is done internally in replaceOptions
        imageContent_.replaceOptions(options);
    });

    tensorField2DInport_.onDisconnect([this]() { imageContent_.clearOptions(); });
}

void TensorField2DToImage::process() {
    auto tensorField = tensorField2DInport_.getData();

    std::shared_ptr<Image> img;

    if (!imageContent_.get()) {
        img = tensorField->getImageRepresentation();
    } else {
        auto layer = util::for_each_type<attributes::types2D>{}(
                         CreateImage{imageContent_.get(), tensorField2DInport_.getData(), this})
                         .layer_;

        img = std::make_shared<Image>(layer);
    }

    imageOutport_.setData(img);
}

}  // namespace inviwo
