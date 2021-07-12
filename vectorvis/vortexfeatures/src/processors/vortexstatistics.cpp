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
    , topScalarNames_("topScalarNames", "Top Scalar Names")
    , ensembleMember_("ensembleMember", "Ensemble Member", 0, {0, ConstraintBehavior::Immutable},
                      {50, ConstraintBehavior::Ignore})
    , skipLastGroup_("skipLastGroup", "Skip Last Group", true)
    , assemble_("assemble", "Assemble", [&]() {
        doAssemble_ = true;
        invalidate(InvalidationLevel::InvalidOutput);
    }) {

    addPort(maskVolumeIn_);
    addPort(scalarVolumesIn_);
    addPort(topScalarVolumesIn_);
    addPort(vorticesIn_);
    addPort(dataOut_);
    addPort(perGroupDataOut_);
    addProperties(scalarNames_, topScalarNames_, ensembleMember_, skipLastGroup_, assemble_);
    scalarNames_.setSerializationMode(PropertySerializationMode::All);
    topScalarNames_.setSerializationMode(PropertySerializationMode::All);

    scalarVolumesIn_.onChange([&]() {
        size_t numScalars = 0;
        size_t numNames = scalarNames_.size();
        if (scalarVolumesIn_.hasData() && scalarVolumesIn_.getData()->size() > 0) {
            numScalars = scalarVolumesIn_.getData()->front()->getDataFormat()->getComponents();
        }

        for (int idx = numNames - 1; idx >= int(numScalars); --idx) {
            scalarNames_.removeProperty(idx);
        }
        for (size_t scalar = numNames; scalar < numScalars; ++scalar) {
            scalarNames_.addProperty(new StringProperty(fmt::format("scalar{}", scalar),
                                                        fmt::format("Scalar{}", scalar),
                                                        fmt::format("Scalar{}", scalar)));
        }
    });

    topScalarVolumesIn_.onChange([&]() {
        size_t numTopScalars = 0;
        size_t numNames = topScalarNames_.size();
        if (topScalarVolumesIn_.hasData()) {
            numTopScalars = topScalarVolumesIn_.getData()->getDataFormat()->getComponents();
        }

        for (int idx = numNames - 1; idx >= int(numTopScalars); --idx) {
            topScalarNames_.removeProperty(idx);
        }
        for (size_t scalar = numNames; scalar < numTopScalars; ++scalar) {
            topScalarNames_.addProperty(new StringProperty(fmt::format("Tscalar{}", scalar),
                                                           fmt::format("TScalar{}", scalar),
                                                           fmt::format("TScalar{}", scalar)));
        }
    });
}

namespace detail {
template <typename T>
std::vector<T>& createColumn(DataFrame& df, const std::string& name, size_t numElements,
                             T initVal = 0) {
    auto& col =
        df.addColumn<T>(name)->getTypedBuffer()->getEditableRAMRepresentation()->getDataContainer();
    col.resize(numElements, initVal);
    return col;
}

struct MakeColumn {
    MakeColumn(std::shared_ptr<DataFrame> perEnsemble, std::shared_ptr<DataFrame> perVortex,
               size_t numGroup, size_t numGroupTimeStep)
        : perEnsembleData(perEnsemble)
        , perVortexData(perVortex)
        , numGroups(numGroup)
        , numGroupTimeSteps(numGroupTimeStep) {}

    template <typename T>
    std::pair<std::vector<T>*, std::vector<T>*> make(const std::string& name, T initVal = 0) {
        auto* col = &detail::createColumn<T>(*perEnsembleData, name, numGroups, initVal);
        auto* seriesCol =
            &detail::createColumn<T>(*perVortexData, name, numGroupTimeSteps, initVal);
        return std::make_pair(col, seriesCol);
    }

