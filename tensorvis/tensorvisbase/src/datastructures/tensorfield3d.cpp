#include <inviwo/tensorvisbase/datastructures/tensorfield3d.h>
#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/core/util/stdextensions.h>
#include <modules/eigenutils/eigenutils.h>
#include <inviwo/tensorvisbase/util/misc.h>
#include <inviwo/core/util/exception.h>

namespace inviwo {

TensorField3D::TensorField3D(const size3_t dimensions, std::vector<dmat3> data, const vec3& extent,
                             float sliceCoord)
    : StructuredGridEntity<3>()
    , dimensions_(dimensions)
    , indexMapper_(dimensions)
    , tensors_(std::move(data))
    , size_(glm::compMul(dimensions))
    , rank_(2)
    , dimensionality_(3) {
    if (size_ != tensors_.size()) {
        throw Exception("Data/dimensions mismatch in TensorField3D constructor.", IVW_CONTEXT);
    }

    computeEigenValuesAndEigenVectors();
    computeNormalizedScreenCoordinates(sliceCoord);
    computeDataMaps();
    setBasis(
        {vec3{extent[0], 0.0f, 0.0f}, vec3{0.0f, extent[1], 0.0f}, vec3{0.0f, 0.0f, extent[2]}});
}

TensorField3D::TensorField3D(const size3_t dimensions, const std::vector<double>& data,
                             const vec3& extent, float sliceCoord)
    : StructuredGridEntity<3>()
    , dimensions_(dimensions)
    , indexMapper_(dimensions)
    , size_(glm::compMul(dimensions))
    , rank_(2)
    , dimensionality_(3) {
    for (size_t i = 0; i < data.size(); i += 9) {
        dmat3 tensor;
        tensor[0][0] = data[i];
        tensor[1][0] = data[i + 1];
        tensor[2][0] = data[i + 2];
        tensor[0][1] = data[i + 3];
        tensor[1][1] = data[i + 4];
        tensor[2][1] = data[i + 5];
        tensor[0][2] = data[i + 6];
        tensor[1][2] = data[i + 7];
        tensor[2][2] = data[i + 8];

        tensors_.push_back(tensor);
    }

    computeEigenValuesAndEigenVectors();
    computeNormalizedScreenCoordinates(sliceCoord);
    computeDataMaps();
    setBasis(
        {vec3{extent[0], 0.0f, 0.0f}, vec3{0.0f, extent[1], 0.0f}, vec3{0.0f, 0.0f, extent[2]}});
}

TensorField3D::TensorField3D(const size3_t dimensions, const std::vector<double>& data,
                             const std::vector<double>& majorEigenValues,
                             const std::vector<double>& middleEigenValues,
                             const std::vector<double>& minorEigenValues,
                             const std::vector<dvec3>& majorEigenVectors,
                             const std::vector<dvec3>& middleEigenVectors,
                             const std::vector<dvec3>& minorEigenVectors, const vec3& extent,
                             float sliceCoord)
    : StructuredGridEntity<3>()
    , dimensions_(dimensions)
    , indexMapper_(dimensions)
    , size_(glm::compMul(dimensions))
    , rank_(2)
    , dimensionality_(3) {
    for (size_t i = 0; i < data.size(); i += 9) {
        dmat3 tensor;
        tensor[0][0] = data[i];
        tensor[1][0] = data[i + 1];
        tensor[2][0] = data[i + 2];
        tensor[0][1] = data[i + 3];
        tensor[1][1] = data[i + 4];
        tensor[2][1] = data[i + 5];
        tensor[0][2] = data[i + 6];
        tensor[1][2] = data[i + 7];
        tensor[2][2] = data[i + 8];

        tensors_.push_back(tensor);
    }

    addMetaData<tensor::MajorEigenValues>(majorEigenValues, TensorFeature::Sigma1);
    addMetaData<tensor::IntermediateEigenValues>(middleEigenValues, TensorFeature::Sigma2);
    addMetaData<tensor::MinorEigenValues>(minorEigenValues, TensorFeature::Sigma3);

    addMetaData<tensor::MajorEigenVectors>(majorEigenVectors, TensorFeature::MajorEigenVector);
    addMetaData<tensor::IntermediateEigenVectors>(middleEigenVectors,
                                                  TensorFeature::IntermediateEigenVector);
    addMetaData<tensor::MinorEigenVectors>(minorEigenVectors, TensorFeature::MinorEigenVector);

    computeNormalizedScreenCoordinates(sliceCoord);
    computeDataMaps();
    setBasis(
        {vec3{extent[0], 0.0f, 0.0f}, vec3{0.0f, extent[1], 0.0f}, vec3{0.0f, 0.0f, extent[2]}});
}

TensorField3D::TensorField3D(
    const size3_t dimensions, std::vector<dmat3> data, const std::vector<double>& majorEigenValues,
    const std::vector<double>& middleEigenValues, const std::vector<double>& minorEigenValues,
    const std::vector<dvec3>& majorEigenVectors, const std::vector<dvec3>& middleEigenVectors,
    const std::vector<dvec3>& minorEigenVectors, const vec3& extent, float sliceCoord)
    : StructuredGridEntity<3>()
    , dimensions_(dimensions)
    , indexMapper_(dimensions)
    , tensors_(std::move(data))
    , size_(glm::compMul(dimensions))
    , rank_(2)
    , dimensionality_(3) {
    addMetaData<tensor::MajorEigenValues>(majorEigenValues, TensorFeature::Sigma1);
    addMetaData<tensor::IntermediateEigenValues>(middleEigenValues, TensorFeature::Sigma2);
    addMetaData<tensor::MinorEigenValues>(minorEigenValues, TensorFeature::Sigma3);

    addMetaData<tensor::MajorEigenVectors>(majorEigenVectors, TensorFeature::MajorEigenVector);
    addMetaData<tensor::IntermediateEigenVectors>(middleEigenVectors,
                                                  TensorFeature::IntermediateEigenVector);
    addMetaData<tensor::MinorEigenVectors>(minorEigenVectors, TensorFeature::MinorEigenVector);

    computeNormalizedScreenCoordinates(sliceCoord);
    computeDataMaps();
    setBasis(
        {vec3{extent[0], 0.0f, 0.0f}, vec3{0.0f, extent[1], 0.0f}, vec3{0.0f, 0.0f, extent[2]}});
}

TensorField3D::TensorField3D(const size_t x, const size_t y, const size_t z,
                             std::vector<dmat3> data, const vec3& extent, float sliceCoord)
    : StructuredGridEntity<3>()
    , dimensions_(size3_t(x, y, z))
    , indexMapper_(size3_t(x, y, z))
    , tensors_(std::move(data))
    , size_(x * y * z)
    , rank_(2)
    , dimensionality_(3) {
    computeEigenValuesAndEigenVectors();
    computeNormalizedScreenCoordinates(sliceCoord);
    computeDataMaps();
    setBasis(
        {vec3{extent[0], 0.0f, 0.0f}, vec3{0.0f, extent[1], 0.0f}, vec3{0.0f, 0.0f, extent[2]}});
}

TensorField3D::TensorField3D(const size_t x, const size_t y, const size_t z,
                             const std::vector<double>& data, const vec3& extent, float sliceCoord)
    : StructuredGridEntity<3>()
    , dimensions_(size3_t(x, y, z))
    , indexMapper_(size3_t(x, y, z))
    , size_(x * y * z)
    , rank_(2)
    , dimensionality_(3) {
    for (size_t i = 0; i < data.size(); i += 9) {
        dmat3 tensor;
        tensor[0][0] = data[i];
        tensor[1][0] = data[i + 1];
        tensor[2][0] = data[i + 2];
        tensor[0][1] = data[i + 3];
        tensor[1][1] = data[i + 4];
        tensor[2][1] = data[i + 5];
        tensor[0][2] = data[i + 6];
        tensor[1][2] = data[i + 7];
        tensor[2][2] = data[i + 8];

        tensors_.push_back(tensor);
    }

    computeEigenValuesAndEigenVectors();
    computeNormalizedScreenCoordinates(sliceCoord);
    computeDataMaps();
    setBasis(
        {vec3{extent[0], 0.0f, 0.0f}, vec3{0.0f, extent[1], 0.0f}, vec3{0.0f, 0.0f, extent[2]}});
}

TensorField3D::TensorField3D(size3_t dimensions, const double* data, const vec3& extent,
                             float sliceCoord)
    : StructuredGridEntity<3>()
    , dimensions_(dimensions)
    , indexMapper_(dimensions)
    , size_(dimensions.x * dimensions.y * dimensions.z)
    , rank_(2)
    , dimensionality_(3) {
    tensors_.resize(size_);
    for (size_t i = 0; i < size_; ++i) {
        std::copy(data + i * 9, data + (i + 1) * 9, glm::value_ptr(tensors_[i]));
    }

    computeEigenValuesAndEigenVectors();
    computeNormalizedScreenCoordinates(sliceCoord);
    computeDataMaps();
    setBasis(
        {vec3{extent[0], 0.0f, 0.0f}, vec3{0.0f, extent[1], 0.0f}, vec3{0.0f, 0.0f, extent[2]}});
}

TensorField3D::TensorField3D(size3_t dimensions, const float* data, const vec3& extent,
                             float sliceCoord)
    : StructuredGridEntity<3>()
    , dimensions_(dimensions)
    , indexMapper_(dimensions)
    , size_(dimensions.x * dimensions.y * dimensions.z)
    , rank_(2)
    , dimensionality_(3) {

    tensors_.resize(size_);
    for (size_t i = 0; i < size_; ++i) {
        std::copy(data + i * 9, data + (i + 1) * 9, glm::value_ptr(tensors_[i]));
    }

    computeEigenValuesAndEigenVectors();
    computeNormalizedScreenCoordinates(sliceCoord);
    computeDataMaps();
    setBasis(
        {vec3{extent[0], 0.0f, 0.0f}, vec3{0.0f, extent[1], 0.0f}, vec3{0.0f, 0.0f, extent[2]}});
}

TensorField3D::TensorField3D(
    const size_t x, const size_t y, const size_t z, const std::vector<double>& data,
    const std::vector<double>& majorEigenValues, const std::vector<double>& middleEigenValues,
    const std::vector<double>& minorEigenValues, const std::vector<dvec3>& majorEigenVectors,
    const std::vector<dvec3>& middleEigenVectors, const std::vector<dvec3>& minorEigenVectors,
    const vec3& extent, float sliceCoord)
    : StructuredGridEntity<3>()
    , dimensions_(size3_t(x, y, z))
    , indexMapper_(size3_t(x, y, z))
    , size_(x * y * z)
    , rank_(2)
    , dimensionality_(3) {
    for (size_t i = 0; i < data.size(); i += 9) {
        dmat3 tensor;
        tensor[0][0] = data[i];
        tensor[1][0] = data[i + 1];
        tensor[2][0] = data[i + 2];
        tensor[0][1] = data[i + 3];
        tensor[1][1] = data[i + 4];
        tensor[2][1] = data[i + 5];
        tensor[0][2] = data[i + 6];
        tensor[1][2] = data[i + 7];
        tensor[2][2] = data[i + 8];

        tensors_.push_back(tensor);
    }

    addMetaData<tensor::MajorEigenValues>(majorEigenValues, TensorFeature::Sigma1);
    addMetaData<tensor::IntermediateEigenValues>(middleEigenValues, TensorFeature::Sigma2);
    addMetaData<tensor::MinorEigenValues>(minorEigenValues, TensorFeature::Sigma3);

    addMetaData<tensor::MajorEigenVectors>(majorEigenVectors, TensorFeature::MajorEigenVector);
    addMetaData<tensor::IntermediateEigenVectors>(middleEigenVectors,
                                                  TensorFeature::IntermediateEigenVector);
    addMetaData<tensor::MinorEigenVectors>(minorEigenVectors, TensorFeature::MinorEigenVector);

    computeNormalizedScreenCoordinates(sliceCoord);
    computeDataMaps();
    setBasis(
        {vec3{extent[0], 0.0f, 0.0f}, vec3{0.0f, extent[1], 0.0f}, vec3{0.0f, 0.0f, extent[2]}});
}

TensorField3D::TensorField3D(const size_t x, const size_t y, const size_t z,
                             std::vector<dmat3> data, const std::vector<double>& majorEigenValues,
                             const std::vector<double>& middleEigenValues,
                             const std::vector<double>& minorEigenValues,
                             const std::vector<dvec3>& majorEigenVectors,
                             const std::vector<dvec3>& middleEigenVectors,
                             const std::vector<dvec3>& minorEigenVectors, const vec3& extent,
                             float sliceCoord)
    : StructuredGridEntity<3>()
    , dimensions_(size3_t(x, y, z))
    , indexMapper_(size3_t(x, y, z))
    , tensors_(std::move(data))
    , size_(x * y * z)
    , rank_(2)
    , dimensionality_(3) {
    addMetaData<tensor::MajorEigenValues>(majorEigenValues, TensorFeature::Sigma1);
    addMetaData<tensor::IntermediateEigenValues>(middleEigenValues, TensorFeature::Sigma2);
    addMetaData<tensor::MinorEigenValues>(minorEigenValues, TensorFeature::Sigma3);

    addMetaData<tensor::MajorEigenVectors>(majorEigenVectors, TensorFeature::MajorEigenVector);
    addMetaData<tensor::IntermediateEigenVectors>(middleEigenVectors,
                                                  TensorFeature::IntermediateEigenVector);
    addMetaData<tensor::MinorEigenVectors>(minorEigenVectors, TensorFeature::MinorEigenVector);

    computeNormalizedScreenCoordinates(sliceCoord);
    computeDataMaps();
    setBasis(
        {vec3{extent[0], 0.0f, 0.0f}, vec3{0.0f, extent[1], 0.0f}, vec3{0.0f, 0.0f, extent[2]}});
}

TensorField3D::TensorField3D(
    const size3_t dimensions, std::vector<dmat3> data,
    const std::unordered_map<uint64_t, std::unique_ptr<tensor::MetaDataBase>>& metaData,
    const vec3& extent, float sliceCoord)
    : StructuredGridEntity<3>()
    , dimensions_(dimensions)
    , indexMapper_(dimensions)
    , tensors_(std::move(data))
    , size_(glm::compMul(dimensions))
    , rank_(2)
    , dimensionality_(3) {
    for (auto& dataItem : metaData) {
        metaData_.insert(std::make_pair(dataItem.first, dataItem.second->clone()));
    }

    computeNormalizedScreenCoordinates(sliceCoord);
    computeDataMaps();
    setBasis(
        {vec3{extent[0], 0.0f, 0.0f}, vec3{0.0f, extent[1], 0.0f}, vec3{0.0f, 0.0f, extent[2]}});
}

TensorField3D::TensorField3D(const TensorField3D& tf)
    : StructuredGridEntity<3>()
    , dataMapEigenValues_(tf.dataMapEigenValues_)
    , dataMapEigenVectors_(tf.dataMapEigenVectors_)
    , dimensions_(tf.dimensions_)
    , indexMapper_(util::IndexMapper3D(dimensions_))
    , tensors_(tf.tensors_)
    , size_(tf.size_)
    , rank_(tf.rank_)
    , dimensionality_(tf.dimensionality_)
    , normalizedVolumePositions_(tf.normalizedVolumePositions_)
    , binaryMask_(tf.binaryMask_) {

    setOffset(tf.getOffset());
    setBasis(tf.getBasis());

    for (const auto& m : tf.metaData_) {
        metaData_.insert(std::make_pair(m.first, m.second->clone()));
    }
}

Document TensorField3D::getDataInfo() const {
    using P = Document::PathComponent;
    using H = utildoc::TableBuilder::Header;
    Document doc;
    utildoc::TableBuilder tb(doc.handle(), P::end());

    tb(H("Type"), "3D tensor field");
    tb(H("Number of Tensors"), tensors_.size());
    tb(H("Dimensions"), dimensions_);
    tb(H("Extent"), getExtent());

    tb(H("Max Eigenvalue (major)"), dataMapEigenValues_[0].valueRange.y);
    tb(H("Min Eigenvalue (major)"), dataMapEigenValues_[0].valueRange.x);
    tb(H("Max Eigenvalue (minor)"), dataMapEigenValues_[2].valueRange.y);
    tb(H("Min Eigenvalue (minor)"), dataMapEigenValues_[2].valueRange.x);

    if (!metaData_.empty()) {
        doc.append("b", "MetaData");
        utildoc::TableBuilder tb2(doc.handle(), P::end());

        tb2(H("Name"), H("No. Components"));
        for (auto&& [key, value] : metaData_) {
            tb2(H(value->getDisplayName()), value->getNumberOfComponents());
        }
    }

    return doc;
}

/**
 *   Returns two volumes representing the tensor field.
 *
 *   xx   xy   xz
 *   yx   yy   yz
 *   zx   zy   zz
 *
 *   If we consider a 3x3 tensor like above and assume it is symmetric such that
 *
 *   xy == yx, xz == zx, yz == zy
 *
 *   we can split it up into two parts: the diagonal and the upper (or lower) triangle.
 *   The first volume of the pair will then store (xx, yy, zz) and the second volume
 *   will store (xy, yz, xz).
 */
std::pair<std::shared_ptr<Volume>, std::shared_ptr<Volume>> TensorField3D::getVolumeRepresentation()
    const {
    std::pair<std::shared_ptr<Volume>, std::shared_ptr<Volume>> ret;

    auto volume1 = std::make_shared<Volume>(dimensions_, DataVec3Float32::get());
    auto volume2 = std::make_shared<Volume>(dimensions_, DataVec3Float32::get());

    auto tensorField = std::make_pair(volume1, volume2);

    auto XXYYZZ = tensorField.first->getEditableRepresentation<VolumeRAM>();
    auto XYYZXZ = tensorField.second->getEditableRepresentation<VolumeRAM>();

    util::IndexMapper3D indexMapper(dimensions_);

    for (size_t z = 0; z < dimensions_.z; z++) {
        for (size_t x = 0; x < dimensions_.x; x++) {
            for (size_t y = 0; y < dimensions_.y; y++) {
                auto tensor = at(size3_t(x, y, z)).second;
                XXYYZZ->setFromDVec3(size3_t(x, y, z),
                                     dvec3(tensor[0][0], tensor[1][1], tensor[2][2]));

                XYYZXZ->setFromDVec3(size3_t(x, y, y),
                                     dvec3(tensor[1][0], tensor[2][1], tensor[2][0]));
            }
        }
    }

    return ret;
}

std::pair<glm::uint8, dmat3&> TensorField3D::at(const size3_t position) {
    glm::uint8 maskVal = 0;

    if (hasMask()) maskVal = binaryMask_[indexMapper_(position)];

    return std::pair<glm::uint8, dmat3&>(maskVal, tensors_[indexMapper_(position)]);
}

std::pair<glm::uint8, dmat3&> TensorField3D::at(const size_t x, const size_t y, const size_t z) {
    glm::uint8 maskVal = 0;

    if (hasMask()) maskVal = binaryMask_[indexMapper_(size3_t(x, y, z))];

    return std::pair<glm::uint8, dmat3&>(maskVal, tensors_[indexMapper_(size3_t(x, y, z))]);
}

std::pair<glm::uint8, dmat3&> TensorField3D::at(const size_t index) {
    glm::uint8 maskVal = 0;

    if (hasMask()) maskVal = binaryMask_[index];

    return std::pair<glm::uint8, dmat3&>(maskVal, tensors_[index]);
}

std::pair<glm::uint8, const dmat3&> TensorField3D::at(const size3_t position) const {
    glm::uint8 maskVal = 0;

    if (hasMask()) maskVal = binaryMask_[indexMapper_(position)];

    return std::pair<glm::uint8, const dmat3&>(maskVal, tensors_[indexMapper_(position)]);
}

std::pair<glm::uint8, const dmat3&> TensorField3D::at(const size_t x, const size_t y,
                                                      const size_t z) const {
    glm::uint8 maskVal = 0;

    if (hasMask()) maskVal = binaryMask_[indexMapper_(size3_t(x, y, z))];

    return std::pair<glm::uint8, const dmat3&>(maskVal, tensors_[indexMapper_(size3_t(x, y, z))]);
}

std::pair<glm::uint8, const dmat3&> TensorField3D::at(const size_t index) const {
    glm::uint8 maskVal = 0;

    if (hasMask()) maskVal = binaryMask_[index];

    return std::pair<glm::uint8, const dmat3&>(maskVal, tensors_[index]);
}

void TensorField3D::setExtent(const vec3& extent) {
    auto basis = getBasis();
    basis[0] = glm::normalize(basis[0]) * extent[0];
    basis[1] = glm::normalize(basis[1]) * extent[1];
    basis[2] = glm::normalize(basis[2]) * extent[2];
    setBasis(basis);
}

vec3 TensorField3D::getNormalizedVolumePosition(const size_t index) const {
    return normalizedVolumePositions_[index];
}

mat4 TensorField3D::getBasisAndOffset() const {
    return getModelMatrix();
}

std::array<std::pair<double, dvec3>, 3> TensorField3D::getSortedEigenValuesAndEigenVectorsForTensor(
    const size_t index) const {
    const auto& majorEigenValues = getMetaData<tensor::MajorEigenValues>();
    const auto& middleEigenValues = getMetaData<tensor::IntermediateEigenValues>();
    const auto& minorEigenValues = getMetaData<tensor::MinorEigenValues>();

    const auto& majorEigenValue = majorEigenValues[index];
    const auto& middleEigenValue = middleEigenValues[index];
    const auto& minorEigenValue = minorEigenValues[index];

    const auto& majorEigenVectors = getMetaData<tensor::MajorEigenVectors>();
    const auto& middleEigenVectors = getMetaData<tensor::IntermediateEigenVectors>();
    const auto& minorEigenVectors = getMetaData<tensor::MinorEigenVectors>();

    const auto& majorEigenVector = majorEigenVectors[index];
    const auto& middleEigenVector = middleEigenVectors[index];
    const auto& minorEigenVector = minorEigenVectors[index];

    std::array<std::pair<double, dvec3>, 3> ret;
    ret[0] = {majorEigenValue, majorEigenVector};
    ret[1] = {middleEigenValue, middleEigenVector};
    ret[2] = {minorEigenValue, minorEigenVector};

    return ret;
}

std::array<std::pair<double, dvec3>, 3> TensorField3D::getSortedEigenValuesAndEigenVectorsForTensor(
    const size3_t pos) const {
    auto index = indexMapper_(pos);

    const auto& majorEigenValues = getMetaData<tensor::MajorEigenValues>();
    const auto& middleEigenValues = getMetaData<tensor::IntermediateEigenValues>();
    const auto& minorEigenValues = getMetaData<tensor::MinorEigenValues>();

    const auto& majorEigenValue = majorEigenValues[index];
    const auto& middleEigenValue = middleEigenValues[index];
    const auto& minorEigenValue = minorEigenValues[index];

    const auto& majorEigenVectors = getMetaData<tensor::MajorEigenVectors>();
    const auto& middleEigenVectors = getMetaData<tensor::IntermediateEigenVectors>();
    const auto& minorEigenVectors = getMetaData<tensor::MinorEigenVectors>();

    const auto& majorEigenVector = majorEigenVectors[index];
    const auto& middleEigenVector = middleEigenVectors[index];
    const auto& minorEigenVector = minorEigenVectors[index];

    std::array<std::pair<double, dvec3>, 3> ret;
    ret[0] = {majorEigenValue, majorEigenVector};
    ret[1] = {middleEigenValue, middleEigenVector};
    ret[2] = {minorEigenValue, minorEigenVector};

    return ret;
}

std::array<double, 3> TensorField3D::getSortedEigenValuesForTensor(const size_t index) const {
    auto ret = std::array<double, 3>();

    const auto& majorEigenValues = getMetaData<tensor::MajorEigenValues>();
    const auto& middleEigenValues = getMetaData<tensor::IntermediateEigenValues>();
    const auto& minorEigenValues = getMetaData<tensor::MinorEigenValues>();

    ret[0] = majorEigenValues[index];
    ret[1] = middleEigenValues[index];
    ret[2] = minorEigenValues[index];
    return ret;
}

std::array<double, 3> TensorField3D::getSortedEigenValuesForTensor(const size3_t& pos) const {
    auto index = indexMapper_(pos);
    auto ret = std::array<double, 3>();

    const auto& majorEigenValues = getMetaData<tensor::MajorEigenValues>();
    const auto& middleEigenValues = getMetaData<tensor::IntermediateEigenValues>();
    const auto& minorEigenValues = getMetaData<tensor::MinorEigenValues>();

    ret[0] = majorEigenValues[index];
    ret[1] = middleEigenValues[index];
    ret[2] = minorEigenValues[index];

    return ret;
}

std::array<dvec3, 3> TensorField3D::getSortedEigenVectorsForTensor(const size_t index) const {
    auto ret = std::array<dvec3, 3>();

    const auto& majorEigenVectors = getMetaData<tensor::MajorEigenVectors>();
    const auto& middleEigenVectors = getMetaData<tensor::IntermediateEigenVectors>();
    const auto& minorEigenVectors = getMetaData<tensor::MinorEigenVectors>();

    ret[0] = majorEigenVectors[index];
    ret[1] = middleEigenVectors[index];
    ret[2] = minorEigenVectors[index];

    return ret;
}

std::array<dvec3, 3> TensorField3D::getSortedEigenVectorsForTensor(const size3_t& pos) const {
    auto index = indexMapper_(pos);
    auto ret = std::array<dvec3, 3>();

    const auto& majorEigenVectors = getMetaData<tensor::MajorEigenVectors>();
    const auto& middleEigenVectors = getMetaData<tensor::IntermediateEigenVectors>();
    const auto& minorEigenVectors = getMetaData<tensor::MinorEigenVectors>();

    ret[0] = majorEigenVectors[index];
    ret[1] = middleEigenVectors[index];
    ret[2] = minorEigenVectors[index];
    return ret;
}

const std::vector<dvec3>& TensorField3D::majorEigenVectors() const {
    return getMetaData<tensor::MajorEigenVectors>();
}

const std::vector<dvec3>& TensorField3D::middleEigenVectors() const {
    return getMetaData<tensor::IntermediateEigenVectors>();
}

const std::vector<dvec3>& TensorField3D::minorEigenVectors() const {
    return getMetaData<tensor::MinorEigenVectors>();
}

const std::vector<double>& TensorField3D::majorEigenValues() const {
    return getMetaData<tensor::MajorEigenValues>();
}

const std::vector<double>& TensorField3D::middleEigenValues() const {
    return getMetaData<tensor::IntermediateEigenValues>();
}

const std::vector<double>& TensorField3D::minorEigenValues() const {
    return getMetaData<tensor::MinorEigenValues>();
}

const std::vector<dmat3>& TensorField3D::tensors() const { return tensors_; }

int TensorField3D::getNumDefinedEntries() const {
    return static_cast<int>(std::count(std::begin(binaryMask_), std::end(binaryMask_), 1));
}

bool TensorField3D::hasMetaData(const TensorFeature feature) const {
    switch (feature) {
        case TensorFeature::I1:
            return hasMetaData<tensor::I1>();
        case TensorFeature::I2:
            return hasMetaData<tensor::I2>();
        case TensorFeature::I3:
            return hasMetaData<tensor::I3>();
        case TensorFeature::J1:
            return hasMetaData<tensor::J1>();
        case TensorFeature::J2:
            return hasMetaData<tensor::J2>();
        case TensorFeature::J3:
            return hasMetaData<tensor::J3>();
        case TensorFeature::Sigma1:
            return hasMetaData<tensor::MajorEigenValues>();
        case TensorFeature::Sigma2:
            return hasMetaData<tensor::IntermediateEigenValues>();
        case TensorFeature::Sigma3:
            return hasMetaData<tensor::MinorEigenValues>();
        case TensorFeature::MajorEigenVector:
            return hasMetaData<tensor::MajorEigenVectors>();
        case TensorFeature::IntermediateEigenVector:
            return hasMetaData<tensor::IntermediateEigenVectors>();
        case TensorFeature::MinorEigenVector:
            return hasMetaData<tensor::MinorEigenVectors>();
        case TensorFeature::LodeAngle:
            return hasMetaData<tensor::LodeAngle>();
        case TensorFeature::Anisotropy:
            return hasMetaData<tensor::Anisotropy>();
        case TensorFeature::LinearAnisotropy:
            return hasMetaData<tensor::LinearAnisotropy>();
        case TensorFeature::PlanarAnisotropy:
            return hasMetaData<tensor::PlanarAnisotropy>();
        case TensorFeature::SphericalAnisotropy:
            return hasMetaData<tensor::SphericalAnisotropy>();
        case TensorFeature::Diffusivity:
            return hasMetaData<tensor::Diffusivity>();
        case TensorFeature::ShearStress:
            return hasMetaData<tensor::ShearStress>();
        case TensorFeature::PureShear:
            return hasMetaData<tensor::PureShear>();
        case TensorFeature::ShapeFactor:
            return hasMetaData<tensor::ShapeFactor>();
        case TensorFeature::IsotropicScaling:
            return hasMetaData<tensor::IsotropicScaling>();
        case TensorFeature::Rotation:
            return hasMetaData<tensor::Rotation>();
        case TensorFeature::FrobeniusNorm:
            return hasMetaData<tensor::FrobeniusNorm>();
        case TensorFeature::HillYieldCriterion:
            return hasMetaData<tensor::HillYieldCriterion>();
        default:
            break;
    }

    return false;
}

bool TensorField3D::hasMetaData(const uint64_t id) const {
    return metaData_.find(id) != metaData_.end();
}

TensorField3D* TensorField3D::clone() const { return new TensorField3D(*this); }

void TensorField3D::computeEigenValuesAndEigenVectors() {
    auto func = [](const dmat3& tensor) -> std::array<std::pair<double, dvec3>, 3> {
        if (tensor == dmat3(0.0)) {
            return {{std::make_pair(0, dvec3{0}), std::make_pair(0, dvec3{0}),
                     std::make_pair(0, dvec3{0})}};
            return std::array<std::pair<double, dvec3>, 3>{std::pair<double, dvec3>{0, dvec3(0)},
                                                           std::pair<double, dvec3>{0, dvec3(0)},
                                                           std::pair<double, dvec3>{0, dvec3(0)}};
        }

        Eigen::EigenSolver<Eigen::Matrix3d> solver(util::glm2eigen(tensor));
        const auto eigenValues = util::eigen2glm<double, 3, 1>(solver.eigenvalues().real());
        const auto eigenVectors = util::eigen2glm<double, 3, 3>(solver.eigenvectors().real());

        const auto range =
            util::as_range(glm::value_ptr(eigenValues), glm::value_ptr(eigenValues) + 3);

        const auto ordering = util::ordering(range, std::greater<double>());

        return {{std::make_pair(eigenValues[ordering[0]], eigenVectors[ordering[0]]),
                 std::make_pair(eigenValues[ordering[1]], eigenVectors[ordering[1]]),
                 std::make_pair(eigenValues[ordering[2]], eigenVectors[ordering[2]])}};
    };

    std::vector<double> majorEigenValues;
    std::vector<double> middleEigenValues;
    std::vector<double> minorEigenValues;

    std::vector<dvec3> majorEigenVectors;
    std::vector<dvec3> middleEigenVectors;
    std::vector<dvec3> minorEigenVectors;

    majorEigenValues.resize(size_);
    middleEigenValues.resize(size_);
    minorEigenValues.resize(size_);

    majorEigenVectors.resize(size_);
    middleEigenVectors.resize(size_);
    minorEigenVectors.resize(size_);

#pragma omp parallel for
    for (int i = 0; i < static_cast<int>(tensors_.size()); i++) {
        auto tensor = tensors_[i];
        auto eigenValuesAndEigenVectors = func(tensor);

        majorEigenVectors[i] = eigenValuesAndEigenVectors[0].second;
        middleEigenVectors[i] = eigenValuesAndEigenVectors[1].second;
        minorEigenVectors[i] = eigenValuesAndEigenVectors[2].second;

        majorEigenValues[i] = eigenValuesAndEigenVectors[0].first;
        middleEigenValues[i] = eigenValuesAndEigenVectors[1].first;
        minorEigenValues[i] = eigenValuesAndEigenVectors[2].first;
    }

    addMetaData<tensor::MajorEigenValues>(majorEigenValues, TensorFeature::Sigma1);
    addMetaData<tensor::IntermediateEigenValues>(middleEigenValues, TensorFeature::Sigma2);
    addMetaData<tensor::MinorEigenValues>(minorEigenValues, TensorFeature::Sigma3);

    addMetaData<tensor::MajorEigenVectors>(majorEigenVectors, TensorFeature::MajorEigenVector);
    addMetaData<tensor::IntermediateEigenVectors>(middleEigenVectors,
                                                  TensorFeature::IntermediateEigenVector);
    addMetaData<tensor::MinorEigenVectors>(minorEigenVectors, TensorFeature::MinorEigenVector);
}

void TensorField3D::computeNormalizedScreenCoordinates(double sliceCoord) {
    normalizedVolumePositions_.resize(size_);

    auto stepSize = getSpacing();

    if (dimensions_.x == 0 || dimensions_.y == 0 || dimensions_.z == 0) {
        LogError("Tensor field 3D has at least one zero-sized dimension!");
        return;
    }

    for (size_t z = 0; z < dimensions_.z; z++) {
        for (size_t y = 0; y < dimensions_.y; y++) {
            for (size_t x = 0; x < dimensions_.x; x++) {
                normalizedVolumePositions_[indexMapper_(size3_t(x, y, z))] =
                    dvec3(dimensions_.x < 2 ? sliceCoord : x * stepSize.x,
                          dimensions_.y < 2 ? sliceCoord : y * stepSize.y,
                          dimensions_.z < 2 ? sliceCoord : z * stepSize.z);
            }
        }
    }
}

void TensorField3D::computeDataMaps() {
    const auto& majorEigenValues = getMetaData<tensor::MajorEigenValues>();
    const auto& middleEigenValues = getMetaData<tensor::IntermediateEigenValues>();
    const auto& minorEigenValues = getMetaData<tensor::MinorEigenValues>();

    dataMapEigenValues_[0].dataRange.x = dataMapEigenValues_[0].valueRange.x =
        *std::min_element(majorEigenValues.begin(), majorEigenValues.end());
    dataMapEigenValues_[0].dataRange.y = dataMapEigenValues_[0].valueRange.y =
        *std::max_element(majorEigenValues.begin(), majorEigenValues.end());
    dataMapEigenValues_[1].dataRange.x = dataMapEigenValues_[1].valueRange.x =
        *std::min_element(middleEigenValues.begin(), middleEigenValues.end());
    dataMapEigenValues_[1].dataRange.y = dataMapEigenValues_[1].valueRange.y =
        *std::max_element(middleEigenValues.begin(), middleEigenValues.end());
    dataMapEigenValues_[2].dataRange.x = dataMapEigenValues_[2].valueRange.x =
        *std::min_element(minorEigenValues.begin(), minorEigenValues.end());
    dataMapEigenValues_[2].dataRange.y = dataMapEigenValues_[2].valueRange.y =
        *std::max_element(minorEigenValues.begin(), minorEigenValues.end());

    auto min_f = [](const dvec3& vec) -> double { return std::min(std::min(vec.x, vec.y), vec.z); };
    auto max_f = [](const dvec3& vec) -> double { return std::max(std::max(vec.x, vec.y), vec.z); };

    auto min = std::numeric_limits<double>::max();
    auto max = std::numeric_limits<double>::lowest();
    for (const auto& v : getMetaData<tensor::MajorEigenVectors>()) {
        min = std::min(min_f(v), min);
        max = std::max(max_f(v), max);
    }
    dataMapEigenVectors_[0].dataRange.x = dataMapEigenVectors_[0].valueRange.x = min;
    dataMapEigenVectors_[0].dataRange.y = dataMapEigenVectors_[0].valueRange.y = max;

    min = std::numeric_limits<double>::max();
    max = std::numeric_limits<double>::lowest();
    for (const auto& v : getMetaData<tensor::IntermediateEigenVectors>()) {
        min = std::min(min_f(v), min);
        max = std::max(max_f(v), max);
    }
    dataMapEigenVectors_[1].dataRange.x = dataMapEigenVectors_[1].valueRange.x = min;
    dataMapEigenVectors_[1].dataRange.y = dataMapEigenVectors_[1].valueRange.y = max;

    min = std::numeric_limits<double>::max();
    max = std::numeric_limits<double>::lowest();
    for (const auto& v : getMetaData<tensor::MinorEigenVectors>()) {
        min = std::min(min_f(v), min);
        max = std::max(max_f(v), max);
    }
    dataMapEigenVectors_[2].dataRange.x = dataMapEigenVectors_[2].valueRange.x = min;
    dataMapEigenVectors_[2].dataRange.y = dataMapEigenVectors_[2].valueRange.y = max;
}

}  // namespace inviwo
