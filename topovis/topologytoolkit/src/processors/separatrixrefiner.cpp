/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019 Inviwo Foundation
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

#include <inviwo/topologytoolkit/processors/separatrixrefiner.h>

#include <ttk/core/base/discretegradient/DiscreteGradient.h>

namespace inviwo {

namespace {

template <size_t N, typename T = double>
class SeparatrixSpringSystem : public SpringSystem<N, T, SeparatrixSpringSystem<N, T>> {
public:
    using Base = SpringSystem<N, T, SeparatrixSpringSystem<N, T>>;
    using SpringIndices = typename Base::SpringIndices;
    static constexpr size_t Components = N;
    using Vector = glm::vec<Components, T>;
    using Type = SeparatrixRefiner::Type;

    SeparatrixSpringSystem(std::shared_ptr<const SpatialSampler<3, 3, double>> sampler,
                           T gradientScale, T timeStep, std::vector<Vector> positions,
                           std::vector<SpringIndices> springs, std::vector<Type> aTypes, T nodeMass,
                           T springLinearConstant, T springSquareConstant, T springLength,
                           T springDampning)
        : Base(timeStep, std::move(positions), std::move(springs))
        , sampler{sampler}
        , gradientScale{gradientScale}
        , types{std::move(aTypes)}
        , globalNodeMass{nodeMass}
        , springLinearConstant{springLinearConstant}
        , springSquareConstant{springSquareConstant}
        , globalSpringLength{springLength}
        , globalSpringDampning{springDampning} {}

    bool isLocked(size_t i) const {
        // return types[i] == Type::minimum || types[i] == Type::maximum || types[i] == Type::sadle;
        return types[i] == Type::sadle;
    }

    Vector externalForce(size_t i) {
        const float factor = [&]() {
            switch (types[i]) {
                case Type::maximum:
                    return 1.0f;
                case Type::maxSeperatrix:
                    return 1.0f;
                case Type::minimum:
                    return -1.0f;
                case Type::minSeperatrix:
                    return -1.0f;
                case Type::sadle:
                    return 0.0f;
                case Type::unkown:
                    return 0.0f;
                default:
                    return 0.0f;
            }
        }();
        return factor * gradientScale *
               static_cast<Vector>(sampler->sample(this->positions_[i], CoordinateSpace::Model));
    }
    T nodeMass(size_t) { return globalNodeMass; }

    T forceMagnitude(size_t, T displacement) {
        return displacement * springLinearConstant +
               springSquareConstant * displacement * displacement;
    }

    T springLength(size_t) const { return globalSpringLength; }
    T springDampning(size_t) const { return globalSpringDampning; }

    void constrainPosition(size_t, Vector&) const {}
    void constrainVelocity(size_t, Vector&) const {}

