/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2015-2025 Inviwo Foundation
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
#include <inviwo/tensorvisbase/datastructures/tensorfield3d.h>
#include <inviwo/core/properties/compositeproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/datastructures/geometry/typedmesh.h>

namespace inviwo {

class IVW_MODULE_TENSORVISBASE_API TensorGlyphProperty : public CompositeProperty {
public:
    virtual std::string_view getClassIdentifier() const override;
    static constexpr std::string_view classIdentifier{"org.inviwo.TensorGlyphProperty"};

    enum class GlyphType {
        Reynolds,
        HYW,
        CombinedReynoldsHYW,
        Superquadric,
        SuperquadricExtended,
        Quadric,
        Cube,
        Cylinder
    };

    TensorGlyphProperty(std::string identifier = std::string("tensorGlyphProperty"),
                        std::string displayName = std::string("Glyph properties"));
    TensorGlyphProperty(const TensorGlyphProperty& rhs);
    virtual TensorGlyphProperty* clone() const override;
    virtual ~TensorGlyphProperty();

    GlyphType type() const;
    size_t resolutionTheta() const;
    size_t resolutionPhi() const;
    float size() const;
    vec4 color() const;
    float gamma() const;

    const std::shared_ptr<BasicMesh> generateGlyph(std::shared_ptr<const TensorField3D> tensorField,
                                                   size_t index, const dvec3 pos);
    const std::shared_ptr<BasicMesh> generateGlyph(std::shared_ptr<const TensorField3D> tensorField,
                                                   size_t index, const dvec3 pos,
                                                   const dvec4& color);
    const std::shared_ptr<BasicMesh> generateGlyph(std::shared_ptr<const TensorField3D> tensorField,
                                                   size_t index, const dvec3 pos, const float size);
    const std::shared_ptr<BasicMesh> generateGlyph(std::shared_ptr<const TensorField3D> tensorField,
                                                   size_t index, const dvec3 pos,
                                                   const dvec4& color, const float size);

    const std::shared_ptr<BasicMesh> generateGlyph(const dmat3& tensor, const dvec3& pos,
                                                   const float size,
                                                   const dvec4& color = dvec4(1.)) const;

    const std::shared_ptr<BasicMesh> generateGlyph(const std::array<double, 3>& eigenvalues,
                                                   const dvec3& pos, const float size,
                                                   const dvec4& color = dvec4(1.)) const;

    const std::shared_ptr<BasicMesh> generateQuadric(const dmat3& tensor, const dvec3& pos,
                                                     const float size,
                                                     const dvec4& color = dvec4(1.)) const;

protected:
    // Properties go here
    OptionProperty<GlyphType> glyphType_;

    OrdinalProperty<size_t> resolutionTheta_;
    OrdinalProperty<size_t> resolutionPhi_;

    FloatProperty size_;

    FloatProperty gamma_;

    FloatVec4Property color_;

    BoolProperty useEigenBasis_;

private:
    auto props() {
        return std::tie(glyphType_, resolutionTheta_, resolutionPhi_, size_, gamma_, color_,
                        useEigenBasis_);
    }
    auto props() const {
        return std::tie(glyphType_, resolutionTheta_, resolutionPhi_, size_, gamma_, color_,
                        useEigenBasis_);
    }

    void evalColorReadOnly();
    double signedExponentiation(double x, double a);
    dvec3 cartesianToSpherical(const dvec3& pos) const;
    std::pair<bool, dvec3> intersectTriangle(const dvec2& coord,
                                             const std::array<dvec2, 3>& tri_verts);

    const std::shared_ptr<BasicMesh> generateSuperquadric(
        std::shared_ptr<const TensorField3D> tensorField, size_t index, const dvec3 pos,
        const dvec4& color, const float size);

    const std::shared_ptr<BasicMesh> generateSuperquadric(const dmat3& tensor, const dvec3& pos,
                                                          const float size,
                                                          const dvec4& color = dvec4(1.)) const;

    const std::shared_ptr<BasicMesh> generateSuperquadric(const std::array<double, 3>& eigenvalues,
                                                          const dvec3& pos, const float size,
                                                          const dvec4& color = dvec4(1.)) const;

    const std::shared_ptr<BasicMesh> generateReynolds(
        std::shared_ptr<const TensorField3D> tensorField, size_t index, const dvec3 pos,
        const float size);

    const std::shared_ptr<BasicMesh> generateQuadric(
        std::shared_ptr<const TensorField3D> tensorField, size_t index, const dvec3 pos,
        const dvec4& color, const float size);

