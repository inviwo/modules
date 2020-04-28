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

#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/datastructures/buffer/buffer.h>
#include <inviwo/core/datastructures/buffer/bufferramprecision.h>
#include <inviwo/core/datastructures/datamapper.h>
#include <inviwo/core/datastructures/image/image.h>
#include <inviwo/core/util/indexmapper.h>
#include <inviwo/tensorvisbase/tensorvisbasemoduledefine.h>
#include <inviwo/tensorvisbase/util/tensorutil.h>
#include <inviwo/tensorvisbase/util/misc.h>
#include <Eigen/Dense>
#include <inviwo/core/datastructures/spatialdata.h>
#include <inviwo/tensorvisbase/datastructures/attributes.h>
#include <inviwo/dataframe/datastructures/dataframe.h>
#include <inviwo/dataframe/datastructures/column.h>
#include <type_traits>
#include <optional>
#include <memory>
#include <inviwo/core/util/constexprhash.h>

namespace inviwo {
/**
 * \class TensorField
 * \brief Base data structure for tensorfields.
 */
template <unsigned int N, typename precision>
class TensorField : public StructuredGridEntity<N> {
public:
    using value_type = precision;
    using sizeN_t = glm::vec<N, std::size_t, glm::defaultp>;
    using matN = glm::mat<N, N, precision, glm::defaultp>;
    using matNb = glm::mat<N + 1, N + 1, precision, glm::defaultp>;
    using vecN = glm::vec<N, precision, glm::defaultp>;

    static constexpr unsigned int dimensionality = N;

    TensorField() = delete;

    TensorField(const sizeN_t& dimensions, const std::vector<matN>& tensors,
                std::shared_ptr<DataFrame> metaData = nullptr);
    TensorField(const sizeN_t& dimensions, std::shared_ptr<std::vector<matN>> tensors,
                std::shared_ptr<DataFrame> metaData = nullptr);

    /**
     * NOTE: This method creates a shallow copy, i.e. the tensors and the meta data are
     * not copied. Rather, the copy points towards the same data as the input field. If you need a
     * deep copy, use the deepCopy method.
     */
    TensorField(const TensorField<N, precision>& tf);

    TensorField& operator=(const TensorField&) = delete;

    // Destructors
    virtual ~TensorField() = default;

    /**
     * NOTE: This method creates a shallow copy, i.e. the tensors and the meta data are
     * not copied. Rather, the copy points towards the same data as the input field. If you need a
     * deep copy, use the deepCopy method.
     */
    virtual TensorField<N, precision>* clone() const = 0;

    /**
     * NOTE: This method constructor creates a deep copy, i.e. the tensors and the meta data are
     * copied. If you need a shallow copy, use the copy constructor or clone method.
     */
    virtual TensorField<N, precision>* deepCopy() const = 0;

    std::string getDataInfo() const;

    template <bool useMask = false, typename ReturnType = std::conditional_t<
                                        useMask, std::pair<bool, const matN&>, matN&>>
    const ReturnType at(sizeN_t position) const;

    template <bool useMask = false, typename ReturnType = std::conditional_t<
                                        useMask, std::pair<bool, const matN&>, matN&>>
    const ReturnType at(size_t index) const;

    sizeN_t getDimensions() const final { return dimensions_; }

    template <typename T>
    glm::vec<N, T, glm::defaultp> getDimensionsAs() const {
        return glm::vec<N, T, glm::defaultp>(dimensions_);
    }

    template <typename T = float>
    glm::vec<N, T> getExtents() const;

    void setExtents(const glm::vec<N, float>& extents);

    template <typename T = size_t>
    glm::vec<N, T> getBounds() const;

    template <typename T = float>
    glm::vec<N, T> getSpacing() const;

    size_t getSize() const { return size_; }

    matNb getBasisAndOffset() const;

    std::shared_ptr<const std::vector<matN>> tensors() const;

    void setMask(const std::vector<glm::uint8>& mask) { binaryMask_ = mask; }
    const std::vector<glm::uint8>& getMask() const { return binaryMask_; }

