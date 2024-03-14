/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2024 Inviwo Foundation
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
#include <ttk/core/base/discreteGradient/DiscreteGradient.h>

namespace inviwo {

namespace {

template <size_t N, typename T, typename PBC>
class SeparatrixSpringSystem : public SpringSystem<N, T, SeparatrixSpringSystem<N, T, PBC>, PBC> {
public:
    using Base = SpringSystem<N, T, SeparatrixSpringSystem<N, T, PBC>, PBC>;
    using SpringIndices = typename Base::SpringIndices;
    static constexpr size_t Components = N;
    using Vector = typename Base::Vector;

    SeparatrixSpringSystem(const SpatialSampler<dvec3>& sampler, T gradientScale, T timeStep,
                           std::vector<Vector> positions, std::vector<SpringIndices> springs,
                           std::vector<topology::CellType> aTypes, T nodeMass,
                           T springLinearConstant, T springSquareConstant, T springLength,
                           T springDampning, Vector origin, Vector ext)
        : Base(timeStep, std::move(positions), std::move(springs), origin, ext)
        , sampler{sampler}
        , gradientScale{gradientScale}
        , types{std::move(aTypes)}
        , globalNodeMass{nodeMass}
        , springLinearConstant{springLinearConstant}
        , springSquareConstant{springSquareConstant}
        , globalSpringLength{springLength}
        , globalSpringDampning{springDampning} {}

    bool isLocked(size_t i) const { return types[i] == topology::CellType::saddle; }

