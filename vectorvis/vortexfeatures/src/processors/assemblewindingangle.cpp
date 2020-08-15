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
#include <inviwo/vortexfeatures/algorithms/vortexutils.h>

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
                       {"first", "Export Only First", ExportVortices::ExportOnlyFirst}})
    , scoreProperties_("scoreProp", "Override Score")
    , matchProperties_("matchProperties", "Match Weights") {

    addPort(vortexIn_);
    addPort(vortexOut_);
    addProperties(timeSlice_, heightSlice_, triggerAccumulation_, exportVortices_, scoreProperties_,
                  matchProperties_);

    auto triggerProcess = [&]() { invalidate(InvalidationLevel::InvalidOutput); };

    vortexIn_.onChange(triggerProcess);
    exportVortices_.onChange(triggerProcess);
    matchProperties_.onChange(triggerProcess);
    matchProperties_.scoreWeight_.onChange(triggerProcess);
    matchProperties_.sizeDiffWeight_.onChange(triggerProcess);
    scoreProperties_.getBoolProperty()->onChange(triggerProcess);
    scoreProperties_.sizeWeight_.onChange(triggerProcess);
    scoreProperties_.roundnessWeight_.onChange(triggerProcess);
}

void AssembleWindingAngle::process() {
    if (!vortexIn_.hasData()) return;

    if (accumulating_) {
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
    }
    assemble();
}

