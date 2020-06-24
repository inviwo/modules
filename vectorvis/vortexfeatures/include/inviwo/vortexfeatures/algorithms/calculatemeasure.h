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

#pragma once

#include <inviwo/vortexfeatures/vortexfeaturesmoduledefine.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/ports/datainport.h>
#include <inviwo/core/ports/dataoutport.h>
#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/core/datastructures/volume/volumeramprecision.h>
#include <inviwo/core/datastructures/image/layer.h>
#include <inviwo/core/datastructures/image/layerram.h>
#include <inviwo/core/datastructures/image/layerramprecision.h>
#include <inviwo/core/util/volumeramutils.h>

#include <type_traits>

namespace inviwo {

template <unsigned SpatialDims>
struct DispatchMeasureParallel {
public:
    using BaseData = std::conditional_t<(SpatialDims == 3), VolumeRAM, LayerRAM>;
    using BaseDataOwner = typename BaseData::ReprOwner;
    template <typename T>
    using BaseDataPrecision =
        std::conditional_t<(SpatialDims == 3), VolumeRAMPrecision<T>, LayerRAMPrecision<T>>;

    template <typename Callable, typename VecType, typename MatType>
    auto callMeasure(Callable&& measure, VecType& velocity, MatType* jacobian)
        -> decltype(measure(velocity,
                            *jacobian)) {  // std::invoke_result_t<Callable, VecType, MatType> {
        //
        return measure(velocity, *jacobian);
    }
    template <typename Callable, typename VecType, typename MatType>
    auto callMeasure(Callable&& measure, VecType& velocity, MatType* jacobian) -> decltype(measure(
        velocity)) {  // std::invoke_result_t<Callable, VecType> {  // decltype(measure(velocity)) {
        return measure(velocity);
    }

