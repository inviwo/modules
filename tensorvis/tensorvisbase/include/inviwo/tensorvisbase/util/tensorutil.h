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

namespace inviwo {
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
std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& os,
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
