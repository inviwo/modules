/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2025 Inviwo Foundation
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
#include <inviwo/core/datastructures/datamapper.h>
#include <inviwo/core/util/indexmapper.h>
#include <inviwo/tensorvisbase/datastructures/tensorfieldmetadata.h>
#include <inviwo/tensorvisbase/tensorvisbasemoduledefine.h>
#include <inviwo/tensorvisbase/util/tensorutil.h>
#include <inviwo/core/datastructures/spatialdata.h>
#include <inviwo/core/datastructures/datatraits.h>
#include <inviwo/core/util/document.h>

#include <warn/push>
#include <warn/ignore/all>
#include <Eigen/Dense>
#include <warn/pop>
#include <unordered_map>

namespace inviwo {
/**
 * \class TensorField3D
 * \brief Data structure for 3D tensorfields.
 */
class IVW_MODULE_TENSORVISBASE_API TensorField3D : public StructuredGridEntity<3> {
public:
    TensorField3D() = delete;

    // Contructors with ready tensors
    TensorField3D(size3_t dimensions, std::vector<dmat3> data, const vec3& extent = vec3(1.0f),
                  float sliceCoord = 0.0f);

    TensorField3D(size_t x, size_t y, size_t z, std::vector<dmat3> data,
                  const vec3& extent = vec3(1.0f), float sliceCoord = 0.0f);
    TensorField3D(size3_t dimensions, std::vector<dmat3> data,
                  const std::vector<double>& majorEigenvalues,
                  const std::vector<double>& middleEigenvalues,
                  const std::vector<double>& minorEigenvalues,
                  const std::vector<dvec3>& majorEigenvectors,
                  const std::vector<dvec3>& middleEigenvectors,
                  const std::vector<dvec3>& minorEigenvectors, const vec3& extent = vec3(1.0f),
                  float sliceCoord = 0.0f);
    TensorField3D(size_t x, size_t y, size_t z, std::vector<dmat3> data,
                  const std::vector<double>& majorEigenvalues,
                  const std::vector<double>& middleEigenvalues,
                  const std::vector<double>& minorEigenvalues,
                  const std::vector<dvec3>& majorEigenvectors,
                  const std::vector<dvec3>& middleEigenvectors,
                  const std::vector<dvec3>& minorEigenvectors, const vec3& extent = vec3(1.0f),
                  float sliceCoord = 0.0f);

    // Constructors with raw data
    TensorField3D(size3_t dimensions, const std::vector<double>& data,
                  const vec3& extent = vec3(1.0f), float sliceCoord = 0.0f);
    TensorField3D(size_t x, size_t y, size_t z, const std::vector<double>& data,
                  const vec3& extent = vec3(1.0f), float sliceCoord = 0.0f);

    // The pointer that is handed in should point to the data that will be copied.
    TensorField3D(size3_t dimensions, const double* data, const vec3& extent = vec3(1.0f),
                  float sliceCoord = 0.0f);
    TensorField3D(size3_t dimensions, const float* data, const vec3& extent = vec3(1.0f),
                  float sliceCoord = 0.0f);

    TensorField3D(size3_t dimensions, const std::vector<double>& data,
                  const std::vector<double>& majorEigenvalues,
                  const std::vector<double>& middleEigenvalues,
                  const std::vector<double>& minorEigenvalues,
                  const std::vector<dvec3>& majorEigenvectors,
                  const std::vector<dvec3>& middleEigenvectors,
                  const std::vector<dvec3>& minorEigenvectors, const vec3& extent = vec3(1.0f),
                  float sliceCoord = 0.0f);
    TensorField3D(size_t x, size_t y, size_t z, const std::vector<double>& data,
                  const std::vector<double>& majorEigenvalues,
                  const std::vector<double>& middleEigenvalues,
                  const std::vector<double>& minorEigenvalues,
                  const std::vector<dvec3>& majorEigenvectors,
                  const std::vector<dvec3>& middleEigenvectors,
                  const std::vector<dvec3>& minorEigenvectors, const vec3& extent = vec3(1.0f),
                  float sliceCoord = 0.0f);

    TensorField3D(
        size3_t dimensions, std::vector<dmat3> data,
        const std::unordered_map<uint64_t, std::unique_ptr<tensor::MetaDataBase>>& metaData,
        const vec3& extent = vec3(1.0f), float sliceCoord = 0.0f);

    TensorField3D& operator=(const TensorField3D&) = delete;

    // Destructors
    virtual ~TensorField3D() = default;

    // Copying and Cloning
    TensorField3D(const TensorField3D& tf);
    virtual TensorField3D* clone() const final;

    virtual const Axis* getAxis(size_t) const override { return nullptr; }

    Document getDataInfo() const;
    std::pair<std::shared_ptr<Volume>, std::shared_ptr<Volume>> getVolumeRepresentation() const;

