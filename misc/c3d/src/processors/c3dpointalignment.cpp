/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2026 Inviwo Foundation
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

#include <inviwo/c3d/processors/c3dpointalignment.h>

#include <Eigen/Dense>

#include <ezc3d/ezc3d.h>
#include <ezc3d/Header.h>
#include <ezc3d/Parameters.h>
#include <ezc3d/Data.h>

namespace inviwo {

namespace {

glm::dmat4 toGLM(const Eigen::Matrix3d& R, const Eigen::Vector3d& t) {
    // GLM is column-major, so glm::dmat4[col][row]
    glm::dmat4 M(1.0);  // identity

    // Rotation (upper-left 3x3)
    for (int col = 0; col < 3; col++) {
        for (int row = 0; row < 3; row++) {
            M[col][row] = R(row, col);
        }
    }

    // Translation (4th column)
    M[3][0] = t(0);
    M[3][1] = t(1);
    M[3][2] = t(2);

    return M;
}

}  // namespace

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo C3DPointAlignment::processorInfo_{
    "org.inviwo.C3DPointAlignment",  // Class identifier
    "C3D Point Alignment",           // Display name
    "Motion Tracking",               // Category
    CodeState::Experimental,         // Code state
    Tags::CPU | Tag{"C3D"},          // Tags
    R"(<Explanation of how to use the processor.>)"_unindentHelp,
};

const ProcessorInfo& C3DPointAlignment::getProcessorInfo() const { return processorInfo_; }

C3DPointAlignment::C3DPointAlignment()
    : Processor{}
    , inport_{"inport"}
    , positionsInport_{"positionsInport"}
    , namesInport_{"namesInport"}
    , transform_{"transform"}
    , frameIdx_{"frameIdx", "Frame Index", util::ordinalCount(0uz)} {

    addPorts(inport_, positionsInport_, namesInport_, transform_);

    addProperties(frameIdx_);
}

void C3DPointAlignment::process() {
    const auto& src = *inport_.getData();
    const auto& header = src.header();

    auto refPoints = positionsInport_.getData();
    auto names = namesInport_.getData();

    if (names->size() != refPoints->size()) {
        throw Exception("Number of names does not match number of reference points");
    }

    Eigen::Matrix<double, Eigen::Dynamic, 3> ref{static_cast<Eigen::Index>(refPoints->size()), 3};
    for (auto&& [i, p] : std::views::zip(std::views::iota(0uz), *refPoints)) {
        ref(i, 0) = p.x;
        ref(i, 1) = p.y;
        ref(i, 2) = p.z;
    }

    std::vector<size_t> refIndices;
    for (const auto& name : *names) {
        refIndices.push_back(src.pointIdx(name));
    }

    const size_t nbFrames = header.nbFrames();
    if (frameIdx_.get() >= nbFrames) {
        throw Exception(SourceContext{}, "Frame index {} is out of range (0-{})", frameIdx_.get(),
                        nbFrames - 1);
    }

    const auto& srcFrame = src.data().frame(frameIdx_.get());

    Eigen::Matrix<double, Eigen::Dynamic, 3> obs{static_cast<Eigen::Index>(refPoints->size()), 3};
    for (auto&& [i, pi] : std::views::zip(std::views::iota(0uz), refIndices)) {
        const auto& p = srcFrame.points().point(pi);
        obs(i, 0) = p.x();
        obs(i, 1) = p.y();
        obs(i, 2) = p.z();
    }

    // 1. Compute centroids
    const Eigen::Vector3d centroid_ref = ref.colwise().mean();
    const Eigen::Vector3d centroid_obs = obs.colwise().mean();

    // 2. Center the point sets
    const auto Xc = ref.rowwise() - centroid_ref.transpose();
    const auto Yc = obs.rowwise() - centroid_obs.transpose();

    // 3. Cross-covariance matrix
    const auto H = Yc.transpose() * Xc;

    // 4. SVD
    Eigen::JacobiSVD<Eigen::MatrixXd> svd(H, Eigen::ComputeFullU | Eigen::ComputeFullV);
    const Eigen::Matrix3d U = svd.matrixU();
    Eigen::Matrix3d V = svd.matrixV();

    // 5. Optimal rotation
    Eigen::Matrix<double, 3, 3> R = U * V.transpose();

    // Reflection correction (ensure proper rotation)
    if (R.determinant() < 0) {
        V.col(2) *= -1.0;
        R = U * V.transpose();
    }

    // 6. Optimal translation
    const auto t = centroid_obs - R * centroid_ref;

    const auto trafo = toGLM(R, t);

    /*
    const auto cref = glm::dvec3{centroid_ref(0), centroid_ref(1), centroid_ref(2)};
    const auto cobs = glm::dvec3{centroid_obs(0), centroid_obs(1), centroid_obs(2)};
    const auto ctransformed = dvec3{trafo * glm::dvec4{cref, 1.0}};
    glm::dvec3 cerror = cobs - ctransformed;
    log::info("Center: error = ({:.3f}, {:.3f}, {:.3f})", cerror.x, cerror.y, cerror.z);

    for (auto&& [i, r, pi] : std::views::zip(std::views::iota(0uz), *refPoints, refIndices)) {
        const auto& p = srcFrame.points().point(pi);
        const auto dest = glm::dvec3{p.x(), p.y(), p.z()};
        const auto transformed = dvec3{trafo * glm::dvec4{r, 1.0}};
        glm::dvec3 error = dest - transformed;
        log::info("Point {}: error = ({:.3f}, {:.3f}, {:.3f})", names->at(i), error.x, error.y,
                  error.z);
    }
    */

    transform_.setData(std::make_shared<mat4>(trafo));
}

}  // namespace inviwo
