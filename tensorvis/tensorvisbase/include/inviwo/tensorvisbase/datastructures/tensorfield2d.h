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

#pragma once

#include <inviwo/core/common/inviwo.h>
#include <inviwo/tensorvisbase/tensorvisbasemoduledefine.h>
#include <inviwo/core/datastructures/image/image.h>
#include <warn/push>
#include <warn/ignore/all>
#include <Eigen/Dense>
#include <warn/pop>
#include <inviwo/core/datastructures/buffer/buffer.h>
#include <inviwo/tensorvisbase/util/tensorutil.h>
#include <inviwo/core/util/indexmapper.h>
#include <inviwo/tensorvisbase/datastructures/tensorfield.h>

namespace inviwo {
/**
 * \class TensorField2D
 * \brief Data structure for 2D tensorfields.
 */
class IVW_MODULE_TENSORVISBASE_API TensorField2D : public TensorField<2, float> {
public:
    using value_type = typename TensorField2D::value_type;

    TensorField2D() = delete;

    TensorField2D(const sizeN_t& dimensions, const std::vector<matN>& tensors,
                  std::shared_ptr<DataFrame> metaData = nullptr);
    TensorField2D(const sizeN_t& dimensions, std::shared_ptr<std::vector<matN>> tensors,
                  std::shared_ptr<DataFrame> metaData = nullptr);

    // Destructors
    virtual ~TensorField2D() = default;

    /**
     * NOTE: This copy constructor creates a shallow copy, i.e. the tensors and the meta data are
     * not copied. Rather, the copy points towards the same data as the input field. If you need a
     * deep copy, use the deepCopy method.
     */
    TensorField2D(const TensorField2D& tf);

    virtual TensorField2D* clone() const final;

    virtual TensorField2D* deepCopy() const final;

    std::shared_ptr<Image> getImageRepresentation() const;

    const dvec2& getNormalizedImagePosition(size_t index) const;
    const std::vector<dvec2>& normalizedImagePositions() const;

protected:
    virtual void initializeDefaultMetaData() final;
    virtual void computeDataMaps() final;

private:
    void computeNormalizedScreenCoordinates();

    std::vector<dvec2> normalizedImagePositions_;

    template <typename T, typename R>
    void addIfNotPresent(std::shared_ptr<DataFrame>, const R& data) const;
};

template <typename T, typename R>
void TensorField2D::addIfNotPresent(std::shared_ptr<DataFrame> df, const R& data) const {
    if (!this->hasMetaData<T>()) {
        df->addColumn(std::make_shared<TemplateColumn<typename T::value_type>>(
            std::string(T::identifier), data));
    }
}

}  // namespace inviwo
