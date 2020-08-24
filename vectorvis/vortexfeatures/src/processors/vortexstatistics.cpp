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

#include <inviwo/vortexfeatures/processors/vortexstatistics.h>
#include <inviwo/vortexfeatures/processors/assemblewindingangle.h>
#include <inviwo/core/util/volumeramutils.h>
#include <inviwo/core/datastructures/volume/volumeramprecision.h>
#include <fmt/format.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VortexStatistics::processorInfo_{
    "org.inviwo.VortexStatistics",  // Class identifier
    "Vortex Statistics",            // Display name
    "Undefined",                    // Category
    CodeState::Experimental,        // Code state
    Tags::None,                     // Tags
};
const ProcessorInfo VortexStatistics::getProcessorInfo() const { return processorInfo_; }

VortexStatistics::VortexStatistics()
    : Processor()
    , maskVolumeIn_("maskingVolume")
    , scalarVolumesIn_("sclarVolumes")
    , topScalarVolumesIn_("surfaceScalarVolumes")
    , vorticesIn_("vorticesIn")
    , dataOut_("overviewData")
    , perGroupDataOut_("perGroupData")
    , scalarNames_("scalarNames", "Scalar Names")
    , topScalarNames_("topScalarNames", "Top Scalar Names") {

    addPort(maskVolumeIn_);
    addPort(scalarVolumesIn_);
    addPort(topScalarVolumesIn_);
    addPort(vorticesIn_);
    addPort(dataOut_);
    addPort(perGroupDataOut_);
    addProperties(scalarNames_, topScalarNames_);
    scalarNames_.setSerializationMode(PropertySerializationMode::All);
    topScalarNames_.setSerializationMode(PropertySerializationMode::All);

    scalarVolumesIn_.onChange([&]() {
        // for (const Property* prop : scalarNames_.getProperties())
        for (int idx = scalarNames_.size() - 1; idx >= 0; --idx) {
            scalarNames_.removeProperty(idx);
        }
        if (!scalarVolumesIn_.hasData() || scalarVolumesIn_.getData()->size() < 1) return;
        size_t numScalars = scalarVolumesIn_.getData()->front()->getDataFormat()->getComponents();
        for (size_t scalar = 0; scalar < numScalars; ++scalar) {
            scalarNames_.addProperty(
                new StringProperty("scalar" + scalar, "Scalar " + scalar, "Scalar" + scalar));
        }
    });

    topScalarVolumesIn_.onChange([&]() {
        // for (const Property* prop : scalarNames_.getProperties())
        for (int idx = topScalarNames_.size() - 1; idx >= 0; --idx) {
            topScalarNames_.removeProperty(idx);
        }
        if (!topScalarVolumesIn_.hasData()) return;
        size_t numScalars = topScalarVolumesIn_.getData()->getDataFormat()->getComponents();
        for (size_t scalar = 0; scalar < numScalars; ++scalar) {
            topScalarNames_.addProperty(
                new StringProperty("Tscalar" + scalar, "TScalar " + scalar, "TScalar" + scalar));
        }
    });
}

namespace detail {
template <typename T>
std::vector<T>& createColumn(DataFrame& df, const std::string& name, size_t numElements) {
    auto& col =
        df.addColumn<T>(name)->getTypedBuffer()->getEditableRAMRepresentation()->getDataContainer();
    col.resize(numElements, 0);
    return col;
}

template <typename T>
std::vector<T>& createColumns(DataFrame& collective, DataFrame& groups, const std::string& name,
                              size_t numGroups, size_t numTimeSteps,
                              std::vector<std::vector<T>*>& groupColumns) {
    for (size_t group = 0; group < numGroups; ++group) {
        groupColumns.push_back(
            &createColumn<T>(groups, fmt::format("{} {}", group, name), numTimeSteps));
    }
    return createColumn<T>(collective, name, numGroups);
}
}  // namespace detail