    void setTensors(std::shared_ptr<std::vector<matN>> tensors);
    void setMetaData(std::shared_ptr<DataFrame> metaData);

    /*
    If the tensor field has a mask, this method return the number of 1s in it -
    telling you how many of the positions in the tensor field are defined.
    */
    int getNumDefinedEntries() const;

    /**
     * Data map for the eigen values of the tensor field.
     * 0 := major, 1 := middle, 2 := minor
     */
    std::array<DataMapper, N> dataMapEigenValues_;
    /**
     * Data map for the eigen vectors of the tensor field (global min/max of all vector components).
     * 0 := major, 1 := middle, 2 := minor
     */
    std::array<DataMapper, N> dataMapEigenVectors_;

    bool hasMask() const { return binaryMask_.size() == size_; }

    const util::IndexMapper<N>& indexMapper() const { return indexMapper_; }

    /**
     * Perform lookup as to whether the specified meta data is available for the tensor field.
     * HINT: If it is not, you might want to add a meta data processor to your network to calculate
     * the desired meta data.
     */
    template <typename T>
    bool hasMetaData() const;

    bool hasMetaData(const std::string& name) const;

    /**
     * Tensor field meta data is stored in a DataFrame. If available, this method returns the column
     * for the meta data specified by T (see attributes.h). Nullopt otherwise.
     */
    template <typename T>
    std::optional<std::shared_ptr<const Column>> getMetaData() const;

    /**
     * This method returns the underlying container for the meta data specified by T (see
     * attributes.h). NOTE: This method does not check whether or not the specified meta data is
     * available.
     */
    template <typename T>
    const std::vector<typename T::value_type>& getMetaDataContainer() const;

    std::shared_ptr<const DataFrame> metaData() const { return metaData_; }
    std::shared_ptr<DataFrame> metaData() { return metaData_; }

    value_type getMajorEigenValue(const size_t index) const {
        return value_type(this->getMetaDataContainer<attributes::MajorEigenValue>()[index]);
    }
    value_type getMinorEigenValue(const size_t index) const {
        return value_type(this->getMetaDataContainer<attributes::MinorEigenValue>()[index]);
    }

    const std::vector<vecN>& majorEigenVectors() const {
        return this->getMetaDataContainer<attributes::MajorEigenVector<N>>();
    }
    const std::vector<vecN>& minorEigenVectors() const {
        return this->getMetaDataContainer<attributes::MinorEigenVector<N>>();
    }

    const std::vector<value_type>& majorEigenValues() const {
        return this->getMetaDataContainer<attributes::MajorEigenValue>();
    }
    const std::vector<value_type>& minorEigenValues() const {
        return this->getMetaDataContainer<attributes::MinorEigenValue>();
    }

protected:
    sizeN_t dimensions_;
    util::IndexMapper<N> indexMapper_;
    std::shared_ptr<std::vector<matN>> tensors_;
    size_t size_;
    std::shared_ptr<DataFrame> metaData_;

    std::vector<glm::uint8> binaryMask_;

