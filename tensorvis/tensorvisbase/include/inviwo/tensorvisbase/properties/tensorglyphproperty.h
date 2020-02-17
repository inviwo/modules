/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2015-2020 Inviwo Foundation
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
#include <inviwo/core/properties/ordinalproperty.h>

namespace inviwo {

/**
 * \class TensorGlyphProperty
 * \brief VERY_BRIEFLY_DESCRIBE_THE_CLASS
 * DESCRIBE_THE_CLASS
 */
class IVW_MODULE_TENSORVISBASE_API TensorGlyphProperty : public CompositeProperty {
public:
    virtual std::string getClassIdentifier() const override;
    static const std::string classIdentifier;

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
                                                   size_t index, const vec3 pos);
    const std::shared_ptr<BasicMesh> generateGlyph(std::shared_ptr<const TensorField3D> tensorField,
                                                   size_t index, const vec3 pos, const vec4& color);
    const std::shared_ptr<BasicMesh> generateGlyph(std::shared_ptr<const TensorField3D> tensorField,
                                                   size_t index, const vec3 pos, const float size);
    const std::shared_ptr<BasicMesh> generateGlyph(std::shared_ptr<const TensorField3D> tensorField,
                                                   size_t index, const vec3 pos, const vec4& color,
                                                   const float size);

    const std::shared_ptr<BasicMesh> generateGlyph(const mat3& tensor, const vec3& pos,
                                                   const float size,
                                                   const vec4& color = vec4(1.)) const;

    const std::shared_ptr<BasicMesh> generateGlyph(const std::array<float, 3>& eigenvalues,
                                                   const vec3& pos, const float size,
                                                   const vec4& color = vec4(1.)) const;

    const std::shared_ptr<BasicMesh> generateQuadric(const mat3& tensor, const vec3& pos,
                                                     const float size,
                                                     const vec4& color = vec4(1.)) const;

protected:
    // Properties go here
    TemplateOptionProperty<GlyphType> glyphType_;

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
    float signedExponentiation(float x, float a);
    vec3 cartesianToSpherical(const vec3& pos) const;
    std::pair<bool, vec3> intersectTriangle(const vec2& coord,
                                            const std::array<vec2, 3>& tri_verts);

    const std::shared_ptr<BasicMesh> generateSuperquadric(
        std::shared_ptr<const TensorField3D> tensorField, size_t index, const vec3 pos,
        const vec4& color, const float size);

    const std::shared_ptr<BasicMesh> generateSuperquadric(const mat3& tensor, const vec3& pos,
                                                          const float size,
                                                          const vec4& color = vec4(1.)) const;

    const std::shared_ptr<BasicMesh> generateSuperquadric(const std::array<float, 3>& eigenvalues,
                                                          const vec3& pos, const float size,
                                                          const vec4& color = vec4(1.)) const;

    const std::shared_ptr<BasicMesh> generateReynolds(
        std::shared_ptr<const TensorField3D> tensorField, size_t index, const vec3 pos,
        const float size);

    const std::shared_ptr<BasicMesh> generateQuadric(
        std::shared_ptr<const TensorField3D> tensorField, size_t index, const vec3 pos,
        const vec4& color, const float size);

    const std::shared_ptr<BasicMesh> generateCube(const mat3& tensor, const vec3& pos,
                                                  const float size,
                                                  const vec4& color = vec4(1.)) const;

    const std::shared_ptr<BasicMesh> generateCylinder(const mat3& tensor, const vec3& pos,
                                                      const float size,
                                                      const vec4& color = vec4(1.)) const;

    const std::shared_ptr<BasicMesh> generateHWY(std::shared_ptr<const TensorField3D> tensorField,
                                                 size_t index, const vec3 pos, const vec4& color,
                                                 const float size);

    const std::shared_ptr<BasicMesh> generateCombinedReynoldsHWY(
        std::shared_ptr<const TensorField3D> tensorField, size_t index, const vec3 pos,
        const vec4& color, const float size);

    const std::shared_ptr<BasicMesh> generateSuperquadricExtended(
        std::shared_ptr<const TensorField3D> tensorField, size_t index, const vec3 pos,
        const float size);

    std::shared_ptr<BasicMesh> createSuperquadric(const std::array<float, 3>& eigenValues,
                                                  const vec3& pos, const float size,
                                                  const vec4& color) const;

    static inline constexpr std::array<std::array<vec2, 3>, 10> tri_uv{
        {{vec2(0.00, 0.00), vec2(0.50, 0.00), vec2(0.25, 0.25)},
         {vec2(0.00, 0.00), vec2(0.25, 0.25), vec2(0.00, 0.50)},
         {vec2(0.00, 0.50), vec2(0.50, 0.00), vec2(0.00, 1.00)},
         {vec2(0.00, 1.00), vec2(0.50, 0.00), vec2(0.50, 0.50)},
         {vec2(0.00, 1.00), vec2(0.50, 0.50), vec2(0.50, 1.00)},
         {vec2(0.50, 1.00), vec2(0.75, 0.75), vec2(1.00, 1.00)},
         {vec2(1.00, 1.00), vec2(0.75, 0.75), vec2(1.00, 0.50)},
         {vec2(1.00, 0.50), vec2(0.50, 1.00), vec2(1.00, 0.00)},
         {vec2(1.00, 0.00), vec2(0.50, 1.00), vec2(0.50, 0.50)},
         {vec2(1.00, 0.00), vec2(0.50, 0.50), vec2(0.50, 0.00)}}};

    static inline constexpr std::array<std::array<vec3, 3>, 10> tri_alpha_beta{
        {{vec3(1.0, 1.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(0.5, 0.5, 0.0)},
         {vec3(1.0, 1.0, 0.0), vec3(0.5, 0.5, 0.0), vec3(1.0, 0.0, 0.0)},
         {vec3(1.0, 2.0, 0.0), vec3(0.0, 2.0, 0.0), vec3(1.0, 4.0, 0.0)},
         {vec3(1.0, 4.0, 0.0), vec3(0.0, 2.0, 0.0), vec3(0.0, 4.0, 2.0)},
         {vec3(1.0, 4.0, 0.0), vec3(0.0, 4.0, 2.0), vec3(1.0, 2.0, 0.0)},
         {vec3(1.0, 0.0, 0.0), vec3(0.5, 0.5, 0.0), vec3(1.0, 1.0, 0.0)},
         {vec3(1.0, 1.0, 0.0), vec3(0.5, 0.5, 0.0), vec3(1.0, 0.0, 0.0)},
         {vec3(1.0, 2.0, 0.0), vec3(0.0, 2.0, 0.0), vec3(1.0, 4.0, 0.0)},
         {vec3(1.0, 4.0, 0.0), vec3(0.0, 2.0, 0.0), vec3(0.0, 4.0, 2.0)},
         {vec3(1.0, 4.0, 0.0), vec3(0.0, 4.0, 2.0), vec3(1.0, 2.0, 0.0)}}};

    static inline constexpr std::array<bool, 10> tri_rotate = {false, true, true,  true,  true,
                                                               true,  true, false, false, false};
};

}  // namespace inviwo