    Vector externalForce(size_t i) {

        const float factor = [&]() {
            switch (types[i]) {
                case topology::CellType::minimum:
                    return 1.0f;
                case topology::CellType::maxSaddle:
                    return 1.0f;
                case topology::CellType::maximum:
                    return -1.0f;
                case topology::CellType::minSaddle:
                    return -1.0f;
                case topology::CellType::saddle:
                    return 0.0f;
                case topology::CellType::saddleSaddle:
                    return 0.0f;
                case topology::CellType::unkown:
                    return 0.0f;
                default:
                    return 0.0f;
            }
        }();
        return factor * gradientScale *
               static_cast<Vector>(sampler.sample(this->positions_[i], CoordinateSpace::Model));
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

    const SpatialSampler<dvec3>& sampler;
    T gradientScale;
    std::vector<topology::CellType> types;
    T globalNodeMass{1};
    T springLinearConstant{1};
    T springSquareConstant{1};
    T globalSpringLength{1};
    T globalSpringDampning{1};
};

struct SpringSettings {
    size_t timesteps;
    float timestep;
    float length;
    float linearConstant;
    float squareConstant;
    float damping;
    float gradientScale;
};

template <size_t SelectionSize, typename T, size_t InputSize>
constexpr auto permutations(const std::array<T, InputSize>& values) {
    const auto power = [](auto base, auto pow) {
        auto res = base;
        if (pow == 0) return decltype(base){0};
        for (size_t i = 1; i < pow; ++i) res *= base;
        return res;
    };

    std::array<std::array<T, SelectionSize>, power(InputSize, SelectionSize)> res{};
    std::array<size_t, SelectionSize> index{0};

    for (size_t n = 0; n < res.size(); ++n) {
        for (size_t s = 0; s < SelectionSize; ++s) {
            res[n][s] = values[index[s]];
        }

        size_t i = 0;
        while (i < index.size() && ++index[i] >= InputSize) {
            index[i] = 0;
            ++i;
        }
    }

    return res;
}

template <bool PBC>
std::shared_ptr<Mesh> refine(const topology::MorseSmaleComplexData& msc,
                             const TopologyColorsProperty& colorProp,
                             const TopologyFilterProperty& filterProp, float sphereRadius,
                             float lineThickness, bool fillPBC,
                             const SpatialSampler<dvec3>& sampler,
                             SpringSettings springSettings) {
    using Sys = SeparatrixSpringSystem<3, float, std::integer_sequence<bool, PBC, PBC, PBC>>;

    const auto& cp = msc.criticalPoints;
    const auto& sp = msc.separatrixPoints;
    const auto& sc = msc.separatrixCells;

    const auto ncp = cp.numberOfPoints;
    const auto nsc = sc.numberOfCells;
    const auto dimensionality = msc.triangulation->getTriangulation().getDimensionality();

    const auto ext = msc.triangulation->getGridExtent();
    const auto origin = msc.triangulation->getGridOrigin();

    std::vector<vec3> positions;
    std::vector<topology::CellType> types;
    std::vector<typename Sys::SpringIndices> springs;

    const auto id = [](const auto& item, auto i) {
        return std::make_tuple(item.cellDimensions[i], item.cellIds[i]);
    };

    std::unordered_map<std::tuple<char, ttk::SimplexId>, size_t> cellIdToPosIndex;
    for (ttk::SimplexId i = 0; i < ncp; i++) {
        positions.emplace_back(cp.points[3 * i + 0], cp.points[3 * i + 1], cp.points[3 * i + 2]);
        types.push_back(topology::extremaDimToType(dimensionality, cp.cellDimensions[i]));
        cellIdToPosIndex[id(cp, i)] = positions.size() - 1;
    }

    std::vector<std::pair<ttk::SimplexId, ttk::SimplexId>> seperatixBeginEnd;
    for (ttk::SimplexId i = 1, begin = 0; i < nsc; ++i) {
        if (sc.separatrixIds[i - 1] != sc.separatrixIds[i]) {
            seperatixBeginEnd.emplace_back(begin, i);
            begin = i;
        }
    }

    seperatixBeginEnd.emplace_back(seperatixBeginEnd.back().second, sc.separatrixIds.size());

    for (auto& sep : seperatixBeginEnd) {
        const auto src = sc.cells[3 * sep.first + 1];
        const auto srcCPIndex = cellIdToPosIndex.find(id(sp, src));
        IVW_ASSERT(srcCPIndex != cellIdToPosIndex.end(), "Should always find a CP index");

        ttk::SimplexId srcPosIndex = srcCPIndex->second;

        for (ttk::SimplexId i = sep.first; i < sep.second - 1; ++i) {
            const auto dstInd = sc.cells[3 * i + 2];

            positions.emplace_back(sp.points[3 * dstInd + 0], sp.points[3 * dstInd + 1],
                                   sp.points[3 * dstInd + 2]);

            types.push_back(topology::seperatrixTypeToType(dimensionality, sc.types[i]));
            springs.emplace_back(srcPosIndex, positions.size() - 1);
            srcPosIndex = positions.size() - 1;
        }

        const auto dst = sc.cells[3 * (sep.second - 1) + 2];
        const auto dstCPIndex = cellIdToPosIndex.find(id(sp, dst));
        IVW_ASSERT(dstCPIndex != cellIdToPosIndex.end(), "Should always find a CP index");

        springs.emplace_back(srcPosIndex, dstCPIndex->second);
    }

    Sys sys{sampler,
            springSettings.gradientScale,
            springSettings.timestep,
            std::move(positions),
            std::move(springs),
            std::move(types),
            1.0f,
            springSettings.linearConstant,
            springSettings.squareConstant,
            springSettings.length,
            springSettings.damping,
            origin,
            ext};
    sys.integrate(springSettings.timesteps);

    std::vector<vec3> vertices;
    std::vector<vec4> colors;
    std::vector<float> radii;

    std::vector<uint32_t> cpIndices;
    std::vector<uint32_t> sepIndices;

    const auto addVertex = [&](const vec3& pos, const vec4& color, float radius) {
        vertices.push_back(pos);
        colors.push_back(color);
        radii.push_back(radius);
        return vertices.size() - 1;
    };

    for (ttk::SimplexId i = 0; i < ncp; i++) {
        if (!filterProp.showExtrema(dimensionality, cp.cellDimensions[i])) continue;
        const auto pos = sys.position(i);
        const auto color = colorProp.getColor(dimensionality, cp.cellDimensions[i]);

        const auto add = [&](const vec3 pos) {
            cpIndices.emplace_back(addVertex(pos, color, sphereRadius));
        };

        if constexpr (PBC) {
            if (fillPBC) {
                constexpr auto values = std::array<float, 3>{{-1.0f, 0.0f, 1.0f}};
                constexpr auto offsets = permutations<3>(values);
                const auto margin = 0.025f * ext;

                for (const auto& offset : offsets) {
                    const auto offsetPos = pos + glm::make_vec3(offset.data()) * ext;
                    if (glm::all(glm::greaterThanEqual(offsetPos, origin - margin) &&
                                 glm::lessThan(offsetPos, origin + ext + margin))) {

                        add(offsetPos);
                    }
                }
            } else {
                add(pos);
            }
        } else {
            add(pos);
        }
    }

    for (const auto& spring : sys.getSprings()) {
        if (!filterProp.showSeperatrix(sys.types[spring.first]) ||
            !filterProp.showSeperatrix(sys.types[spring.second]))
            continue;

        const auto add = [&](const vec3& pos1, const vec3& pos2) {
            sepIndices.emplace_back(addVertex(pos1, *colorProp.arc_, lineThickness));
            sepIndices.emplace_back(addVertex(pos2, *colorProp.arc_, lineThickness));
        };

        const auto& pos1 = sys.position(spring.first);
        const auto& pos2 = sys.position(spring.second);

        if constexpr (PBC) {
            const auto insertEdge = [&](vec3 pos1, vec3 pos2) {
                if (fillPBC) {
                    constexpr auto values = std::array<float, 3>{{-1.0f, 0.0f, 1.0f}};
                    constexpr auto offsets = permutations<3>(values);
                    const vec3 margin{0.025f * ext};

                    for (const auto& offset : offsets) {
                        const auto o = glm::make_vec3(offset.data());
                        const auto offsetPos1 = pos1 + o * ext;
                        const auto offsetPos2 = pos2 + o * ext;

                        if (glm::all(glm::greaterThanEqual(offsetPos1, origin - margin) &&
                                     glm::lessThan(offsetPos1, origin + ext + margin)) ||
                            glm::all(glm::greaterThanEqual(offsetPos2, origin - margin) &&
                                     glm::lessThan(offsetPos2, origin + ext + margin))) {
                            add(offsetPos1, offsetPos2);
                        }
                    }
                } else {
                    add(pos1, pos2);
                }
            };

            if (glm::any(glm::greaterThan(glm::abs(pos1 - pos2), 0.5f * ext))) {
                const auto offset = vec3{glm::lessThan(pos1 - pos2, -0.5f * ext)} * ext -
                                    vec3{glm::greaterThan(pos1 - pos2, 0.5f * ext)} * ext;
                insertEdge(pos1 + offset, pos2);
            } else {
                insertEdge(pos1, pos2);
            }

        } else {
            add(pos1, pos2);
        }
    }

    auto mesh = std::make_shared<Mesh>(DrawType::Points, ConnectivityType::None);
    mesh->addBuffer(BufferType::PositionAttrib, util::makeBuffer(std::move(vertices)));
    mesh->addBuffer(BufferType::ColorAttrib, util::makeBuffer(std::move(colors)));
    mesh->addBuffer(BufferType::RadiiAttrib, util::makeBuffer(std::move(radii)));
    // mesh->addBuffer(BufferType::PickingAttrib, util::makeBuffer(std::move(picking)));

    mesh->addIndices(Mesh::MeshInfo(DrawType::Points, ConnectivityType::None),
                     util::makeIndexBuffer(std::move(cpIndices)));
    mesh->addIndices(Mesh::MeshInfo(DrawType::Lines, ConnectivityType::None),
                     util::makeIndexBuffer(std::move(sepIndices)));

    // vertex positions are already transformed
    mesh->setModelMatrix(mat4(1.0f));
    mesh->setWorldMatrix(msc.triangulation->getWorldMatrix());
    mesh->copyMetaDataFrom(*msc.triangulation);

    return mesh;
}

}  // namespace

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