    virtual void initializeDefaultMetaData() = 0;
    virtual void computeDataMaps() = 0;
};

template <unsigned int N, typename precision>
inline TensorField<N, precision>::TensorField(const sizeN_t& dimensions,
                                              const std::vector<matN>& tensors,
                                              std::shared_ptr<DataFrame> metaData)
    : StructuredGridEntity<N>()
    , dimensions_(dimensions)
    , indexMapper_(util::IndexMapper<N>(dimensions))
    , tensors_(std::make_shared<std::vector<matN>>(tensors))
    , size_(glm::compMul(dimensions))
    , metaData_(metaData) {
    if (!metaData_) {
        metaData_ = std::make_shared<DataFrame>();
    }
}

template <unsigned int N, typename precision>
inline TensorField<N, precision>::TensorField(const sizeN_t& dimensions,
                                              std::shared_ptr<std::vector<matN>> tensors,
                                              std::shared_ptr<DataFrame> metaData)
    : StructuredGridEntity<N>()
    , dimensions_(dimensions)
    , indexMapper_(util::IndexMapper<N>(dimensions))
    , tensors_(std::make_shared<std::vector<matN>>(*tensors))
    , size_(glm::compMul(dimensions))
    , metaData_(metaData) {
    if (!metaData_) {
        metaData_ = std::make_shared<DataFrame>();
    }
}

template <unsigned int N, typename precision>
inline TensorField<N, precision>::TensorField(const TensorField<N, precision>& tf)
    : StructuredGridEntity<N>()
    , dataMapEigenValues_(tf.dataMapEigenValues_)
    , dataMapEigenVectors_(tf.dataMapEigenVectors_)
    , dimensions_(tf.dimensions_)
    , indexMapper_(util::IndexMapper<N>(dimensions_))
    , tensors_(tf.tensors_)
    , size_(tf.size_)
    , metaData_(tf.metaData_)
    , binaryMask_(tf.binaryMask_) {
    this->setOffset(tf.getOffset());
    this->setBasis(tf.getBasis());
}

template <unsigned int N, typename precision>
inline std::string TensorField<N, precision>::getDataInfo() const {
    std::stringstream ss;
    ss << "<table border='0' cellspacing='0' cellpadding='0' "
          "style='border-color:white;white-space:pre;'>/n"
       << tensorutil::getHTMLTableRowString("Type", std::to_string(N) + "D tensor field")
       << tensorutil::getHTMLTableRowString("Number of tensors", tensors_->size())
       << tensorutil::getHTMLTableRowString("Dimensions", dimensions_)
       << tensorutil::getHTMLTableRowString("Max major field eigenvalue",
                                            dataMapEigenValues_[0].valueRange.y)
       << tensorutil::getHTMLTableRowString("Min major field eigenvalue",
                                            dataMapEigenValues_[0].valueRange.x);
    if (N > 2) {
        ss << tensorutil::getHTMLTableRowString("Max intermediate field eigenvalue",
                                                dataMapEigenValues_[1].valueRange.y)
           << tensorutil::getHTMLTableRowString("Min intermediate field eigenvalue",
                                                dataMapEigenValues_[1].valueRange.x);
    }
    ss << tensorutil::getHTMLTableRowString("Max minor field eigenvalue",
                                            dataMapEigenValues_[2].valueRange.y)
       << tensorutil::getHTMLTableRowString("Min minor field eigenvalue",
                                            dataMapEigenValues_[2].valueRange.x)
       << tensorutil::getHTMLTableRowString("Extends", getExtents())

       << "</table>";
    return ss.str();
}

template <unsigned int N, typename precision>
template <bool useMask, typename ReturnType>
inline const ReturnType TensorField<N, precision>::at(sizeN_t position) const {
    return this->at<useMask>(indexMapper_(position));
}

template <unsigned int N, typename precision>
template <bool useMask, typename ReturnType>
inline const ReturnType TensorField<N, precision>::at(size_t index) const {
    if constexpr (useMask) {
        return std::make_pair<const bool, const matN&>(binaryMask_[index],
                                                       tensors_->operator[](index));
    } else {
        return tensors_->operator[](index);
    }
}

template <unsigned int N, typename precision>
template <typename T>
inline glm::vec<N, T> TensorField<N, precision>::getExtents() const {
    const auto basis = this->getBasis();

    glm::vec<N, T> extents{};

    for (unsigned int i{0}; i < N; ++i) {
        extents[i] = glm::length(basis[i]);
    }

    return extents;
}

template <unsigned int N, typename precision>
inline void TensorField<N, precision>::setExtents(const glm::vec<N, float>& extents) {
    auto basis = this->getBasis();

    for (unsigned int i{0}; i < N; ++i) {
        basis[i] = glm::normalize(basis[i]) * extents[i];
    }

    this->setBasis(basis);
}

template <unsigned int N, typename precision>
template <typename T>
inline glm::vec<N, T> TensorField<N, precision>::getBounds() const {
    const auto b = this->getDimensions() - sizeN_t(1);
    return glm::vec<N, T>(glm::max(b, sizeN_t(1)));
}

template <unsigned int N, typename precision>
template <typename T>
inline glm::vec<N, T> TensorField<N, precision>::getSpacing() const {
    return getExtents<T>() / getBounds<T>();
}

template <unsigned int N, typename precision>
inline typename TensorField<N, precision>::matNb TensorField<N, precision>::getBasisAndOffset()
    const {
    auto basis = this->getBasis();
    auto offset = this->getOffset();

    matNb modelMatrix;

    for (unsigned int i{0}; i < N; ++i) {
        modelMatrix[i] = glm::vec<N + 1, precision>(basis[i], precision(0));
    }

    modelMatrix[N] = glm::vec<N + 1, precision>(offset, precision(1));

    return modelMatrix;
}

template <unsigned int N, typename precision>
inline std::shared_ptr<const std::vector<typename TensorField<N, precision>::matN>>
TensorField<N, precision>::tensors() const {
    return tensors_;
}

template <unsigned int N, typename precision>
inline void TensorField<N, precision>::setTensors(
    std::shared_ptr<std::vector<typename TensorField<N, precision>::matN>> tensors) {
    tensors_ = tensors;
}

template <unsigned int N, typename precision>
inline void TensorField<N, precision>::setMetaData(std::shared_ptr<DataFrame> metaData) {
    metaData_ = metaData;
}

template <unsigned int N, typename precision>
inline int TensorField<N, precision>::getNumDefinedEntries() const {
    return static_cast<int>(std::count(std::begin(binaryMask_), std::end(binaryMask_), 1));
}

namespace {
struct HasMetaData {
    HasMetaData() = default;

