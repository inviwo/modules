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

#include <inviwo/tensorvisbase/datastructures/tensorfield2d.h>
#include <inviwo/core/datastructures/image/imageram.h>

namespace inviwo {
TensorField2D::TensorField2D(const sizeN_t& dimensions, const std::vector<matN>& tensors,
                             std::shared_ptr<DataFrame> metaData)
    : TensorField<2, float>(dimensions, tensors, metaData) {
    initializeDefaultMetaData();
    computeDataMaps();
    computeNormalizedScreenCoordinates();
}

TensorField2D::TensorField2D(const sizeN_t& dimensions, std::shared_ptr<std::vector<matN>> tensors,
                             std::shared_ptr<DataFrame> metaData)
    : TensorField<2, float>(dimensions, tensors, metaData) {
    initializeDefaultMetaData();
    computeDataMaps();
    computeNormalizedScreenCoordinates();
}

TensorField2D::TensorField2D(const TensorField2D& tf)
    : TensorField<2, float>(tf), normalizedImagePositions_(tf.normalizedImagePositions_) {
    initializeDefaultMetaData();
    computeDataMaps();
}

TensorField2D* TensorField2D::clone() const { return new TensorField2D(*this); }

TensorField2D* TensorField2D::deepCopy() const {
    auto tf = new TensorField2D(*this);

    tf->setTensors(std::make_shared<std::vector<matN>>(*tensors_));
    tf->setMetaData(std::make_shared<DataFrame>(*metaData_));

    return tf;
}

std::shared_ptr<Image> TensorField2D::getImageRepresentation() const {
    using layer_type = glm::vec<4, value_type>;
    auto dataFormat = new DataFormat<layer_type>();
    auto layer = std::make_shared<Layer>(dimensions_, dataFormat);
    auto data =
        static_cast<LayerRAMPrecision<layer_type>*>(layer->getEditableRepresentation<LayerRAM>())
            ->getDataTyped();

    for (size_t i{0}; i < glm::compMul(dimensions_); ++i) {
        const auto& tensor = at(i);
        data[i] = vec4(tensor[0][0], tensor[1][0], tensor[0][1], tensor[1][1]);
    }

    return std::make_shared<Image>(layer);
}

const dvec2& TensorField2D::getNormalizedImagePosition(const size_t index) const {
    return normalizedImagePositions_[index];
}

const std::vector<dvec2>& TensorField2D::normalizedImagePositions() const {
    return normalizedImagePositions_;
}

void TensorField2D::initializeDefaultMetaData() {
    // clang-format off
    if (this->hasMetaData<attributes::MajorEigenValue>() &&
        this->hasMetaData<attributes::MinorEigenValue>() &&
        this->hasMetaData<attributes::MajorEigenVector2D>() &&
        this->hasMetaData<attributes::MinorEigenVector2D>()) {
        return;
    }
    // clang-format on

    auto newMetaData = std::make_shared<DataFrame>(*metaData_);

    auto func = [](const matN& tensor) -> std::array<std::pair<value_type, vecN>, 2> {
        if (tensor == matN(0)) {
            return {std::make_pair(value_type{0}, vecN{0}), std::make_pair(value_type{0}, vecN{0})};
        }

        Eigen::EigenSolver<Eigen::Matrix2f> solver(util::glm2eigen(tensor));
        const auto eigenValues = util::eigen2glm<value_type, 2, 1>(solver.eigenvalues().real());
        const auto eigenVectors = util::eigen2glm<value_type, 2, 2>(solver.eigenvectors().real());

        const auto range =
            util::as_range(glm::value_ptr(eigenValues), glm::value_ptr(eigenValues) + 2);

        const auto ordering = util::ordering(range, std::greater<float>());

        return {{std::make_pair(eigenValues[ordering[0]], eigenVectors[ordering[0]]),
                 std::make_pair(eigenValues[ordering[1]], eigenVectors[ordering[1]])}};
    };

    std::vector<value_type> majorEigenValues;
    std::vector<value_type> minorEigenValues;

    std::vector<vecN> majorEigenVectors;
    std::vector<vecN> intermediateEigenVectors;
    std::vector<vecN> minorEigenVectors;

    majorEigenValues.resize(size_);
    minorEigenValues.resize(size_);

    majorEigenVectors.resize(size_);
    minorEigenVectors.resize(size_);

    const auto& t_ref = *tensors_;

#pragma omp parallel for
    for (int i = 0; i < static_cast<int>(tensors_->size()); i++) {
        auto tensor = t_ref[i];
        auto eigenValuesAndEigenVectors = func(tensor);

        majorEigenVectors[i] = eigenValuesAndEigenVectors[0].second;
        minorEigenVectors[i] = eigenValuesAndEigenVectors[1].second;

        majorEigenValues[i] = eigenValuesAndEigenVectors[0].first;
        minorEigenValues[i] = eigenValuesAndEigenVectors[1].first;
    }

    addIfNotPresent<attributes::MajorEigenValue>(newMetaData, majorEigenValues);
    addIfNotPresent<attributes::MinorEigenValue>(newMetaData, minorEigenValues);
    addIfNotPresent<attributes::MajorEigenVector2D>(newMetaData, majorEigenVectors);
    addIfNotPresent<attributes::MinorEigenVector2D>(newMetaData, minorEigenVectors);

    newMetaData->updateIndexBuffer();

    metaData_ = newMetaData;
}

void TensorField2D::computeDataMaps() {
    const auto& majorEigenValues = this->majorEigenValues();
    const auto& minorEigenValues = this->minorEigenValues();

    dataMapEigenValues_[0].dataRange.x = dataMapEigenValues_[0].valueRange.x =
        *std::min_element(majorEigenValues.begin(), majorEigenValues.end());
    dataMapEigenValues_[0].dataRange.y = dataMapEigenValues_[0].valueRange.y =
        *std::max_element(majorEigenValues.begin(), majorEigenValues.end());
    dataMapEigenValues_[1].dataRange.x = dataMapEigenValues_[1].valueRange.x =
        *std::min_element(minorEigenValues.begin(), minorEigenValues.end());
    dataMapEigenValues_[1].dataRange.y = dataMapEigenValues_[1].valueRange.y =
        *std::max_element(minorEigenValues.begin(), minorEigenValues.end());

    auto min_f = [](const vecN& vec) -> value_type { return std::min(vec.x, vec.y); };
    auto max_f = [](const vecN& vec) -> value_type { return std::max(vec.x, vec.y); };

    auto min = std::numeric_limits<value_type>::max();
    auto max = std::numeric_limits<value_type>::lowest();

    for (const auto& v : this->majorEigenVectors()) {
        min = std::min(min_f(v), min);
        max = std::max(max_f(v), max);
    }

    dataMapEigenVectors_[0].dataRange.x = dataMapEigenVectors_[0].valueRange.x = min;
    dataMapEigenVectors_[0].dataRange.y = dataMapEigenVectors_[0].valueRange.y = max;

    min = std::numeric_limits<value_type>::max();
    max = std::numeric_limits<value_type>::lowest();
    for (const auto& v : this->minorEigenVectors()) {
        min = std::min(min_f(v), min);
        max = std::max(max_f(v), max);
    }
    dataMapEigenVectors_[1].dataRange.x = dataMapEigenVectors_[1].valueRange.x = min;
    dataMapEigenVectors_[1].dataRange.y = dataMapEigenVectors_[1].valueRange.y = max;
}

void TensorField2D::computeNormalizedScreenCoordinates() {
    normalizedImagePositions_.resize(size_);

    auto xFrac = 1.0 / static_cast<double>(dimensions_.x - 1);
    auto yFrac = 1.0 / static_cast<double>(dimensions_.y - 1);

    if (dimensions_.x == 0 || dimensions_.y == 0) {
        LogError("Tensor field 2D has at least one zero-sized dimension!");
        return;
    }

    for (size_t y = 0; y < dimensions_.y; y++) {
        for (size_t x = 0; x < dimensions_.x; x++) {
            normalizedImagePositions_[indexMapper_(size2_t(x, y))] =
                dvec2(dimensions_.x < 2 ? 0. : x * xFrac, dimensions_.y < 2 ? 0. : y * yFrac);
        }
    }
}
}  // namespace inviwo
