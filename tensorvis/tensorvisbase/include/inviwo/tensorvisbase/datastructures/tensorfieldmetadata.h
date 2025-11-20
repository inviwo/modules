/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2017-2025 Inviwo Foundation
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

#include <inviwo/core/util/constexprhash.h>
#include <inviwo/core/util/formats.h>
#include <inviwo/core/util/enumtraits.h>
#include <inviwo/tensorvisbase/util/tensorutil.h>
#include <modules/base/algorithm/dataminmax.h>

#include <algorithm>
#include <utility>
#include <sstream>
#include <fstream>

namespace inviwo {

enum class TensorFeature : size_t {
    I1 = util::constexpr_hash("I1"),
    I2 = util::constexpr_hash("I2"),
    I3 = util::constexpr_hash("I3"),
    J1 = util::constexpr_hash("J1"),
    J2 = util::constexpr_hash("J2"),
    J3 = util::constexpr_hash("J3"),
    Sigma1 = util::constexpr_hash("Sigma1"),
    Sigma2 = util::constexpr_hash("Sigma2"),
    Sigma3 = util::constexpr_hash("Sigma3"),
    MajorEigenVector = util::constexpr_hash("MajorEigenVector"),
    IntermediateEigenVector = util::constexpr_hash("IntermediateEigenVector"),
    MinorEigenVector = util::constexpr_hash("MinorEigenVector"),
    LodeAngle = util::constexpr_hash("LodeAngle"),
    Anisotropy = util::constexpr_hash("Anisotropy"),
    LinearAnisotropy = util::constexpr_hash("LinearAnisotropy"),
    PlanarAnisotropy = util::constexpr_hash("PlanarAnisotropy"),
    SphericalAnisotropy = util::constexpr_hash("SphericalAnisotropy"),
    Diffusivity = util::constexpr_hash("Diffusivity"),
    ShearStress = util::constexpr_hash("ShearStress"),
    PureShear = util::constexpr_hash("PureShear"),
    ShapeFactor = util::constexpr_hash("ShapeFactor"),
    IsotropicScaling = util::constexpr_hash("IsotropicScaling"),
    Rotation = util::constexpr_hash("Rotation"),
    FrobeniusNorm = util::constexpr_hash("FrobeniusNorm"),
    HillYieldCriterion = util::constexpr_hash("HillYieldCriterion"),
    Unspecified = util::constexpr_hash("Unspecified"),
    NumberOfTensorFeatures = util::constexpr_hash("NumberOfTensorFeatures")
};

template <>
struct EnumTraits<TensorFeature> {
    static constexpr std::string_view name() { return "TensorFeature"; }
};

template <class Elem, class Traits>
std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& os,
                                             TensorFeature feature) {
    switch (feature) {
        case TensorFeature::I1:
            os << tensorutil::i1_str;
            break;
        case TensorFeature::I2:
            os << tensorutil::i2_str;
            break;
        case TensorFeature::I3:
            os << tensorutil::i3_str;
            break;
        case TensorFeature::J1:
            os << tensorutil::j1_str;
            break;
        case TensorFeature::J2:
            os << tensorutil::j2_str;
            break;
        case TensorFeature::J3:
            os << tensorutil::j3_str;
            break;
        case TensorFeature::Sigma1:
            os << tensorutil::sigma1safe_str;
            break;
        case TensorFeature::Sigma2:
            os << tensorutil::sigma2safe_str;
            break;
        case TensorFeature::Sigma3:
            os << tensorutil::sigma3safe_str;
            break;
        case TensorFeature::MajorEigenVector:
            os << "Major eigenvector";
            break;
        case TensorFeature::IntermediateEigenVector:
            os << "Intermediate eigenvector";
            break;
        case TensorFeature::MinorEigenVector:
            os << "Minor eigenvector";
            break;
        case TensorFeature::LodeAngle:
            os << "Lode angle";
            break;
        case TensorFeature::Anisotropy:
            os << "Anisotropy";
            break;
        case TensorFeature::LinearAnisotropy:
            os << "Linear Anisotropy";
            break;
        case TensorFeature::PlanarAnisotropy:
            os << "Planar Anisotropy";
            break;
        case TensorFeature::SphericalAnisotropy:
            os << "Spherical Anisotropy";
            break;
        case TensorFeature::Diffusivity:
            os << "Diffusivity";
            break;
        case TensorFeature::ShearStress:
            os << "Shear Stress";
            break;
        case TensorFeature::PureShear:
            os << "Pure Shear";
            break;
        case TensorFeature::ShapeFactor:
            os << "Shape Factor";
            break;
        case TensorFeature::IsotropicScaling:
            os << "IsotropicScaling";
            break;
        case TensorFeature::Rotation:
            os << "Rotation";
            break;
        case TensorFeature::FrobeniusNorm:
            os << "Frobenius Norm";
            break;
        case TensorFeature::HillYieldCriterion:
            os << "Hill Yield Criterion";
            break;
        case TensorFeature::Unspecified:
            os << "Unspecified";
            break;
        default:
            os << "Undefined";
            break;
    }

    return os;
}

