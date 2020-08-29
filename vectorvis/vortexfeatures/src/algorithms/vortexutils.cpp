/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020 Inviwo Foundation
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

#include <inviwo/vortexfeatures/algorithms/vortexutils.h>

namespace inviwo {

namespace vortexutil {
bool matchedByCenter(const Vortex& outer, const Vortex& inner) {
    return glm::distance(outer.center, inner.center) < (outer.minRadius + inner.maxRadius) * 0.5;
    // std::max(outer.minRadius, inner.maxRadius);
}

VortexSet groupVorticesByCenter(VortexSet& vortices) {
    VortexSet outSet(vortices.getModelMatrix(), vortices.getWorldMatrix());
    std::sort(vortices.begin(), vortices.end(),
              [](const Vortex& a, const Vortex& b) { return a.avgRadius > b.avgRadius; });
    std::vector<char> grouped(vortices.size());
    for (size_t idx = 0; idx < vortices.size(); ++idx) {
        if (grouped[idx]) continue;
        grouped[idx] = 1;
        outSet.push_back(std::move(vortices[idx]));
        const Vortex* outerVort = &outSet.back();
        for (size_t compIdx = idx + 1; compIdx < vortices.size(); ++compIdx) {
            if (!grouped[compIdx] && matchedByCenter(*outerVort, vortices[compIdx])) {
                grouped[compIdx] = 2;
                outSet.push_back(std::move(vortices[compIdx]));
                outerVort = &outSet.back();
            }
        }
        outSet.startNewGroup();
    }
    outSet.mergeLastGroups();
    vortices.clear();
    return outSet;
}

int matchGroupByCenter(const Vortex& seedVort, const VortexSet& vortices) {
    if (vortices.size() == 0) return -1;
    double minDist = std::numeric_limits<double>::max();
    int minDistGroup = -1;
    for (size_t group = 0; group < vortices.numGroups(); ++group) {
        if (glm::distance((vortices.endGroup(group) - 1)->center, seedVort.center) >
            seedVort.maxRadius)
            continue;
        for (auto vort = vortices.beginGroup(group); vort != vortices.endGroup(group); ++vort) {
            double dist = glm::distance2(vort->center, seedVort.center);
            if (dist < minDist) {
                minDist = dist;
                minDistGroup = group;
            }
        }
    }
    return minDistGroup;
}

VortexSet removeSurroundingVortices(VortexSet& vortices) {
    VortexSet outSet(vortices.getModelMatrix(), vortices.getWorldMatrix());
    for (size_t group = 0; group < vortices.numGroups(); ++group) {
        bool remove = true;
        for (auto vort = vortices.beginGroup(group); vort != vortices.endGroup(group); ++vort) {
            if (remove) {
                remove = false;
                for (size_t compGroup = group + 1; compGroup < vortices.numGroups(); ++compGroup) {
                    if (vort->containsPoint(vortices.beginGroup(compGroup)->center)) {
                        remove = true;
                        break;
                    }
                }
            }
            if (!remove) {
                outSet.push_back(std::move(*vort));
            }
        }
        if (!remove) outSet.startNewGroup();
    }
    outSet.mergeLastGroups();
    return outSet;
}

}  // namespace vortexutil

}  // namespace inviwo