    /*
     * Returns a pair of a glm::uint8 and dmat3.
     * The dmat3 is the tensor. Since the field stores tensors at every position
     * it has a mask defining where it is actually defined and where not. It is 1
     * if there is data at this position and 0 if not. If the mask value is zero,
     * the tensor will be a 0 tensor. If the mask is not set for the tensor field,
     * the mask value return will always be 0.
     */
    std::pair<glm::uint8, dmat3&> at(size3_t position);
    /*
     * Returns a pair of a glm::uint8 and dmat3.
     * The dmat3 is the tensor. Since the field stores tensors at every position
     * it has a mask defining where it is actually defined and where not. It is 1
     * if there is data at this position and 0 if not. If the mask value is zero,
     * the tensor will be a 0 tensor. If the mask is not set for the tensor field,
     * the mask value return will always be 0.
     */
    std::pair<glm::uint8, dmat3&> at(size_t x, const size_t y, const size_t z);
    /*
     * Returns a pair of a glm::uint8 and dmat3.
     * The dmat3 is the tensor. Since the field stores tensors at every position
     * it has a mask defining where it is actually defined and where not. It is 1
     * if there is data at this position and 0 if not. If the mask value is zero,
     * the tensor will be a 0 tensor. If the mask is not set for the tensor field,
     * the mask value return will always be 0.
     */
    std::pair<glm::uint8, dmat3&> at(size_t index);

    /*
     * Returns a pair of a glm::uint8 and dmat3.
     * The dmat3 is the tensor. Since the field stores tensors at every position
     * it has a mask defining where it is actually defined and where not. It is 1
     * if there is data at this position and 0 if not. If the mask value is zero,
     * the tensor will be a 0 tensor. If the mask is not set for the tensor field,
     * the mask value return will always be 0.
     */
    std::pair<glm::uint8, const dmat3&> at(size3_t position) const;
    /*
     * Returns a pair of a glm::uint8 and dmat3.
     * The dmat3 is the tensor. Since the field stores tensors at every position
     * it has a mask defining where it is actually defined and where not. It is 1
     * if there is data at this position and 0 if not. If the mask value is zero,
     * the tensor will be a 0 tensor. If the mask is not set for the tensor field,
     * the mask value return will always be 0.
     */
    std::pair<glm::uint8, const dmat3&> at(size_t x, size_t y, size_t z) const;
    /*
     * Returns a pair of a glm::uint8 and dmat3.
     * The dmat3 is the tensor. Since the field stores tensors at every position
     * it has a mask defining where it is actually defined and where not. It is 1
     * if there is data at this position and 0 if not. If the mask value is zero,
     * the tensor will be a 0 tensor. If the mask is not set for the tensor field,
     * the mask value return will always be 0.
     */
    std::pair<glm::uint8, const dmat3&> at(size_t index) const;

    size3_t getDimensions() const final { return dimensions_; }

    template <typename T = double>
    glm::tvec3<T> getExtent() const {
        const auto basis = getBasis();
        return glm::tvec3<T>(glm::length(basis[0]), glm::length(basis[1]), glm::length(basis[2]));
    }

    void setExtent(const vec3& extent);

    template <typename T = size_t>
    glm::tvec3<T> getBounds() const {
        const auto b = getDimensions() - size3_t(1);
        return glm::tvec3<T>(glm::max(b, size3_t(1)));
    }
    template <typename T = double>
    glm::tvec3<T> getSpacing() const {
        auto bounds = getBounds<T>();

        auto extent = getExtent<T>();

        return extent / bounds;
    }

    size_t getSize() const { return size_; }
    glm::u8 rank() const { return 2; }
    glm::u8 dimensionality() const { return 3; }

    vec3 getNormalizedVolumePosition(size_t index) const;

    template <typename T = double>
    T getMajorEigenValue(const size_t index) const {
        return T(getMetaData<tensor::MajorEigenValues>()[index]);
    }
    template <typename T = double>
    T getMiddleEigenValue(const size_t index) const {
        return T(getMetaData<tensor::IntermediateEigenValues>()[index]);
    }
    template <typename T = double>
    T getMinorEigenValue(const size_t index) const {
        return T(getMetaData<tensor::MinorEigenValues>()[index]);
    }

    mat4 getBasisAndOffset() const;

    std::array<std::pair<double, dvec3>, 3> getSortedEigenValuesAndEigenVectorsForTensor(
        size_t index) const;
    std::array<std::pair<double, dvec3>, 3> getSortedEigenValuesAndEigenVectorsForTensor(
        size3_t pos) const;
    std::array<double, 3> getSortedEigenValuesForTensor(size_t index) const;
    std::array<double, 3> getSortedEigenValuesForTensor(const size3_t& pos) const;
    std::array<dvec3, 3> getSortedEigenVectorsForTensor(size_t index) const;
    std::array<dvec3, 3> getSortedEigenVectorsForTensor(const size3_t& pos) const;