    template <typename PrecisionType, typename Callable>
    auto operator()(const PrecisionType* grid, Callable&& measure, double invalidValue = NAN,
                    double newInvalidValue = -4242) -> std::shared_ptr<BaseDataOwner>;
};

template <unsigned SpatialDims>
template <typename PrecisionType, typename Callable>
auto DispatchMeasureParallel<SpatialDims>::operator()(const PrecisionType* grid, Callable&& measure,
                                                      double invalidValue = NAN,
                                                      double newInvalidValue = -4242)
    -> std::shared_ptr<BaseDataOwner> {
    using Vec = util::PrecisionValueType<decltype(grid)>;  // FormatType;
    constexpr unsigned DataDims = util::extent<Vec>::value;
    using Mat = typename util::glmtype<double, SpatialDims, DataDims>::type;
    using SpatialVec = typename util::glmtype<double, SpatialDims, 1>::type;
    using DoubleVec = typename util::glmtype<double, DataDims>::type;

    // Figure out function signature.
    constexpr bool ComputeJacobian = std::is_invocable_v<Callable, Vec, Mat>;

    static_assert(std::is_invocable_v<Callable, Vec, Mat> || std::is_invocable_v<Callable, Vec> ||
                      std::is_invocable_v<Callable, Mat>,
                  "Can not call the given function with velocity and/or Jacobian.");
    using ReturnVec = typename std::conditional_t<  // Switch return type.
        ComputeJacobian,                            // v and J?
        std::invoke_result<Callable, Vec, Mat>, std::invoke_result<Callable, Vec>>::type;

    const Vec* rawData = grid->getDataTyped();
    auto gridDims = grid->getDimensions();
    size3_t gridDims3D(gridDims);
    BaseDataPrecision<ReturnVec>* result = new BaseDataPrecision<ReturnVec>(gridDims);
    ReturnVec* rawDataOut = result->getDataTyped();

    size3_t linOffsets(1, gridDims3D.x, gridDims3D.x * gridDims3D.y);
    SpatialVec cellScale(0);
    if constexpr (ComputeJacobian) {
        auto worldMatrix = grid->getOwner()->getWorldMatrix();
        for (unsigned dim = 0; dim < SpatialDims; ++dim) cellScale[dim] = worldMatrix[dim][dim];
    }

    unsigned long voxelCount = 0;
    ReturnVec voxelSum(0);
    auto voxelMin = DataFormat<typename util::value_type<ReturnVec>::type>::max();
    auto voxelMax = DataFormat<typename util::value_type<ReturnVec>::type>::lowest();

    // Read velocity and compute Jacobian in each data point.
    util::forEachVoxel(gridDims3D, [&](size3_t pos) {
        size_t linIdx = VolumeRAM::posToIndex(pos, gridDims3D);

        // Skip points with invalid value.
        Vec velocity = rawData[linIdx];
        if (util::glm_convert<double>(velocity) == invalidValue) {
            rawDataOut[linIdx] = ReturnVec(invalidValue);
            return;
        }
        voxelCount++;

        // Compute Jacobian if needed.
        if constexpr (ComputeJacobian) {
            Mat jacobian(0);
            for (glm::length_t dim = 0; dim < SpatialDims; ++dim) {
                Vec rightVel = velocity;
                Vec leftVel = velocity;
                double dist = 0;
                if (pos[dim] > 0) {
                    leftVel = rawData[linIdx - linOffsets[dim]];
                    if (static_cast<double>(util::glmcomp(leftVel, 0)) == invalidValue) {
                        leftVel = velocity;
                    } else {
                        dist += cellScale[dim];
                    }
                }
                if (pos[dim] < gridDims[dim] - 1) {
                    rightVel = rawData[linIdx + linOffsets[dim]];
                    if (static_cast<double>(util::glmcomp(rightVel, 0)) == invalidValue) {
                        rightVel = velocity;
                    } else {
                        dist += cellScale[dim];
                    }
                }
                if (dist == 0) {
                    rawDataOut[linIdx] = ReturnVec(invalidValue);
                    return;
                }
                jacobian[dim] = util::glm_convert<DoubleVec>(rightVel - leftVel) / cellScale[dim];
            }
            rawDataOut[linIdx] = this->template callMeasure<Callable, Vec, Mat>(
                std::forward<Callable&&>(measure), velocity, &jacobian);

        } else {  // !ComputeJacobian
            rawDataOut[linIdx] = this->template callMeasure<Callable, Vec, Mat>(
                std::forward<Callable&&>(measure), velocity, nullptr);
        }
        voxelSum += rawDataOut[linIdx];
        for (size_t dim = 0; dim < DataDims; ++dim) {
            voxelMin = std::min(voxelMin, util::glmcomp(rawDataOut[linIdx], dim));
            voxelMax = std::max(voxelMax, util::glmcomp(rawDataOut[linIdx], dim));
        }
    });

    std::cout << "Average: " << voxelSum / voxelCount;

    // Set invalid voxels to minimum.
    if (!std::isnan(invalidValue)) {
        util::forEachVoxel(gridDims3D, [&](size3_t pos) {
            size_t linIdx = VolumeRAM::posToIndex(pos, gridDims3D);
            auto val = rawDataOut[linIdx];
            if (util::glm_convert<double>(val) == invalidValue) {
                rawDataOut[linIdx] = ReturnVec(voxelMin);
                return;
            }
        });
    }
    // Assemble statistics.
    auto volume =
        std::make_shared<BaseDataOwner>(std::shared_ptr<inviwo::VolumeRepresentation>(result));
    // Explicit min/max to account for NAN.
    // auto minVal = voxelMin > invalidValue ? invalidValue : voxelMin;
    // auto maxVal = voxelMax < invalidValue ? invalidValue : voxelMax;
    volume->dataMap_.valueRange = dvec2(voxelMin, voxelMax);
    volume->dataMap_.dataRange = volume->dataMap_.valueRange;
    return volume;
}

template <unsigned SpatialDims>
class IVW_MODULE_VORTEXFEATURES_API MeasureProcessor : public Processor {
public:
    using Dispatcher = DispatchMeasureParallel<SpatialDims>;
    using BaseDataOwner = typename Dispatcher::BaseDataOwner;

    // Should give warning if not orthogonal!!!
    MeasureProcessor();
    virtual ~MeasureProcessor() = default;

    template <template <class> class Predicate = dispatching::filter::All, typename Callable>
    void dispatchMeasure(Callable&& measure, double invalidValue = NAN,
                         double newInvalidValue = -4242);

private:
    DataInport<BaseDataOwner> inport_;
    DataOutport<BaseDataOwner> outport_;
};
template <unsigned SpatialDims>
MeasureProcessor<SpatialDims>::MeasureProcessor() : inport_("dataIn"), outport_("dataOut") {
    addPort(inport_);
    addPort(outport_);
}

template <unsigned SpatialDims>
template <template <class> class Predicate, typename Callable>
void MeasureProcessor<SpatialDims>::dispatchMeasure(Callable&& measure, double invalidValue,
                                                    double newInvalidValue) {
    if (!inport_.hasData()) {
        outport_.setData(nullptr);
        return;
    }
    std::shared_ptr<const BaseDataOwner> inData = inport_.getData();
    const BaseData* baseRep = inData->template getRepresentation<BaseData>();

    DispatchMeasureParallel<SpatialDims> dispatcher;

    auto result = baseRep->template dispatch<std::shared_ptr<BaseDataOwner>, Predicate>(
        dispatcher, std::forward<Callable&&>(measure), invalidValue, newInvalidValue);

    outport_.setData(result);
}

// volumeram dispatch->VolumeRAMPrecision<typename Format::type>
}  // namespace inviwo