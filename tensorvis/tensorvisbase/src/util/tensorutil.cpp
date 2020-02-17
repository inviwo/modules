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

#include <inviwo/tensorvisbase/util/tensorutil.h>

#include <inviwo/core/util/exception.h>

namespace inviwo {

namespace tensorutil {
vec4 tensor2DToDvec4(const dmat2 &tensor) {
    return vec4(tensor[0][0], tensor[1][0], tensor[1][1], 1.0);
}
/*
 * Calculates the stress deviator part of a tensor, see
 * https://en.wikipedia.org/wiki/Cauchy_stress_tensor#Stress_deviator_tensor
 */
dmat3 stressDeviatorTensor(const dmat3 &tensor) {
    return tensor - (1. / 3.) * util::trace(tensor) * dmat3(1.);
}

/*
 * Calculates the mean normal stress part of a tensor, see
 * https://en.wikipedia.org/wiki/Cauchy_stress_tensor#Stress_deviator_tensor
 */
dmat3 meanNormalStressTensor(const dmat3 &tensor) { return tensor - stressDeviatorTensor(tensor); }

/*
 * Calculates the mean hydrostatic stress part of a tensor, see
 * https://en.wikipedia.org/wiki/Cauchy_stress_tensor#Stress_deviator_tensor
 */
dmat3 meanHydrostaticStressTensor(const dmat3 &tensor) { return meanNormalStressTensor(tensor); }

/*
 * Calculates the volumetric stress part of a tensor, see
 * https://en.wikipedia.org/wiki/Cauchy_stress_tensor#Stress_deviator_tensor
 */
dmat3 volumetricStressTensor(const dmat3 &tensor) { return meanNormalStressTensor(tensor); }

/*
Calculates and return 3 pairs of eigenvector and eigenvalue. The array is sorted such that
lambda1>lambda2>lambda3
*/
std::array<std::pair<double, dvec3>, 3> calculateEigenValuesAndEigenVectors(const dmat3 &tensor) {
    if (tensor == dmat3(0.0)) {
        return std::array<std::pair<double, dvec3>, 3>{std::pair<double, dvec3>{0, dvec3(0)},
                                                       std::pair<double, dvec3>{0, dvec3(0)},
                                                       std::pair<double, dvec3>{0, dvec3(0)}};
    }

    Eigen::EigenSolver<Eigen::Matrix<double, 3, 3>> solver(util::glm2eigen(tensor));

    auto lambda1 = solver.eigenvalues().col(0)[0].real();
    auto lambda2 = solver.eigenvalues().col(0)[1].real();
    auto lambda3 = solver.eigenvalues().col(0)[2].real();

    auto ev1 = dvec3(solver.eigenvectors().col(0).real()[0], solver.eigenvectors().col(0).real()[1],
                     solver.eigenvectors().col(0).real()[2]);
    auto ev2 = dvec3(solver.eigenvectors().col(1).real()[0], solver.eigenvectors().col(1).real()[1],
                     solver.eigenvectors().col(1).real()[2]);
    auto ev3 = dvec3(solver.eigenvectors().col(2).real()[0], solver.eigenvectors().col(2).real()[1],
                     solver.eigenvectors().col(2).real()[2]);

    std::array<std::pair<double, dvec3>, 3> sortable;
    sortable[0] = {lambda1, ev1};
    sortable[1] = {lambda2, ev2};
    sortable[2] = {lambda3, ev3};

    std::sort(sortable.begin(), sortable.end(),
              [](const std::pair<double, dvec3> &pairA, const std::pair<double, dvec3> &pairB) {
                  return pairA.first > pairB.first;
              });

    return sortable;
}

/*
Calculates and return the eigenvales of the tensor in the form of lambda1>lambda2>lambda3
*/
std::array<double, 3> calculateEigenValues(const dmat3 &tensor) {
    if (tensor == dmat3(0.0)) {
        return std::array<double, 3>{0, 0, 0};
    }

    Eigen::EigenSolver<Eigen::Matrix<double, 3, 3>> solver(util::glm2eigen(tensor));

    auto lambda1 = solver.eigenvalues().col(0)[0].real();
    auto lambda2 = solver.eigenvalues().col(0)[1].real();
    auto lambda3 = solver.eigenvalues().col(0)[2].real();

    auto sortable = std::array<double, 3>{lambda1, lambda2, lambda3};

    std::sort(sortable.begin(), sortable.end(),
              [](const double A, const double B) { return A > B; });

    return sortable;
}

dmat3 IVW_MODULE_TENSORVISBASE_API calculateEigenSystem(const dmat3 &tensor) {
    if (tensor == dmat3(0.0)) {
        return dmat3(0.0);
    }

    Eigen::EigenSolver<Eigen::Matrix<double, 3, 3>> solver(util::glm2eigen(tensor));

    auto lambda1 = solver.eigenvalues().col(0)[0].real();
    auto lambda2 = solver.eigenvalues().col(0)[1].real();
    auto lambda3 = solver.eigenvalues().col(0)[2].real();

    auto ev1 = dvec3(solver.eigenvectors().col(0).real()[0], solver.eigenvectors().col(0).real()[1],
                     solver.eigenvectors().col(0).real()[2]);
    auto ev2 = dvec3(solver.eigenvectors().col(1).real()[0], solver.eigenvectors().col(1).real()[1],
                     solver.eigenvectors().col(1).real()[2]);
    auto ev3 = dvec3(solver.eigenvectors().col(2).real()[0], solver.eigenvectors().col(2).real()[1],
                     solver.eigenvectors().col(2).real()[2]);

    std::array<std::pair<double, dvec3>, 3> sortable;
    sortable[0] = {lambda1, ev1};
    sortable[1] = {lambda2, ev2};
    sortable[2] = {lambda3, ev3};

    std::sort(sortable.begin(), sortable.end(),
              [](const std::pair<double, dvec3> &pairA, const std::pair<double, dvec3> &pairB) {
                  return pairA.first > pairB.first;
              });

    return dmat3(sortable[0].second, sortable[1].second, sortable[2].second);
}

dmat2 getProjectedTensor(const dmat3 tensor, const CartesianCoordinateAxis axis) {
    dmat2 newTensor;

    switch (axis) {
        case CartesianCoordinateAxis::X:
            newTensor[0][0] = tensor[1][1];
            newTensor[1][0] = tensor[2][1];
            newTensor[0][1] = tensor[1][2];
            newTensor[1][1] = tensor[2][2];
            break;
        case CartesianCoordinateAxis::Y:
            newTensor[0][0] = tensor[0][0];
            newTensor[1][0] = tensor[2][0];
            newTensor[0][1] = tensor[0][2];
            newTensor[1][1] = tensor[2][2];
            break;
        case CartesianCoordinateAxis::Z:
            newTensor[0][0] = tensor[0][0];
            newTensor[1][0] = tensor[1][0];
            newTensor[0][1] = tensor[0][1];
            newTensor[1][1] = tensor[1][1];
            break;
    }
    return newTensor;
}

std::pair<dmat2, dmat2> decompose(const dmat2 &tensor, Decomposition decomposition) {
    dmat2 T1(0.);
    dmat2 T2(0.);

    switch (decomposition) {
        case Decomposition::Isotropic_Anisotropic:
            T1 = (1. / 3.) * util::trace(tensor) * dmat2(1.);
            T2 = tensor - T1;
            break;
        case Decomposition::Shape_Orientation:

            break;
        case Decomposition::Stretch_Rotation:
            break;
        case Decomposition::Symmetric_Antisymmetric:
            for (size_t i = 0; i < 2; i++) {
                for (size_t j = 0; j < 2; j++) {
                    T1[i][j] = 0.5 * (tensor[i][j] + tensor[j][i]);
                    T2[i][j] = 0.5 * (tensor[i][j] - tensor[j][i]);
                }
            }
            break;
        default:
            break;
    }

    return std::make_pair(T1, T2);
}

std::pair<dmat3, dmat3> decompose(const dmat3 &tensor, Decomposition decomposition) {
    dmat3 T1(0.);
    dmat3 T2(0.);

    switch (decomposition) {
        case Decomposition::Isotropic_Anisotropic:
            T1 = (1. / 3.) * util::trace(tensor) * dmat3(1.);
            T2 = tensor - T1;
            break;
        case Decomposition::Shape_Orientation:
            break;
        case Decomposition::Stretch_Rotation:
            break;
        case Decomposition::Symmetric_Antisymmetric:
            for (size_t i = 0; i < 3; i++) {
                for (size_t j = 0; j < 3; j++) {
                    T1[i][j] = 0.5 * (tensor[i][j] + tensor[j][i]);
                    T2[i][j] = 0.5 * (tensor[i][j] - tensor[j][i]);
                }
            }
            break;
        default:
            break;
    }

    return std::make_pair(T1, T2);
}

dmat3 reconstruct(const std::array<double, 3> &eigenvalues,
                  const std::array<dvec3, 3> &eigenvectors) {
    auto lambda = dmat3();
    lambda[0][0] = eigenvalues[0];
    lambda[1][1] = eigenvalues[1];
    lambda[2][2] = eigenvalues[2];

    auto S = dmat3(eigenvectors[0], eigenvectors[1], eigenvectors[2]);
    auto S_inv = glm::inverse(S);

    return S * lambda * S_inv;
}

}  // namespace tensorutil
}  // namespace inviwo