void AssembleWindingAngle::assemble() {
    if (vortexList_.size() < 1 || vortexList_[0].size() < 1 || accumulating_) return;
    if (scoreProperties_.getBoolProperty()->get()) {
        for (size_t time = 0; time < vortexList_.size(); ++time) {
            for (size_t height = 0; height < vortexList_[time].size(); ++height) {
                scoreProperties_.computeScores(*vortexList_[time][height]);
            }
        }
        LogWarn("Score range[0][0]: " << vortexList_[0][0]->getScoreRange());
    }
    outVortices_ = std::make_shared<VortexSet>(vortexList_[0][0]->getModelMatrix(),
                                               vortexList_[0][0]->getWorldMatrix());
    if (exportVortices_.get() == ExportVortices::NoMatching) {
        for (size_t time = 0; time < vortexList_.size(); ++time) {
            const auto& timeSlice = vortexList_[time];
            for (size_t height = 0; height < timeSlice.size(); ++height) {
                const auto heightSlice = timeSlice[height];
                if (heightSlice->size() == 0) continue;
                size_t numGroupsBefore = outVortices_->numGroups();
                outVortices_->append(*heightSlice);
                for (auto vort = outVortices_->beginGroup(numGroupsBefore);
                     vort != outVortices_->end(); ++vort) {
                    vort->timeSlice = time;
                    vort->heightSlice = height;
                }
                outVortices_->updateRanges(outVortices_->size());
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
        auto findClosestOverlap = [&](const Vortex& vort,
                                      const VortexSet& set) -> std::tuple<const Vortex*, size_t> {
            int group = vortexutil::matchGroupByCenter(vort, set);
            if (group < 0) return {nullptr, -1};
            return {&matchProperties_.bestMatchInGroup(vort, set, group), group};
        };

        auto pushBackGroup = [&](size_t time, size_t height, size_t group,
                                 const Vortex* bestMatch) {
            groupMatched[time][height][group] = 1;
            const auto vortexSet = vortexList_[time][height];
            if (bestMatch) {
                outVortices_->push_back(*bestMatch);
                outVortices_->back().timeSlice = time;
                outVortices_->back().heightSlice = height;
            }
            if (exportVortices_.get() == ExportVortices::ExportAll) {
                for (auto vort = vortexSet->beginGroup(group); vort != vortexSet->endGroup(group);
                     ++vort) {
                    if (&*vort == bestMatch) continue;
                    outVortices_->push_back(*vort);
                    outVortices_->back().timeSlice = time;
                    outVortices_->back().heightSlice = height;
                }
            }
        };

        size_t seedHeight = std::min(vortexList_[0].size() - 1, size_t(3));

        for (size_t time = 0; time < vortexList_.size(); ++time) {
            const auto& heightStack = vortexList_[time];

            // Take only eddies: vortices that appear on the surface.
            const auto vortexSet = heightStack[seedHeight];
            for (size_t group = 0; group < vortexSet->numGroups(); ++group) {
                if (groupMatched[time][seedHeight][group] != 0) continue;

                const Vortex* topLayerVortex = &*std::max_element(
                    vortexSet->beginGroup(group), vortexSet->endGroup(group),
                    [](const Vortex& a, const Vortex& b) { return a.score < b.score; });

                pushBackGroup(time, seedHeight, group, topLayerVortex);

                // Go through all heights.
                for (size_t searchTime = time; searchTime < vortexList_.size(); ++searchTime) {

                    // New time slice? Match vortex on top slice.
                    if (searchTime != time) {

                        auto nextVortTuple = findClosestOverlap(
                            *topLayerVortex, *vortexList_[searchTime][seedHeight]);
                        const Vortex* nextVort = std::get<0>(nextVortTuple);
                        if (!nextVort) {
                            break;  // No unbroken time-line - vortex stops here.
                        }
                        pushBackGroup(searchTime, seedHeight, std::get<1>(nextVortTuple), nextVort);
                        topLayerVortex = nextVort;
                    }

                    const Vortex* lastHeightVortex = topLayerVortex;
                    // Go downward through height.
                    for (size_t height = seedHeight + 1; height < heightStack.size(); ++height) {
                        auto nextVortTuple =
                            findClosestOverlap(*lastHeightVortex, *vortexList_[searchTime][height]);
                        const Vortex* nextVort = std::get<0>(nextVortTuple);
                        if (!nextVort) {
                            break;  // No unbroken time-line - vortex stops here.
                        }
                        pushBackGroup(searchTime, height, std::get<1>(nextVortTuple), nextVort);
                        lastHeightVortex = nextVort;
                    }
                    lastHeightVortex = topLayerVortex;
                    for (int height = seedHeight - 1; height >= 0; --height) {
                        auto nextVortTuple =
                            findClosestOverlap(*lastHeightVortex, *vortexList_[searchTime][height]);
                        const Vortex* nextVort = std::get<0>(nextVortTuple);
                        if (!nextVort) {
                            break;  // No unbroken time-line - vortex stops here.
                        }
                        pushBackGroup(searchTime, height, std::get<1>(nextVortTuple), nextVort);
                        lastHeightVortex = nextVort;
                    }
                }
                outVortices_->startNewGroup();
            }
        }

        // Fill in the remaining vortices that were not connected as a single big group.
        for (size_t time = 0; time < vortexList_.size(); ++time) {
            for (size_t height = 0; height < groupMatched[time].size(); ++height) {
                for (size_t group = 0; group < groupMatched[time][height].size(); ++group) {
                    if (groupMatched[time][height][group] == 0) {
                        pushBackGroup(time, height, group, nullptr);
                    }
                }
            }
        }
    }
    outVortices_->updateRanges(outVortices_->size());
    vortexOut_.setData(outVortices_);
}

void AssembleWindingAngle::triggerAccumulate() {
    vortexList_.resize(timeSlice_.getMaxValue());
    for (size_t idx = 0; idx < vortexList_.size(); ++idx) {
        vortexList_[idx].clear();
    }
    accumulating_ = true;

    if (timeSlice_.get() == 1 && heightSlice_.get() == 1) {
        lastVortexSet_ = nullptr;
    } else {
        lastVortexSet_ = vortexIn_.getData();
    }
    timeSlice_.set(1);
    heightSlice_.set(1);
}

AssembleWindingAngle::MatchProperties::MatchProperties(const std::string& identifier,
                                                       const std::string& displayName)
    : CompositeProperty(identifier, displayName)
    , scoreWeight_("scoreWeight", "Score Weight", 0.5, 0, 1, 0.01)
    , sizeDiffWeight_("sizeDiff", "Size Diff Weight", 0.5, 0, 1, 0.01) {
    addProperties(scoreWeight_, sizeDiffWeight_);
}

const Vortex& AssembleWindingAngle::MatchProperties::bestMatchInGroup(const Vortex& seedVort,
                                                                      const VortexSet& vortices,
                                                                      size_t group) {
    dvec2 scoreRange = vortices.getScoreRange();
    // The smaller, the better.
    auto scoreVort = [&](const Vortex& vort) -> double {
        return (scoreRange.y - vort.score) / (scoreRange.y - scoreRange.x) * *scoreWeight_ +
               (std::abs(seedVort.minRadius - vort.minRadius) / seedVort.minRadius +
                std::abs(seedVort.avgRadius - vort.avgRadius) / seedVort.avgRadius +
                std::abs(seedVort.maxRadius - vort.maxRadius) / seedVort.maxRadius) *
                   *sizeDiffWeight_;
    };
    return *std::min_element(
        vortices.beginGroup(group), vortices.endGroup(group),
        [&](const Vortex& a, const Vortex& b) { return scoreVort(a) < scoreVort(b); });
}

}  // namespace inviwo