namespace tensor {

struct MetaDataBase {
    MetaDataBase() = default;

    virtual ~MetaDataBase() = default;

    virtual MetaDataBase* clone() const = 0;

    // Getters
    virtual uint64_t getId() const = 0;

    virtual size_t getNumberOfComponents() const = 0;

    virtual std::string getDisplayName() const = 0;

    virtual const void* getDataPtr() const = 0;

    // Serialization
    virtual void serialize(std::ofstream& outFile) const = 0;

    virtual void deserialize(std::ifstream& inFile, size_t numElements) = 0;
};

template <typename T>
struct MetaDataType : MetaDataBase {
    using TType = T;
    using DataType = std::vector<T>;

    MetaDataType() = default;

    explicit MetaDataType(std::vector<T> data, TensorFeature type);

    ~MetaDataType() override = default;

    virtual MetaDataType<T>* clone() const override = 0;

    // Getters
    std::pair<TType, TType> getMinMax() const;

    std::string getDisplayName() const override {
        std::stringstream ss;
        ss << type_;
        return ss.str();
    };

    size_t getNumberOfComponents() const override;

    const std::vector<T>& getData() const;

    uint64_t getId() const override = 0;

    const void* getDataPtr() const override;

    // Serialization
    void serialize(std::ofstream& outFile) const override;

    void deserialize(std::ifstream& inFile, size_t numElements) override;

    DataType data_;
    TensorFeature type_ = TensorFeature::NumberOfTensorFeatures;
};

template <typename T>
void MetaDataType<T>::serialize(std::ofstream& outFile) const {
    auto id = getId();
    const auto dataPtr = static_cast<const T*>(data_.data());

    outFile.write(reinterpret_cast<const char*>(&id), sizeof(uint64_t));
    outFile.write(reinterpret_cast<const char*>(&type_), sizeof(uint64_t));
    outFile.write(reinterpret_cast<const char*>(dataPtr), sizeof(T) * data_.size());
}

template <typename T>
void MetaDataType<T>::deserialize(std::ifstream& inFile, const size_t numElements) {
    data_.resize(numElements);
    auto data = data_.data();

    inFile.read(reinterpret_cast<char*>(&type_), sizeof(uint64_t));
    inFile.read(reinterpret_cast<char*>(data), sizeof(MetaDataType<T>::TType) * numElements);
}

template <typename T>
std::pair<typename MetaDataType<T>::TType, typename MetaDataType<T>::TType>
MetaDataType<T>::getMinMax() const {
    auto minmax = util::dataMinMax(data_.data(), data_.size());
    return std::make_pair(util::glm_convert<MetaDataType<T>::TType>(minmax.first),
                          util::glm_convert<MetaDataType<T>::TType>(minmax.second));
}

template <typename T>
const std::vector<T>& MetaDataType<T>::getData() const {
    return data_;
}

template <typename T>
MetaDataType<T>::MetaDataType(std::vector<T> data, const TensorFeature type)
    : data_(std::move(data)), type_(type) {}

template <typename T>
const void* MetaDataType<T>::getDataPtr() const {
    return data_.data();
}

template <typename T>
size_t MetaDataType<T>::getNumberOfComponents() const {
    return util::extent<MetaDataType<T>::TType>::value;
}

/***************************************************
 *                SPECIALIZATIONS                  *
 ***************************************************/

struct I1 : public MetaDataType<glm::f64> {
    I1() = default;