    std::shared_ptr<DataFrame> perEnsembleData, perVortexData;
    size_t numGroups, numGroupTimeSteps;
};

// template <typename T>
// std::vector<T>& createColumns(DataFrame& collective, DataFrame& groups, const std::string& name,
//                               size_t numGroups, size_t numTimeSteps,
//                               std::vector<std::vector<T>*>& groupColumns) {
//     createColumn<T>(groups, fmt::format("{} {}", group, name), numTimeSteps));
//     }
//     return createColumn<T>(collective, name, numGroups);
// }
}  // namespace detail

void VortexStatistics::process() {
    if (!doAssemble_) return;
    if (!maskVolumeIn_.hasData() || !maskVolumeIn_.getData() || !maskVolumeIn_.getData()->size() ||
        !scalarVolumesIn_.hasData() || !scalarVolumesIn_.getData() ||
        scalarVolumesIn_.getData()->size() != maskVolumeIn_.getData()->size() ||
        !topScalarVolumesIn_.hasData() || !vorticesIn_.hasData())
        return;
    doAssemble_ = false;

    auto vortices = vorticesIn_.getData();
    const size_t numGroups =
        std::min(static_cast<size_t>(maskVolumeIn_.getData()->back()->dataMap_.dataRange.y - 1),
                 skipLastGroup_ ? vortices->numGroups() - 1 : vortices->numGroups());

    const size_t numTimeSteps = maskVolumeIn_.getData()->size();
    const size_t numGroupTimeSteps = numTimeSteps * numGroups;

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

    // ============ Create data columns for both data frames ============ //

    auto perEnsembleData = std::make_shared<DataFrame>(static_cast<std::uint32_t>(numGroups));
    auto perVortexData = std::make_shared<DataFrame>(static_cast<std::uint32_t>(numGroupTimeSteps));

    detail::MakeColumn makeColumn(perEnsembleData, perVortexData, numGroups, numGroupTimeSteps);

    makeColumn.make<int>("Ensemble", ensembleMember_.get());
    auto colGroup = makeColumn.make<int>("Vortex ID", 0);
    auto& colTimeStep = detail::createColumn<int>(*perVortexData, "Time", numGroupTimeSteps);
    auto colNumVoxels = makeColumn.make<int>("Num Voxels", 0);
    auto colNumTopVoxels = makeColumn.make<int>("Num Surface Voxels", 0);
    auto colMaxDepth = makeColumn.make<int>("Max Depth", 0);

    auto colAspectRatio = makeColumn.make<float>("Surface Aspect Ratio", 0);
    auto colAvgRadius = makeColumn.make<float>("Surface Avg Radius", 0);

    auto& colLifeTime = detail::createColumn<int>(*perEnsembleData, "Life Time", numGroups, 0);
    auto& colStartTime =
        detail::createColumn<int>(*perEnsembleData, "Start Time", numGroups, numTimeSteps);
    // std::fill(colStartTime.begin(), colStartTime.end(), numTimeSteps);
    // std::fill(colEnsemble.first->begin(), colEnsemble.first->end(), ensembleMember_.get());
    // std::fill(colEnsemble.second->begin(), colEnsemble.second->end(), ensembleMember_.get());
    // auto& colRotation = detail::createColumn<std::string>(*perEnsembleData, "Rotation",
    // numGroups);
    auto columnRotation = perEnsembleData->addCategoricalColumn("Rotation", numGroups);
    auto& colRotation =
        columnRotation->getTypedBuffer()->getEditableRAMRepresentation()->getDataContainer();
    columnRotation->set(0, "CW");
    columnRotation->set(1, "CCW");

    auto& colCenterX = detail::createColumn<float>(*perVortexData, "Center X", numGroupTimeSteps);
    auto& colCenterY = detail::createColumn<float>(*perVortexData, "Center X", numGroupTimeSteps);

    auto& colStartX = detail::createColumn<float>(*perEnsembleData, "Start Center X", numGroups, 0);
    auto& colStartY = detail::createColumn<float>(*perEnsembleData, "Start Center Y", numGroups, 0);
    auto& colEndX = detail::createColumn<float>(*perEnsembleData, "End Center X", numGroups);
    auto& colEndY = detail::createColumn<float>(*perEnsembleData, "End Center Y", numGroups);
    auto& colTravel =
        detail::createColumn<float>(*perEnsembleData, "Travelled Distance", numGroups, 0);
    // ============ Create scalar columns from given names ============ //
    std::vector<std::pair<std::vector<float>*, std::vector<float>*>> colScalars;
    for (auto nameProp : scalarNames_.getProperties()) {
        auto nameStringProp = dynamic_cast<StringProperty*>(nameProp);
        if (nameStringProp) {
            colScalars.emplace_back(
                makeColumn.make<float>(fmt::format("Surface {}", nameStringProp->get()), 0));
            colScalars.emplace_back(
                makeColumn.make<float>(fmt::format("Avg {}", nameStringProp->get()), 0));
            colScalars.emplace_back(
                makeColumn.make<float>(fmt::format("Min {}", nameStringProp->get()), 0));
            colScalars.emplace_back(
                makeColumn.make<float>(fmt::format("Max {}", nameStringProp->get()), 0));
        }
    }

    std::vector<std::pair<std::vector<float>*, std::vector<float>*>> colTopScalars;
    for (auto nameProp : topScalarNames_.getProperties()) {
        auto nameStringProp = dynamic_cast<StringProperty*>(nameProp);
        if (nameStringProp) {
            colTopScalars.emplace_back(
                makeColumn.make<float>(fmt::format("Extreme {}", nameStringProp->get()), 0));
            colTopScalars.emplace_back(
                makeColumn.make<float>(fmt::format("Min {}", nameStringProp->get()), 0));
            colTopScalars.emplace_back(
                makeColumn.make<float>(fmt::format("Max {}", nameStringProp->get()), 0));
        }
    }

    // Check the number of scalars.
    const VolumeRAM* scalarVolFirst =
        scalarVolumesIn_.getData()->front()->getRepresentation<VolumeRAM>();
    const VolumeRAM* topScalarVol = topScalarVolumesIn_.getData()->getRepresentation<VolumeRAM>();
    const size_t numScalars = scalarVolFirst->getDataFormat()->getComponents();
    const size_t numTopScalars = topScalarVol->getDataFormat()->getComponents();
    ivwAssert(numScalars * 4 == colScalars.size() && numTopScalars * 3 == colTopScalars.size(),
              "Name count differs from scalar count.");
    perEnsembleData->updateIndexBuffer();
    perVortexData->updateIndexBuffer();

    auto assignMin = [](auto& vecVal, auto val) {
        if (vecVal == 0) vecVal = val;
        vecVal = std::min(vecVal, val);
    };
    auto assignMax = [](auto& vecVal, auto val) {
        if (vecVal == 0) vecVal = val;
        vecVal = std::max(vecVal, val);
    };
    auto assignExtremum = [](auto& vecVal, auto val) {
        if (vecVal == 0)
            vecVal = val;
        else if (std::abs(val) > std::abs(vecVal))
            vecVal = val;
    };
    std::vector<dvec2> centers(numGroups, dvec2(0, 0));
    const VolumeRAM* firstMask = maskVolumeIn_.getData()->front()->getRepresentation<VolumeRAM>();

    firstMask->dispatch<void, dispatching::filter::Scalars>([&](const auto* mask) {
        // First arg: const VolumeRAMPrecision<T>*
        using PrecisionType = decltype(mask);

        for (int time = 0; time < int(numTimeSteps); ++time) {
            PrecisionType maskVolume = dynamic_cast<PrecisionType>(
                maskVolumeIn_.getData()->at(time).get()->getRepresentation<VolumeRAM>());
            ivwAssert(maskVolume, "Different data types in mask volume.");
            ivwAssert(glm::all(maskVolume->getDimensions() == volumeDims),
                      "Time slice size is not the same as first slice.");
            auto* maskData = maskVolume->getDataTyped();

            // std::vector<std::mutex> groupMutex(numGroups);
            const VolumeRAM* scalarVol =
                scalarVolumesIn_.getData()->at(time)->getRepresentation<VolumeRAM>();

            // ============ Callback per voxel ============ //
            auto indexCallback = [&](const size3_t& idx) {
                int group = static_cast<int>(maskData[VolumeRAM::posToIndex(idx, volumeDims)]);
                if (group <= 0) return;
                if (group >= int(numGroups)) {
                    // std::cout << "Overlapping vortices? ID " << group << std::endl;
                    return;
                }
                group -= 1;
                size_t idxGroupTime = numGroups * time + group;

                colStartTime[group] = std::min(colStartTime[group], time);
                colLifeTime[group] = time - colStartTime[group];

                // std::lock_guard<std::mutex> lock(groupMutex[group]);
                colNumVoxels.first->at(group)++;
                colNumVoxels.second->at(idxGroupTime)++;
                assignMax(colMaxDepth.first->at(group), int(idx.z));
                assignMax(colMaxDepth.second->at(idxGroupTime), int(idx.z));

                // Sample scalar volume time series.
                dvec4 scalarSample = scalarVol->getAsDVec4(idx);
                for (size_t s = 0; s < numScalars; ++s) {
                    colScalars[s * 4 + 1].first->at(group) += scalarSample[s];
                    colScalars[s * 4 + 1].second->at(idxGroupTime) += scalarSample[s];

                    assignMin(colScalars[s * 4 + 2].first->at(group), float(scalarSample[s]));
                    assignMin(colScalars[s * 4 + 2].second->at(idxGroupTime),
                              (float(scalarSample[s])));

                    assignMax(colScalars[s * 4 + 3].first->at(group), float(scalarSample[s]));
                    assignMax(colScalars[s * 4 + 3].second->at(idxGroupTime),
                              (float(scalarSample[s])));
                }

                // ============ Surface voxel ============ //
                if (idx.z != AssembleWindingAngle::SEED_DEPTH) return;
                colNumTopVoxels.first->at(group)++;
                colNumTopVoxels.second->at(idxGroupTime)++;
                for (size_t s = 0; s < numScalars; ++s) {
                    colScalars[s * 4].first->at(group) += scalarSample[s];
                    colScalars[s * 4].second->at(idxGroupTime) += scalarSample[s];
                }

                // Sample surface.
                scalarSample = topScalarVol->getAsDVec4({idx.x, idx.y, time});
                for (size_t s = 0; s < numTopScalars; ++s) {
                    assignExtremum(colTopScalars[s * 3].first->at(group), scalarSample[s]);
                    assignExtremum(colTopScalars[s * 3].second->at(idxGroupTime), scalarSample[s]);

                    assignMin(colTopScalars[s * 3 + 1].first->at(group), float(scalarSample[s]));
                    assignMin(colTopScalars[s * 3 + 1].second->at(idxGroupTime),
                              (float(scalarSample[s])));

                    assignMax(colTopScalars[s * 3 + 2].first->at(group), float(scalarSample[s]));
                    assignMax(colTopScalars[s * 3 + 2].second->at(idxGroupTime),
                              float(scalarSample[s]));
                }
            };

            util::forEachVoxel(*maskVolume, indexCallback);

            for (size_t group = 0; group < numGroups; ++group) {
                size_t idxGroupTime = numGroups * time + group;
                colTimeStep[idxGroupTime] = time;
                colGroup.second->at(idxGroupTime) = group;

                // if (colTopNumVoxels.first->at(group) > 0) {
                //     colTopScalars[s * 4].first->at(group) /=
                //     colTopNumVoxels.first->at(group);
                // }
                // if (colNumVoxels.first->at(group) > 0) {
                //     colScalars[s * 4 + 1].first->at(group) /= colNumVoxels.first->at(group);
                // }

                // for (size_t time = 0; time < numTimeSteps; ++time) {
                if (colNumTopVoxels.second->at(idxGroupTime) > 0) {
                    // for (size_t s = 0; s < numTopScalars; ++s) {
                    //     colTopScalars[s * 3].second->at(idxGroupTime) /=
                    //         colNumTopVoxels.second->at(idxGroupTime);
                    // }
                    for (size_t s = 0; s < numScalars; ++s) {
                        colScalars[s * 4].second->at(idxGroupTime) /=
                            colNumTopVoxels.second->at(idxGroupTime);
                    }
                }
                if (colNumVoxels.second->at(idxGroupTime) > 0) {
                    for (size_t s = 0; s < numScalars; ++s) {
                        colScalars[s * 4 + 1].second->at(idxGroupTime) /=
                            colNumVoxels.second->at(idxGroupTime);
                    }
                }

                auto vortex =
                    std::find_if(vortices->beginGroup(group), vortices->endGroup(group),
                                 [&](const Vortex& vort) {
                                     return vort.heightSlice == AssembleWindingAngle::SEED_DEPTH;
                                 });
                if (vortex == vortices->endGroup(group)) {
                    LogWarn("Found vortex not on seed depth.");
                    continue;
                }

                if (std::isfinite(vortex->maxRadius / vortex->minRadius)) {
                    // std::cout << "Inf! " << vortex->maxRadius << " / " << vortex->minRadius
                    //           << std::endl;
                    colAspectRatio.second->at(idxGroupTime) = vortex->maxRadius / vortex->minRadius;
                    colAspectRatio.first->at(group) += vortex->maxRadius / vortex->minRadius;
                }
                colAvgRadius.second->at(idxGroupTime) = vortex->avgRadius;
                colAvgRadius.first->at(group) += vortex->avgRadius;

                colRotation[group] =
                    (vortex->rotation == Vortex::Turning::Clockwise) ? 0 : 1;  //"CW" : "CCW";

                colCenterX[idxGroupTime] = vortex->center.x;
                colCenterY[idxGroupTime] = vortex->center.y;
                if (time != colStartTime[group]) {
                    // size_t idxLastTime = numGroups * (time - 1) + group;
                    colTravel[group] += glm::distance(
                        vortex->center, centers[group]);  // dvec2(colCenterX[idxLastTime],
                                                          // colCenterY[idxLastTime]));
                    centers[group] = vortex->center;

                    colEndX[group] = vortex->center.x;
                    colEndY[group] = vortex->center.y;
                } else {
                    colStartX[group] = vortex->center.x;
                    colStartY[group] = vortex->center.y;
                }
            }
        }  // For each time...

        // Not per time:
        for (size_t group = 0; group < numGroups; ++group) {
            if (colNumTopVoxels.first->at(group) > 0) {
                // for (size_t s = 0; s < numTopScalars; ++s) {
                //     colTopScalars[s * 3].first->at(group) /=
                //     colNumTopVoxels.first->at(group);
                // }
                for (size_t s = 0; s < numScalars; ++s) {
                    colScalars[s * 4 + 0].first->at(group) /= colNumTopVoxels.first->at(group);
                }
            }
            if (colNumVoxels.first->at(group) > 0) {
                for (size_t s = 0; s < numScalars; ++s) {
                    colScalars[s * 4 + 1].first->at(group) /= colNumVoxels.first->at(group);
                }
            }

            colGroup.first->at(group) = group;
            if (colLifeTime[group] == 0) continue;
            colAspectRatio.first->at(group) /= colLifeTime[group];
            colAvgRadius.first->at(group) /= colLifeTime[group];
        }
    });

    dataOut_.setData(perEnsembleData);
    perGroupDataOut_.setData(perVortexData);
}

}  // namespace inviwo
