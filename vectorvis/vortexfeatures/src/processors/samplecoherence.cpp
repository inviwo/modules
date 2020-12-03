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

#include <inviwo/vortexfeatures/processors/samplecoherence.h>
#include <inviwo/core/util/spatialsampler.h>
#include <inviwo/core/util/volumeramutils.h>
#include <modules/base/algorithm/dataminmax.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo SampleCoherence::processorInfo_{
    "org.inviwo.SampleCoherence",  // Class identifier
    "Sample Coherence",            // Display name
    "Undefined",                   // Category
    CodeState::Experimental,       // Code state
    Tags::None,                    // Tags
};
const ProcessorInfo SampleCoherence::getProcessorInfo() const { return processorInfo_; }

SampleCoherence::SampleCoherence()
    : PoolProcessor(pool::Option::DelayDispatch)
    , maskVolumeIn_("mask")
    , velocityIn_("velocity")
    , coherenceOut_("coherencePerVortex")
    , linesOut_("lines")

    , integratorSettings_("integratorSettings", "Integrator Settings")
    , samplingStep_("samplingStep", "Sampling Step", 2, 1, 20)
    , lineOutputHeightStep_("lineOutputHeightStep", "Height Step Line Output", 0,
                            {0, ConstraintBehavior::Immutable}, {0, ConstraintBehavior::Mutable})
    , singleGroup_("singleGroup", "Single Group?", false)
    , group_("selectedGroup", "Group", 0, 0, 0)
    , averageCoherence_("averageCoherence", "Average Coherence", 0, {0, ConstraintBehavior::Ignore},
                        {0, ConstraintBehavior::Ignore}, 0, InvalidationLevel::Valid,
                        PropertySemantics::Text)
    , weightedCoherence_("weightedCoherence", "Volume-Weighted Coherence", 0,
                         {0, ConstraintBehavior::Ignore}, {0, ConstraintBehavior::Ignore}, 0,
                         InvalidationLevel::Valid, PropertySemantics::Text) {

    addPort(maskVolumeIn_);
    addPort(velocityIn_);
    addPort(coherenceOut_);
    addPort(linesOut_);
    addProperties(integratorSettings_, samplingStep_, lineOutputHeightStep_, singleGroup_, group_,
                  averageCoherence_, weightedCoherence_);
    group_.visibilityDependsOn(singleGroup_, [](auto p) { return p.get(); });

    integratorSettings_.stepDirection_.set(IntegralLineProperties::Direction::FWD);
    integratorSettings_.stepDirection_.setVisible(false);
    integratorSettings_.seedPointsSpace_.set(CoordinateSpace::Data);
    integratorSettings_.seedPointsSpace_.setVisible(false);
    integratorSettings_.stepSize_.set(1.0 / integratorSettings_.numberOfSteps_.get());
    integratorSettings_.stepSize_.setVisible(false);
    integratorSettings_.onChange([this]() {
        if (!velocityIn_.hasData()) return;
        integratorSettings_.stepSize_.set(
            1.0 / ((velocityIn_.getData()->size() - 1) * integratorSettings_.numberOfSteps_.get()));
    });
}

bool detail::ComputeCoherenceHelper::isInside(const VolumeRAM& mask, dvec3 pos, double maskOrigin) {
    // if (!withinBoundsDataSpace(pos)) {
    //     return Vector<DataDims, T>{0};
    // }
    const dvec3 samplePos = pos * dvec3(mask.getDimensions() - size3_t(1));
    const size3_t indexPos = size3_t(samplePos);
    const dvec3 interpolants = samplePos - dvec3(indexPos);

    double samples[8];
    samples[0] = mask.getAsDouble(indexPos);
    samples[1] = mask.getAsDouble(indexPos + size3_t(1, 0, 0));
    samples[2] = mask.getAsDouble(indexPos + size3_t(0, 1, 0));
    samples[3] = mask.getAsDouble(indexPos + size3_t(1, 1, 0));

    // bool onSide = interpolants.z < std::numeric_limits<double>::epsilon();

    // if (!onSide) {
    samples[4] = mask.getAsDouble(indexPos + size3_t(0, 0, 1));
    samples[5] = mask.getAsDouble(indexPos + size3_t(1, 0, 1));
    samples[6] = mask.getAsDouble(indexPos + size3_t(0, 1, 1));
    samples[7] = mask.getAsDouble(indexPos + size3_t(1, 1, 1));
    // }

    for (double& val : samples) {
        val = val < maskOrigin ? 0.0 : 1.0;
    }

    double interpolated =
        // onSide ? Interpolation<Vector<DataDims, T>, P>::bilinear(samples, dvec2(interpolants)) :
        Interpolation<double>::trilinear(samples, interpolants);
    return interpolated >= 0.9;
}

