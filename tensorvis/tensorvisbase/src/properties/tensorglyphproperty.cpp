/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2015-2018 Inviwo Foundation
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

#include <modules/tensorvisbase/properties/tensorglyphproperty.h>
#include <modules/tensorvisbase/datastructures/deformablesphere.h>
#include <modules/tensorvisbase/datastructures/deformablecube.h>
#include <modules/tensorvisbase/datastructures/deformablecylinder.h>

namespace inviwo {
const std::string TensorGlyphProperty::classIdentifier{"org.inviwo.TensorGlyphProperty"};
std::string TensorGlyphProperty::getClassIdentifier() const { return classIdentifier; }

//constexpr std::array<std::array<dvec2, 3>, 10> TensorGlyphProperty::tri_uv;

//constexpr std::array<std::array<dvec3, 3>, 10> TensorGlyphProperty::tri_alpha_beta;

TensorGlyphProperty::TensorGlyphProperty(std::string identifier, std::string displayName)
    : CompositeProperty(identifier, displayName)
    , glyphType_(
          "glyphType", "Glyph type",
          {{"reynolds", "Reynolds", GlyphType::Reynolds},
           {"hwy", "HWY", GlyphType::HYW},
           {"combined_reynolds_hwy", "Combined Reynolds and HWY", GlyphType::CombinedReynoldsHYW},
           {"superquadric", "Superquadric", GlyphType::Superquadric},
           {"superquadric_extended", "Superquadric extended", GlyphType::SuperquadricExtended},
           {"quadric", "Quadric", GlyphType::Quadric},
           {"cube", "Cube", GlyphType::Cube},
           {"cylinder", "Cylinder", GlyphType::Cylinder}})
    , resolutionTheta_("resolutionTheta", "Resolution theta", 32, 3, 256, 1)
    , resolutionPhi_("resolutionPhi", "Resolution phi", 32, 3, 128, 1)
    , size_("size", "Size", 1.f, 0.01f, 2.f, 0.01f)
    , gamma_("gamma", "Gamme value", 3.0, 0.0, 10.0, 0.01f)
    , color_("color", "Color", vec4(1.f), vec4(0.f), vec4(1.f), vec4(.001f),
             InvalidationLevel::InvalidOutput, PropertySemantics::Color)
    , useEigenBasis_("useEigenBasis", "Use eigen basis", true) {
    glyphType_.setSelectedValue(GlyphType::Superquadric);
    glyphType_.setCurrentStateAsDefault();
    addProperty(glyphType_);
    addProperty(resolutionTheta_);
    addProperty(resolutionPhi_);
    addProperty(size_);
    addProperty(gamma_);
    addProperty(color_);
    addProperty(useEigenBasis_);

    useEigenBasis_.setVisible(false);

    gamma_.setVisible(false);

    color_.setSemantics(PropertySemantics::Color);
    color_.setVisible(false);

    glyphType_.onChange([this]() { evalColorReadOnly(); });
}

TensorGlyphProperty::TensorGlyphProperty(const TensorGlyphProperty& rhs)
    : CompositeProperty(rhs)
    , glyphType_(rhs.glyphType_)
    , resolutionTheta_(rhs.resolutionTheta_)
    , resolutionPhi_(rhs.resolutionPhi_)
    , size_(rhs.size_)
    , gamma_(rhs.gamma_)
    , color_(rhs.color_)
    , useEigenBasis_(rhs.useEigenBasis_) {}

TensorGlyphProperty& TensorGlyphProperty::operator=(const TensorGlyphProperty& that) {
    if (this != &that) {
        CompositeProperty::operator=(that);
        glyphType_ = that.glyphType_;
    }
    return *this;
}

TensorGlyphProperty* TensorGlyphProperty::clone() const { return new TensorGlyphProperty(*this); }

TensorGlyphProperty::~TensorGlyphProperty() = default;

TensorGlyphProperty::GlyphType TensorGlyphProperty::type() const { return glyphType_.get(); }

size_t TensorGlyphProperty::resolutionTheta() const { return resolutionTheta_.get(); }

size_t TensorGlyphProperty::resolutionPhi() const { return resolutionPhi_.get(); }

float TensorGlyphProperty::size() const { return size_.get(); }

vec4 TensorGlyphProperty::color() const { return color_.get(); }

float TensorGlyphProperty::gamma() const { return gamma_.get(); }

void TensorGlyphProperty::evalColorReadOnly() {
    switch (static_cast<GlyphType>(glyphType_.get())) {
        case GlyphType::CombinedReynoldsHYW:
            color_.setVisible(false);
            gamma_.setVisible(false);
            useEigenBasis_.setVisible(false);
            break;
        case GlyphType::HYW:
            color_.setVisible(true);
            gamma_.setVisible(false);
            useEigenBasis_.setVisible(false);
            break;
        case GlyphType::Reynolds:
            color_.setVisible(false);
            gamma_.setVisible(false);
            useEigenBasis_.setVisible(false);
            break;
        case GlyphType::Superquadric:
            color_.setVisible(true);
            gamma_.setVisible(true);
            useEigenBasis_.setVisible(true);
            break;
        case GlyphType::SuperquadricExtended:
            color_.setVisible(false);
            gamma_.setVisible(false);
            useEigenBasis_.setVisible(true);
            break;
        case GlyphType::Quadric:
            color_.setVisible(true);
            gamma_.setVisible(false);
            useEigenBasis_.setVisible(false);
            break;
        default:
            break;
    }
}

double TensorGlyphProperty::signedExponentiation(double x, double a) {
    auto sgn = glm::sign(x);
    return sgn * std::pow(std::abs(x), a);
}

dvec3 TensorGlyphProperty::cartesianToSpherical(const dvec3& pos) const {
    auto r = glm::length(pos);
    auto phi = glm::acos(pos.z / r);
    auto theta = std::atan2(pos.y, pos.x);
    return dvec3(r, phi, theta);
}

// From Ericsson Real-time collision detection
std::pair<bool, dvec3> TensorGlyphProperty::intersectTriangle(
    const dvec2& coord, const std::array<dvec2, 3>& tri_verts) {
    dvec2 p = coord;
    dvec2 a = tri_verts[0];
    dvec2 b = tri_verts[1];
    dvec2 c = tri_verts[2];

    dvec2 v0 = b - a;
    dvec2 v1 = c - a;
    dvec2 v2 = p - a;

    double d00 = dot(v0, v0);
    double d01 = dot(v0, v1);
    double d11 = dot(v1, v1);
    double d20 = dot(v2, v0);
    double d21 = dot(v2, v1);
    double ood = 1. / (d00 * d11 - d01 * d01);

    double v = (d11 * d20 - d01 * d21) * ood;
    double w = (d00 * d21 - d01 * d20) * ood;

    if (v < 0.0f || v > 1.0f) return {false, dvec3(0)};
    if (w < 0.0f || v + w > 1.0f) return {false, dvec3(0)};

    return {true, dvec3(1. - v - w, v, w)};
}

const std::shared_ptr<BasicMesh> TensorGlyphProperty::generateSuperquadric(
    std::shared_ptr<const TensorField3D> tensorField, size_t index, const dvec3 pos,
    const dvec4& color, const double size) {
    DeformableSphere sphere(resolutionTheta_.get(), resolutionPhi_.get(), color);

    auto eigenValuesAndEigenVectors =
        tensorField->getSortedEigenValuesAndEigenVectorsForTensor(index);

    auto magicScalingNumer = 0.00001;
    std::transform(eigenValuesAndEigenVectors.begin(), eigenValuesAndEigenVectors.end(),
                   eigenValuesAndEigenVectors.begin(),
                   [magicScalingNumer](const std::pair<double, dvec3>& a) {
                       return std::make_pair(
                           glm::sign(a.first) * glm::max(glm::abs(a.first), magicScalingNumer),
                           glm::normalize(a.second));
                   });

    std::sort(eigenValuesAndEigenVectors.begin(), eigenValuesAndEigenVectors.end(),
              [](const std::pair<double, dvec3>& pairA, const std::pair<double, dvec3>& pairB) {
                  return glm::abs(pairA.first) > glm::abs(pairB.first);
              });

    std::transform(
        eigenValuesAndEigenVectors.begin(), eigenValuesAndEigenVectors.end(),
        eigenValuesAndEigenVectors.begin(),
        [maxEv = eigenValuesAndEigenVectors[0].first](const std::pair<double, dvec3>& pair) {
            auto newVal = pair.first / maxEv;
            return std::make_pair(newVal, pair.second * newVal);
        });

    std::array<double, 3> eigenValues{glm::abs(eigenValuesAndEigenVectors[0].first),
                                      glm::abs(eigenValuesAndEigenVectors[1].first),
                                      glm::abs(eigenValuesAndEigenVectors[2].first)};

    auto basis = dmat3(eigenValuesAndEigenVectors[0].second, eigenValuesAndEigenVectors[1].second,
                       eigenValuesAndEigenVectors[2].second);
    if (!useEigenBasis_.get())
        basis = glm::diagonal3x3(dvec3(eigenValues[0], eigenValues[1], eigenValues[2]));

    // Ensure RHS
    if (glm::dot(glm::cross(basis[0], basis[1]), basis[2]) < 0.0) {
        basis[2] = -basis[2];
    }

    auto denominator = eigenValues[0] + eigenValues[1] + eigenValues[2];
    auto linearAnisotropy = (eigenValues[0] - eigenValues[1]) / denominator;
    auto planarAnisotropy = (2. * (eigenValues[1] - eigenValues[2])) / denominator;

    double alpha;
    double beta;

    if (linearAnisotropy >= planarAnisotropy) {
        alpha = glm::pow(1. - planarAnisotropy, gamma_.get());
        beta = glm::pow(1. - linearAnisotropy, gamma_.get());
    } else {
        alpha = glm::pow(1. - linearAnisotropy, gamma_.get());
        beta = glm::pow(1. - planarAnisotropy, gamma_.get());
    }

    sphere.deform(
        [&](vec3& v) {
            /// Geometry calculation
            auto sphericalCoords = cartesianToSpherical(v);

            auto sinphi = glm::sin(sphericalCoords.y);
            auto cosphi = glm::cos(sphericalCoords.y);
            auto sintheta = glm::sin(sphericalCoords.z);
            auto costheta = glm::cos(sphericalCoords.z);

            int sgnsinphi = glm::sign(sinphi);
            int sgncosphi = glm::sign(cosphi);
            int sgnsintheta = glm::sign(sintheta);
            int sgncostheta = glm::sign(costheta);

            if (linearAnisotropy >= planarAnisotropy) {
                v.x = sgncosphi * std::pow(std::abs(cosphi), beta);
                v.y = (-sgnsintheta * std::pow(std::abs(sintheta), alpha)) *
                      (sgnsinphi * std::pow(std::abs(sinphi), beta));
                v.z = (sgncostheta * std::pow(std::abs(costheta), alpha)) *
                      (sgnsinphi * std::pow(std::abs(sinphi), beta));
            } else {
                v.x = (sgncostheta * std::pow(std::abs(costheta), alpha)) *
                      (sgnsinphi * std::pow(std::abs(sinphi), beta));
                v.y = (sgnsintheta * std::pow(std::abs(sintheta), alpha)) *
                      (sgnsinphi * std::pow(std::abs(sinphi), beta));
                v.z = sgncosphi * std::pow(std::abs(cosphi), beta);
            }
        },
        false);

    sphere.transform(pos, dvec3(size));

    auto mesh = sphere.getGeometry();

    mesh->setBasis(basis);

    return mesh;
}

const std::shared_ptr<BasicMesh> TensorGlyphProperty::generateSuperquadric(
    const dmat3& tensor, const dvec3& pos, const float size, const dvec4& color) const {
    return std::shared_ptr<BasicMesh>();
}

const std::shared_ptr<BasicMesh> TensorGlyphProperty::generateSuperquadric(
    const std::array<double, 3>& eigenValues, const dvec3& pos, const float size,
    const dvec4& color) const {
    DeformableSphere sphere(resolutionTheta_.get(), resolutionPhi_.get(), color);

    auto basis = glm::diagonal3x3(dvec3(eigenValues[0], eigenValues[1], eigenValues[2]));

    auto denominator = eigenValues[0] + eigenValues[1] + eigenValues[2];
    auto linearAnisotropy = (eigenValues[0] - eigenValues[1]) / denominator;
    auto planarAnisotropy = (2. * (eigenValues[1] - eigenValues[2])) / denominator;

    double alpha;
    double beta;

    if (linearAnisotropy >= planarAnisotropy) {
        alpha = glm::pow(1. - planarAnisotropy, gamma_.get());
        beta = glm::pow(1. - linearAnisotropy, gamma_.get());
    } else {
        alpha = glm::pow(1. - linearAnisotropy, gamma_.get());
        beta = glm::pow(1. - planarAnisotropy, gamma_.get());
    }

    sphere.deform(
        [&](vec3& v) {
            /// Geometry calculation
            auto sphericalCoords = cartesianToSpherical(v);

            auto sinphi = glm::sin(sphericalCoords.y);
            auto cosphi = glm::cos(sphericalCoords.y);
            auto sintheta = glm::sin(sphericalCoords.z);
            auto costheta = glm::cos(sphericalCoords.z);

            int sgnsinphi = glm::sign(sinphi);
            int sgncosphi = glm::sign(cosphi);
            int sgnsintheta = glm::sign(sintheta);
            int sgncostheta = glm::sign(costheta);

            if (linearAnisotropy >= planarAnisotropy) {
                v.x = sgncosphi * std::pow(std::abs(cosphi), beta);
                v.y = (-sgnsintheta * std::pow(std::abs(sintheta), alpha)) *
                      (sgnsinphi * std::pow(std::abs(sinphi), beta));
                v.z = (sgncostheta * std::pow(std::abs(costheta), alpha)) *
                      (sgnsinphi * std::pow(std::abs(sinphi), beta));
            } else {
                v.x = (sgncostheta * std::pow(std::abs(costheta), alpha)) *
                      (sgnsinphi * std::pow(std::abs(sinphi), beta));
                v.y = (sgnsintheta * std::pow(std::abs(sintheta), alpha)) *
                      (sgnsinphi * std::pow(std::abs(sinphi), beta));
                v.z = sgncosphi * std::pow(std::abs(cosphi), beta);
            }
        },
        false);

    sphere.transform(pos, dvec3(size));

    auto mesh = sphere.getGeometry();

    mesh->setBasis(basis);

    return mesh;
}

const std::shared_ptr<BasicMesh> TensorGlyphProperty::generateReynolds(
    std::shared_ptr<const TensorField3D> tensorField, size_t index, const dvec3 pos,
    const double size) {
    DeformableSphere sphere(resolutionTheta_.get(), resolutionPhi_.get());

    const auto tensor = mat3(tensorField->at(index).second);

    sphere.deform([tensor](vec3& v, vec4& c) {
        const auto displacement = tensor * v;
        const auto scale = glm::dot(displacement, v);
        v = v * scale;
        if (scale < 0.f) {
            c = vec4(1, 0, 0, 1);
            v = -v;
        } else {
            c = vec4(0, 1, 0, 1);
        }
    });

    sphere.transform(pos, dvec3(size));

    return sphere.getGeometry();
}

const std::shared_ptr<BasicMesh> TensorGlyphProperty::generateQuadric(
    std::shared_ptr<const TensorField3D> tensorField, size_t index, const dvec3 pos,
    const dvec4& color, const double size) {
    DeformableSphere sphere(resolutionTheta_.get(), resolutionPhi_.get(), color);

    const auto tensor = mat3(tensorField->at(index).second);

    sphere.deform([tensor](vec3& v) { v = tensor * v; });

    sphere.transform(pos, dvec3(size));

    return sphere.getGeometry();
}

const std::shared_ptr<BasicMesh> TensorGlyphProperty::generateQuadric(const dmat3& tensor,
                                                                      const dvec3& pos,
                                                                      const float size,
                                                                      const dvec4& color) const {
    DeformableSphere sphere(resolutionTheta_.get(), resolutionPhi_.get(), color);

    sphere.deform([tensor](vec3& v) { v = mat3(tensor) * v; });

    sphere.transform(pos, dvec3(size));

    return sphere.getGeometry();
}

const std::shared_ptr<BasicMesh> TensorGlyphProperty::generateCube(const dmat3& tensor,
                                                                   const dvec3& pos,
                                                                   const float size,
                                                                   const dvec4& color) const {
    DeformableCube cube(color);

    cube.deform([tensor](vec3& v) { v = mat3(tensor) * v; });

    cube.transform(pos, dvec3(size));

    return cube.getGeometry();
}

const std::shared_ptr<BasicMesh> TensorGlyphProperty::generateCylinder(const dmat3& tensor,
                                                                       const dvec3& pos,
                                                                       const float size,
                                                                       const dvec4& color) const {
    DeformableCylinder cylinder(resolutionTheta_.get(), color);

    cylinder.deform([tensor](vec3& v) { v = mat3(tensor) * v; });

    cylinder.transform(pos, dvec3(size));

    return cylinder.getGeometry();
}

const std::shared_ptr<BasicMesh> TensorGlyphProperty::generateHWY(
    std::shared_ptr<const TensorField3D> tensorField, size_t index, const dvec3 pos,
    const dvec4& color, const double size) {
    DeformableSphere sphere(resolutionTheta_.get(), resolutionPhi_.get(), color);

    const auto tensor = mat3(tensorField->at(index).second);

    sphere.deform([tensor](vec3& v) {
        const auto displacement = tensor * v;
        const auto normalPart = v * glm::dot(displacement, v);
        const auto orthoPart = displacement - normalPart;

        auto scale = glm::length(orthoPart);
        v = v * scale;
    });

    sphere.transform(pos, dvec3(size));

    return sphere.getGeometry();
}

const std::shared_ptr<BasicMesh> TensorGlyphProperty::generateCombinedReynoldsHWY(
    std::shared_ptr<const TensorField3D> tensorField, size_t index, const dvec3 pos,
    const dvec4& color, const double size) {
    auto reynolds = generateReynolds(tensorField, index, pos, size);
    auto hwy = generateHWY(tensorField, index, pos, color, size);
    reynolds->append(hwy.get());

    return reynolds;
}

const std::shared_ptr<BasicMesh> TensorGlyphProperty::generateSuperquadricExtended(
    std::shared_ptr<const TensorField3D> tensorField, size_t index, const dvec3 pos,
    const double size) {
    std::array<std::array<dvec2, 3>, 10> tri_uv{
            {{dvec2(0.00, 0.00), dvec2(0.50, 0.00), dvec2(0.25, 0.25)},
                    {dvec2(0.00, 0.00), dvec2(0.25, 0.25), dvec2(0.00, 0.50)},
                    {dvec2(0.00, 0.50), dvec2(0.50, 0.00), dvec2(0.00, 1.00)},
                    {dvec2(0.00, 1.00), dvec2(0.50, 0.00), dvec2(0.50, 0.50)},
                    {dvec2(0.00, 1.00), dvec2(0.50, 0.50), dvec2(0.50, 1.00)},
                    {dvec2(0.50, 1.00), dvec2(0.75, 0.75), dvec2(1.00, 1.00)},
                    {dvec2(1.00, 1.00), dvec2(0.75, 0.75), dvec2(1.00, 0.50)},
                    {dvec2(1.00, 0.50), dvec2(0.50, 1.00), dvec2(1.00, 0.00)},
                    {dvec2(1.00, 0.00), dvec2(0.50, 1.00), dvec2(0.50, 0.50)},
                    {dvec2(1.00, 0.00), dvec2(0.50, 0.50), dvec2(0.50, 0.00)}}};

    std::array<std::array<dvec3, 3>, 10> tri_alpha_beta{
            {{dvec3(1.0, 1.0, 0.0), dvec3(1.0, 0.0, 0.0), dvec3(0.5, 0.5, 0.0)},
                    {dvec3(1.0, 1.0, 0.0), dvec3(0.5, 0.5, 0.0), dvec3(1.0, 0.0, 0.0)},
                    {dvec3(1.0, 2.0, 0.0), dvec3(0.0, 2.0, 0.0), dvec3(1.0, 4.0, 0.0)},
                    {dvec3(1.0, 4.0, 0.0), dvec3(0.0, 2.0, 0.0), dvec3(0.0, 4.0, 2.0)},
                    {dvec3(1.0, 4.0, 0.0), dvec3(0.0, 4.0, 2.0), dvec3(1.0, 2.0, 0.0)},
                    {dvec3(1.0, 0.0, 0.0), dvec3(0.5, 0.5, 0.0), dvec3(1.0, 1.0, 0.0)},
                    {dvec3(1.0, 1.0, 0.0), dvec3(0.5, 0.5, 0.0), dvec3(1.0, 0.0, 0.0)},
                    {dvec3(1.0, 2.0, 0.0), dvec3(0.0, 2.0, 0.0), dvec3(1.0, 4.0, 0.0)},
                    {dvec3(1.0, 4.0, 0.0), dvec3(0.0, 2.0, 0.0), dvec3(0.0, 4.0, 2.0)},
                    {dvec3(1.0, 4.0, 0.0), dvec3(0.0, 4.0, 2.0), dvec3(1.0, 2.0, 0.0)}}};

    DeformableSphere sphere(resolutionTheta_.get(), resolutionPhi_.get());

    auto eigenValuesAndEigenVectors =
        tensorField->getSortedEigenValuesAndEigenVectorsForTensor(index);

    auto magicScalingNumer = 0.00001;
    std::transform(eigenValuesAndEigenVectors.begin(), eigenValuesAndEigenVectors.end(),
                   eigenValuesAndEigenVectors.begin(),
                   [magicScalingNumer](const std::pair<double, dvec3>& a) {
                       return std::make_pair(
                           glm::sign(a.first) * glm::max(glm::abs(a.first), magicScalingNumer),
                           glm::normalize(a.second));
                   });

    std::sort(eigenValuesAndEigenVectors.begin(), eigenValuesAndEigenVectors.end(),
              [](const std::pair<double, dvec3>& pairA, const std::pair<double, dvec3>& pairB) {
                  return glm::abs(pairA.first) > glm::abs(pairB.first);
              });

    std::transform(
        eigenValuesAndEigenVectors.begin(), eigenValuesAndEigenVectors.end(),
        eigenValuesAndEigenVectors.begin(),
        [maxEv = eigenValuesAndEigenVectors[0].first](const std::pair<double, dvec3>& pair) {
            auto newVal = pair.first / maxEv;
            return std::make_pair(newVal, pair.second * newVal);
        });

    if (glm::dot(
            glm::cross(eigenValuesAndEigenVectors[0].second, eigenValuesAndEigenVectors[1].second),
            eigenValuesAndEigenVectors[2].second) < 0.0) {
        eigenValuesAndEigenVectors[2].second = -eigenValuesAndEigenVectors[2].second;
    }

    std::array<double, 3> eigenValues{glm::abs(eigenValuesAndEigenVectors[0].first),
                                      glm::abs(eigenValuesAndEigenVectors[1].first),
                                      glm::abs(eigenValuesAndEigenVectors[2].first)};

    auto basis = dmat3(eigenValuesAndEigenVectors[0].second, eigenValuesAndEigenVectors[1].second,
                       eigenValuesAndEigenVectors[2].second);

    if (!useEigenBasis_.get())
        basis = glm::diagonal3x3(dvec3(eigenValues[0], eigenValues[1], eigenValues[2]));

    const auto frobeniusNorm = tensorField->getMetaData<FrobeniusNorm>()[index];
    auto lamda = tensorField->getSortedEigenValuesForTensor(index);

    std::transform(lamda.begin(), lamda.end(), lamda.begin(),
                   [frobeniusNorm](const double& a) { return a / frobeniusNorm; });

    // Necessary?
    std::sort(lamda.begin(), lamda.end(), std::greater<double>());

    auto max_abs_lambda =
        glm::max(glm::max(glm::abs(lamda[0]), glm::abs(lamda[1])), glm::abs(lamda[2]));

    lamda[0] /= max_abs_lambda;
    lamda[1] /= max_abs_lambda;
    lamda[2] /= max_abs_lambda;

    auto u = 0.5f * (1. + lamda[1]);
    auto v =
        (lamda[0] > -lamda[2]) ? 0.5f * (1. + lamda[2]) - u + 1. : 0.5f * (lamda[0] - 1.) - u + 1.;
    dvec3 alpha_beta_betaprim;

    for (size_t i = 0; i < tri_uv.size(); i++) {
        auto res = intersectTriangle(dvec2(u, v), tri_uv[i]);
        if (res.first) {
            auto bc = res.second;
            auto comp = tri_alpha_beta[i];
            alpha_beta_betaprim = bc[0] * comp[0] + bc[1] * comp[1] + bc[2] * comp[2];

            if (tri_rotate[i]) {
                // std::swap(basis[0], basis[2]);
            }

            break;
        }
    }

    sphere.deform([&](vec3& v, vec4& c) {
        auto alpha = alpha_beta_betaprim.x;
        auto beta = alpha_beta_betaprim.y;
        auto beta_prim = alpha_beta_betaprim.z;

        auto spherical_coords = cartesianToSpherical(v);
        auto phi = spherical_coords.y;
        auto theta = spherical_coords.z;

        if (beta_prim > 0.) {
            auto y_beta = signedExponentiation(glm::sin(theta), alpha) *
                          signedExponentiation(glm::sin(phi), beta);
            auto z = signedExponentiation(glm::cos(phi), beta);
            auto s_beta_prim =
                signedExponentiation(glm::acos(glm::pow(z, 1. / beta_prim)), beta_prim);
            auto s_max = signedExponentiation(glm::sin(phi), beta);

            v.x = signedExponentiation(glm::cos(theta), alpha) *
                  signedExponentiation(glm::sin(phi), beta);
            v.y = y_beta * s_beta_prim / s_max;
            v.z = z;
        } else {
            v.x = signedExponentiation(glm::cos(theta), alpha) *
                  signedExponentiation(glm::sin(phi), beta);
            v.y = signedExponentiation(glm::sin(theta), alpha) *
                  signedExponentiation(glm::sin(phi), beta);
            v.z = signedExponentiation(glm::cos(phi), beta);
        }

        std::array<double, 3> eigenValues{glm::abs(eigenValuesAndEigenVectors[0].first),
                                          glm::abs(eigenValuesAndEigenVectors[1].first),
                                          glm::abs(eigenValuesAndEigenVectors[2].first)};

        auto mat = glm::diagonal3x3(dvec3(eigenValues[0], eigenValues[1], eigenValues[2]));
        auto sign = glm::dot(dvec3(v), mat * dvec3(v));
        if (sign >= 0.) {
            c = vec4(0., 0., 1., 1.);
        } else {
            c = vec4(1., 0., 0., 1.);
        }
    });

    sphere.transform(pos, dvec3(size));

    auto mesh = sphere.getGeometry();

    mesh->setBasis(basis);

    return mesh;
}

const std::shared_ptr<BasicMesh> TensorGlyphProperty::generateGlyph(
    std::shared_ptr<const TensorField3D> tensorField, size_t index, const dvec3 pos) {
    switch (glyphType_.get()) {
        case GlyphType::Reynolds:
            return generateReynolds(tensorField, index, pos, size_.get());
        case GlyphType::HYW:
            return generateHWY(tensorField, index, pos, color_.get(), size_.get());
        case GlyphType::CombinedReynoldsHYW:
            return generateCombinedReynoldsHWY(tensorField, index, pos, color_.get(), size_.get());
        case GlyphType::Superquadric:
            return generateSuperquadric(tensorField, index, pos, color_.get(), size_.get());
        case GlyphType::SuperquadricExtended:
            return generateSuperquadricExtended(tensorField, index, pos, size_.get());
        case GlyphType::Quadric:
            return generateQuadric(tensorField, index, pos, color_.get(), size_.get());
        default:
            break;
    }
    return std::make_shared<BasicMesh>();
}

const std::shared_ptr<BasicMesh> TensorGlyphProperty::generateGlyph(
    std::shared_ptr<const TensorField3D> tensorField, size_t index, const dvec3 pos,
    const dvec4& color) {
    switch (glyphType_.get()) {
        case GlyphType::Reynolds:
            return generateReynolds(tensorField, index, pos, size_.get());
        case GlyphType::HYW:
            return generateHWY(tensorField, index, pos, color, size_.get());
        case GlyphType::CombinedReynoldsHYW:
            return generateCombinedReynoldsHWY(tensorField, index, pos, color, size_.get());
        case GlyphType::Superquadric:
            return generateSuperquadric(tensorField, index, pos, color, size_.get());
        case GlyphType::SuperquadricExtended:
            return generateSuperquadricExtended(tensorField, index, pos, size_.get());
        case GlyphType::Quadric:
            return generateQuadric(tensorField, index, pos, color, size_.get());
        default:
            break;
    }
    return std::make_shared<BasicMesh>();
}

const std::shared_ptr<BasicMesh> TensorGlyphProperty::generateGlyph(
    std::shared_ptr<const TensorField3D> tensorField, size_t index, const dvec3 pos,
    const float size) {
    switch (glyphType_.get()) {
        case GlyphType::Reynolds:
            return generateReynolds(tensorField, index, pos, size);
        case GlyphType::HYW:
            return generateHWY(tensorField, index, pos, color_.get(), size);
        case GlyphType::CombinedReynoldsHYW:
            return generateCombinedReynoldsHWY(tensorField, index, pos, color_.get(), size);
        case GlyphType::Superquadric:
            return generateSuperquadric(tensorField, index, pos, color_.get(), size);
        case GlyphType::SuperquadricExtended:
            return generateSuperquadricExtended(tensorField, index, pos, size);
        case GlyphType::Quadric:
            return generateQuadric(tensorField, index, pos, color_.get(), size);
        default:
            break;
    }
    return std::make_shared<BasicMesh>();
}

const std::shared_ptr<BasicMesh> TensorGlyphProperty::generateGlyph(
    std::shared_ptr<const TensorField3D> tensorField, size_t index, const dvec3 pos,
    const dvec4& color, const float size) {
    switch (glyphType_.get()) {
        case GlyphType::Reynolds:
            return generateReynolds(tensorField, index, pos, size);
        case GlyphType::HYW:
            return generateHWY(tensorField, index, pos, color, size);
        case GlyphType::CombinedReynoldsHYW:
            return generateCombinedReynoldsHWY(tensorField, index, pos, color, size);
        case GlyphType::Superquadric:
            return generateSuperquadric(tensorField, index, pos, color, size);
        case GlyphType::SuperquadricExtended:
            return generateSuperquadricExtended(tensorField, index, pos, size);
        case GlyphType::Quadric:
            return generateQuadric(tensorField, index, pos, color, size);
        default:
            break;
    }
    return std::make_shared<BasicMesh>();
}

const std::shared_ptr<BasicMesh> TensorGlyphProperty::generateGlyph(const dmat3& tensor,
                                                                    const dvec3& pos,
                                                                    const float size,
                                                                    const dvec4& color) const {
    switch (glyphType_.get()) {
        case GlyphType::Quadric:
            return generateQuadric(tensor, pos, size, color);
        case GlyphType::Cube:
            return generateCube(tensor, pos, size, color);
        case GlyphType::Cylinder:
            return generateCylinder(tensor, pos, size, color);
        default:
            LogError("Not implemented.");
    }

    return std::make_shared<BasicMesh>();
}

const std::shared_ptr<BasicMesh> TensorGlyphProperty::generateGlyph(
    const std::array<double, 3>& eigenvalues, const dvec3& pos, const float size,
    const dvec4& color) const {
    return generateSuperquadric(eigenvalues, pos, size, color);
}

}  // namespace inviwo