    std::shared_ptr<const SpatialSampler<3, 3, double>> sampler;
    T gradientScale;
    std::vector<Type> types;
    T globalNodeMass{1};
    T springLinearConstant{1};
    T springSquareConstant{1};
    T globalSpringLength{1};
    T globalSpringDampning{1};
};

}  // namespace

namespace detail {}  // namespace detail

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo SeparatrixRefiner::processorInfo_{
    "org.inviwo.SeparatrixRefiner",  // Class identifier
    "Separatrix Refiner",            // Display name
    "Topology",                      // Category
    CodeState::Experimental,         // Code state
    "CPU, Topology, TTK",            // Tags
};
const ProcessorInfo SeparatrixRefiner::getProcessorInfo() const { return processorInfo_; }

SeparatrixRefiner::SeparatrixRefiner()
    : Processor()
    , inport_{"inport"}
    , sampler_{"sampler"}
    , outport_{"outport"}
    , timesteps_{"timesteps", "Timesteps", size_t{100}, size_t{0}, size_t{1000000}}
    , timestep_{"timestep", "Timestep", 0.01f, 0.0f, 100.0f}
    , springLength_{"springLength", "Spring Length", 0.01f, 0.0f, 100.0f}
    , springLinearConstant_{"springLinearConstant", "Spring Linear Constant", 1.0f, -2.0f, 2.0f}
    , springSquareConstant_{"springSquareConstant", "Spring Square Constant", 0.0f, -20.0f, 20.0f}
    , springDamping_{"springDamping", "Spring Damping", 0.01f, 0.0f, 2.0f}
    , gradientScale_{"gradientScale", "Gradient Scale", 1.0f, -1.0f, 1.0f} {

    addPort(inport_);
    addPort(sampler_);
    addPort(outport_);

    addProperties(timesteps_, timestep_, springLength_, springLinearConstant_,
                  springSquareConstant_, springDamping_, gradientScale_);
}

void SeparatrixRefiner::process() {
    using Sys = SeparatrixSpringSystem<3, float>;

    const auto msc = inport_.getData();

    const auto ncp = msc->criticalPoints.numberOfPoints;
    const auto nsp = msc->separatrixPoints.numberOfPoints;
    const auto nsc = msc->separatrixCells.numberOfCells;

    std::vector<vec3> positions;
    positions.reserve(ncp + nsp);
    std::vector<Type> types;
    types.reserve(ncp + nsp);

    const auto id = [](const auto& item, auto i) {
        return std::make_tuple(item.cellDimensions[i], item.cellIds[i]);
    };
    std::unordered_map<std::tuple<char, ttk::SimplexId>, size_t> cellIdToPosIndex;

    std::unordered_map<ttk::SimplexId, Type> cellIdToType;

    for (ttk::SimplexId i = 0; i < ncp; i++) {
        cellIdToPosIndex[id(msc->criticalPoints, i)] = positions.size();
        positions.push_back({msc->criticalPoints.points[3 * i + 0],
                             msc->criticalPoints.points[3 * i + 1],
                             msc->criticalPoints.points[3 * i + 2]});

        if (msc->criticalPoints.cellDimensions[i] == 0) {
            types.push_back(Type::minimum);
            cellIdToType[msc->criticalPoints.cellIds[i]] = Type::minimum;
        } else if (msc->criticalPoints.cellDimensions[i] == 1) {
            types.push_back(Type::sadle);
            cellIdToType[msc->criticalPoints.cellIds[i]] = Type::sadle;
        } else {
            types.push_back(Type::maximum);
            cellIdToType[msc->criticalPoints.cellIds[i]] = Type::maximum;
        }
    }

    for (ttk::SimplexId i = 0; i < nsp; i++) {
        if (const auto [it, inserted] =
                cellIdToPosIndex.emplace(id(msc->separatrixPoints, i), positions.size());
            inserted) {
            positions.push_back({msc->separatrixPoints.points[3 * i + 0],
                                 msc->separatrixPoints.points[3 * i + 1],
                                 msc->separatrixPoints.points[3 * i + 2]});
            types.push_back(Type::unkown);
        }
    }

    std::vector<Sys::SpringIndices> springs;
    springs.reserve(nsc);

    for (ttk::SimplexId i = 0; i < nsc; ++i) {
        IVW_ASSERT(msc->separatrixCells.cells[3 * i + 0] == 2,
                   "Not sure if this is ever anything else...");
        const auto from = msc->separatrixCells.cells[3 * i + 1];
        const auto to = msc->separatrixCells.cells[3 * i + 2];

        const auto fromIndex = cellIdToPosIndex.find(id(msc->separatrixPoints, from));
        const auto toIndex = cellIdToPosIndex.find(id(msc->separatrixPoints, to));

        IVW_ASSERT(fromIndex != cellIdToPosIndex.end(), "Should always find a index");
        IVW_ASSERT(toIndex != cellIdToPosIndex.end(), "Should always find a index");
        IVW_ASSERT(fromIndex->second < positions.size(), "Should always point into positions");
        IVW_ASSERT(toIndex->second < positions.size(), "Should always point into positions");

        springs.emplace_back(fromIndex->second, toIndex->second);

        auto sourceTypeIt = cellIdToType.find(msc->separatrixCells.sourceIds[i]);
        auto destTypeIt = cellIdToType.find(msc->separatrixCells.destinationIds[i]);

        if (sourceTypeIt->second == Type::minimum) {
            if (types[fromIndex->second] == Type::unkown)
                types[fromIndex->second] = Type::minSeperatrix;
            if (types[toIndex->second] == Type::unkown)
                types[toIndex->second] = Type::minSeperatrix;
        } else if (sourceTypeIt->second == Type::maximum) {
            if (types[fromIndex->second] == Type::unkown)
                types[fromIndex->second] = Type::maxSeperatrix;
            if (types[toIndex->second] == Type::unkown)
                types[toIndex->second] = Type::maxSeperatrix;
        } else if (destTypeIt->second == Type::minimum) {
            if (types[fromIndex->second] == Type::unkown)
                types[fromIndex->second] = Type::minSeperatrix;
            if (types[toIndex->second] == Type::unkown)
                types[toIndex->second] = Type::minSeperatrix;
        } else if (destTypeIt->second == Type::maximum) {
            if (types[fromIndex->second] == Type::unkown)
                types[fromIndex->second] = Type::maxSeperatrix;
            if (types[toIndex->second] == Type::unkown)
                types[toIndex->second] = Type::maxSeperatrix;
        }
    }

    Sys sys{sampler_.getData(),     *gradientScale_,  *timestep_,     std::move(positions),
            std::move(springs),     std::move(types), 1.0f,           *springLinearConstant_,
            *springSquareConstant_, *springLength_,   *springDamping_};
    sys.integrate(timesteps_);

    auto res = std::make_shared<topology::MorseSmaleComplexData>(*msc);

    for (ttk::SimplexId i = 0; i < ncp; i++) {
        const auto index = cellIdToPosIndex.find(id(res->criticalPoints, i));
        IVW_ASSERT(index != cellIdToPosIndex.end(), "Should always find a index");
        const auto refinedPos = sys.position(index->second);
        res->criticalPoints.points[3 * i + 0] = refinedPos[0];
        res->criticalPoints.points[3 * i + 1] = refinedPos[1];
        res->criticalPoints.points[3 * i + 2] = refinedPos[2];
    }

    for (ttk::SimplexId i = 0; i < nsp; i++) {
        const auto index = cellIdToPosIndex.find(id(res->separatrixPoints, i));
        IVW_ASSERT(index != cellIdToPosIndex.end(), "Should always find a index");
        const auto refinedPos = sys.position(index->second);
        res->separatrixPoints.points[3 * i + 0] = refinedPos[0];
        res->separatrixPoints.points[3 * i + 1] = refinedPos[1];
        res->separatrixPoints.points[3 * i + 2] = refinedPos[2];
    }

    outport_.setData(res);
}

}  // namespace inviwo
