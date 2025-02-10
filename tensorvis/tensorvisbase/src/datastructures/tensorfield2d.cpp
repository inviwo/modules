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

#include <inviwo/tensorvisbase/datastructures/tensorfield2d.h>
#include <inviwo/core/datastructures/image/imageram.h>
#include <modules/eigenutils/eigenutils.h>

namespace inviwo {
TensorField2D::TensorField2D(const size2_t dimensions, const std::vector<dmat2>& data,
                             const dvec2& extent)
    : dimensions_(dimensions)
    , extent_(extent)
    , size_(dimensions.x * dimensions.y)
    , rank_(2)
    , dimensionality_(2)
    , indexMapper_(dimensions)
    , tensors_(data) {
    computeEigenValuesAndEigenVectors();
    computeNormalizedScreenCoordinates();
}

TensorField2D::TensorField2D(const size2_t dimensions, const std::vector<double>& data,
                             const dvec2& extent)
    : dimensions_(dimensions)
    , extent_(extent)
    , size_(dimensions.x * dimensions.y)
    , rank_(2)
    , dimensionality_(2)
    , indexMapper_(dimensions) {
    for (size_t i = 0; i < data.size(); i += 4) {
        dmat2 tensor;
        tensor[0][0] = data[i];
        tensor[1][0] = data[i + 1];
        tensor[0][1] = data[i + 2];
        tensor[1][1] = data[i + 3];

        tensors_.push_back(tensor);
    }

    computeEigenValuesAndEigenVectors();
    computeNormalizedScreenCoordinates();
}

TensorField2D::TensorField2D(const size2_t dimensions, const std::vector<double>& data,
                             const std::vector<double>& majorEigenvalues,
                             const std::vector<double>& minorEigenvalues,
                             const std::vector<dvec2>& majorEigenvectors,
                             const std::vector<dvec2>& minorEigenvectors, const dvec2& extent)
    : majorEigenVectors_(majorEigenvectors)
    , minorEigenVectors_(minorEigenvectors)
    , majorEigenValues_(majorEigenvalues)
    , minorEigenValues_(minorEigenvalues)
    , dimensions_(dimensions)
    , extent_(extent)
    , offset_(dvec2(0.0))
    , size_(dimensions.x * dimensions.y)
    , rank_(2)
    , dimensionality_(2)
    , indexMapper_(dimensions) {
    for (size_t i = 0; i < data.size(); i += 4) {
        dmat3 tensor;
        tensor[0][0] = data[i];
        tensor[1][0] = data[i + 1];
        tensor[0][1] = data[i + 2];
        tensor[1][1] = data[i + 3];

        tensors_.emplace_back(tensor);
    }
    computeNormalizedScreenCoordinates();
}

TensorField2D::TensorField2D(const size2_t dimensions, const std::vector<dmat2>& data,
                             const std::vector<double>& majorEigenvalues,
                             const std::vector<double>& minorEigenvalues,
                             const std::vector<dvec2>& majorEigenvectors,
                             const std::vector<dvec2>& minorEigenvectors, const dvec2& extent)
    : majorEigenVectors_(majorEigenvectors)
    , minorEigenVectors_(minorEigenvectors)
    , majorEigenValues_(majorEigenvalues)
    , minorEigenValues_(minorEigenvalues)
    , dimensions_(dimensions)
    , extent_(extent)
    , offset_(dvec2(0.0))
    , size_(dimensions.x * dimensions.y)
    , rank_(2)
    , dimensionality_(2)
    , indexMapper_(dimensions)
    , tensors_(data) {
    computeNormalizedScreenCoordinates();
}

TensorField2D::TensorField2D(const size_t x, const size_t y, const std::vector<dmat2>& data,
                             const dvec2& extent)
    : dimensions_(size2_t(x, y))
    , extent_(extent)
    , size_(x * y)
    , rank_(2)
    , dimensionality_(2)
    , indexMapper_(size2_t(x, y))
    , tensors_(data) {
    computeEigenValuesAndEigenVectors();
    computeNormalizedScreenCoordinates();
}

TensorField2D::TensorField2D(const size_t x, const size_t y, const std::vector<double>& data,
                             const dvec2& extent)
    : dimensions_(size2_t(x, y))
    , extent_(extent)
    , size_(x * y)
    , rank_(2)
    , dimensionality_(2)
    , indexMapper_(size2_t(x, y)) {
    for (size_t i = 0; i < data.size(); i += 4) {
        dmat2 tensor;
        tensor[0][0] = data[i];
        tensor[1][0] = data[i + 1];
        tensor[0][1] = data[i + 2];
        tensor[1][1] = data[i + 3];

        tensors_.push_back(tensor);
    }

    computeEigenValuesAndEigenVectors();
    computeNormalizedScreenCoordinates();
}

TensorField2D::TensorField2D(const size_t x, const size_t y, const std::vector<double>& data,
                             const std::vector<double>& majorEigenvalues,
                             const std::vector<double>& minorEigenvalues,
                             const std::vector<dvec2>& majorEigenvectors,
                             const std::vector<dvec2>& minorEigenvectors, const dvec2& extent)
    : majorEigenVectors_(majorEigenvectors)
    , minorEigenVectors_(minorEigenvectors)
    , majorEigenValues_(majorEigenvalues)
    , minorEigenValues_(minorEigenvalues)
    , dimensions_(size2_t(x, y))
    , extent_(extent)
    , offset_(dvec3(0.0))
    , size_(x * y)
    , rank_(2)
    , dimensionality_(2)
    , indexMapper_(size2_t(x, y)) {
    for (size_t i = 0; i < data.size(); i += 4) {
        dmat3 tensor;
        tensor[0][0] = data[i];
        tensor[1][0] = data[i + 1];
        tensor[0][1] = data[i + 2];
        tensor[1][1] = data[i + 3];

        tensors_.emplace_back(tensor);
    }
    computeNormalizedScreenCoordinates();
}

TensorField2D::TensorField2D(const size_t x, const size_t y, const std::vector<dmat2>& data,
                             const std::vector<double>& majorEigenvalues,
                             const std::vector<double>& minorEigenvalues,
                             const std::vector<dvec2>& majorEigenvectors,
                             const std::vector<dvec2>& minorEigenvectors, const dvec2& extent)
    : majorEigenVectors_(majorEigenvectors)
    , minorEigenVectors_(minorEigenvectors)
    , majorEigenValues_(majorEigenvalues)
    , minorEigenValues_(minorEigenvalues)
    , dimensions_(size2_t(x, y))
    , extent_(extent)
    , offset_(dvec3(0.0))
    , size_(x * y)
    , rank_(2)
    , dimensionality_(2)
    , indexMapper_(size2_t(x, y))
    , tensors_(data) {
    computeNormalizedScreenCoordinates();
}

Document TensorField2D::getDataInfo() const {
    using P = Document::PathComponent;
    using H = utildoc::TableBuilder::Header;
    Document doc;
    utildoc::TableBuilder tb(doc.handle(), P::end());

    tb(H("Type"), "2D tensor field");
    tb(H("Number of Tensors"), tensors_.size());
    tb(H("Dimensions"), dimensions_);
    tb(H("Extent"), extent_);

    tb(H("Max Eigenvalue (major)"),
       *std::max_element(majorEigenValues_.begin(), majorEigenValues_.end()));
    tb(H("Min Eigenvalue (major)"),
       *std::min_element(majorEigenValues_.begin(), majorEigenValues_.end()));
    tb(H("Max Eigenvalue (minor)"),
       *std::max_element(minorEigenValues_.begin(), minorEigenValues_.end()));
    tb(H("Min Eigenvalue (minor)"),
       *std::min_element(minorEigenValues_.begin(), minorEigenValues_.end()));

    return doc;
}

std::shared_ptr<Image> TensorField2D::getImageRepresentation() const {
    std::shared_ptr<Image> ret = std::make_shared<Image>(dimensions_, DataVec4Float32::get());

    auto colorLayer = ret->getEditableRepresentation<ImageRAM>()->getColorLayerRAM();

    for (size_t y = 0; y < dimensions_.y; y++) {
        for (size_t x = 0; x < dimensions_.x; x++) {
            auto tensor = at(x, y);

            colorLayer->setFromDVec4(size2_t(x, y),
                                     dvec4(tensor[0][0], tensor[1][0], tensor[0][1], tensor[1][1]));
        }
    }

    return ret;
}

dmat2& TensorField2D::at(const size2_t position) { return tensors_[indexMapper_(position)]; }

dmat2& TensorField2D::at(const size_t x, const size_t y) {
    return tensors_[indexMapper_(size2_t(x, y))];
}

dmat2& TensorField2D::at(const size_t index) { return tensors_[index]; }

const dmat2& TensorField2D::at(const size2_t position) const {
    return tensors_[indexMapper_(position)];
}

const dmat2& TensorField2D::at(const size_t x, const size_t y) const {
    return tensors_[indexMapper_(size2_t(x, y))];
}

const dmat2& TensorField2D::at(const size_t index) const { return tensors_[index]; }

size_t TensorField2D::getSize() const { return size_; }

dmat2 TensorField2D::getBasis() const {
    return dmat2(dvec2(extent_.x, 0.0), dvec2(0.0, extent_.y));
}

dmat3 TensorField2D::getBasisAndOffset() const {
    auto basis = dmat2(dvec2(extent_.x, 0.0), dvec2(0.0, extent_.y));

    dmat3 modelMatrix;

    modelMatrix[0][0] = basis[0][0];
    modelMatrix[0][1] = basis[0][1];
    modelMatrix[1][0] = basis[1][0];
    modelMatrix[1][1] = basis[1][1];

    modelMatrix[2][0] = offset_[0];
    modelMatrix[2][1] = offset_[1];

    modelMatrix[0][2] = 0.0;
    modelMatrix[1][2] = 0.0;
    modelMatrix[2][2] = 1.0;

    return modelMatrix;
}

const dvec2& TensorField2D::getNormalizedImagePosition(const size_t index) const {
    return normalizedImagePositions_[index];
}

const dvec2& TensorField2D::getMinorEigenVector(const size_t index) const {
    return minorEigenVectors_[index];
}

const dvec2& TensorField2D::getMinorEigenVector(const size2_t& position) const {
    return minorEigenVectors_[indexMapper_(position)];
}

const dvec2& TensorField2D::getMajorEigenVector(const size_t index) const {
    return majorEigenVectors_[index];
}

const dvec2& TensorField2D::getMajorEigenVector(const size2_t& position) const {
    return majorEigenVectors_[indexMapper_(position)];
}

const std::vector<dvec2>& TensorField2D::minorEigenVectors() const { return minorEigenVectors_; }

const std::vector<dvec2>& TensorField2D::majorEigenVectors() const { return majorEigenVectors_; }

const std::vector<double>& TensorField2D::minorEigenValues() const { return minorEigenValues_; }

const std::vector<double>& TensorField2D::majorEigenValues() const { return majorEigenValues_; }

const std::vector<dvec2>& TensorField2D::normalizedImagePositions() const {
    return normalizedImagePositions_;
}

const std::vector<dmat2>& TensorField2D::tensors() const { return tensors_; }

std::array<std::pair<double, dvec2>, 2> TensorField2D::getSortedEigenValuesAndEigenVectorsForTensor(
    const size_t index) const {
    const auto& majorEigenValue = majorEigenValues_[index];
    const auto& minorEigenValue = minorEigenValues_[index];

    const auto& majorEigenVector = majorEigenVectors_[index];
    const auto& minorEigenVector = minorEigenVectors_[index];

    std::array<std::pair<double, dvec2>, 2> ret;
    ret[0] = {majorEigenValue, majorEigenVector};
    ret[1] = {minorEigenValue, minorEigenVector};

    return ret;
}

std::array<std::pair<double, dvec2>, 2> TensorField2D::getSortedEigenValuesAndEigenVectorsForTensor(
    const size2_t& pos) const {
    auto index = indexMapper_(pos);

    const auto& majorEigenValue = majorEigenValues_[index];
    const auto& minorEigenValue = minorEigenValues_[index];

    const auto& majorEigenVector = majorEigenVectors_[index];
    const auto& minorEigenVector = minorEigenVectors_[index];

    std::array<std::pair<double, dvec2>, 2> ret;
    ret[0] = {majorEigenValue, majorEigenVector};
    ret[1] = {minorEigenValue, minorEigenVector};

    return ret;
}

std::array<double, 2> TensorField2D::getSortedEigenValuesForTensor(const size_t index) const {
    auto ret = std::array<double, 2>();
    ret[0] = majorEigenValues_[index];
    ret[1] = minorEigenValues_[index];
    return ret;
}

std::array<double, 2> TensorField2D::getSortedEigenValuesForTensor(const size2_t& pos) const {
    auto index = indexMapper_(pos);

    auto ret = std::array<double, 2>();
    ret[0] = majorEigenValues_[index];
    ret[1] = minorEigenValues_[index];
    return ret;
}

std::array<dvec2, 2> TensorField2D::getSortedEigenVectorsForTensor(const size_t index) const {
    auto ret = std::array<dvec2, 2>();
    ret[0] = majorEigenVectors_[index];
    ret[1] = minorEigenVectors_[index];
    return ret;
}

std::array<dvec2, 2> TensorField2D::getSortedEigenVectorsForTensor(const size2_t& pos) const {
    auto index = indexMapper_(pos);

    auto ret = std::array<dvec2, 2>();
    ret[0] = majorEigenVectors_[index];
    ret[1] = minorEigenVectors_[index];
    return ret;
}

void TensorField2D::computeEigenValuesAndEigenVectors() {
    auto func = [](const dmat2& tensor) -> std::array<std::pair<double, dvec2>, 2> {
        Eigen::EigenSolver<Eigen::Matrix<double, 2, 2>> solver(util::glm2eigen(tensor));

        auto lambda1 = solver.eigenvalues().col(0)[0].real();
        auto lambda2 = solver.eigenvalues().col(0)[1].real();

        auto ev1 =
            dvec2(solver.eigenvectors().col(0).real()[0], solver.eigenvectors().col(0).real()[1]);
        auto ev2 =
            dvec2(solver.eigenvectors().col(1).real()[0], solver.eigenvectors().col(1).real()[1]);

        std::array<std::pair<double, dvec2>, 2> sortable;
        sortable[0] = {lambda1, ev1};
        sortable[1] = {lambda2, ev2};

        std::sort(sortable.begin(), sortable.end(),
                  [](const std::pair<double, dvec2>& pairA, const std::pair<double, dvec2>& pairB) {
                      return pairA.first > pairB.first;
                  });

        return sortable;
    };

    majorEigenValues_.resize(size_);
    minorEigenValues_.resize(size_);
    majorEigenVectors_.resize(size_);
    minorEigenVectors_.resize(size_);

#pragma omp parallel for
    for (int i = 0; i < static_cast<int>(tensors_.size()); i++) {
        auto tensor = tensors_[i];

        auto eigenValuesAndEigenVectors = func(tensor);

        majorEigenVectors_[i] = eigenValuesAndEigenVectors[0].second;
        minorEigenVectors_[i] = eigenValuesAndEigenVectors[1].second;

        majorEigenValues_[i] = eigenValuesAndEigenVectors[0].first;
        minorEigenValues_[i] = eigenValuesAndEigenVectors[1].first;
    }
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
