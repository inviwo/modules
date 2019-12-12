#include <inviwo/tensorvisbase/datastructures/tensorfield3d.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/core/util/stdextensions.h>
#include <modules/eigenutils/eigenutils.h>
#include <inviwo/tensorvisbase/util/misc.h>
#include <inviwo/core/util/exception.h>

namespace inviwo {

TensorField3D::TensorField3D(const size3_t &dimensions, const std::vector<mat3> &tensors)
    : dimensions_(dimensions)
    , tensors_(std::make_shared<std::vector<mat3>>(tensors))
    , indexMapper_(util::IndexMapper3D(dimensions)) {}

TensorField3D::TensorField3D(const size3_t &dimensions, const std::vector<mat3> &tensors,
                             const DataFrame &metaData)
    : dimensions_(dimensions)
    , indexMapper_(util::IndexMapper3D(dimensions))
    , tensors_(std::make_shared<std::vector<mat3>>(tensors))
    , metaData_(std::make_shared<DataFrame>(metaData)) {}

TensorField3D::TensorField3D(const size3_t &dimensions,
                             std::shared_ptr<const std::vector<mat3>> tensors,
                             std::shared_ptr<const DataFrame> metaData)
    : dimensions_(dimensions)
    , indexMapper_(util::IndexMapper3D(dimensions))
    , tensors_(tensors)
    , metaData_(metaData) {}

TensorField3D::TensorField3D(const TensorField3D &tf)
    : StructuredGridEntity<3>()
    , dataMapEigenValues_(tf.dataMapEigenValues_)
    , dataMapEigenVectors_(tf.dataMapEigenVectors_)
    , dimensions_(tf.dimensions_)
    , indexMapper_(util::IndexMapper3D(dimensions_))
    , tensors_(tf.tensors_)
    , size_(tf.size_)
    , rank_(tf.rank_)
    , dimensionality_(tf.dimensionality_)
    , binaryMask_(tf.binaryMask_) {
    setOffset(tf.getOffset());
    setBasis(tf.getBasis());
}

std::string TensorField3D::getDataInfo() const {
    std::stringstream ss;
    ss << "<table border='0' cellspacing='0' cellpadding='0' "
          "style='border-color:white;white-space:pre;'>/n"
       << tensorutil::getHTMLTableRowString("Type", "3D tensor field")
       << tensorutil::getHTMLTableRowString("Number of tensors", tensors_->size())
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
       << tensorutil::getHTMLTableRowString("Extends", getExtents())

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
                auto tensor = at(size3_t(x, y, z));
                XXYYZZ->setFromDVec3(size3_t(x, y, z),
                                     dvec3(tensor[0][0], tensor[1][1], tensor[2][2]));

                XYYZXZ->setFromDVec3(size3_t(x, y, y),
                                     dvec3(tensor[1][0], tensor[2][1], tensor[2][0]));
            }
        }
    }

    return ret;
}

void TensorField3D::setExtents(const vec3 &extents) {
    auto basis = getBasis();
    basis[0] = glm::normalize(basis[0]) * extents[0];
    basis[1] = glm::normalize(basis[1]) * extents[1];
    basis[2] = glm::normalize(basis[2]) * extents[2];
    setBasis(basis);
}

mat4 TensorField3D::getBasisAndOffset() const {
    auto basis = getBasis();
    auto offset = getOffset();

    mat4 modelMatrix;

    modelMatrix[0][0] = basis[0][0];
    modelMatrix[0][1] = basis[0][1];
    modelMatrix[0][2] = basis[0][2];

    modelMatrix[1][0] = basis[1][0];
    modelMatrix[1][1] = basis[1][1];
    modelMatrix[1][2] = basis[1][2];

    modelMatrix[2][0] = basis[2][0];
    modelMatrix[2][1] = basis[2][1];
    modelMatrix[2][2] = basis[2][2];

    modelMatrix[3][0] = offset[0];
    modelMatrix[3][1] = offset[1];
    modelMatrix[3][2] = offset[2];

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

std::shared_ptr<std::vector<mat3>> TensorField3D::tensors() const { return tensors_; }

int TensorField3D::getNumDefinedEntries() const {
    return static_cast<int>(std::count(std::begin(binaryMask_), std::end(binaryMask_), 1));
}

TensorField3D *TensorField3D::clone() const { return new TensorField3D(*this); }

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
    for (int i = 0; i < static_cast<int>(tensors_->size()); i++) {
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

vec3 TensorField3D::getNormalizedVolumePosition(const size_t index, const double sliceCoord) const {
    auto stepSize = getSpacing();
    const auto pos = indexMapper_(index);
    return vec3(dimensions_.x < 2 ? sliceCoord : pos.x * stepSize.x,
                dimensions_.y < 2 ? sliceCoord : pos.y * stepSize.y,
                dimensions_.z < 2 ? sliceCoord : pos.z * stepSize.z);
}

std::vector<vec3> TensorField3D::getNormalizedScreenCoordinates(double sliceCoord) {
    std::vector<vec3> normalizedVolumePositions;
    normalizedVolumePositions.resize(size_);

    auto stepSize = getSpacing();

    if (dimensions_.x == 0 || dimensions_.y == 0 || dimensions_.z == 0) {
        LogError("Tensor field 3D has at least one zero-sized dimension!");
        return;
    }

    for (size_t z = 0; z < dimensions_.z; z++) {
        for (size_t y = 0; y < dimensions_.y; y++) {
            for (size_t x = 0; x < dimensions_.x; x++) {
                const auto index = indexMapper_(size3_t(x, y, z));
                normalizedVolumePositions[index] = getNormalizedVolumePosition(index, sliceCoord);
            }
        }
    }

    return normalizedVolumePositions;
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