    , colors_("colors", "Colors")
    , filters_("filters", "Filters")
    , sphereRadius_("sphereRadius", "Extrema Radius", 0.05f, 0.0f, 10.0f)
    , lineThickness_("lineThickness", "Seperatrix Thickness", 0.025f, 0.0f, 10.0f)
    , fillPBC_{"fillPBC", "Add repeated item on boundaries", true}

    , springSys_("springSys", "Spring System")
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

    springSys_.addProperties(timesteps_, timestep_, springLength_, springLinearConstant_,
                             springSquareConstant_, springDamping_, gradientScale_);

    addProperties(colors_, sphereRadius_, lineThickness_, fillPBC_, filters_, springSys_);
}

void SeparatrixRefiner::process() {
    auto msc = inport_.getData();

    SpringSettings settings{*timesteps_,
                            *timestep_,
                            *springLength_,
                            *springLinearConstant_,
                            *springSquareConstant_,
                            *springDamping_,
                            *gradientScale_};

    if (msc->triangulation->getTriangulation().usesPeriodicBoundaryConditions()) {
        outport_.setData(refine<true>(*msc, colors_, filters_, *sphereRadius_, *lineThickness_,
                                      *fillPBC_, *sampler_.getData(), settings));
    } else {
        outport_.setData(refine<false>(*msc, colors_, filters_, *sphereRadius_, *lineThickness_,
                                       *fillPBC_, *sampler_.getData(), settings));
    }
}

}  // namespace inviwo
