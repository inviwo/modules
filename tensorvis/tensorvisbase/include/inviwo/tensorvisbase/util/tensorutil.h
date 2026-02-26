/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2026 Inviwo Foundation
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

#include <inviwo/tensorvisbase/tensorvisbasemoduledefine.h>

#include <inviwo/core/datastructures/geometry/geometrytype.h>
#include <inviwo/core/util/glmmat.h>
#include <inviwo/core/util/glmvec.h>

#include <iostream>

namespace inviwo::tensorutil {

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

constexpr std::string_view format_as(YieldCriterion yieldCriterion) {
    switch (yieldCriterion) {
        case YieldCriterion::William_Warnke:
            return "William-Warnke";
        case YieldCriterion::Von_Mises:
            return "von Mises";
        case YieldCriterion::Mohr_Coulomb:
            return "Mohr-Coulomb";
        case YieldCriterion::Tresca:
            return "Tresca";
        case YieldCriterion::Drucker_Prager:
            return "Drucker-Prager";
        case YieldCriterion::Matrix_Constituent_Failure_Criterion:
            return "Matrix constituent";
        case YieldCriterion::MohrCoulomb_InvariantBased:
            return "Mohr-Coulomb invariant based";
        case YieldCriterion::Tsai_Hill:
            return "Tsai-Hill";
        case YieldCriterion::Tsai_Wu:
            return "Tsai-Wu";
    }
}

template <class Elem, class Traits>
std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& os,
                                             YieldCriterion yieldCriterion) {
    os << format_as(yieldCriterion);
    return os;
}

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
std::pair<dmat2, dmat2> IVW_MODULE_TENSORVISBASE_API decompose(const dmat2& tensor,
                                                               Decomposition decomposition);

/*
 * Decomposes the tensor into the specified components. The method returns a
 * pair of tensors where pair.first and pair.last equal to the order specified
 * in the decomposition enumerator, e.g. for Symmetric_Antisymmetric pair.first
 * is the symmetric part and pair.second the antisymmetric part. See:
 * https://diglib.eg.org/handle/10.1111/v32i1pp049-074
 */
std::pair<dmat3, dmat3> IVW_MODULE_TENSORVISBASE_API decompose(const dmat3& tensor,
                                                               Decomposition decomposition);

dmat3 IVW_MODULE_TENSORVISBASE_API reconstruct(const std::array<double, 3>& eigenvalues,
                                               const std::array<dvec3, 3>& eigenvectors);

vec4 IVW_MODULE_TENSORVISBASE_API tensor2DToDvec4(const dmat2& tensor);

double IVW_MODULE_TENSORVISBASE_API trace(const dmat2& tensor);

double IVW_MODULE_TENSORVISBASE_API trace(const dmat3& tensor);

double IVW_MODULE_TENSORVISBASE_API calculateI1(const dmat3& tensor);
double IVW_MODULE_TENSORVISBASE_API calculateI2(const dmat3& tensor);
double IVW_MODULE_TENSORVISBASE_API calculateI3(const dmat3& tensor);

double IVW_MODULE_TENSORVISBASE_API calculateJ1(const dmat3& tensor);
double IVW_MODULE_TENSORVISBASE_API calculateJ2(const dmat3& tensor);
double IVW_MODULE_TENSORVISBASE_API calculateJ3(const dmat3& tensor);

double IVW_MODULE_TENSORVISBASE_API calculateLodeAngle(const dmat3& tensor);

dmat3 IVW_MODULE_TENSORVISBASE_API stressDeviatorTensor(const dmat3& tensor);

dmat3 IVW_MODULE_TENSORVISBASE_API meanNormalStressTensor(const dmat3& tensor);
dmat3 IVW_MODULE_TENSORVISBASE_API meanHydrostaticStressTensor(const dmat3& tensor);
dmat3 IVW_MODULE_TENSORVISBASE_API volumetricStressTensor(const dmat3& tensor);

std::array<std::pair<double, dvec3>, 3> IVW_MODULE_TENSORVISBASE_API
calculateEigenValuesAndEigenVectors(const dmat3& tensor);

std::array<double, 3> IVW_MODULE_TENSORVISBASE_API calculateEigenValues(const dmat3& tensor);

dmat3 IVW_MODULE_TENSORVISBASE_API calculateEigenSystem(const dmat3& tensor);

constexpr std::string_view lamda_str{"λ"};
constexpr std::string_view lamda1_str{"λ₁"};
constexpr std::string_view lamda2_str{"λ₂"};
constexpr std::string_view lamda3_str{"λ₃"};

constexpr std::string_view theta_str{"θ"};
constexpr std::string_view phi_str{"φ"};

constexpr std::string_view sigma_str{"σ"};
constexpr std::string_view sigma1_str{"σ₁"};
constexpr std::string_view sigma1safe_str{"σ1"};

constexpr std::string_view sigma2_str{"σ₂"};
constexpr std::string_view sigma2safe_str{"σ2"};

constexpr std::string_view sigma3_str{"σ₃"};
constexpr std::string_view sigma3safe_str{"σ3"};

constexpr std::string_view i1_str{"I₁"};
constexpr std::string_view i2_str{"I₂"};
constexpr std::string_view i3_str{"I₃"};
constexpr std::string_view j1_str{"J₁"};
constexpr std::string_view j2_str{"J₂"};
constexpr std::string_view j3_str{"J₃"};
constexpr std::string_view tau_str{"τ"};

}  // namespace inviwo::tensorutil
