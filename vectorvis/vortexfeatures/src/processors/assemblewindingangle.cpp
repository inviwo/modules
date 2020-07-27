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

#include <inviwo/vortexfeatures/processors/assemblewindingangle.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo AssembleWindingAngle::processorInfo_{
    "org.inviwo.AssembleWindingAngle",  // Class identifier
    "Assemble Winding Angle",           // Display name
    "Undefined",                        // Category
    CodeState::Experimental,            // Code state
    Tags::None,                         // Tags
};
const ProcessorInfo AssembleWindingAngle::getProcessorInfo() const { return processorInfo_; }

AssembleWindingAngle::AssembleWindingAngle()
    : vortexIn_("vortexIn")
    , vortexOut_("vortexOut")
    , timeSlice_("timeSlice", "Time Slice", 1, 1, 2, 1, InvalidationLevel::Valid)
    , heightSlice_("heightSlice", "Height Slice", 1, 1, 2, 1, InvalidationLevel::Valid)
    , triggerAccumulation_("triggerAccum", "Accumulate", [this]() { triggerAccumulate(); })
    , exportVortices_("exportVorts", "Export",
                      {{"noMatching", "No Matching", ExportVortices::NoMatching},
                       {"all", "Export All", ExportVortices::ExportAll},
                       {"first", "Export Only First", ExportVortices::ExportOnlyFirst}}) {

    addPort(vortexIn_);
    addPort(vortexOut_);
    addProperties(timeSlice_, heightSlice_, triggerAccumulation_, exportVortices_);
}