void SampleCoherence::process() {
    if (!maskVolumeIn_.hasData() || !velocityIn_.hasData() || maskVolumeIn_.getData()->size() < 2 ||
        velocityIn_.getData()->size() < 2  //||
                                           // maskVolumeIn_.getData()->at(0)->getDimensions() !=
                                           //     velocityIn_.getData()->at(0)->getDimensions()

    ) {
        // LogWarn("==> Input not good enough for Coherence?!?!");
        return;
    }

    // timeLineSets_.clear();
    auto sampler = std::make_shared<VolumeSequenceSampler>(velocityIn_.getData(), false);
    integratorSettings_.stepDirection_.set(IntegralLineProperties::Direction::FWD);
    PathLine3DTracer fwd(sampler, integratorSettings_);
    integratorSettings_.stepDirection_.set(IntegralLineProperties::Direction::BWD);
    PathLine3DTracer bwd(sampler, integratorSettings_);

    // LogWarn("Calculating Coherence");

    detail::ComputeCoherenceHelper coherenceHelper(
        velocityIn_.getData(), maskVolumeIn_.getData(), sampler, std::move(fwd), std::move(bwd),
        singleGroup_.get(), group_.get(), samplingStep_.get());

    // Dispatch the calculation to the thread pool. The second argument is a callback
    // that will get evaluated on the main thread after the calculation is done. The callback
    // should take the result of the calculation as argument.
    // dispatchOne(coherenceHelper,
    //             [this](detail::CoherenceInfo* result) { postProcessCoherence(result); });

    postProcessCoherence(coherenceHelper());
}

detail::CoherenceInfo* detail::ComputeCoherenceHelper::operator()() {

    size4_t volSize = {velocities_->at(0)->getDimensions(), velocities_->size()};
    vec4 invDim = vec4(1.0f) / vec4(volSize);

    CoherenceInfo* result = new CoherenceInfo();

    // std::mutex lineWriteMutex;

    // size_t sampledPortion = samplingStep_.get() * samplingStep_.get();
    size_t numTimes = velocities_->size();
    std::vector<size_t> timeNumGroups(numTimes);
    for (size_t time = 0; time < numTimes; ++time) {
        timeNumGroups[time] =
            static_cast<size_t>(util::volumeMinMax(masks_->at(time).get()).second.x);
        // std::cout << time << " num Groups: " << timeNumGroups[time] << std::endl;
    }

    size_t numGroups =
        *std::max_element(timeNumGroups.begin(), timeNumGroups.end()
                          // masks->begin(), masks->end(), [](auto a, auto b) {
                          // return util::volumeMinMax(*a).second < util::volumeMinMax(*b).second;}
                          ) -
        1;

    // enum ShouldIntegrate { NotYet, StartNext, Yes };
    // std::vector<ShouldIntegrate> timeStatus(numGroups, ShouldIntegrate::NotYet);
    std::vector<size_t> timeNumSteps(numGroups, 0);

    // std::vector<size_t> timeNumVoxels(numGroups, 0);
    // std::vector<double> timeCoherenceFwd(numGroups, 0), timeCoherenceBwd(numGroups, 0);
    result->CoherencePerGroup.resize(numGroups, 0);
    result->SizePerGroup.resize(numGroups, 0);
    result->LinesByHeight.resize(
        volSize.z,
        std::make_shared<IntegralLineSet>(sampler_->getModelMatrix(), sampler_->getWorldMatrix()));

    std::vector<size_t> startTime(numGroups, size_t(-1));
    std::vector<size_t> endTime(numGroups, 0);
    std::vector<MetaLine> allLines;
    // std::vector<double> coherenceForward(numGroups, 0), numLinesBwd(numGroups, 0);

    // Calculate coherence per time.
    // for (int time = numTimes - 1; time >= 0; --time) {
    for (size_t time = 0; time < numTimes; ++time) {
        // auto heightLines = std::make_shared<IntegralLineSet>(sampler_->getModelMatrix(),
        //                                                      sampler_->getWorldMatrix());

        // size_t numVoxels = 0;
        auto mask = masks_->at(time)->getRepresentation<VolumeRAM>();

        util::forEachVoxel(*mask, [&](size3_t idx) {
            double maskSample = mask->getAsDouble(idx);
            if (maskSample == 0 || (std::rand() % seedStep_) != 0)
                // idx.x % seedStep_ != 0 || idx.y % seedStep_ != 0)
                return;
            size_t group = maskSample - 1;

            if (singleGroup_ && (group != group_)) return;

            startTime[group] = std::min(startTime[group], time);
            endTime[group] = std::max(endTime[group], time);

            // timeNumVoxels[maskSample - 1]++;
            dvec4 seed((0.5 + idx.x) * invDim.x, (0.5 + idx.y) * invDim.y, (0.5 + idx.z) * invDim.z,
                       (0.5 + time) * invDim.w);

            // Integrate pathline.

            std::array<MetaLine, 2> lines = {
                MetaLine(group, time, idx.z, IntegralLineProperties::Direction::FWD,
                         std::move(tracerFwd_.traceFrom(seed))),
                MetaLine(group, time, idx.z, IntegralLineProperties::Direction::BWD,
                         std::move(tracerBwd_.traceFrom(seed)))};
            auto pointsBwd = lines[1].Line.getPositions();
            std::reverse(pointsBwd.begin(), pointsBwd.end());

            // result->Lines->push_back(std::move(line), IntegralLineSet::SetIndex::No);
            double mult = 0.5;
            for (auto& line : lines) {
                std::vector<dvec3>& points = line.Line.getPositions();
                auto& velocities = line.Line.getMetaData<dvec3>("velocity", true);
                auto& timestamp = line.Line.getMetaData<double>("timestamp", false);
                velocities.resize(points.size(), dvec3(0.5 + mult));

                if (points.size() < 2) continue;
                // timeCoherence[maskSample - 1] += 1.0;
                // double coherence = 1.0;
                line.Coherence = 1.0;

                for (size_t p = 0; p < points.size(); ++p) {
                    // for (auto& point : points) {
                    auto& point = points[p];
                    double interp = double(p) / (points.size() - 1);
                    if (!isInside(*mask, point, maskSample)) {
                        // timeCoherence[maskSample - 1] -= (1.0 - interp);
                        line.Coherence = interp;
                        // for (; p < points.size(); ++p) {
                        //     velocities.push_back({0, 0, 0});
                        // }
                        // velocities[p] = {0.5 + mult, 0, 0};
                        break;
                    }
                    velocities[p] = {0.5 + 0.5 * interp * mult, 0, 0};
                }
                for (size_t p = 0; p < points.size(); ++p) {
                    std::swap(points[p].z, timestamp[p]);
                }

                // std::lock_guard<std::mutex> lock(lineWriteMutex);
                // timeLines->push_back(std::move(line), IntegralLineSet::SetIndex::No);
                allLines.emplace_back(std::move(line));
                mult = -0.5;
            }
        });
        // std::cout << "Did all voxels at time " << time << "!\n\t" << numVoxels << " voxels."
        //           << std::endl;
        // for (size_t group = 0; group < numGroups; ++group) {
        //     if (timeCoherence[group] > 0) {
        //         result->CoherencePerGroup[group] += timeCoherence[group] / timeNumVoxels[group];
        //         timeNumSteps[group]++;
        //         result->SizePerGroup[group] += timeNumVoxels[group];
        //     }
        //     if (timeStatus[group] == ShouldIntegrate::StartNext)
        //         timeStatus[group] = ShouldIntegrate::Yes;
        //     timeCoherence[group] = 0;
        //     timeNumVoxels[group] = 0;
        // }
        // if (timeLines->size() > 0) {
        //     result->Lines.push_back(timeLines);
        //     std::cout << "\tSize " << timeLines->back().getPositions().size() << std::endl;
        // }
        // std::cout << "Added lines!" << std::endl;
        // // progress(time + 1, numTimes);
    }

    // for (size_t group = 0; group < numGroups; ++group) {
    //     result->CoherencePerGroup[group] /= timeNumSteps[group];
    // }
    for (auto& line : allLines) {
        // Trying to integrate outside this group's time range?
        if (line.Dir == IntegralLineProperties::Direction::FWD) {
            if (line.Time + 1 > endTime[line.Group]) continue;
        } else {
            if (line.Time - 1 < startTime[line.Group]) continue;
        }

        result->CoherencePerGroup[line.Group] += line.Coherence;
        result->SizePerGroup[line.Group]++;
        result->LinesByHeight[line.Height]->push_back(line.Line, IntegralLineSet::SetIndex::No);
    }
    // LogWarn("num lines at height 0: " << result->LinesByHeight[0]->size());
    return result;
}

