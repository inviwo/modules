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
 * 1. Redistributions of source code must retain the above copyright notice,
 *this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 *FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#pragma once

#include <warn/push>
#include <warn/ignore/all>
#include <Eigen/Dense>
#include <warn/pop>
#include <inviwo/core/datastructures/geometry/basicmesh.h>
#include <modules/eigenutils/eigenutils.h>
#include <modules/opengl/shader/shaderutils.h>
#include <inviwo/tensorvisbase/tensorvisbasemoduledefine.h>
#include <limits>

namespace inviwo {
namespace util {
/**
 * Returns the trace of the tensor.
 */
template <typename T, glm::length_t N>
T trace(const glm::mat<N, N, T> &tensor) {
    T sum{0};

    for (unsigned int i{0}; i < N; ++i) {
        sum += tensor[i][i];
    }
    return sum;
}

/**
 * Returns the Nth eigen value of the tensor (counting starts at 0, i.e. the first eigen value has
 * index 0).
 */
template <unsigned int N, typename T, glm::length_t M>
T eigenvalue(const glm::mat<M, M, T> &tensor) {
    if constexpr (N < M) {
        if (tensor == glm::mat<M, M, T>(0)) {
            return T(0);
        }

        Eigen::EigenSolver<Eigen::Matrix<T, M, M>> solver(util::glm2eigen(tensor));

        auto sortable = std::array<T, M>{};

        for (unsigned int i{0}; i < M; ++i) {
            sortable[i] = solver.eigenvalues().col(0)[i].real();
        }

        std::sort(sortable.begin(), sortable.end(), [](const T A, const T B) { return A > B; });

        return sortable[N];
    }
}

/**
 * Returns the Nth eigen vector of the tensor (counting starts at 0, i.e. the first eigen value has
 * index 0).
 */
template <unsigned int N, typename T, glm::length_t M>
glm::vec<M, T> eigenvector(const glm::mat<M, M, T> &tensor) {
    using vec_type = glm::vec<M, T>;

    if constexpr (N < M) {
        if (tensor == glm::mat<M, M, T>(0)) {
            return vec_type(0);
        }

        Eigen::EigenSolver<Eigen::Matrix<T, M, M>> solver(util::glm2eigen(tensor));

        std::vector<std::pair<T, vec_type>> sortable;
        for (unsigned int i{0}; i < M; ++i) {
            auto vec = vec_type{0};
            for (unsigned int j{0}; j < M; ++j) {
                vec[j] = solver.eigenvectors().col(i).real()[j];
            }
            sortable.emplace_back(solver.eigenvalues().col(0)[i].real(), vec);
        }

        std::sort(sortable.begin(), sortable.end(),
                  [](const std::pair<T, vec_type> &pairA, const std::pair<T, vec_type> &pairB) {
                      return pairA.first > pairB.first;
                  });

        return sortable[N].second;
    }
}

namespace detail {
template <typename T>
T constexpr constexpr_sqrt_helper_f(T x, T curr, T prev) {
    if constexpr (std::is_floating_point_v<T>) {
        return curr == prev ? curr
                            : detail::constexpr_sqrt_helper_f(x, T(0.5) * (curr + x / curr), curr);
    }
}

template <typename T>
constexpr T constexpr_sqrt_helper_i(const T x, const T lo, const T hi) {
    if constexpr (std::is_integral_v<T>) {
        if (lo == hi) return lo;

        const T mid = (lo + hi + 1) / 2;

        if (x / mid < mid)
            return constexpr_sqrt_helper_i(x, lo, mid - 1);
        else
            return constexpr_sqrt_helper_i(x, mid, hi);
    }
}
}  // namespace detail

/**
 * Constexpr version of the square root method.
 *
 * \param x Value of which the square root should be calculated.
 * \return Square root of x.
 */
template <typename T>
T constexpr constexpr_sqrt(T x) {
    if constexpr (std::is_floating_point_v<T>) {
        if (x >= 0 && x < std::numeric_limits<T>::infinity())
            return detail::constexpr_sqrt_helper_f(x, x, T(0));
    }
    if constexpr (std::is_integral_v<T>) {
        return detail::constexpr_sqrt_helper_i(x, T(0), x / 2 + 1);
    }

    return std::numeric_limits<T>::quiet_NaN();
}

enum class YieldCriterion {
    William_Warnke,
    Von_Mises,
    Mohr_Coulomb,
    Tresca,
    Drucker_Prager,
    Matrix_Constituent_Failure_Criterion,
    MohrCoulomb_InvariantBased,
    Tsai_Hill,
    Tsai_Wu
};

template <class Elem, class Traits>
std::basic_ostream<Elem, Traits> &operator<<(std::basic_ostream<Elem, Traits> &os,
                                             YieldCriterion yieldCriterion) {
    switch (yieldCriterion) {
        case YieldCriterion::William_Warnke:
            os << "William-Warnke";
            break;
        case YieldCriterion::Von_Mises:
            os << "von Mises";
            break;
        case YieldCriterion::Mohr_Coulomb:
            os << "Mohr-Coulomb";
            break;
        case YieldCriterion::Tresca:
            os << "Tresca";
            break;
        case YieldCriterion::Drucker_Prager:
            os << "Drucker-Prager";
            break;
        case YieldCriterion::Matrix_Constituent_Failure_Criterion:
            os << "Matrix constituent";
            break;
        case YieldCriterion::MohrCoulomb_InvariantBased:
            os << "Mohr-Coulomb invariant based";
            break;
        case YieldCriterion::Tsai_Hill:
            os << "Tsai-Hill";
            break;
        case YieldCriterion::Tsai_Wu:
            os << "Tsai-Wu";
            break;
    }

    return os;
}

}  // namespace util
namespace tensorutil {

enum class Anisotropy {
    abs_lamda1_minus_lamda2,
    abs_lamda1_minus_lamda3,
    barycentric,
    abs_lamda1_minus_abs_lamda2
};

/*
 * Specifies several common decompositions for tensors.
 * See: https://diglib.eg.org/handle/10.1111/v32i1pp049-074
 */
enum class Decomposition {
    Symmetric_Antisymmetric,
    Stretch_Rotation,
    Shape_Orientation,
    Isotropic_Anisotropic
};

/*
 * Projects the 3D tensor T onto the given cartesian plane. This means for
 * cartesian plane spanned by
 *
 *                                         | Txx   Tyx |
 * [x,y] we get the projected tensor T_p = |           |
 *                                         | Txy   Tyy |
 *
 *
 *                                         | Txz   Tzx |
 * [x,z] we get the projected tensor T_p = |           |
 *                                         | Txz   Tzz |
 *
 *
 *                                         | Tyy   Tzy |
 * [y,z] we get the projected tensor T_p = |           |
 *                                         | Tyz   Tzz |
 */
dmat2 IVW_MODULE_TENSORVISBASE_API getProjectedTensor(const dmat3 tensor,
                                                      const CartesianCoordinateAxis axis);

/*
 * Decomposes the tensor into the specified components. The method returns a
 * pair of tensors where pair.first and pair.last equal to the order specified
 * in the decomposition enumerator, e.g. for Symmetric_Antisymmetric pair.first
 * is the symmetric part and pair.second the antisymmetric part. See:
 * https://diglib.eg.org/handle/10.1111/v32i1pp049-074
 */
std::pair<dmat2, dmat2> IVW_MODULE_TENSORVISBASE_API decompose(const dmat2 &tensor,
                                                               Decomposition decomposition);

/*
 * Decomposes the tensor into the specified components. The method returns a
 * pair of tensors where pair.first and pair.last equal to the order specified
 * in the decomposition enumerator, e.g. for Symmetric_Antisymmetric pair.first
 * is the symmetric part and pair.second the antisymmetric part. See:
 * https://diglib.eg.org/handle/10.1111/v32i1pp049-074
 */
std::pair<dmat3, dmat3> IVW_MODULE_TENSORVISBASE_API decompose(const dmat3 &tensor,
                                                               Decomposition decomposition);

dmat3 IVW_MODULE_TENSORVISBASE_API reconstruct(const std::array<double, 3> &eigenvalues,
                                               const std::array<dvec3, 3> &eigenvectors);

vec4 IVW_MODULE_TENSORVISBASE_API tensor2DToDvec4(const dmat2 &tensor);

dmat3 IVW_MODULE_TENSORVISBASE_API stressDeviatorTensor(const dmat3 &tensor);

dmat3 IVW_MODULE_TENSORVISBASE_API meanNormalStressTensor(const dmat3 &tensor);
dmat3 IVW_MODULE_TENSORVISBASE_API meanHydrostaticStressTensor(const dmat3 &tensor);
dmat3 IVW_MODULE_TENSORVISBASE_API volumetricStressTensor(const dmat3 &tensor);

std::array<std::pair<double, dvec3>, 3> IVW_MODULE_TENSORVISBASE_API
calculateEigenValuesAndEigenVectors(const dmat3 &tensor);

std::array<double, 3> IVW_MODULE_TENSORVISBASE_API calculateEigenValues(const dmat3 &tensor);

dmat3 IVW_MODULE_TENSORVISBASE_API calculateEigenSystem(const dmat3 &tensor);

static const std::string lamda_str{u8"λ"};

static const std::string lamda1_str{u8"λ₁"};

static const std::string lamda2_str{u8"λ₂"};

static const std::string lamda3_str{u8"λ₃"};

static const std::string theta_str{u8"θ"};

static const std::string phi_str{u8"φ"};

static const std::string sigma_str{u8"σ"};

static const std::string sigma1_str{u8"σ₁"};
static const std::string sigma1safe_str{u8"σ1"};

static const std::string sigma2_str{u8"σ₂"};
static const std::string sigma2safe_str{u8"σ2"};

static const std::string sigma3_str{u8"σ₃"};
static const std::string sigma3safe_str{u8"σ3"};

static const std::string i1_str{u8"I₁"};

static const std::string i2_str{u8"I₂"};

static const std::string i3_str{u8"I₃"};

static const std::string j1_str{u8"J₁"};

static const std::string j2_str{u8"J₂"};

static const std::string j3_str{u8"J₃"};

static const std::string tau_str{u8"τ"};
}  // namespace tensorutil

}  // namespace inviwo
