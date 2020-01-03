#pragma once

#include <inviwo/core/common/inviwo.h>
#include <inviwo/tensorvisbase/datastructures/tensorfield.h>
#include <inviwo/core/datastructures/volume/volume.h>

namespace inviwo {
/**
 * \class TensorField3D
 * \brief Data structure for 3D tensorfields.
 */
class IVW_MODULE_TENSORVISBASE_API TensorField3D : public TensorField<3, float> {
public:
    TensorField3D() = delete;

    TensorField3D(const size3_t& dimensions, const std::vector<mat3>& tensors);
    TensorField3D(const size3_t& dimensions, std::shared_ptr<std::vector<mat3>> tensors);

    TensorField3D(const size3_t& dimensions, const std::vector<mat3>& tensors,
                  const DataFrame& metaData);
    TensorField3D(const size3_t& dimensions, std::shared_ptr<std::vector<mat3>> tensors,
                  std::shared_ptr<DataFrame> metaData);

    /**
     * NOTE: This copy constructor creates a shallow copy, i.e. the tensors and the meta data are
     * not copied. Rather, the copy points towards the same data as the input field. If you need a
     * deep copy, use the deepCopy method.
     */
    TensorField3D(const TensorField3D& tf);

    virtual TensorField3D* clone() const final;

    template <typename T = float>
    T getMajorEigenValue(const size_t index) const {
        return T(this->getMetaDataContainer<attributes::MajorEigenValue>()[index]);
    }
    template <typename T = float>
    T getIntermediateEigenValue(const size_t index) const {
        return T(this->getMetaDataContainer<attributes::IntermediateEigenValue>()[index]);
    }
    template <typename T = float>
    T getMinorEigenValue(const size_t index) const {
        return T(this->getMetaDataContainer<attributes::MinorEigenValue>()[index]);
    }

    const std::vector<vec3>& majorEigenVectors() const {
        return this->getMetaDataContainer<attributes::MajorEigenVector>();
    }
    const std::vector<vec3>& intermediateEigenVectors() const {
        return this->getMetaDataContainer<attributes::IntermediateEigenVector>();
    }
    const std::vector<vec3>& minorEigenVectors() const {
        return this->getMetaDataContainer<attributes::MinorEigenVector>();
    }

    const std::vector<float>& majorEigenValues() const {
        return this->getMetaDataContainer<attributes::MajorEigenValue>();
    }
    const std::vector<float>& intermediateEigenValues() const {
        return this->getMetaDataContainer<attributes::IntermediateEigenValue>();
    }
    const std::vector<float>& minorEigenValues() const {
        return this->getMetaDataContainer<attributes::MinorEigenValue>();
    }

    std::array<std::shared_ptr<Volume>, 3> getVolumeRepresentation() const;
    vec3 getNormalizedVolumePosition(size_t index, double sliceCoord) const;
    std::optional<std::vector<vec3>> getNormalizedScreenCoordinates(float sliceCoord);

protected:
    void initializeDefaultMetaData();
    void computeDataMaps();

private:
    template <typename T, typename R>
    void addIfNotPresent(std::shared_ptr<DataFrame>, const R& data) const;
};

template <typename T, typename R>
void TensorField3D::addIfNotPresent(std::shared_ptr<DataFrame> df, const R& data) const {
    if (!this->hasMetaData<T>()) {
        df->addColumn(
            std::make_shared<TemplateColumn<T::value_type>>(std::string(T::identifier), data));
    }
}
}  // namespace inviwo