void SampleCoherence::postProcessCoherence(detail::CoherenceInfo* result) {
    if (!result) throw Exception("No result from coherence computation");
    lineOutputHeightStep_.setMaxValue(result->LinesByHeight.size());
    heightLineSets_ = std::move(result->LinesByHeight);
    if (lineOutputHeightStep_.get() < heightLineSets_.size()) {
        linesOut_.setData(heightLineSets_[lineOutputHeightStep_.get()]);
        std::cout << "Output lines" << std::endl;
    } else {
        // LogWarn("heightLineSet is too small?! " << lineOutputHeightStep_.get() << " not smaller "
        //                                         << heightLineSets_.size());
        linesOut_.setData(nullptr);
    }
    // LogWarn("Exporting stuff.");
    double coherence = 0;
    double weightedCoherence = 0;
    double totalSize = 0;
    size_t numValidGroups = 0;

    for (size_t group = 0; group < result->CoherencePerGroup.size(); ++group) {
        if (result->SizePerGroup[group] == 0) continue;
        numValidGroups++;
        // LogWarn("Coherence #" << group << " \t="
        //                       << (result->CoherencePerGroup[group] /
        //                       result->SizePerGroup[group]));
        coherence += result->CoherencePerGroup[group] / result->SizePerGroup[group];
        weightedCoherence += result->CoherencePerGroup[group];
        totalSize += result->SizePerGroup[group];
    }
    if (numValidGroups > 0) {
        coherence /= numValidGroups;
        weightedCoherence /= totalSize;
    }
    std::cout << "coherence: " << coherence << std::endl;
    std::cout << "weighted coherence: " << weightedCoherence << std::endl;
    averageCoherence_.set(coherence);
    weightedCoherence_.set(weightedCoherence);
}

}  // namespace inviwo
