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
            if (tensorField_->hasMetaData<T>()) {
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

        availableMetaData_.push_back(tensorField_->hasMetaData<T>());
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
