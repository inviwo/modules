#pragma once

#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/datastructures/buffer/buffer.h>
#include <inviwo/core/datastructures/buffer/bufferramprecision.h>
#include <inviwo/core/datastructures/datamapper.h>
#include <inviwo/core/datastructures/image/image.h>
#include <inviwo/core/util/indexmapper.h>
#include <inviwo/tensorvisbase/tensorvisbasemoduledefine.h>
#include <inviwo/tensorvisbase/util/tensorutil.h>
#include <Eigen/Dense>
#include <inviwo/core/datastructures/spatialdata.h>
#include <inviwo/tensorvisbase/datastructures/attributes.h>
#include <inviwo/dataframe/datastructures/dataframe.h>
#include <inviwo/dataframe/datastructures/column.h>
#include <type_traits>
#include <optional>
#include <memory>

namespace inviwo {
/**
 * \class TensorField3D
 * \brief Data structure for 3D tensorfields.
 */
class IVW_MODULE_TENSORVISBASE_API TensorField3D : public StructuredGridEntity<3> {
public:
    TensorField3D() = delete;

    TensorField3D(const size3_t& dimensions, const std::vector<mat3>& tensors);
    TensorField3D(const size3_t& dimensions, std::shared_ptr<const std::vector<mat3>> tensors);

    TensorField3D(const size3_t& dimensions, const std::vector<mat3>& tensors,
                  const DataFrame& metaData);
    TensorField3D(const size3_t& dimensions, std::shared_ptr<const std::vector<mat3>> tensors,
                  std::shared_ptr<const DataFrame> metaData);

    /**
     * NOTE: This copy constructor creates a shallow copy, i.e. the tensors and the meta data are
     * not copied. Rather, the copy points towards the same data as the input field. If you need a
     * deep copy, use the deepCopy method.
     */
    TensorField3D(const TensorField3D& tf);

    TensorField3D& operator=(const TensorField3D&) = delete;

    // Destructors
    virtual ~TensorField3D() = default;

    /**
     * NOTE: This method constructor creates a deep copy, i.e. the tensors and the meta data are
     * copied. If you need a shallow copy, use the copy constructor or clone method.
     */
    std::shared_ptr<TensorField3D> deepCopy() const;
    /**
     * NOTE: This method creates a shallow copy, i.e. the tensors and the meta data are
     * not copied. Rather, the copy points towards the same data as the input field. If you need a
     * deep copy, use the deepCopy method.
     */
    virtual TensorField3D* clone() const final;

    std::string getDataInfo() const;
    std::array<std::shared_ptr<Volume>, 3> getVolumeRepresentation() const;

    template <bool useMask = false>
    const auto at(size3_t position) const;

    template <bool useMask = false>
    const auto at(size_t x, size_t y, size_t z) const;

    template <bool useMask = false>
    const auto at(size_t index) const;

    size3_t getDimensions() const final { return dimensions_; }

    template <typename T = float>
    glm::tvec3<T> getExtents() const;

    void setExtents(const vec3& extents);

    template <typename T = size_t>
    glm::tvec3<T> getBounds() const;

    template <typename T = float>
    glm::tvec3<T> getSpacing() const;

    size_t getSize() const { return size_; }

    vec3 getNormalizedVolumePosition(size_t index, double sliceCoord) const;
    std::optional<std::vector<vec3>> getNormalizedScreenCoordinates(float sliceCoord);

    template <typename T = float>
    T getMajorEigenValue(const size_t index) const;
    template <typename T = float>
    T getIntermediateEigenValue(const size_t index) const;
    template <typename T = float>
    T getMinorEigenValue(const size_t index) const;

    mat4 getBasisAndOffset() const;

    const std::vector<vec3>& majorEigenVectors() const;
    const std::vector<vec3>& intermediateEigenVectors() const;
    const std::vector<vec3>& minorEigenVectors() const;

    const std::vector<float>& majorEigenValues() const;
    const std::vector<float>& intermediateEigenValues() const;
    const std::vector<float>& minorEigenValues() const;

    std::shared_ptr<const std::vector<mat3>> tensors() const;

    void setMask(const std::vector<glm::uint8>& mask) { binaryMask_ = mask; }
    const std::vector<glm::uint8>& getMask() const { return binaryMask_; }

    void setTensors(std::shared_ptr<const std::vector<mat3>> tensors);
    void setMetaData(std::shared_ptr<const DataFrame> metaData);

    /*
    If the tensor field has a mask, this method return the number of 1s in it -
    telling you how many of the positions in the tensor field are defined.
    */
    int getNumDefinedEntries() const;

