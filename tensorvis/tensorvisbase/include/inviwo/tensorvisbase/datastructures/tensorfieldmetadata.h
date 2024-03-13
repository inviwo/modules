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
    static std::string name() { return "TensorFeature"; }
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

#include "tensorfieldmetadataspecializations.h"

}  // namespace tensor

}  //  namespace inviwo
