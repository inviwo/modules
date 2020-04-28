/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2020 Inviwo Foundation
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

#include <type_traits>
#include <string>

#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/compositeproperty.h>

#include <inviwo/dataframe/datastructures/column.h>

#include <inviwo/tensorvisbase/datastructures/tensorfield2d.h>
#include <inviwo/tensorvisbase/datastructures/tensorfield3d.h>

namespace inviwo {
namespace util {
template <unsigned N>
struct AddMetaDataProperties {
    using TensorFieldType =
        std::conditional_t<N == 2, TensorField2D, std::conditional_t<N == 3, TensorField3D, void>>;

    AddMetaDataProperties() = delete;
    AddMetaDataProperties(Processor* p, std::shared_ptr<const TensorFieldType> tensorField)
        : processor(p), tensorField_(tensorField) {}

    template <typename T>
    void operator()() {
        auto identifier = std::string(T::identifier);
        replaceInString(identifier, " ", "");
        auto prop =
            new BoolProperty(identifier, std::string(T::identifier),
                             tensorField_ ? tensorField_->template hasMetaData<T>() : false);
        prop->setSerializationMode(PropertySerializationMode::All);
        processor->getPropertiesByType<CompositeProperty>().front()->addProperty(prop);
    }

private:
    Processor* processor;
    std::shared_ptr<const TensorFieldType> tensorField_;
};

template <unsigned N>
struct AddRemoveMetaData {
    using TensorFieldType =
        std::conditional_t<N == 2, TensorField2D, std::conditional_t<N == 3, TensorField3D, void>>;
    AddRemoveMetaData() = delete;
    AddRemoveMetaData(std::shared_ptr<TensorFieldType> tensorField) : tensorField_(tensorField) {}

    template <typename T>
    void operator()(const size_t id, const bool add) {
        auto metaData = tensorField_->metaData();

        if (id == util::constexpr_hash(T::identifier)) {
            if (tensorField_->template hasMetaData<T>()) {
                if (!add) {
                    metaData->dropColumn(std::string(T::identifier));
                }
            } else {
                if (add) {
                    metaData->addColumn(std::make_shared<TemplateColumn<typename T::value_type>>(
                        std::string(T::identifier),
                        T::calculate(tensorField_->tensors(), metaData)));
                }
            }
        }
    }

private:
    std::shared_ptr<TensorFieldType> tensorField_;
};

template <unsigned N>
struct HasMetaData {
    using TensorFieldType =
        std::conditional_t<N == 2, TensorField2D, std::conditional_t<N == 3, TensorField3D, void>>;
    HasMetaData() = delete;
    HasMetaData(std::shared_ptr<const TensorFieldType> tensorField) : tensorField_(tensorField) {}

    template <typename T>
    void operator()() {
        auto metaData = tensorField_->metaData();

        availableMetaData_.push_back(tensorField_->template hasMetaData<T>());
    }

    std::vector<bool> availableMetaData_;

private:
    std::shared_ptr<const TensorFieldType> tensorField_;
};

inline std::vector<std::string> attributeStringList(std::shared_ptr<const TensorField2D> tf) {
    std::vector<std::string> stringList{};

    for (auto col : *tf->metaData()) {
        if (col->getHeader() == "index") continue;

        stringList.push_back(col->getHeader());
    }

    return stringList;
}
}  // namespace util
}  // namespace inviwo