    /**
     * Data map for the eigen values of the tensor field.
     * 0 := major, 1 := middle, 2 := minor
     */
    std::array<DataMapper, 3> dataMapEigenValues_;
    /**
     * Data map for the eigen vectors of the tensor field (global min/max of all vector components).
     * 0 := major, 1 := middle, 2 := minor
     */
    std::array<DataMapper, 3> dataMapEigenVectors_;

    bool hasMask() const { return binaryMask_.size() == size_; }

    const util::IndexMapper3D& indexMapper() const { return indexMapper_; }

    template <typename T>
    bool hasMetaData() const;

    template <typename T>
    std::optional<std::shared_ptr<const Column>> getMetaData() const;

    template <typename T>
    const std::vector<typename T::value_type>& getMetaDataContainer() const;

    std::shared_ptr<const DataFrame> metaData() const { return metaData_; }

protected:
    void initializeDefaultMetaData();
    void computeDataMaps();

    size3_t dimensions_;
    util::IndexMapper3D indexMapper_;
    std::shared_ptr<const std::vector<mat3>> tensors_;
    size_t size_;
    std::shared_ptr<const DataFrame> metaData_;

    std::vector<glm::uint8> binaryMask_;

private:
    template <typename T, typename R>
    void addIfNotPresent(std::shared_ptr<DataFrame>, const R& data) const;
};

template <bool useMask>
const auto TensorField3D::at(size3_t position) const {
    return this->at<useMask>(indexMapper_(position));
}

template <bool useMask>
const auto TensorField3D::at(size_t x, size_t y, size_t z) const {
    return this->at<useMask>(indexMapper_(size3_t{x, y, z}));
}

template <bool useMask>
const auto TensorField3D::at(size_t index) const {
    if constexpr (useMask) {
        return std::make_pair<const bool, const mat3&>(binaryMask_[index],
                                                       tensors_->operator[](index));
    } else {
        return tensors_->operator[](index);
    }
}

template <typename T>
glm::tvec3<T> TensorField3D::getExtents() const {
    const auto basis = getBasis();
    return glm::tvec3<T>(glm::length(basis[0]), glm::length(basis[1]), glm::length(basis[2]));
}

template <typename T>
glm::tvec3<T> TensorField3D::getBounds() const {
    const auto b = getDimensions() - size3_t(1);
    return glm::tvec3<T>(glm::max(b, size3_t(1)));
}

template <typename T>
glm::tvec3<T> TensorField3D::getSpacing() const {
    auto bounds = getBounds<T>();

    auto extent = getExtents<T>();

    return extent / bounds;
}

template <typename T>
T TensorField3D::getMajorEigenValue(const size_t index) const {
    return T(this->getMetaDataContainer<attributes::Lambda1>()[index]);
}
template <typename T>
T TensorField3D::getIntermediateEigenValue(const size_t index) const {
    return T(this->getMetaDataContainer<attributes::Lambda2>()[index]);
}
template <typename T>
T TensorField3D::getMinorEigenValue(const size_t index) const {
    return T(this->getMetaDataContainer<attributes::Lambda3>()[index]);
}

/**
 * Perform lookup as to whether the specified meta data is available for the tensor field.
 * HINT: If it is not, you might want to add a meta data processor to your network to calculate the
 * desired meta data.
 */
template <typename T>
bool TensorField3D::hasMetaData() const {
    if constexpr (std::is_base_of_v<attributes::AttributeBase, T>) {
        const auto& headers = metaData_->getHeaders();
        const auto name = std::string(T::identifier);
        auto pred = [name](const std::pair<std::string, const DataFormatBase*>& pair) -> bool {
            return pair.first == name;
        };

        return std::find_if(headers.begin(), headers.end(), pred) != headers.end();
    }
}

/**
 * Tensor field meta data is stored in a DataFrame. If avaliable, this method returns the column for
 * the meta data specified by T (see attributes.h). Nullopt otherwise.
 */
template <typename T>
std::optional<std::shared_ptr<const Column>> TensorField3D::getMetaData() const {
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

/**
 * This method returns the underlying container for the meta data specified by T (see attributes.h).
 * NOTE: This method does not check whether or not the specified meta data is available.
 */
template <typename T>
const std::vector<typename T::value_type>& TensorField3D::getMetaDataContainer() const {
    if constexpr (std::is_base_of_v<attributes::AttributeBase, T>) {
        auto column = *this->getMetaData<T>();
        auto bufferRAM = std::dynamic_pointer_cast<const Buffer<T::value_type>>(column->getBuffer())
                             ->getRAMRepresentation();

        return bufferRAM->getDataContainer();
    }
}

template <typename T, typename R>
void TensorField3D::addIfNotPresent(std::shared_ptr<DataFrame> df, const R& data) const {
    if (!this->hasMetaData<T>()) {
        df->addColumn(
            std::make_shared<TemplateColumn<T::value_type>>(std::string(T::identifier), data));
    }
}
}  // namespace inviwo
