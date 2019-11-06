#ifdef _MSC_VER
//#pragma optimize("", off)
#elif ((__GNUC__ > 3) && (__GNUC_MINOR__ > 3))
//#pragma GCC push_options
//#pragma GCC optimize("O0")
#endif

#include <inviwo/tensorvisbase/datastructures/tensorfield3d.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/core/util/stdextensions.h>

#include <modules/eigenutils/eigenutils.h>
#include <inviwo/tensorvisbase/util/misc.h>

#include <inviwo/core/util/exception.h>

namespace inviwo {

TensorField3D::TensorField3D(const size3_t dimensions, std::vector<dmat3> data, const dvec3 &extent,
                             double sliceCoord)
    : dimensions_(dimensions)
    , extent_(extent)
    , indexMapper_(dimensions)
    , tensors_(std::move(data))
    , size_(glm::compMul(dimensions))
    , rank_(2)
    , dimensionality_(3)
    , offset_(dvec3(0.0)) {
    if (size_ != tensors_.size()) {
        throw Exception("Data/dimensions mismatch in TensorField3D constructor.", IVW_CONTEXT);
    }

    computeEigenValuesAndEigenVectors();
    computeNormalizedScreenCoordinates(sliceCoord);
    computeDataMaps();
}

TensorField3D::TensorField3D(const size3_t dimensions, const std::vector<double> &data,
                             const dvec3 &extent, double sliceCoord)
    : dimensions_(dimensions)
    , extent_(extent)
    , indexMapper_(dimensions)
    , size_(glm::compMul(dimensions))
    , rank_(2)
    , dimensionality_(3)
    , offset_(dvec3(0.0)) {
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
}

TensorField3D::TensorField3D(const size3_t dimensions, const std::vector<double> &data,
                             const std::vector<double> &majorEigenValues,
                             const std::vector<double> &middleEigenValues,
                             const std::vector<double> &minorEigenValues,
                             const std::vector<dvec3> &majorEigenVectors,
                             const std::vector<dvec3> &middleEigenVectors,
                             const std::vector<dvec3> &minorEigenVectors, const dvec3 &extent,
                             double sliceCoord)
    : dimensions_(dimensions)
    , extent_(extent)
    , indexMapper_(dimensions)
    , size_(glm::compMul(dimensions))
    , rank_(2)
    , dimensionality_(3)
    , offset_(dvec3(0.0)) {
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

    addMetaData<MajorEigenValues>(majorEigenValues, TensorFeature::Sigma1);
    addMetaData<IntermediateEigenValues>(middleEigenValues, TensorFeature::Sigma2);
    addMetaData<MinorEigenValues>(minorEigenValues, TensorFeature::Sigma3);

    addMetaData<MajorEigenVectors>(majorEigenVectors, TensorFeature::MajorEigenVector);
    addMetaData<IntermediateEigenVectors>(middleEigenVectors,
                                          TensorFeature::IntermediateEigenVector);
    addMetaData<MinorEigenVectors>(minorEigenVectors, TensorFeature::MinorEigenVector);

    computeNormalizedScreenCoordinates(sliceCoord);
    computeDataMaps();
}

TensorField3D::TensorField3D(
    const size3_t dimensions, std::vector<dmat3> data, const std::vector<double> &majorEigenValues,
    const std::vector<double> &middleEigenValues, const std::vector<double> &minorEigenValues,
    const std::vector<dvec3> &majorEigenVectors, const std::vector<dvec3> &middleEigenVectors,
    const std::vector<dvec3> &minorEigenVectors, const dvec3 &extent, double sliceCoord)
    : dimensions_(dimensions)
    , extent_(extent)
    , indexMapper_(dimensions)
    , tensors_(std::move(data))
    , size_(glm::compMul(dimensions))
    , rank_(2)
    , dimensionality_(3)
    , offset_(dvec3(0.0)) {
    addMetaData<MajorEigenValues>(majorEigenValues, TensorFeature::Sigma1);
    addMetaData<IntermediateEigenValues>(middleEigenValues, TensorFeature::Sigma2);
    addMetaData<MinorEigenValues>(minorEigenValues, TensorFeature::Sigma3);

    addMetaData<MajorEigenVectors>(majorEigenVectors, TensorFeature::MajorEigenVector);
    addMetaData<IntermediateEigenVectors>(middleEigenVectors,
                                          TensorFeature::IntermediateEigenVector);
    addMetaData<MinorEigenVectors>(minorEigenVectors, TensorFeature::MinorEigenVector);

    computeNormalizedScreenCoordinates(sliceCoord);
    computeDataMaps();
}

TensorField3D::TensorField3D(const size_t x, const size_t y, const size_t z,
                             std::vector<dmat3> data, const dvec3 &extent, double sliceCoord)
    : dimensions_(size3_t(x, y, z))
    , extent_(extent)
    , indexMapper_(size3_t(x, y, z))
    , tensors_(std::move(data))
    , size_(x * y * z)
    , rank_(2)
    , dimensionality_(3)
    , offset_(dvec3(0.0)) {
    computeEigenValuesAndEigenVectors();
    computeNormalizedScreenCoordinates(sliceCoord);
    computeDataMaps();
}

TensorField3D::TensorField3D(const size_t x, const size_t y, const size_t z,
                             const std::vector<double> &data, const dvec3 &extent,
                             double sliceCoord)
    : dimensions_(size3_t(x, y, z))
    , extent_(extent)
    , indexMapper_(size3_t(x, y, z))
    , size_(x * y * z)
    , rank_(2)
    , dimensionality_(3)
    , offset_(dvec3(0.0)) {
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
}

TensorField3D::TensorField3D(size3_t dimensions, const double *data, const dvec3 &extent,
                             double sliceCoord)
    : dimensions_(dimensions)
    , extent_(extent)
    , indexMapper_(dimensions)
    , size_(dimensions.x * dimensions.y * dimensions.z)
    , rank_(2)
    , dimensionality_(3)
    , offset_(dvec3(0.0)) {
    tensors_.resize(size_);
    for (size_t i = 0; i < size_; ++i) {
        std::copy(data + i * 9, data + (i + 1) * 9, glm::value_ptr(tensors_[i]));
    }

    computeEigenValuesAndEigenVectors();
    computeNormalizedScreenCoordinates(sliceCoord);
    computeDataMaps();
}

TensorField3D::TensorField3D(size3_t dimensions, const float *data, const dvec3 &extent,
                             double sliceCoord)
    : dimensions_(dimensions)
    , extent_(extent)
    , indexMapper_(dimensions)
    , size_(dimensions.x * dimensions.y * dimensions.z)
    , rank_(2)
    , dimensionality_(3)
    , offset_(dvec3(0.0)) {

    tensors_.resize(size_);
    for (size_t i = 0; i < size_; ++i) {
        std::copy(data + i * 9, data + (i + 1) * 9, glm::value_ptr(tensors_[i]));
    }

    computeEigenValuesAndEigenVectors();
    computeNormalizedScreenCoordinates(sliceCoord);
    computeDataMaps();
}

TensorField3D::TensorField3D(
    const size_t x, const size_t y, const size_t z, const std::vector<double> &data,
    const std::vector<double> &majorEigenValues, const std::vector<double> &middleEigenValues,
    const std::vector<double> &minorEigenValues, const std::vector<dvec3> &majorEigenVectors,
    const std::vector<dvec3> &middleEigenVectors, const std::vector<dvec3> &minorEigenVectors,
    const dvec3 &extent, double sliceCoord)
    : dimensions_(size3_t(x, y, z))
    , extent_(extent)
    , indexMapper_(size3_t(x, y, z))
    , size_(x * y * z)
    , rank_(2)
    , dimensionality_(3)
    , offset_(dvec3(0.0)) {
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

    addMetaData<MajorEigenValues>(majorEigenValues, TensorFeature::Sigma1);
    addMetaData<IntermediateEigenValues>(middleEigenValues, TensorFeature::Sigma2);
    addMetaData<MinorEigenValues>(minorEigenValues, TensorFeature::Sigma3);

    addMetaData<MajorEigenVectors>(majorEigenVectors, TensorFeature::MajorEigenVector);
    addMetaData<IntermediateEigenVectors>(middleEigenVectors,
                                          TensorFeature::IntermediateEigenVector);
    addMetaData<MinorEigenVectors>(minorEigenVectors, TensorFeature::MinorEigenVector);

    computeNormalizedScreenCoordinates(sliceCoord);
    computeDataMaps();
}

TensorField3D::TensorField3D(const size_t x, const size_t y, const size_t z,
                             std::vector<dmat3> data, const std::vector<double> &majorEigenValues,
                             const std::vector<double> &middleEigenValues,
                             const std::vector<double> &minorEigenValues,
                             const std::vector<dvec3> &majorEigenVectors,
                             const std::vector<dvec3> &middleEigenVectors,
                             const std::vector<dvec3> &minorEigenVectors, const dvec3 &extent,
                             double sliceCoord)
    : dimensions_(size3_t(x, y, z))
    , extent_(extent)
    , indexMapper_(size3_t(x, y, z))
    , tensors_(std::move(data))
    , size_(x * y * z)
    , rank_(2)
    , dimensionality_(3)
    , offset_(dvec3(0.0)) {
    addMetaData<MajorEigenValues>(majorEigenValues, TensorFeature::Sigma1);
    addMetaData<IntermediateEigenValues>(middleEigenValues, TensorFeature::Sigma2);
    addMetaData<MinorEigenValues>(minorEigenValues, TensorFeature::Sigma3);

    addMetaData<MajorEigenVectors>(majorEigenVectors, TensorFeature::MajorEigenVector);
    addMetaData<IntermediateEigenVectors>(middleEigenVectors,
                                          TensorFeature::IntermediateEigenVector);
    addMetaData<MinorEigenVectors>(minorEigenVectors, TensorFeature::MinorEigenVector);

    computeNormalizedScreenCoordinates(sliceCoord);
    computeDataMaps();
}

TensorField3D::TensorField3D(
    const size3_t dimensions, std::vector<dmat3> data,
    const std::unordered_map<uint64_t, std::unique_ptr<MetaDataBase>> &metaData,
    const dvec3 &extent, double sliceCoord)
    : dimensions_(dimensions)
    , extent_(extent)
    , indexMapper_(dimensions)
    , tensors_(std::move(data))
    , size_(glm::compMul(dimensions))
    , rank_(2)
    , dimensionality_(3)
    , offset_(dvec3(0.0)) {
    for (auto &dataItem : metaData) {
        metaData_.insert(std::make_pair(dataItem.first, dataItem.second->clone()));
    }

    computeNormalizedScreenCoordinates(sliceCoord);
    computeDataMaps();
}

std::string TensorField3D::getDataInfo() const {
    std::stringstream ss;
    ss << "<table border='0' cellspacing='0' cellpadding='0' "
          "style='border-color:white;white-space:pre;'>/n"
       << tensorutil::getHTMLTableRowString("Type", "3D tensor field")
       << tensorutil::getHTMLTableRowString("Number of tensors", tensors_.size())
       << tensorutil::getHTMLTableRowString("Dimensions", dimensions_)
       << tensorutil::getHTMLTableRowString("Max major field eigenvalue",
                                            dataMapEigenValues_[0].valueRange.y)
       << tensorutil::getHTMLTableRowString("Min major field eigenvalue",
                                            dataMapEigenValues_[0].valueRange.x)
       << tensorutil::getHTMLTableRowString("Max intermediate field eigenvalue",
                                            dataMapEigenValues_[1].valueRange.y)
       << tensorutil::getHTMLTableRowString("Min intermediate field eigenvalue",
                                            dataMapEigenValues_[1].valueRange.x)
       << tensorutil::getHTMLTableRowString("Max minor field eigenvalue",
                                            dataMapEigenValues_[2].valueRange.y)
       << tensorutil::getHTMLTableRowString("Min minor field eigenvalue",
                                            dataMapEigenValues_[2].valueRange.x)
       << tensorutil::getHTMLTableRowString("Extends", extent_)

       << "</table>";
    return ss.str();
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

void TensorField3D::setXCoordinates(const double *) {}

void TensorField3D::setYCoordinates(const double *) {}

void TensorField3D::setZCoordinates(const double *) {}

void TensorField3D::setCoordinates(const std::vector<dvec3> &coordinates) {
    coordinates_ = coordinates;
}

std::pair<glm::uint8, dmat3 &> TensorField3D::at(const size3_t position) {
    glm::uint8 maskVal = 0;

    if (hasMask()) maskVal = binaryMask_[indexMapper_(position)];

    return std::pair<glm::uint8, dmat3 &>(maskVal, tensors_[indexMapper_(position)]);
}

std::pair<glm::uint8, dmat3 &> TensorField3D::at(const size_t x, const size_t y, const size_t z) {
    glm::uint8 maskVal = 0;

    if (hasMask()) maskVal = binaryMask_[indexMapper_(size3_t(x, y, z))];

    return std::pair<glm::uint8, dmat3 &>(maskVal, tensors_[indexMapper_(size3_t(x, y, z))]);
}

std::pair<glm::uint8, dmat3 &> TensorField3D::at(const size_t index) {
    glm::uint8 maskVal = 0;

    if (hasMask()) maskVal = binaryMask_[index];

    return std::pair<glm::uint8, dmat3 &>(maskVal, tensors_[index]);
}

std::pair<glm::uint8, const dmat3 &> TensorField3D::at(const size3_t position) const {
    glm::uint8 maskVal = 0;

    if (hasMask()) maskVal = binaryMask_[indexMapper_(position)];

    return std::pair<glm::uint8, const dmat3 &>(maskVal, tensors_[indexMapper_(position)]);
}

std::pair<glm::uint8, const dmat3 &> TensorField3D::at(const size_t x, const size_t y,
                                                       const size_t z) const {
    glm::uint8 maskVal = 0;

    if (hasMask()) maskVal = binaryMask_[indexMapper_(size3_t(x, y, z))];

    return std::pair<glm::uint8, const dmat3 &>(maskVal, tensors_[indexMapper_(size3_t(x, y, z))]);
}

std::pair<glm::uint8, const dmat3 &> TensorField3D::at(const size_t index) const {
    glm::uint8 maskVal = 0;

    if (hasMask()) maskVal = binaryMask_[index];

    return std::pair<glm::uint8, const dmat3 &>(maskVal, tensors_[index]);
}

const dvec3 &TensorField3D::getNormalizedVolumePosition(const size_t index) const {
    return normalizedVolumePositions_[index];
}

const dvec3 &TensorField3D::getCoordinate(const size_t index) const { return coordinates_[index]; }

std::pair<glm::uint8, dmat3> TensorField3D::sample(
    const dvec3 &position, const tensorutil::InterpolationMethod method) const {
    // Position is in texture space [0,1], translate to index space
    const auto bounds = getBounds<double>();
    const auto indexPosition = position * bounds;

    glm::uint8 maskValue = 0;

    dmat3 val{0.0};

    if (method == tensorutil::InterpolationMethod::Linear) {
        const auto xm = glm::floor(indexPosition.x);
        const auto ym = glm::floor(indexPosition.y);
        const auto zm = glm::floor(indexPosition.z);

        const auto xp = glm::ceil(indexPosition.x);
        const auto yp = glm::ceil(indexPosition.y);
        const auto zp = glm::ceil(indexPosition.z);

        const auto xFrac = glm::fract(position.x * bounds.x);
        const auto yFrac = glm::fract(position.y * bounds.y);
        const auto zFrac = glm::fract(position.z * bounds.z);

        // Upper layer
        const auto &tensor010 = at(size3_t(xm, yp, zm)).second;
        const auto &tensor011 = at(size3_t(xm, yp, zp)).second;
        const auto tensorUpperLeft = glm::mix(tensor010, tensor011, zFrac);

        const auto &tensor111 = at(size3_t(xp, yp, zp)).second;
        const auto &tensor110 = at(size3_t(xp, yp, zm)).second;
        const auto tensorUpperRight = glm::mix(tensor110, tensor111, zFrac);

        const auto tensorUpper = glm::mix(tensorUpperLeft, tensorUpperRight, xFrac);

        // Lower layer
        const auto &tensor000 = at(size3_t(xm, ym, zm)).second;
        const auto &tensor001 = at(size3_t(xm, ym, zp)).second;
        const auto tensorLowerLeft = glm::mix(tensor000, tensor001, zFrac);

        const auto &tensor100 = at(size3_t(xp, ym, zm)).second;
        const auto &tensor101 = at(size3_t(xp, ym, zp)).second;
        const auto tensorLowerRight = glm::mix(tensor100, tensor101, zFrac);

        const auto tensorLower = glm::mix(tensorLowerLeft, tensorLowerRight, xFrac);

        val = glm::mix(tensorLower, tensorUpper, yFrac);
    } else if (method == tensorutil::InterpolationMethod::Nearest) {
        val = at(size3_t(glm::round(indexPosition))).second;
    }

    return std::pair<glm::uint8, dmat3>(maskValue, val);
}

dmat3 TensorField3D::getBasis() const {
    return dmat3(dvec3(extent_.x, 0.0, 0.0), dvec3(0.0, extent_.y, 0.0),
                 dvec3(0.0, 0.0, extent_.z));
}

dmat4 TensorField3D::getBasisAndOffset() const {
    auto basis =
        dmat3(dvec3(extent_.x, 0.0, 0.0), dvec3(0.0, extent_.y, 0.0), dvec3(0.0, 0.0, extent_.z));

    dmat4 modelMatrix;

    modelMatrix[0][0] = basis[0][0];
    modelMatrix[0][1] = basis[0][1];
    modelMatrix[0][2] = basis[0][2];

    modelMatrix[1][0] = basis[1][0];
    modelMatrix[1][1] = basis[1][1];
    modelMatrix[1][2] = basis[1][2];

    modelMatrix[2][0] = basis[2][0];
    modelMatrix[2][1] = basis[2][1];
    modelMatrix[2][2] = basis[2][2];

    modelMatrix[3][0] = offset_[0];
    modelMatrix[3][1] = offset_[1];
    modelMatrix[3][2] = offset_[2];

    modelMatrix[0][3] = 0.0;
    modelMatrix[1][3] = 0.0;
    modelMatrix[2][3] = 0.0;
    modelMatrix[3][3] = 1.0;

    return modelMatrix;
}

std::array<std::pair<double, dvec3>, 3> TensorField3D::getSortedEigenValuesAndEigenVectorsForTensor(
    const size_t index) const {
    const auto &majorEigenValues = getMetaData<MajorEigenValues>();
    const auto &middleEigenValues = getMetaData<IntermediateEigenValues>();
    const auto &minorEigenValues = getMetaData<MinorEigenValues>();

    const auto &majorEigenValue = majorEigenValues[index];
    const auto &middleEigenValue = middleEigenValues[index];
    const auto &minorEigenValue = minorEigenValues[index];

    const auto &majorEigenVectors = getMetaData<MajorEigenVectors>();
    const auto &middleEigenVectors = getMetaData<IntermediateEigenVectors>();
    const auto &minorEigenVectors = getMetaData<MinorEigenVectors>();

    const auto &majorEigenVector = majorEigenVectors[index];
    const auto &middleEigenVector = middleEigenVectors[index];
    const auto &minorEigenVector = minorEigenVectors[index];

    std::array<std::pair<double, dvec3>, 3> ret;
    ret[0] = {majorEigenValue, majorEigenVector};
    ret[1] = {middleEigenValue, middleEigenVector};
    ret[2] = {minorEigenValue, minorEigenVector};

    return ret;
}

std::array<std::pair<double, dvec3>, 3> TensorField3D::getSortedEigenValuesAndEigenVectorsForTensor(
    const size3_t pos) const {
    auto index = indexMapper_(pos);

    const auto &majorEigenValues = getMetaData<MajorEigenValues>();
    const auto &middleEigenValues = getMetaData<IntermediateEigenValues>();
    const auto &minorEigenValues = getMetaData<MinorEigenValues>();

    const auto &majorEigenValue = majorEigenValues[index];
    const auto &middleEigenValue = middleEigenValues[index];
    const auto &minorEigenValue = minorEigenValues[index];

    const auto &majorEigenVectors = getMetaData<MajorEigenVectors>();
    const auto &middleEigenVectors = getMetaData<IntermediateEigenVectors>();
    const auto &minorEigenVectors = getMetaData<MinorEigenVectors>();

    const auto &majorEigenVector = majorEigenVectors[index];
    const auto &middleEigenVector = middleEigenVectors[index];
    const auto &minorEigenVector = minorEigenVectors[index];

    std::array<std::pair<double, dvec3>, 3> ret;
    ret[0] = {majorEigenValue, majorEigenVector};
    ret[1] = {middleEigenValue, middleEigenVector};
    ret[2] = {minorEigenValue, minorEigenVector};

    return ret;
}

std::array<double, 3> TensorField3D::getSortedEigenValuesForTensor(const size_t index) const {
    auto ret = std::array<double, 3>();

    const auto &majorEigenValues = getMetaData<MajorEigenValues>();
    const auto &middleEigenValues = getMetaData<IntermediateEigenValues>();
    const auto &minorEigenValues = getMetaData<MinorEigenValues>();

    ret[0] = majorEigenValues[index];
    ret[1] = middleEigenValues[index];
    ret[2] = minorEigenValues[index];
    return ret;
}

std::array<double, 3> TensorField3D::getSortedEigenValuesForTensor(const size3_t &pos) const {
    auto index = indexMapper_(pos);
    auto ret = std::array<double, 3>();

    const auto &majorEigenValues = getMetaData<MajorEigenValues>();
    const auto &middleEigenValues = getMetaData<IntermediateEigenValues>();
    const auto &minorEigenValues = getMetaData<MinorEigenValues>();

    ret[0] = majorEigenValues[index];
    ret[1] = middleEigenValues[index];
    ret[2] = minorEigenValues[index];

    return ret;
}

std::array<dvec3, 3> TensorField3D::getSortedEigenVectorsForTensor(const size_t index) const {
    auto ret = std::array<dvec3, 3>();

    const auto &majorEigenVectors = getMetaData<MajorEigenVectors>();
    const auto &middleEigenVectors = getMetaData<IntermediateEigenVectors>();
    const auto &minorEigenVectors = getMetaData<MinorEigenVectors>();

    ret[0] = majorEigenVectors[index];
    ret[1] = middleEigenVectors[index];
    ret[2] = minorEigenVectors[index];

    return ret;
}

std::array<dvec3, 3> TensorField3D::getSortedEigenVectorsForTensor(const size3_t &pos) const {
    auto index = indexMapper_(pos);
    auto ret = std::array<dvec3, 3>();

    const auto &majorEigenVectors = getMetaData<MajorEigenVectors>();
    const auto &middleEigenVectors = getMetaData<IntermediateEigenVectors>();
    const auto &minorEigenVectors = getMetaData<MinorEigenVectors>();

    ret[0] = majorEigenVectors[index];
    ret[1] = middleEigenVectors[index];
    ret[2] = minorEigenVectors[index];
    return ret;
}

const std::vector<dvec3> &TensorField3D::majorEigenVectors() const {
    return getMetaData<MajorEigenVectors>();
}

const std::vector<dvec3> &TensorField3D::middleEigenVectors() const {
    return getMetaData<IntermediateEigenVectors>();
}

const std::vector<dvec3> &TensorField3D::minorEigenVectors() const {
    return getMetaData<MinorEigenVectors>();
}

const std::vector<double> &TensorField3D::majorEigenValues() const {
    return getMetaData<MajorEigenValues>();
}

const std::vector<double> &TensorField3D::middleEigenValues() const {
    return getMetaData<IntermediateEigenValues>();
}

const std::vector<double> &TensorField3D::minorEigenValues() const {
    return getMetaData<MinorEigenValues>();
}

const std::vector<dmat3> &TensorField3D::tensors() const { return tensors_; }

int TensorField3D::getNumDefinedEntries() const {
    return static_cast<int>(std::count(std::begin(binaryMask_), std::end(binaryMask_), 1));
}

bool TensorField3D::hasMetaData(const TensorFeature feature) const {
    switch (feature) {
        case TensorFeature::I1:
            return hasMetaData<I1>();
        case TensorFeature::I2:
            return hasMetaData<I2>();
        case TensorFeature::I3:
            return hasMetaData<I3>();
        case TensorFeature::J1:
            return hasMetaData<J1>();
        case TensorFeature::J2:
            return hasMetaData<J2>();
        case TensorFeature::J3:
            return hasMetaData<J3>();
        case TensorFeature::Sigma1:
            return hasMetaData<MajorEigenValues>();
        case TensorFeature::Sigma2:
            return hasMetaData<IntermediateEigenValues>();
        case TensorFeature::Sigma3:
            return hasMetaData<MinorEigenValues>();
        case TensorFeature::MajorEigenVector:
            return hasMetaData<MajorEigenVectors>();
        case TensorFeature::IntermediateEigenVector:
            return hasMetaData<IntermediateEigenVectors>();
        case TensorFeature::MinorEigenVector:
            return hasMetaData<MinorEigenVectors>();
        case TensorFeature::LodeAngle:
            return hasMetaData<LodeAngle>();
        case TensorFeature::Anisotropy:
            return hasMetaData<Anisotropy>();
        case TensorFeature::LinearAnisotropy:
            return hasMetaData<LinearAnisotropy>();
        case TensorFeature::PlanarAnisotropy:
            return hasMetaData<PlanarAnisotropy>();
        case TensorFeature::SphericalAnisotropy:
            return hasMetaData<SphericalAnisotropy>();
        case TensorFeature::Diffusivity:
            return hasMetaData<Diffusivity>();
        case TensorFeature::ShearStress:
            return hasMetaData<ShearStress>();
        case TensorFeature::PureShear:
            return hasMetaData<PureShear>();
        case TensorFeature::ShapeFactor:
            return hasMetaData<ShapeFactor>();
        case TensorFeature::IsotropicScaling:
            return hasMetaData<IsotropicScaling>();
        case TensorFeature::Rotation:
            return hasMetaData<Rotation>();
        case TensorFeature::FrobeniusNorm:
            return hasMetaData<FrobeniusNorm>();
        case TensorFeature::HillYieldCriterion:
            return hasMetaData<HillYieldCriterion>();
        default:
            break;
    }

    return false;
}

bool TensorField3D::hasMetaData(const uint64_t id) const {
    return metaData_.find(id) != metaData_.end();
}

std::shared_ptr<TensorField3D> TensorField3D::clone() const {
    auto tensorField = std::make_shared<TensorField3D>(dimensions_, tensors_, metaData_, extent_);
    tensorField->setOffset(getOffset());

    if (hasMask()) tensorField->setMask(binaryMask_);

    return tensorField;
}

void TensorField3D::computeEigenValuesAndEigenVectors() {
    auto func = [](const dmat3 &tensor) -> std::array<std::pair<double, dvec3>, 3> {
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

    addMetaData<MajorEigenValues>(majorEigenValues, TensorFeature::Sigma1);
    addMetaData<IntermediateEigenValues>(middleEigenValues, TensorFeature::Sigma2);
    addMetaData<MinorEigenValues>(minorEigenValues, TensorFeature::Sigma3);

    addMetaData<MajorEigenVectors>(majorEigenVectors, TensorFeature::MajorEigenVector);
    addMetaData<IntermediateEigenVectors>(middleEigenVectors,
                                          TensorFeature::IntermediateEigenVector);
    addMetaData<MinorEigenVectors>(minorEigenVectors, TensorFeature::MinorEigenVector);
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
    const auto &majorEigenValues = getMetaData<MajorEigenValues>();
    const auto &middleEigenValues = getMetaData<IntermediateEigenValues>();
    const auto &minorEigenValues = getMetaData<MinorEigenValues>();

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

    auto min_f = [](const dvec3 &vec) -> double { return std::min(std::min(vec.x, vec.y), vec.z); };
    auto max_f = [](const dvec3 &vec) -> double { return std::max(std::max(vec.x, vec.y), vec.z); };

    auto min = std::numeric_limits<double>::max();
    auto max = std::numeric_limits<double>::lowest();
    for (const auto &v : getMetaData<MajorEigenVectors>()) {
        min = std::min(min_f(v), min);
        max = std::max(max_f(v), max);
    }
    dataMapEigenVectors_[0].dataRange.x = dataMapEigenVectors_[0].valueRange.x = min;
    dataMapEigenVectors_[0].dataRange.y = dataMapEigenVectors_[0].valueRange.y = max;

    min = std::numeric_limits<double>::max();
    max = std::numeric_limits<double>::lowest();
    for (const auto &v : getMetaData<IntermediateEigenVectors>()) {
        min = std::min(min_f(v), min);
        max = std::max(max_f(v), max);
    }
    dataMapEigenVectors_[1].dataRange.x = dataMapEigenVectors_[1].valueRange.x = min;
    dataMapEigenVectors_[1].dataRange.y = dataMapEigenVectors_[1].valueRange.y = max;

    min = std::numeric_limits<double>::max();
    max = std::numeric_limits<double>::lowest();
    for (const auto &v : getMetaData<MinorEigenVectors>()) {
        min = std::min(min_f(v), min);
        max = std::max(max_f(v), max);
    }
    dataMapEigenVectors_[2].dataRange.x = dataMapEigenVectors_[2].valueRange.x = min;
    dataMapEigenVectors_[2].dataRange.y = dataMapEigenVectors_[2].valueRange.y = max;
}

}  // namespace inviwo

#if ((__GNUC__ > 3) && (__GNUC_MINOR__ > 3))
#pragma GCC pop_options
#endif
