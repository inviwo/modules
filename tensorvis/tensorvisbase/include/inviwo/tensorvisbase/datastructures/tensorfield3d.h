#pragma once

#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/datastructures/buffer/buffer.h>
#include <inviwo/core/datastructures/datamapper.h>
#include <inviwo/core/datastructures/image/image.h>
#include <inviwo/core/util/indexmapper.h>
#include <inviwo/tensorvisbase/tensorvisbasemoduledefine.h>
#include <inviwo/tensorvisbase/util/tensorutil.h>
#include <Eigen/Dense>
#include <inviwo/core/datastructures/spatialdata.h>
#include <inviwo/tensorvisbase/datastructures/attributes.h>
#include <inviwo/dataframe/datastructures/dataframe.h>

namespace inviwo {
/**
 * \class TensorField3D
 * \brief Data structure for 3D tensorfields.
 */
class IVW_MODULE_TENSORVISBASE_API TensorField3D : public StructuredGridEntity<3> {
public:
    TensorField3D() = delete;

    TensorField3D(const size3_t &dimensions, const std::vector<mat3> &tensors);

    TensorField3D(const size3_t &dimensions, const std::vector<mat3> &tensors,
                  const DataFrame &metaData);

    TensorField3D(const size3_t &dimensions, std::shared_ptr<const std::vector<mat3>> tensors,
                  std::shared_ptr<const DataFrame> metaData);

    TensorField3D(const TensorField3D &tf);

    TensorField3D &operator=(const TensorField3D &) = delete;

    // Destructors
    virtual ~TensorField3D() = default;

    // Copying and Cloning

    virtual TensorField3D *clone() const final;

    std::string getDataInfo() const;
    std::pair<std::shared_ptr<Volume>, std::shared_ptr<Volume>> getVolumeRepresentation() const;

    template <bool useMask = false>
    const auto at(size3_t position) const;

    template <bool useMask = false>
    const auto at(size_t index) const;

    size3_t getDimensions() const final { return dimensions_; }

    template <typename T = float>
    glm::tvec3<T> getExtents() const {
        const auto basis = getBasis();
        return glm::tvec3<T>(glm::length(basis[0]), glm::length(basis[1]), glm::length(basis[2]));
    }

    void setExtents(const vec3 &extents);

    template <typename T = size_t>
    glm::tvec3<T> getBounds() const {
        const auto b = getDimensions() - size3_t(1);
        return glm::tvec3<T>(glm::max(b, size3_t(1)));
    }
    template <typename T = float>
    glm::tvec3<T> getSpacing() const {
        auto bounds = getBounds<T>();

        auto extent = getExtents<T>();

        return extent / bounds;
    }

    size_t getSize() const { return size_; }
    glm::u8 rank() const { return 2; }
    glm::u8 dimensionality() const { return 3; }

    vec3 getNormalizedVolumePosition(size_t index, double sliceCoord) const;
    std::vector<vec3> getNormalizedScreenCoordinates(double sliceCoord);

    template <typename T = float>
    T getMajorEigenValue(const size_t index) const {
        return T(getMetaData<MajorEigenValues>()[index]);
    }
    template <typename T = float>
    T getMiddleEigenValue(const size_t index) const {
        return T(getMetaData<IntermediateEigenValues>()[index]);
    }
    template <typename T = float>
    T getMinorEigenValue(const size_t index) const {
        return T(getMetaData<MinorEigenValues>()[index]);
    }

    mat4 getBasisAndOffset() const;

    std::array<std::pair<float, vec3>, 3> getSortedEigenValuesAndEigenVectorsForTensor(
        size_t index) const;
    std::array<std::pair<float, vec3>, 3> getSortedEigenValuesAndEigenVectorsForTensor(
        size3_t pos) const;
    std::array<float, 3> getSortedEigenValuesForTensor(size_t index) const;
    std::array<float, 3> getSortedEigenValuesForTensor(const size3_t &pos) const;
    std::array<vec3, 3> getSortedEigenVectorsForTensor(size_t index) const;
    std::array<vec3, 3> getSortedEigenVectorsForTensor(const size3_t &pos) const;

    const std::vector<vec3> &majorEigenVectors() const;
    const std::vector<vec3> &middleEigenVectors() const;
    const std::vector<vec3> &minorEigenVectors() const;

    const std::vector<float> &majorEigenValues() const;
    const std::vector<float> &middleEigenValues() const;
    const std::vector<float> &minorEigenValues() const;

    std::shared_ptr<std::vector<mat3>> tensors() const;

    void setMask(const std::vector<glm::uint8> &mask) { binaryMask_ = mask; }
    const std::vector<glm::uint8> &getMask() const { return binaryMask_; }

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

    const util::IndexMapper3D &indexMapper() const { return indexMapper_; }

    template <typename T>
    bool hasMetaData() const;

    template <typename T>
    auto getMetaData() const;

    template <typename T, typename R>
    const std::vector<R> &getMetaDataContainer() const;

    std::shared_ptr<const DataFrame> metaData() const { return metaData_; }

protected:
    void computeEigenValuesAndEigenVectors();
    void computeDataMaps();

    size3_t dimensions_;
    util::IndexMapper3D indexMapper_;
    std::shared_ptr<const std::vector<mat3>> tensors_;
    size_t size_;
    glm::u8 rank_;
    glm::u8 dimensionality_;
    std::shared_ptr<const DataFrame> metaData_;

    std::vector<glm::uint8> binaryMask_;
};

template <bool useMask>
const auto TensorField3D::at(size3_t position) const {
    return this->at<useMask>(indexMapper_(position))
}

template <bool useMask>
const auto TensorField3D::at(size_t index) const {
    if constexpr (useMask) {
        return std::make_pair<const bool, const mat3 &>(binaryMask_[index],
                                                        tensors_->operator[](index));
    } else {
        return tensors_->operator[](index);
    }
}
}  // namespace inviwo
