#pragma once

#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/ports/datainport.h>
#include <inviwo/core/ports/dataoutport.h>
#include <modules/tensorvisbase/datastructures/tensorfieldmetadata.h>
#include <modules/tensorvisbase/tensorvisbasemoduledefine.h>

namespace inviwo {
using InvariantSpaceAxis = std::vector<glm::f64>;
struct IVW_MODULE_TENSORVISBASE_API InvariantSpace {
    InvariantSpace() = default;
    InvariantSpace(size_t numberOfDimensions, const std::vector<std::string>& identifiers,
                   const std::vector<TensorFeature>& metaDataTypes);
    ~InvariantSpace() = default;

    glm::u8 getNumberOfDimensions() const { return static_cast<glm::u8>(data_.size()); }

    size_t getNumElements() const {
        if (!data_.size())
            return 0;
        else
            return data_[0]->size();
    }
    void addAxis(const std::string& identifier, std::vector<double>* data, TensorFeature type) {
        identifiers_.push_back(identifier);
        metaDataTypes_.push_back(type);

        auto minmax = std::minmax_element(data->begin(), data->end());

        minmax_.push_back({*minmax.first, *minmax.second});

        data_.push_back(data);
    }

    template <typename T>
    void addAxis(const MetaDataType<T>* metaData, const std::string& name = "") {
        identifiers_.push_back(name.empty() ? metaData->getDisplayName() : name);
        metaDataTypes_.push_back(metaData->type_);
        minmax_.push_back({{metaData->getMinMax().first, metaData->getMinMax().second}});

        data_.push_back(new std::vector<T>(metaData->getData()));
    }

    void addAxes(const InvariantSpace& invariantSpace) {
        for (int i = 0; i < invariantSpace.getNumberOfDimensions(); ++i) {
            identifiers_.push_back(invariantSpace.identifiers_[i]);
            metaDataTypes_.push_back(invariantSpace.metaDataTypes_[i]);
            minmax_.push_back(invariantSpace.minmax_[i]);

            data_.push_back(new std::vector<double>(invariantSpace.data_[i]->begin(),
                                                    invariantSpace.data_[i]->end()));
        }
    }

    void addAxes(std::shared_ptr<const InvariantSpace> invariantSpace) {
        for (int i = 0; i < invariantSpace->getNumberOfDimensions(); ++i) {
            identifiers_.push_back(invariantSpace->identifiers_[i]);
            metaDataTypes_.push_back(invariantSpace->metaDataTypes_[i]);
            minmax_.push_back(invariantSpace->minmax_[i]);

            data_.push_back(new std::vector<double>(invariantSpace->data_[i]->begin(),
                                                    invariantSpace->data_[i]->end()));
        }
    }

    void addPoint(const std::vector<double>& v) {
        if (v.size() != data_.size()) {
            LogError("Tried to add feature of wrong dimensionality ("
                     << std::to_string(v.size()) << " instead of " << std::to_string(data_.size())
                     << ")");
            return;
        }
        size_t i = 0;
        for (const auto val : v) {
            data_[i]->push_back(val);

            // update minmax
            minmax_[i][0] = std::min(minmax_[i][0], val);
            minmax_[i][1] = std::max(minmax_[i][1], val);

            i++;
        }
    }

    std::vector<double> getPoint(size_t idx) const {
        std::vector<double> feature;

        if (!data_.size()) return feature;
        if (!getNumElements()) return feature;

        for (size_t j = 0; j < getNumberOfDimensions(); j++) {
            feature.push_back((*(data_[j]))[idx]);
        }

        return feature;
    }

    std::vector<double> flatten() const {
        std::vector<double> flattened;

        if (!data_.size()) return flattened;
        if (!getNumElements()) return flattened;

        for (size_t i = 0; i < getNumElements(); i++) {
            for (size_t j = 0; j < getNumberOfDimensions(); j++) {
                flattened.push_back((*(data_[j]))[i]);
            }
        }

        return flattened;
    }

    void clear() { data_.clear(); }

    void setNumberOfDimensions(size_t n) {
        if (data_.size() > n) {
            LogError("Reduction of dimensionality would mean loss of data. Aborting");
        }

        data_.resize(n);
        for (auto& entry : data_) {
            entry = new InvariantSpaceAxis();
        }
    }

    void setNumberOfElements(size_t n) {
        for (auto axis : data_) {
            axis->resize(n);
        }
    }

    const std::vector<double>& operator[](size_t idx) const { return *data_[idx]; }

    const std::vector<double>& operator[](int idx) const { return *data_[idx]; }

    /// \returns the begin iterator
    std::vector<std::vector<double>*>::iterator begin() { return data_.begin(); }

    /// \returns the begin const iterator
    std::vector<std::vector<double>*>::const_iterator begin() const { return data_.cbegin(); }

    /// \returns the end iterator
    std::vector<std::vector<double>*>::iterator end() { return data_.end(); }

    /// \returns the end const iterator
    std::vector<std::vector<double>*>::const_iterator end() const { return data_.cend(); }

    const std::vector<InvariantSpaceAxis*>& data() const { return data_; }

    const auto& getIdentifier(size_t index) const { return identifiers_[index]; }
    const auto& getIdentifiers() const { return identifiers_; }

    auto getMetaDataType(size_t index) const { return metaDataTypes_[index]; }
    const auto& getMetaDataTypes() const { return metaDataTypes_; }

    const auto& getMinMax(size_t index) const { return minmax_[index]; }
    const auto& getMinMaxes() const { return minmax_; }

    std::string getDataInfo() const;

private:
    std::vector<InvariantSpaceAxis*> data_;
    std::vector<std::string> identifiers_;
    std::vector<TensorFeature> metaDataTypes_;
    std::vector<std::array<glm::f64, 2>> minmax_;
};

/**
 * \ingroup ports
 */
using InvariantSpaceInport = DataInport<InvariantSpace>;

/**
 * \ingroup ports
 */
using InvariantSpaceOutport = DataOutport<InvariantSpace>;

template <>
struct DataTraits<InvariantSpace> {
    static std::string classIdentifier() { return "org.inviwo.InvariantSpace"; }
    static std::string dataName() { return "InvariantSpace"; }
    static uvec3 colorCode() { return uvec3(10, 150, 235); }
    static Document info(const InvariantSpace& data) {
        std::ostringstream oss;
        oss << data.getDataInfo();
        Document doc;
        doc.append("p", oss.str());
        return doc;
    }
};
}  // namespace inviwo