    const std::shared_ptr<BasicMesh> generateCube(const dmat3& tensor, const dvec3& pos,
                                                  const float size,
                                                  const dvec4& color = dvec4(1.)) const;

    const std::shared_ptr<BasicMesh> generateCylinder(const dmat3& tensor, const dvec3& pos,
                                                      const float size,
                                                      const dvec4& color = dvec4(1.)) const;

    const std::shared_ptr<BasicMesh> generateHWY(std::shared_ptr<const TensorField3D> tensorField,
                                                 size_t index, const dvec3 pos, const dvec4& color,
                                                 const float size);

    const std::shared_ptr<BasicMesh> generateCombinedReynoldsHWY(
        std::shared_ptr<const TensorField3D> tensorField, size_t index, const dvec3 pos,
        const dvec4& color, const float size);

    const std::shared_ptr<BasicMesh> generateSuperquadricExtended(
        std::shared_ptr<const TensorField3D> tensorField, size_t index, const dvec3 pos,
        const float size);

    std::shared_ptr<BasicMesh> createSuperquadric(const std::array<double, 3>& eigenValues,
                                                  const dvec3& pos, const float size,
                                                  const dvec4& color) const;

    //    static constexpr std::array<std::array<dvec2, 3>, 10> tri_uv{
    //        {{dvec2(0.00, 0.00), dvec2(0.50, 0.00), dvec2(0.25, 0.25)},
    //         {dvec2(0.00, 0.00), dvec2(0.25, 0.25), dvec2(0.00, 0.50)},
    //         {dvec2(0.00, 0.50), dvec2(0.50, 0.00), dvec2(0.00, 1.00)},
    //         {dvec2(0.00, 1.00), dvec2(0.50, 0.00), dvec2(0.50, 0.50)},
    //         {dvec2(0.00, 1.00), dvec2(0.50, 0.50), dvec2(0.50, 1.00)},
    //         {dvec2(0.50, 1.00), dvec2(0.75, 0.75), dvec2(1.00, 1.00)},
    //         {dvec2(1.00, 1.00), dvec2(0.75, 0.75), dvec2(1.00, 0.50)},
    //         {dvec2(1.00, 0.50), dvec2(0.50, 1.00), dvec2(1.00, 0.00)},
    //         {dvec2(1.00, 0.00), dvec2(0.50, 1.00), dvec2(0.50, 0.50)},
    //         {dvec2(1.00, 0.00), dvec2(0.50, 0.50), dvec2(0.50, 0.00)}}};
    //
    //    static constexpr std::array<std::array<dvec3, 3>, 10> tri_alpha_beta{
    //        {{dvec3(1.0, 1.0, 0.0), dvec3(1.0, 0.0, 0.0), dvec3(0.5, 0.5, 0.0)},
    //         {dvec3(1.0, 1.0, 0.0), dvec3(0.5, 0.5, 0.0), dvec3(1.0, 0.0, 0.0)},
    //         {dvec3(1.0, 2.0, 0.0), dvec3(0.0, 2.0, 0.0), dvec3(1.0, 4.0, 0.0)},
    //         {dvec3(1.0, 4.0, 0.0), dvec3(0.0, 2.0, 0.0), dvec3(0.0, 4.0, 2.0)},
    //         {dvec3(1.0, 4.0, 0.0), dvec3(0.0, 4.0, 2.0), dvec3(1.0, 2.0, 0.0)},
    //         {dvec3(1.0, 0.0, 0.0), dvec3(0.5, 0.5, 0.0), dvec3(1.0, 1.0, 0.0)},
    //         {dvec3(1.0, 1.0, 0.0), dvec3(0.5, 0.5, 0.0), dvec3(1.0, 0.0, 0.0)},
    //         {dvec3(1.0, 2.0, 0.0), dvec3(0.0, 2.0, 0.0), dvec3(1.0, 4.0, 0.0)},
    //         {dvec3(1.0, 4.0, 0.0), dvec3(0.0, 2.0, 0.0), dvec3(0.0, 4.0, 2.0)},
    //         {dvec3(1.0, 4.0, 0.0), dvec3(0.0, 4.0, 2.0), dvec3(1.0, 2.0, 0.0)}}};

    const std::array<bool, 10> tri_rotate = {false, true, true,  true,  true,
                                             true,  true, false, false, false};
};

}  // namespace inviwo