    const std::vector<dvec3>& majorEigenVectors() const;
    const std::vector<dvec3>& middleEigenVectors() const;
    const std::vector<dvec3>& minorEigenVectors() const;

    const std::vector<double>& majorEigenValues() const;
    const std::vector<double>& middleEigenValues() const;
    const std::vector<double>& minorEigenValues() const;

    const std::vector<dmat3>& tensors() const;

    void setMask(const std::vector<glm::uint8>& mask) { binaryMask_ = mask; }
    const std::vector<glm::uint8>& getMask() const { return binaryMask_; }

    /*
    If the tensor field has a mask, this method return the number of 1s in it -
    telling you how many of the positions in the tensor field are defined.
    */
    int getNumDefinedEntries() const;

    /*
     * 0 = major, 1 = middle, 2 = minor
     */
    std::array<DataMapper, 3> dataMapEigenValues_;
    /*
     * 0 = major, 1 = middle, 2 = minor
     */
    std::array<DataMapper, 3> dataMapEigenVectors_;

    bool hasMask() const { return binaryMask_.size() == size_; }

    const util::IndexMapper3D& indexMapper() const { return indexMapper_; }

    template <typename T>
    bool hasMetaData() const {
        return metaData_.find(T::id()) != std::end(metaData_);
    }
    bool hasMetaData(TensorFeature feature) const;
    bool hasMetaData(const uint64_t id) const;

    // Returns a reference to the actual data
    template <typename T>
    const typename T::DataType& getMetaData() const {
        const auto it = metaData_.find(T::id());
        if (it == metaData_.end()) {
            throw Exception("Could not locate metadata for ID " + std::to_string(T::id()));
        }
        const auto ptr = static_cast<const T*>(it->second.get());
        return ptr->data_;
    }

    // returns a pointer to the MetaDataType object
    template <typename T>
    auto getMetaDataContainer() const {
        const auto it = metaData_.find(T::id());
        if (it == metaData_.end()) {
            throw Exception("Could not locate metadata for ID " + std::to_string(T::id()));
        }
        return static_cast<const T*>(it->second.get());
    }

    // Returns a pointer to the actual data
    template <typename T>
    auto getMetaDataPtr() const {
        const auto it = metaData_.find(T::id());
        if (it == metaData_.end()) {
            throw Exception("Could not locate metadata for ID " + std::to_string(T::id()));
        }
        const auto ptr = static_cast<const T*>(it->second.get());
        return &(ptr->data_);
    }

    auto getMetaDataContainer(const uint64_t id) const {
        const auto it = metaData_.find(id);
        if (it == metaData_.end()) {
            throw Exception("Could not locate metadata for ID " + std::to_string(id));
        }
        return it->second.get();
    }

    template <typename T, typename S>
    void addMetaData(const S& data, TensorFeature type) {
        auto metaData = std::make_unique<T>(data, type);
        metaData_.insert(std::make_pair(T::id(), std::move(metaData)));
    }

    template <typename T, typename S>
    void addMetaData(const uint64_t id, const S& data, TensorFeature type) {
        auto metaData = std::make_unique<T>(data, type);
        metaData_.insert(std::make_pair(id, std::move(metaData)));
    }

    template <typename T>
    void removeMetaData() {
        if (hasMetaData<T>()) {
            metaData_.erase(T::id());
        }
    }

    const std::unordered_map<uint64_t, std::unique_ptr<tensor::MetaDataBase>>& metaData() const {
        return metaData_;
    }

protected:
    void computeEigenValuesAndEigenVectors();
    void computeNormalizedScreenCoordinates(double sliceCoord);
    void computeDataMaps();

    size3_t dimensions_;
    util::IndexMapper3D indexMapper_;
    std::vector<dmat3> tensors_;
    size_t size_;
    glm::u8 rank_;
    glm::u8 dimensionality_;
    std::vector<vec3> normalizedVolumePositions_;
    std::unordered_map<uint64_t, std::unique_ptr<tensor::MetaDataBase>> metaData_;

    std::vector<glm::uint8> binaryMask_;
};

template <>
struct DataTraits<TensorField3D> {
    static constexpr std::string_view classIdentifier() { return "org.inviwo.TensorField3D"; }
    static constexpr std::string_view dataName() { return "TensorField3D"; }
    static constexpr uvec3 colorCode() { return {10, 150, 135}; }
    static Document info(const TensorField3D& data) {
        Document doc;
        doc.append("b", "TensorField3D", {{"style", "color:white;"}});
        doc.append(data.getDataInfo());
        return doc;
    }
};

}  // namespace inviwo
