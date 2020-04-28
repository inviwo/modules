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

namespace inviwo {
namespace attributes {
template <glm::length_t N>
inline std::vector<typename Trace::value_type> Trace::calculate(
    std::shared_ptr<const std::vector<glm::mat<N, N, typename Trace::scalar_type>>> tensors,
    std::shared_ptr<const DataFrame> metaData) {
    std::vector<typename Trace::value_type> trace{};
    for (const auto& tensor : *tensors) {
        trace.emplace_back(util::trace(tensor));
    }
    return trace;
}

template <glm::length_t N>
inline std::vector<typename Norm::value_type> Norm::calculate(
    std::shared_ptr<const std::vector<glm::mat<N, N, typename Norm::scalar_type>>> tensors,
    std::shared_ptr<const DataFrame> metaData) {
    std::vector<typename Norm::value_type> norm{};
    for (const auto& tensor : *tensors) {
        typename Norm::value_type sum{0};
        for (unsigned int i{0}; i < N; ++i) {
            for (unsigned int j{0}; j < N; ++j) {
                sum += tensor[i][j] * tensor[i][j];
            }
        }
        norm.push_back(std::sqrt(sum));
    }
    return norm;
}

template <glm::length_t N>
inline std::vector<typename MajorEigenValue::value_type> MajorEigenValue::calculate(
    std::shared_ptr<const std::vector<glm::mat<N, N, typename MajorEigenValue::scalar_type>>>
        tensors,
    std::shared_ptr<const DataFrame> metaData) {
    std::vector<typename MajorEigenValue::value_type> l1{};
    for (const auto& tensor : *tensors) {
        l1.emplace_back(util::eigenvalue<0>(tensor));
    }
    return l1;
}

template <glm::length_t N>
inline std::vector<typename IntermediateEigenValue::value_type> IntermediateEigenValue::calculate(
    std::shared_ptr<const std::vector<glm::mat<N, N, typename IntermediateEigenValue::scalar_type>>>
    tensors,
    std::shared_ptr<const DataFrame> metaData) {
    if constexpr (N % 2) {
        std::vector<typename IntermediateEigenValue::value_type> l2{};
        for (const auto& tensor : *tensors) {
            l2.emplace_back(util::eigenvalue<N / 2>(tensor));
        }
        return l2;
    }
}

template <glm::length_t N>
inline std::vector<typename MinorEigenValue::value_type> MinorEigenValue::calculate(
    std::shared_ptr<const std::vector<glm::mat<N, N, typename MinorEigenValue::scalar_type>>>
    tensors,
    std::shared_ptr<const DataFrame> metaData) {
    std::vector<typename MinorEigenValue::value_type> ln{};
    for (const auto& tensor : *tensors) {
        ln.emplace_back(util::eigenvalue<N - 1>(tensor));
    }
    return ln;
}

template <glm::length_t N>
inline std::vector<typename VectorBase<N>::value_type> MajorEigenVector<N>::calculate(
    std::shared_ptr<const std::vector<glm::mat<N, N, typename VectorBase<N>::scalar_type>>>
    tensors,
    std::shared_ptr<const DataFrame> metaData) {
    std::vector<typename VectorBase<N>::value_type> ev{};
    for (const auto& tensor : *tensors) {
        ev.emplace_back(util::eigenvector<0>(tensor));
    }
    return ev;
}

template <glm::length_t N>
inline std::vector<typename VectorBase<N>::value_type> IntermediateEigenVector<N>::calculate(
    std::shared_ptr<const std::vector<glm::mat<N, N, typename VectorBase<N>::scalar_type>>>
    tensors,
    std::shared_ptr<const DataFrame> metaData) {
    if constexpr (N % 2) {
        std::vector<typename VectorBase<N>::value_type> ev{};
        for (const auto& tensor : *tensors) {
            ev.emplace_back(util::eigenvector<N / 2>(tensor));
        }
        return ev;
    }
}

template <glm::length_t N>
inline std::vector<typename VectorBase<N>::value_type> MinorEigenVector<N>::calculate(
    std::shared_ptr<const std::vector<glm::mat<N, N, typename VectorBase<N>::scalar_type>>>
    tensors,
    std::shared_ptr<const DataFrame> metaData) {
    std::vector<typename VectorBase<N>::value_type> ev{};
    for (const auto& tensor : *tensors) {
        ev.emplace_back(util::eigenvector<N - 1>(tensor));
    }
    return ev;
}

//template <glm::length_t N>
//inline std::vector<typename XXX::value_type> XXX::calculate(
//    std::shared_ptr<const std::vector<glm::mat<N, N, typename XXX::scalar_type>>>
//    tensors,
//    std::shared_ptr<const DataFrame> metaData) {
//    std::vector<typename XXX::value_type> YYY{};
//    for (const auto& tensor : *tensors) {
//        ;
//    }
//    return YYY;
//}

template <glm::length_t N>
inline std::vector<typename I1::value_type> I1::calculate(
    std::shared_ptr<const std::vector<glm::mat<N, N, typename I1::scalar_type>>>
    tensors,
    std::shared_ptr<const DataFrame> metaData) {
        auto i1 = std::vector<value_type>();
        i1.reserve(tensors->size());
        for (const auto& tensor : *tensors) {
            i1.emplace_back(util::trace(tensor));
        }
        return i1;
    }

template <glm::length_t N>
inline std::vector<typename I2::value_type> I2::calculate(
    std::shared_ptr<const std::vector<glm::mat<N, N, typename I2::scalar_type>>>
    tensors,
    std::shared_ptr<const DataFrame> metaData) {
        if constexpr (N == 3) {
            auto i2 = std::vector<value_type>();
            for (const auto& tensor : *tensors) {
                i2.emplace_back(tensor[0][0] * tensor[1][1] + tensor[1][1] * tensor[2][2] +
                                tensor[0][0] * tensor[2][2] - tensor[0][1] * tensor[0][1] -
                                tensor[1][2] * tensor[1][2] - tensor[2][0] * tensor[2][0]);
            }
            return i2;
        }
    }

template <glm::length_t N>
inline std::vector<typename I3::value_type> I3::calculate(
    std::shared_ptr<const std::vector<glm::mat<N, N, typename I3::scalar_type>>>
    tensors,
    std::shared_ptr<const DataFrame> metaData) {
        if constexpr (N == 3) {
            auto i3 = std::vector<value_type>();
            for (const auto& tensor : *tensors) {
                i3.emplace_back(tensor[0][0] * tensor[1][1] * tensor[2][2] +
                                value_type(2) * tensor[0][1] * tensor[1][2] * tensor[2][0] -
                                tensor[0][1] * tensor[0][1] * tensor[2][2] -
                                tensor[1][2] * tensor[1][2] * tensor[0][0] -
                                tensor[2][0] * tensor[2][0] * tensor[1][1]);
            }
            return i3;
        }
    }

template <glm::length_t N>
inline std::vector<typename J1::value_type> J1::calculate(
    std::shared_ptr<const std::vector<glm::mat<N, N, typename J1::scalar_type>>>
    tensors,
    std::shared_ptr<const DataFrame> metaData) {
        auto j1 = std::vector<value_type>(tensors->size());
        std::fill(j1.begin(), j1.end(), value_type(0));
        return j1;
    }

template <glm::length_t N>
inline std::vector<typename J2::value_type> J2::calculate(
    std::shared_ptr<const std::vector<glm::mat<N, N, typename J2::scalar_type>>>
    tensors,
    std::shared_ptr<const DataFrame> metaData) {
        if constexpr (N == 3) {
            auto j2 = std::vector<value_type>();
            const auto i1 = I1::calculate(tensors, metaData);
            const auto i2 = I2::calculate(tensors, metaData);
            for (size_t i{0}; i < tensors->size(); ++i) {
                j2.emplace_back((value_type(1. / 3.)) * i1[i] * i1[i] - i2[i]);
            }
            return j2;
        }
    }

template <glm::length_t N>
inline std::vector<typename J3::value_type> J3::calculate(
    std::shared_ptr<const std::vector<glm::mat<N, N, typename J3::scalar_type>>>
    tensors,
    std::shared_ptr<const DataFrame> metaData) {
        if constexpr (N == 3) {
            auto j3 = std::vector<value_type>();
            const auto i1 = I1::calculate(tensors, metaData);
            const auto i2 = I2::calculate(tensors, metaData);
            const auto i3 = I3::calculate(tensors, metaData);

            for (size_t i{0}; i < tensors->size(); ++i) {
                j3.emplace_back(value_type(2.0 / 27.0) * i1[i] * i1[i] * i1[i] -
                                value_type(1.0 / 3.0) * i1[i] * i2[i] + i3[i]);
            }
            return j3;
        }
    }

template <glm::length_t N>
inline std::vector<typename LodeAngle::value_type> LodeAngle::calculate(
    std::shared_ptr<const std::vector<glm::mat<N, N, typename LodeAngle::scalar_type>>>
    tensors,
    std::shared_ptr<const DataFrame> metaData) {
        if constexpr (N == 3) {
            auto lodeAngle = std::vector<value_type>();
            constexpr auto a =
                (value_type(3.0) * util::constexpr_sqrt(value_type(3.0))) * value_type(0.5);
            constexpr auto third = value_type(1.0 / 3.0);

            const auto j2 = J2::calculate(tensors, metaData);
            const auto j3 = J3::calculate(tensors, metaData);

            for (size_t i{0}; i < tensors->size(); ++i) {
                const auto b = j3[i] / std::pow(j2[i], value_type(1.5));

                lodeAngle.emplace_back(third * std::acos(a * b));
            }
            return lodeAngle;
        }
    }

namespace {
template <glm::length_t N, typename T>
std::vector<std::array<T, N>> sortedEigenValues(
    std::shared_ptr<const std::vector<glm::mat<N, N, T>>> tensors,
    std::shared_ptr<const DataFrame> metaData) {

    std::vector<T> mae;
    std::vector<T> ine;
    std::vector<T> mie;

    if (!metaData->getColumn(std::string(MajorEigenValue::identifier))) {
        mae = MajorEigenValue::calculate(tensors, metaData);
    }
    if (!metaData->getColumn(std::string(IntermediateEigenValue::identifier))) {
        ine = IntermediateEigenValue::calculate(tensors, metaData);
    }
    if (!metaData->getColumn(std::string(MinorEigenValue::identifier))) {
        mie = MinorEigenValue::calculate(tensors, metaData);
    }

    const auto& majorEigenValues =
        mae.empty() ? std::dynamic_pointer_cast<const TemplateColumn<T>>(
                          metaData->getColumn(std::string(MajorEigenValue::identifier)))
                          ->getTypedBuffer()
                          ->getRAMRepresentation()
                          ->getDataContainer()
                    : mae;
    const auto& intermediateEigenValues =
        ine.empty() ? std::dynamic_pointer_cast<const TemplateColumn<T>>(
                          metaData->getColumn(std::string(IntermediateEigenValue::identifier)))
                          ->getTypedBuffer()
                          ->getRAMRepresentation()
                          ->getDataContainer()
                    : ine;
    const auto& minorEigenValues =
        mie.empty() ? std::dynamic_pointer_cast<const TemplateColumn<T>>(
                          metaData->getColumn(std::string(MinorEigenValue::identifier)))
                          ->getTypedBuffer()
                          ->getRAMRepresentation()
                          ->getDataContainer()
                    : mie;

    auto ev = std::vector<std::array<T, N>>{};

    for (size_t i = 0; i < majorEigenValues.size(); i++) {
        std::array<T, N> eigenValues{majorEigenValues[i], intermediateEigenValues[i],
                                     minorEigenValues[i]};

        std::transform(eigenValues.begin(), eigenValues.end(), eigenValues.begin(),
                       [](const T& val) { return glm::abs(val); });

        std::sort(eigenValues.begin(), eigenValues.end(),
                  [](const T& valA, const T& valB) { return valA > valB; });

        ev.push_back(eigenValues);
    }

    return ev;
}
}  // namespace

template <glm::length_t N>
inline std::vector<typename Anisotropy::value_type> Anisotropy::calculate(
    std::shared_ptr<const std::vector<glm::mat<N, N, typename Anisotropy::scalar_type>>>
    tensors,
    std::shared_ptr<const DataFrame> metaData) {
        if constexpr (N == 3) {
            std::vector<scalar_type> anisotropy{};
            auto ev = sortedEigenValues(tensors, metaData);
            for (const auto& eigenValues : ev) {
                auto denominator = eigenValues[0] + eigenValues[N / 2] + eigenValues[N - 1];
                if (denominator < std::numeric_limits<scalar_type>::epsilon())
                    denominator = std::numeric_limits<scalar_type>::epsilon();

                anisotropy.emplace_back(std::abs(eigenValues[0] - eigenValues[2]));
            }
            return anisotropy;
        }
    }

template <glm::length_t N>
inline std::vector<typename LinearAnisotropy::value_type> LinearAnisotropy::calculate(
    std::shared_ptr<const std::vector<glm::mat<N, N, typename LinearAnisotropy::scalar_type>>>
    tensors,
    std::shared_ptr<const DataFrame> metaData) {
        if constexpr (N == 3) {
            std::vector<scalar_type> linearAnisotropy{};
            auto ev = sortedEigenValues(tensors, metaData);
            for (const auto& eigenValues : ev) {
                auto denominator = eigenValues[0] + eigenValues[N / 2] + eigenValues[N - 1];
                if (denominator < std::numeric_limits<scalar_type>::epsilon())
                    denominator = std::numeric_limits<scalar_type>::epsilon();

                linearAnisotropy.emplace_back((eigenValues[0] - eigenValues[1]) / denominator);
            }
            return linearAnisotropy;
        }
    }

template <glm::length_t N>
inline std::vector<typename PlanarAnisotropy::value_type> PlanarAnisotropy::calculate(
    std::shared_ptr<const std::vector<glm::mat<N, N, typename PlanarAnisotropy::scalar_type>>>
    tensors,
    std::shared_ptr<const DataFrame> metaData) {
        if constexpr (N == 3) {
            std::vector<scalar_type> planarAnisotropy{};
            auto ev = sortedEigenValues(tensors, metaData);
            for (const auto& eigenValues : ev) {
                auto denominator = eigenValues[0] + eigenValues[N / 2] + eigenValues[N - 1];
                if (denominator < std::numeric_limits<scalar_type>::epsilon())
                    denominator = std::numeric_limits<scalar_type>::epsilon();

                planarAnisotropy.emplace_back((scalar_type(2) * (eigenValues[1] - eigenValues[2])) /
                                              denominator);
            }
            return planarAnisotropy;
        }
    }

template <glm::length_t N>
inline std::vector<typename SphericalAnisotropy::value_type> SphericalAnisotropy::calculate(
    std::shared_ptr<const std::vector<glm::mat<N, N, typename SphericalAnisotropy::scalar_type>>>
    tensors,
    std::shared_ptr<const DataFrame> metaData) {
        if constexpr (N == 3) {
            std::vector<scalar_type> sphericalAnisotropy{};
            auto ev = sortedEigenValues(tensors, metaData);
            for (const auto& eigenValues : ev) {
                auto denominator = eigenValues[0] + eigenValues[N / 2] + eigenValues[N - 1];
                if (denominator < std::numeric_limits<scalar_type>::epsilon())
                    denominator = std::numeric_limits<scalar_type>::epsilon();

                sphericalAnisotropy.emplace_back((scalar_type(3) * eigenValues[2]) / denominator);
            }
            return sphericalAnisotropy;
        }
    }

template <glm::length_t N>
inline std::vector<typename FrobeniusNorm::value_type> FrobeniusNorm::calculate(
    std::shared_ptr<const std::vector<glm::mat<N, N, typename FrobeniusNorm::scalar_type>>>
    tensors,
    std::shared_ptr<const DataFrame> metaData) {
        std::vector<value_type> ret;
        ret.resize(tensors->size());
        return ret;
    }

}  // namespace attributes
}  // namespace inviwo