void VortexStatistics::process() {
    if (!maskVolumeIn_.hasData() || !maskVolumeIn_.getData() || !maskVolumeIn_.getData()->size() ||
        !scalarVolumesIn_.hasData() || !scalarVolumesIn_.getData() ||
        scalarVolumesIn_.getData()->size() != maskVolumeIn_.getData()->size() ||
        !topScalarVolumesIn_.hasData() || !vorticesIn_.hasData())
        return;

    auto vortices = vorticesIn_.getData();
    const size_t numGroups =
        static_cast<size_t>(maskVolumeIn_.getData()->back()->dataMap_.dataRange.y);
    const size_t numTimeSteps = maskVolumeIn_.getData()->size();
    const size3_t volumeDims = maskVolumeIn_.getData()->front()->getDimensions();
    const size3_t scalarDims = scalarVolumesIn_.getData()->front()->getDimensions();
    const size3_t topScalarDims = topScalarVolumesIn_.getData()->getDimensions();
    if (volumeDims != scalarDims || topScalarDims.x != scalarDims.x ||
        topScalarDims.y != scalarDims.y || topScalarDims.z != scalarVolumesIn_.getData()->size()) {
        LogWarn("Dimensions not matching:\n\tmask: \t"
                << volumeDims << 'x' << numTimeSteps  // Mask.
                << "\n\tscalars: \t" << scalarDims << 'x'
                << scalarVolumesIn_.getData()->size()  // Scalar.
                << "\n\ttop:   \t" << topScalarDims);  // Top layer scalar.
        return;
    }

    auto perEnsembleData = std::make_shared<DataFrame>(static_cast<std::uint32_t>(numGroups - 1));
    auto perVortexData = std::make_shared<DataFrame>(static_cast<std::uint32_t>(numTimeSteps));
    dataOut_.setData(perEnsembleData);
    perGroupDataOut_.setData(perVortexData);

    std::vector<std::vector<size_t>*> groupNumVoxels;
    std::vector<std::vector<size_t>*> groupNumTopVoxels;
    std::vector<std::vector<size_t>*> groupMaxDepth;

    auto numVoxels = detail::createColumns<size_t>(*perEnsembleData, *perVortexData, "No Voxels",
                                                   numGroups, numTimeSteps, groupNumVoxels);
    auto numTopVoxels =
        detail::createColumns<size_t>(*perEnsembleData, *perVortexData, "Top No Voxels", numGroups,
                                      numTimeSteps, groupNumTopVoxels);
    auto maxDepth = detail::createColumns<size_t>(*perEnsembleData, *perVortexData, "Max Depth",
                                                  numGroups, numTimeSteps, groupMaxDepth);
    auto lifeTime = detail::createColumn<size_t>(*perEnsembleData, "Life Time", numGroups);
    auto rotation = detail::createColumn<std::string>(*perEnsembleData, "Rotation", numGroups);

    std::vector<std::vector<float>*> groupTopAspectRatio;
    std::vector<std::vector<float>*> groupTopAvgRadius;
    auto topAspectRatio =
        detail::createColumns<float>(*perEnsembleData, *perVortexData, "Top Aspect Ratio",
                                     numGroups, numTimeSteps, groupTopAspectRatio);
    auto topAvgRadius =
        detail::createColumns<float>(*perEnsembleData, *perVortexData, "Top Avg Radius", numGroups,
                                     numTimeSteps, groupTopAvgRadius);

    std::vector<std::vector<float>*> scalars;
    std::vector<std::vector<std::vector<float>*>> groupScalars;
    for (auto nameProp : scalarNames_.getProperties()) {
        auto nameStringProp = dynamic_cast<StringProperty*>(nameProp);
        if (nameStringProp) {
            groupScalars.emplace_back();
            scalars.push_back(&detail::createColumns<float>(
                *perEnsembleData, *perVortexData, "Top Avg " + nameStringProp->get(), numGroups,
                numTimeSteps, groupScalars.back()));
            scalars.push_back(&detail::createColumns<float>(
                *perEnsembleData, *perVortexData, "Avg " + nameStringProp->get(), numGroups,
                numTimeSteps, groupScalars.back()));
            scalars.push_back(&detail::createColumns<float>(
                *perEnsembleData, *perVortexData, "Min " + nameStringProp->get(), numGroups,
                numTimeSteps, groupScalars.back()));
            scalars.push_back(&detail::createColumns<float>(
                *perEnsembleData, *perVortexData, "Max " + nameStringProp->get(), numGroups,
                numTimeSteps, groupScalars.back()));
        }
    }

    std::vector<std::vector<float>*> topScalars;
    std::vector<std::vector<std::vector<float>*>> groupTopScalars;
    for (auto nameProp : topScalarNames_.getProperties()) {
        auto nameStringProp = dynamic_cast<StringProperty*>(nameProp);
        if (nameStringProp) {
            groupTopScalars.emplace_back();
            topScalars.push_back(&detail::createColumns<float>(
                *perEnsembleData, *perVortexData, "Top Avg " + nameStringProp->get(), numGroups,
                numTimeSteps, groupTopScalars.back()));
            topScalars.push_back(&detail::createColumns<float>(
                *perEnsembleData, *perVortexData, "Avg " + nameStringProp->get(), numGroups,
                numTimeSteps, groupTopScalars.back()));
            topScalars.push_back(&detail::createColumns<float>(
                *perEnsembleData, *perVortexData, "Min " + nameStringProp->get(), numGroups,
                numTimeSteps, groupTopScalars.back()));
            topScalars.push_back(&detail::createColumns<float>(
                *perEnsembleData, *perVortexData, "Max " + nameStringProp->get(), numGroups,
                numTimeSteps, groupTopScalars.back()));
        }
    }
    perEnsembleData->updateIndexBuffer();
    perVortexData->updateIndexBuffer();

    auto assignMin = [](auto& vecVal, auto val) { vecVal = std::min(vecVal, val); };
    auto assignMax = [](auto& vecVal, auto val) { vecVal = std::min(vecVal, val); };

    const VolumeRAM* firstMask = maskVolumeIn_.getData()->front()->getRepresentation<VolumeRAM>();

    firstMask->dispatch<void, dispatching::filter::Scalars>([&](const auto* mask) {
        // first arg: const VolumeRAMPrecision<typename Format::type>*
        using PrecisionType = decltype(mask);
        std::vector<size_t> startTime(numGroups, numTimeSteps);
        for (size_t time = 0; time < numTimeSteps; ++time) {
            PrecisionType maskVolume = dynamic_cast<PrecisionType>(
                maskVolumeIn_.getData()->at(time).get()->getRepresentation<VolumeRAM>());
            ivwAssert(maskVolume, "Different data types in mask volume.");
            auto* maskData = maskVolume->getDataTyped();

            std::vector<std::mutex> groupMutex(numGroups);
            const VolumeRAM* scalarVol =
                scalarVolumesIn_.getData()->at(time)->getRepresentation<VolumeRAM>();
            const VolumeRAM* topScalarVol =
                topScalarVolumesIn_.getData()->getRepresentation<VolumeRAM>();
            const size_t numScalars = scalarVol->getDataFormat()->getComponents();
            const size_t numTopScalars = topScalarVol->getDataFormat()->getComponents();

            auto indexCallback = [&](const size3_t& idx) {
                auto group = maskData[VolumeRAM::posToIndex(idx, volumeDims)];
                if (group <= 0) return;

                startTime[group] = std::min(startTime[group], time);
                lifeTime[group] = time - startTime[group];

                std::lock_guard<std::mutex> lock(groupMutex[group]);
                numVoxels[group]++;
                (*groupNumVoxels[group])[time]++;
                assignMax(maxDepth[group], idx.z);
                assignMax((*groupMaxDepth[group])[time], idx.z);

                // Sample scalar volume time series.
                dvec4 scalarSample = scalarVol->getAsDVec4(idx);
                for (size_t s = 0; s < numScalars; ++s) {
                    (*scalars[s * 4 + 1])[group] += scalarSample[s];
                    (*groupScalars[s * 4 + 1][group])[time] += scalarSample[s];

                    assignMin((*scalars[s * 4 + 2])[group], float(scalarSample[s]));
                    assignMin((*groupScalars[s * 4 + 2][group])[time], (float(scalarSample[s])));

                    assignMax((*scalars[s * 4 + 3])[group], float(scalarSample[s]));
                    assignMax((*groupScalars[s * 4 + 3][group])[time], float(scalarSample[s]));
                }

                if (idx.z == AssembleWindingAngle::SEED_DEPTH) {
                    numTopVoxels[group]++;
                    (*groupNumTopVoxels[group])[time]++;
                    for (size_t s = 0; s < numScalars; ++s) {
                        (*scalars[s * 4])[group] += scalarSample[s];
                        (*groupScalars[s * 4][group])[time] += scalarSample[s];
                    }

                    // Sample surface.
                    scalarSample = scalarTopVol->getAsDVec4({idx.x, idx.y, time});
                    for (size_t s = 0; s < numTopScalars; ++s) {
                        (*topScalars[s * 4 + 1])[group] += scalarSample[s];
                        (*groupTopScalars[s * 4 + 1][group])[time] += scalarSample[s];

                        assignMin((*topScalars[s * 4 + 2])[group], float(scalarSample[s]));
                        assignMin((*groupTopScalars[s * 4 + 2][group])[time],
                                  (float(scalarSample[s])));

                        assignMax((*topScalars[s * 4 + 3])[group], float(scalarSample[s]));
                        assignMax((*groupTopScalars[s * 4 + 3][group])[time],
                                  float(scalarSample[s]));
                    }
                }
            };

            util::forEachVoxelParallel(*maskVolume, indexCallback);

            for (size_t group = 0; group < numGroups; ++group) {
                for (size_t s = 0; s < numScalars; ++s) {
                    (*scalars[s * 4])[group] /= numTopVoxels[group];
                    (*scalars[s * 4 + 1])[group] /= numVoxels[group];

                    for (size_t time = 0; time < numTimeSteps; ++time) {
                        (*groupScalars[s * 4][group])[time] /= (*groupNumTopVoxels[group])[time];
                        (*groupScalars[s * 4 + 1][group])[time] /= (*groupNumVoxels[group])[time];
                    }
                }

                auto vortex =
                    std::find_if(vortices->beginGroup(group), vortices->endGroup(group),
                                 [&](const Vortex& vort) {
                                     return vort.heightSlice == AssembleWindingAngle::SEED_DEPTH;
                                 });
                if (vortex == vortices->endGroup(group)) continue;

                (*groupTopAspectRatio[group])[time] = vortex->maxRadius / vortex->minRadius;
                (*groupTopAvgRadius[group])[time] = vortex->avgRadius;
                topAspectRatio[group] += vortex->maxRadius / vortex->minRadius;
                topAvgRadius[group] += vortex->avgRadius;
            }
        }

        for (size_t group = 0; group < numGroups; ++group) {
            rotation[group] = (vortices->beginGroup(group)->rotation == Vortex::Turning::Clockwise)
                                  ? "CW"
                                  : "CCW";
            topAspectRatio[group] /= lifeTime[group];
            topAvgRadius[group] /= lifeTime[group];
        }
    });
}

}  // namespace inviwo