    explicit I1(const std::vector<double>& data, TensorFeature type) : MetaDataType(data, type) {};

    I1* clone() const final { return new I1(data_, type_); }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("I1"); }
};

struct I2 : MetaDataType<glm::f64> {
    I2() = default;

    explicit I2(const std::vector<double>& data, TensorFeature type) : MetaDataType(data, type) {};

    I2* clone() const final { return new I2(data_, type_); }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("I2"); }
};

struct I3 : MetaDataType<glm::f64> {
    I3() = default;

    explicit I3(const std::vector<double>& data, TensorFeature type) : MetaDataType(data, type) {};

    I3* clone() const final { return new I3(data_, type_); }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("I3"); }
};

struct J1 : MetaDataType<glm::f64> {
    J1() = default;

    explicit J1(const std::vector<double>& data, TensorFeature type) : MetaDataType(data, type) {};

    J1* clone() const final { return new J1(data_, type_); }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("J1"); }
};

struct J2 : MetaDataType<glm::f64> {
    J2() = default;

    explicit J2(const std::vector<double>& data, TensorFeature type) : MetaDataType(data, type) {};

    J2* clone() const final { return new J2(data_, type_); }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("J2"); }
};

struct J3 : MetaDataType<glm::f64> {
    J3() = default;

    explicit J3(const std::vector<double>& data, TensorFeature type) : MetaDataType(data, type) {};

    J3* clone() const final { return new J3(data_, type_); }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("J3"); }
};

struct MajorEigenVectors : MetaDataType<dvec3> {
    MajorEigenVectors() = default;

    explicit MajorEigenVectors(const std::vector<dvec3>& data, TensorFeature type)
        : MetaDataType(data, type) {};

    MajorEigenVectors* clone() const final { return new MajorEigenVectors(data_, type_); }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("MajorEigenVector"); }
};

struct IntermediateEigenVectors : MetaDataType<dvec3> {
    IntermediateEigenVectors() = default;

    explicit IntermediateEigenVectors(const std::vector<dvec3>& data, TensorFeature type)
        : MetaDataType(data, type) {};

    IntermediateEigenVectors* clone() const final {
        return new IntermediateEigenVectors(data_, type_);
    }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("IntermediateEigenVector"); }
};

struct MinorEigenVectors : MetaDataType<dvec3> {
    MinorEigenVectors() = default;

    explicit MinorEigenVectors(const std::vector<dvec3>& data, TensorFeature type)
        : MetaDataType(data, type) {};

    MinorEigenVectors* clone() const final { return new MinorEigenVectors(data_, type_); }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("MinorEigenVector"); }
};

struct MajorEigenValues : MetaDataType<glm::f64> {
    MajorEigenValues() = default;

    explicit MajorEigenValues(const std::vector<double>& data, TensorFeature type)
        : MetaDataType(data, type) {};

    MajorEigenValues* clone() const final { return new MajorEigenValues(data_, type_); }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("Sigma1"); }
};

struct IntermediateEigenValues : MetaDataType<glm::f64> {
    IntermediateEigenValues() = default;

    explicit IntermediateEigenValues(const std::vector<double>& data, TensorFeature type)
        : MetaDataType(data, type) {};

    IntermediateEigenValues* clone() const final {
        return new IntermediateEigenValues(data_, type_);
    }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("Sigma2"); }
};

struct MinorEigenValues : MetaDataType<glm::f64> {
    MinorEigenValues() = default;

    explicit MinorEigenValues(const std::vector<double>& data, TensorFeature type)
        : MetaDataType(data, type) {};

    MinorEigenValues* clone() const final { return new MinorEigenValues(data_, type_); }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("Sigma3"); }
};

struct LodeAngle : MetaDataType<glm::f64> {
    LodeAngle() = default;

    explicit LodeAngle(const std::vector<double>& data, TensorFeature type)
        : MetaDataType(data, type) {};

    LodeAngle* clone() const final { return new LodeAngle(data_, type_); }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("LodeAngle"); }
};

