/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2021 Inviwo Foundation
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
#include <inviwo/tensorvisbase/tensorvisbasemoduledefine.h>
#include <inviwo/core/datastructures/image/image.h>
#include <inviwo/core/datastructures/buffer/buffer.h>
#include <inviwo/tensorvisbase/util/tensorutil.h>
#include <inviwo/core/util/indexmapper.h>
#include <inviwo/core/datastructures/datatraits.h>
#include <inviwo/core/util/document.h>

#include <warn/push>
#include <warn/ignore/all>
#include <Eigen/Dense>
#include <warn/pop>

namespace inviwo {
/**
 * \class TensorField2D
 * \brief Data structure for 2D tensorfields.
 */
class IVW_MODULE_TENSORVISBASE_API TensorField2D {
public:
    TensorField2D() = delete;

    // Contructors with ready tensors
    TensorField2D(size2_t dimensions, const std::vector<dmat2>& data,
                  const dvec2& extends = dvec2(1.0));
    TensorField2D(size_t x, size_t y, const std::vector<dmat2>& data,
                  const dvec2& extends = dvec2(1.0));
    TensorField2D(size2_t dimensions, const std::vector<dmat2>& data,
                  const std::vector<double>& majorEigenvalues,
                  const std::vector<double>& minorEigenvalues,
                  const std::vector<dvec2>& majorEigenvectors,
                  const std::vector<dvec2>& minorEigenvectors, const dvec2& extends = dvec2(1.0));
    TensorField2D(size_t x, size_t y, const std::vector<dmat2>& data,
                  const std::vector<double>& majorEigenvalues,
                  const std::vector<double>& minorEigenvalues,
                  const std::vector<dvec2>& majorEigenvectors,
                  const std::vector<dvec2>& minorEigenvectors, const dvec2& extends = dvec2(1.0));

    // Constructors with raw data
    TensorField2D(size2_t dimensions, const std::vector<double>& data,
                  const dvec2& extends = dvec2(1.0));
    TensorField2D(size_t x, size_t y, const std::vector<double>& data,
                  const dvec2& extends = dvec2(1.0));
    TensorField2D(size2_t dimensions, const std::vector<double>& data,
                  const std::vector<double>& majorEigenvalues,
                  const std::vector<double>& minorEigenvalues,
                  const std::vector<dvec2>& majorEigenvectors,
                  const std::vector<dvec2>& minorEigenvectors, const dvec2& extends = dvec2(1.0));
    TensorField2D(size_t x, size_t y, const std::vector<double>& data,
                  const std::vector<double>& majorEigenvalues,
                  const std::vector<double>& minorEigenvalues,
                  const std::vector<dvec2>& majorEigenvectors,
                  const std::vector<dvec2>& minorEigenvectors, const dvec2& extends = dvec2(1.0));

    // Destructors
    virtual ~TensorField2D() = default;

    std::string getDataInfo() const;
    std::shared_ptr<Image> getImageRepresentation() const;

    dmat2& at(size2_t position);
    dmat2& at(size_t x, size_t y);
    dmat2& at(size_t index);

    const dmat2& at(size2_t position) const;
    const dmat2& at(size_t x, size_t y) const;
    const dmat2& at(size_t index) const;

    template <typename T = size_t>
    const glm::tvec2<T> getDimensions() const {
        return glm::tvec2<T>(dimensions_);
    }
    template <typename T = double>
    glm::tvec2<T> getExtents() const {
        return glm::tvec2<T>(extends_);
    }
    template <typename T = size_t>
    const glm::tvec2<T> getBounds() const {
        return glm::tvec2<T>(dimensions_ - size2_t(1));
    }
    size_t getSize() const;
    glm::u8 rank() const { return rank_; }
    glm::u8 dimensionality() const { return dimensionality_; }

    template <typename T = double>
    T getMinorEigenValue(const size_t index) const {
        return T(minorEigenValues_[index]);
    }
    template <typename T = double>
    T getMajorEigenValue(const size_t index) const {
        return T(majorEigenValues_[index]);
    }

    dmat2 getBasis() const;
    dmat3 getBasisAndOffset() const;
    dvec2 getOffset() const { return offset_; };
    void setOffset(const dvec2& offset) { offset_ = offset; };

    const dvec2& getNormalizedImagePosition(size_t index) const;
    const dvec2& getMinorEigenVector(size_t index) const;
    const dvec2& getMinorEigenVector(const size2_t& position) const;
    const dvec2& getMajorEigenVector(size_t index) const;
    const dvec2& getMajorEigenVector(const size2_t& position) const;

    const std::vector<dvec2>& minorEigenVectors() const;
    const std::vector<dvec2>& majorEigenVectors() const;
    const std::vector<double>& minorEigenValues() const;
    const std::vector<double>& majorEigenValues() const;
    const std::vector<dvec2>& normalizedImagePositions() const;
    const std::vector<dmat2>& tensors() const;

    std::array<std::pair<double, dvec2>, 2> getSortedEigenValuesAndEigenVectorsForTensor(
        size_t index) const;
    std::array<std::pair<double, dvec2>, 2> getSortedEigenValuesAndEigenVectorsForTensor(
        const size2_t& pos) const;
    std::array<double, 2> getSortedEigenValuesForTensor(size_t index) const;
    std::array<double, 2> getSortedEigenValuesForTensor(const size2_t& pos) const;
    std::array<dvec2, 2> getSortedEigenVectorsForTensor(size_t index) const;
    std::array<dvec2, 2> getSortedEigenVectorsForTensor(const size2_t& pos) const;

private:
    void computeEigenValuesAndEigenVectors();
    void computeNormalizedScreenCoordinates();

    std::vector<dvec2> majorEigenVectors_;
    std::vector<dvec2> minorEigenVectors_;
    std::vector<double> majorEigenValues_;
    std::vector<double> minorEigenValues_;
    std::vector<dvec2> normalizedImagePositions_;
    std::vector<dvec2> coordinates_;
    size2_t dimensions_;
    dvec2 extends_;
    dvec2 offset_;
    size_t size_;
    glm::u8 rank_;
    glm::u8 dimensionality_;
    util::IndexMapper2D indexMapper_;
    std::vector<dmat2> tensors_;
};

template <>
struct DataTraits<TensorField2D> {
    static std::string classIdentifier() { return "org.inviwo.TensorField2D"; }
    static std::string dataName() { return "TensorField2D"; }
    static uvec3 colorCode() { return uvec3(10, 150, 135); }
    static Document info(const TensorField2D& data) {
        std::ostringstream oss;
        oss << data.getDataInfo();
        Document doc;
        doc.append("p", oss.str());
        return doc;
    }
};

}  // namespace inviwo