    template <typename T>
    void operator()(const std::string& name) {
        if (util::constexpr_hash(std::string_view(name)) == util::constexpr_hash(T::identifier)) {
            wasFound_ = true;
        }
    }

    bool wasFound_{false};
};
}  // namespace

template <unsigned int N, typename precision>
inline bool TensorField<N, precision>::hasMetaData(const std::string& name) const {
    if constexpr (N == 3) {
        return util::for_each_type<attributes::types3D>{}(HasMetaData{}, name).wasFound_;
    }
    return false;
}

template <unsigned int N, typename precision>
template <typename T>
inline bool TensorField<N, precision>::hasMetaData() const {
    if constexpr (std::is_base_of_v<attributes::AttributeBase, T>) {
        const auto& headers = metaData_->getHeaders();
        const auto name = std::string(T::identifier);
        auto pred = [name](const std::pair<std::string, const DataFormatBase*>& pair) -> bool {
            return pair.first == name;
        };

        return std::find_if(headers.begin(), headers.end(), pred) != headers.end();
    }
}

template <unsigned int N, typename precision>
template <typename T>
inline std::optional<std::shared_ptr<const Column>> TensorField<N, precision>::getMetaData() const {
    if constexpr (std::is_base_of_v<attributes::AttributeBase, T>) {
        const auto& headers = metaData_->getHeaders();
        const auto name = std::string(T::identifier);
        auto pred = [name](const std::pair<std::string, const DataFormatBase*>& pair) -> bool {
            return pair.first == name;
        };

        if (std::find_if(headers.begin(), headers.end(), pred) != headers.end()) {
            return metaData_->getColumn(name);
        }

        return std::nullopt;
    }
}

template <unsigned int N, typename precision>
template <typename T>
inline const std::vector<typename T::value_type>& TensorField<N, precision>::getMetaDataContainer()
    const {
    if constexpr (std::is_base_of_v<attributes::AttributeBase, T>) {
        auto column = *this->getMetaData<T>();
        auto bufferRAM =
            std::dynamic_pointer_cast<const Buffer<typename T::value_type>>(column->getBuffer())
                ->getRAMRepresentation();

        return bufferRAM->getDataContainer();
    }
}
}  // namespace inviwo