struct Anisotropy : MetaDataType<glm::f64> {
    Anisotropy() = default;

    explicit Anisotropy(const std::vector<double>& data, TensorFeature type)
        : MetaDataType(data, type) {};

    Anisotropy* clone() const final { return new Anisotropy(data_, type_); }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("Anisotropy"); }
};

struct LinearAnisotropy : MetaDataType<glm::f64> {
    LinearAnisotropy() = default;

    explicit LinearAnisotropy(const std::vector<double>& data, TensorFeature type)
        : MetaDataType(data, type) {};

    LinearAnisotropy* clone() const final { return new LinearAnisotropy(data_, type_); }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("LinearAnisotropy"); }
};

struct PlanarAnisotropy : MetaDataType<glm::f64> {
    PlanarAnisotropy() = default;

    explicit PlanarAnisotropy(const std::vector<double>& data, TensorFeature type)
        : MetaDataType(data, type) {};

    PlanarAnisotropy* clone() const final { return new PlanarAnisotropy(data_, type_); }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("PlanarAnisotropy"); }
};

struct SphericalAnisotropy : MetaDataType<glm::f64> {
    SphericalAnisotropy() = default;

    explicit SphericalAnisotropy(const std::vector<double>& data, TensorFeature type)
        : MetaDataType(data, type) {};

    SphericalAnisotropy* clone() const final { return new SphericalAnisotropy(data_, type_); }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("SphericalAnisotropy"); }
};

struct Diffusivity : MetaDataType<glm::f64> {
    Diffusivity() = default;

    explicit Diffusivity(const std::vector<double>& data, TensorFeature type)
        : MetaDataType(data, type) {};

    Diffusivity* clone() const final { return new Diffusivity(data_, type_); }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("Diffusivity"); }
};

struct ShearStress : MetaDataType<glm::f64> {
    ShearStress() = default;

    explicit ShearStress(const std::vector<double>& data, TensorFeature type)
        : MetaDataType(data, type) {};

    ShearStress* clone() const final { return new ShearStress(data_, type_); }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("ShearStress"); }
};

struct PureShear : MetaDataType<glm::f64> {
    PureShear() = default;

    explicit PureShear(const std::vector<double>& data, TensorFeature type)
        : MetaDataType(data, type) {};

    PureShear* clone() const final { return new PureShear(data_, type_); }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("PureShear"); }
};

struct ShapeFactor : MetaDataType<glm::f64> {
    ShapeFactor() = default;

    explicit ShapeFactor(const std::vector<double>& data, TensorFeature type)
        : MetaDataType(data, type) {};

    ShapeFactor* clone() const final { return new ShapeFactor(data_, type_); }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("ShapeFactor"); }
};

struct IsotropicScaling : MetaDataType<glm::f64> {
    IsotropicScaling() = default;

    explicit IsotropicScaling(const std::vector<double>& data, TensorFeature type)
        : MetaDataType(data, type) {};

    IsotropicScaling* clone() const final { return new IsotropicScaling(data_, type_); }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("IsotropicScaling"); }
};

struct Rotation : MetaDataType<glm::f64> {
    Rotation() = default;

    explicit Rotation(const std::vector<double>& data, TensorFeature type)
        : MetaDataType(data, type) {};

    Rotation* clone() const final { return new Rotation(data_, type_); }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("Rotation"); }
};

struct FrobeniusNorm : MetaDataType<glm::f64> {
    FrobeniusNorm() = default;

    explicit FrobeniusNorm(const std::vector<double>& data, TensorFeature type)
        : MetaDataType(data, type) {};

    FrobeniusNorm* clone() const final { return new FrobeniusNorm(data_, type_); }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("FrobeniusNorm"); }
};

struct HillYieldCriterion : MetaDataType<glm::f64> {
    HillYieldCriterion() = default;

    explicit HillYieldCriterion(const std::vector<double>& data, TensorFeature type)
        : MetaDataType(data, type) {};

    HillYieldCriterion* clone() const final { return new HillYieldCriterion(data_, type_); }

    uint64_t getId() const final { return id(); }

    static constexpr uint64_t id() { return util::constexpr_hash("HillYieldCriterion"); }
};

}  // namespace tensor

}  //  namespace inviwo