void AssembleWindingAngle::process() {
    if (!vortexIn_.hasData() || !accumulating_) return;
    if (!vortexIn_.isChanged()) return;
    // auto vortices = vortexIn_.getData();
    if (vortexIn_.getData() == lastVortexSet_) {
        return;
    }

    lastVortexSet_ = vortexIn_.getData();
    vortexList_[timeSlice_.get() - 1].push_back(vortexIn_.getData());

    if (heightSlice_.get() < heightSlice_.getMaxValue()) {
        heightSlice_.set(heightSlice_.get() + 1);
        return;
    } else if (timeSlice_.get() < timeSlice_.getMaxValue()) {
        timeSlice_.set(timeSlice_.get() + 1);
        heightSlice_.set(1);
        return;
    }
    // Do the brunt work.
    accumulating_ = false;
    auto assembledVortices = std::make_shared<VortexSet>(vortexList_[0][0]->getModelMatrix(),
                                                         vortexList_[0][0]->getWorldMatrix());
    if (exportVortices_.get() == ExportVortices::NoMatching) {
        for (size_t time = 0; time < vortexList_.size(); ++time) {
            // for (const auto& timeSlice : vortexList_) {
            const auto& timeSlice = vortexList_[time];
            for (size_t height = 0; height < timeSlice.size(); ++height) {
                // for (const auto& heightSlice : timeSlice) {
                const auto heightSlice = timeSlice[height];
                if (heightSlice->size() == 0) continue;
                size_t numGroupsBefore = assembledVortices->numGroups();
                assembledVortices->append(*heightSlice);
                for (auto vort = assembledVortices->beginGroup(numGroupsBefore);
                     vort != assembledVortices->end(); ++vort) {
                    vort->timeSlice = time;
                    vort->heightSlice = height;
                }
            }
        }
    } else {
        // Keeping track of vortices already added.
        std::vector<std::vector<std::vector<char>>> groupMatched(vortexList_.size());
        for (size_t time = 0; time < groupMatched.size(); ++time) {
            groupMatched[time].resize(vortexList_[time].size());
            for (size_t height = 0; height < vortexList_[time].size(); ++height) {
                groupMatched[time][height].resize(vortexList_[time][height]->numGroups());
            }
        }

        // Check whether a vortex overlaps with another from a group.
        auto findClosestOverlap = [](const Vortex& vort, const VortexSet& set) -> int {
            std::vector<size_t> closestIDs(set.numGroups());
            std::iota(closestIDs.begin(), closestIDs.end(), 0);
            std::sort(closestIDs.begin(), closestIDs.end(), [&](size_t a, size_t b) {
                double distA = glm::distance2(vort.center, set.beginGroup(a)->center);
                double distB = glm::distance2(vort.center, set.beginGroup(b)->center);
                return distA < distB;
            });
            for (size_t v : closestIDs) {
                if (vort.containsPoint(set.beginGroup(v)->center) ||
                    set.beginGroup(v)->containsPoint(vort.center)) {
                    return v;
                }
            }
            return -1;
        };

        auto pushBackGroup = [&](size_t time, size_t height, size_t group) {
            const auto vortexSet = vortexList_[time][height];
            for (auto vort = vortexSet->beginGroup(group); vort != vortexSet->endGroup(group);
                 ++vort) {
                assembledVortices->push_back(*vort);
                assembledVortices->back().timeSlice = time;
                assembledVortices->back().heightSlice = height;
                if (exportVortices_.get() == ExportVortices::ExportOnlyFirst) break;
            }
        };

        for (size_t time = 0; time < vortexList_.size(); ++time) {
            const auto& heightStack = vortexList_[time];

            // Take only eddies: vortices that appear on the surface.
            const auto vortexSet = heightStack[0];
            for (size_t group = 0; group < vortexSet->numGroups(); ++group) {
                if (groupMatched[time][0][group] != 0) continue;

                const Vortex* topLayerVortex = &*vortexSet->beginGroup(group);
                groupMatched[time][0][group] = 1;
                pushBackGroup(time, 0, group);

                // Go through all heights.
                for (size_t searchTime = time; searchTime < vortexList_.size(); ++searchTime) {

                    // New time slice? Match vortex on top slice.
                    if (searchTime != time) {
                        int nextInitVortex =
                            findClosestOverlap(*topLayerVortex, *vortexList_[searchTime][0]);
                        if (nextInitVortex < 0 ||
                            groupMatched[searchTime][0][nextInitVortex] != 0) {
                            // LogWarn("===> Ended after " << searchTime - time << " time steps.");
                            break;  // No unbroken time-line - vortex stops here.
                        }
                        pushBackGroup(searchTime, 0, nextInitVortex);
                        topLayerVortex = &*(vortexList_[searchTime][0])->beginGroup(nextInitVortex);
                        groupMatched[searchTime][0][nextInitVortex] = 1;
                    }

                    const Vortex* lastHeightVortex = topLayerVortex;
                    // Go downward through height.
                    for (size_t height = 1; height < heightStack.size(); ++height) {
                        int nextVortex =
                            findClosestOverlap(*lastHeightVortex, *vortexList_[searchTime][height]);
                        if (nextVortex < 0 || groupMatched[searchTime][height][nextVortex] != 0) {
                            // LogWarn("==> " << height << " steps deep.");
                            break;  // No unbroken height-line - vortex stops here.
                        }
                        groupMatched[searchTime][height][nextVortex] = 1;
                        pushBackGroup(searchTime, height, nextVortex);
                    }
                }
                assembledVortices->startNewGroup();
            }
        }

        // Fill in the remaining vortices that were not connected as a single big group.
        for (size_t time = 0; time < vortexList_.size(); ++time) {
            for (size_t height = 0; height < groupMatched[time].size(); ++height) {
                for (size_t group = 0; group < groupMatched[time][height].size(); ++group) {
                    if (groupMatched[time][height][group] == 0) {
                        pushBackGroup(time, height, group);
                    }
                }
            }
        }
        LogWarn("----> " << assembledVortices->sizeGroup(assembledVortices->numGroups() - 1)
                         << " vortices left over.");
    }
    assembledVortices->updateRanges(assembledVortices->size());
    vortexOut_.setData(assembledVortices);
}

void AssembleWindingAngle::triggerAccumulate() {
    vortexList_.resize(timeSlice_.getMaxValue());
    for (size_t idx = 0; idx < vortexList_.size(); ++idx) {
        vortexList_[idx].clear();
    }
    accumulating_ = true;

    if (timeSlice_.get() == 1 && heightSlice_.get() == 1) {
        lastVortexSet_ = nullptr;  // vortexIn_.getData();
    }
    timeSlice_.set(1);
    heightSlice_.set(1);
}

}  // namespace inviwo
