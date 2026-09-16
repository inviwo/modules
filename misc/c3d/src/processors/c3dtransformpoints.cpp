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

#include <inviwo/c3d/processors/c3dtransformpoints.h>

#include <inviwo/core/util/glm.h>

#include <ezc3d/ezc3d.h>
#include <ezc3d/Header.h>
#include <ezc3d/Parameters.h>
#include <ezc3d/Data.h>

#include <ranges>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo C3DTransformPoints::processorInfo_{
    "org.inviwo.C3DTransformPoints",  // Class identifier
    "C3DTransform Points",            // Display name
    "Data Operation",                 // Category
    CodeState::Experimental,          // Code state
    Tags::CPU | Tag{"C3D"},           // Tags
    R"(Apply a transformation to the points in a c3d dataset)"_unindentHelp,
};

const ProcessorInfo& C3DTransformPoints::getProcessorInfo() const { return processorInfo_; }

C3DTransformPoints::C3DTransformPoints()
    : PoolProcessor{}
    , inport_{"inport", ""_help}
    , outport_{"outport", ""_help}
    , refs_{{{"ref1", "Reference 1", "phantom:skull1"},
             {"ref2", "Reference 2", "phantom:skull3"},
             {"ref3", "Reference 3", "phantom:skull2"},
             {"ref4", "Reference 4", "phantom:skull4"}}}
    , refGroup_{"refGroup", "Reference Markers"}
    , transforms_("transformations", "Transformation Stack") {

    addPorts(inport_, outport_);
    refGroup_.addProperties(refs_[0], refs_[1], refs_[2], refs_[3]);
    addProperties(refGroup_, transforms_);
}

namespace {
/*
 * Estimate the basis of the coordinate system defined by the 4 markers.
 * X axis is defined by the vector from marker A to marker B
 * Y axis from c and c projected on X,
 * Z axis from d and d projected on X
 */
dmat4 basisEstimation(dvec3 a, dvec3 b, dvec3 c, dvec3 d) {
    const auto x = normalize(a - b);
    const auto y = normalize(c - (a + dot(x, c - a) * x));
    const auto z = normalize(d - (a + dot(x, d - a) * x));

    return {dvec4{x, 0.0}, dvec4{y, 0.0}, dvec4{z, 0.0}, dvec4{a, 1.0}};
}

dvec3 toGLM(const ezc3d::DataNS::Points3dNS::Point& p) { return dvec3{p.x(), p.y(), p.z()}; }
}  // namespace

void C3DTransformPoints::process() {

    const auto calc = [src = inport_.getData(), refGroup = refGroup_.isChecked(),
                       transform = transforms_.getMatrix(),
                       refs = refs_ | std::views::transform([](auto& p) { return p.get(); }) |
                              std::ranges::to<std::vector>()](
                          pool::Stop stop, pool::Progress progress) -> std::shared_ptr<C3D> {
        if (stop) return {};
        progress(0.0);

        std::array<size_t, refPoints> refIndices{};
        if (refGroup) {
            for (auto&& [ref, ind] : std::views::zip(refs, refIndices)) {
                ind = src->pointIdx(ref);
            }
        }

        auto dst = std::shared_ptr<C3D>(src->clone());

        const auto& header = dst->header();
        const size_t nbFrames = header.nbFrames();
        const size_t nbPoints = header.nb3dPoints();

        for (size_t f = 0; f < nbFrames; ++f) {
            if (f % 100 == 0) progress(f, nbFrames);
            auto& frame = dst->data().frame(f);
            auto& points = frame.points();
            const auto basis = [&]() {
                if (refGroup) {
                    const auto a = toGLM(points.point(refIndices[0]));
                    const auto b = toGLM(points.point(refIndices[1]));
                    const auto c = toGLM(points.point(refIndices[2]));
                    const auto d = toGLM(points.point(refIndices[3]));
                    return glm::inverse(basisEstimation(a, b, c, d));
                } else {
                    return dmat4{1.0};
                }
            }();

            for (size_t p = 0; p < nbPoints; ++p) {
                auto& point = points.point(p);
                const auto newPos = dvec3{dmat4{transform} * basis * dvec4{toGLM(point), 1.0}};
                point.set(newPos.x, newPos.y, newPos.z);
            }
        }
        progress(1.0);
        return dst;
    };

    dispatchOne(calc, [this](std::shared_ptr<C3D> data) {
        outport_.setData(std::move(data));
        newResults();
    });
}

}  // namespace inviwo
