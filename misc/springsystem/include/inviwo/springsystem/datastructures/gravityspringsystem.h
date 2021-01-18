/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018-2021 Inviwo Foundation
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

#include <inviwo/springsystem/springsystemmoduledefine.h>
#include <inviwo/core/common/inviwo.h>

#include <inviwo/springsystem/datastructures/springsystem.h>

namespace inviwo {

/**
 * \brief Spring mass system config using a constant force, e.g. gravitational force.
 * @see SpringMassConfig
 * @see SpringSystem
 */
template <size_t N, typename ComponentType = double>
class GravitySpringSystem
    : public SpringSystem<N, ComponentType, GravitySpringSystem<N, ComponentType>> {
public:
    using Base = SpringSystem<N, ComponentType, GravitySpringSystem<N, ComponentType>>;
    using SpringIndices = typename Base::SpringIndices;
    static constexpr size_t Components = N;
    using Vector = glm::vec<Components, ComponentType>;

    GravitySpringSystem(ComponentType timeStep, std::vector<Vector> positions,
                        std::vector<SpringIndices> springs, std::vector<bool> aLockedNodes,
                        Vector externalForce, ComponentType nodeMass, ComponentType springConstant,
                        ComponentType springLength, ComponentType springDampning)
        : Base(timeStep, std::move(positions), std::move(springs))
        , lockedNodes{std::move(aLockedNodes)}
        , globalExternalForce{externalForce}
        , globalNodeMass{nodeMass}
        , globalSpringConstant{springConstant}
        , globalSpringLength{springLength}
        , globalSpringDampning{springDampning} {}

    bool isLocked(size_t i) const { return lockedNodes[i]; }
    Vector externalForce(size_t) { return globalExternalForce; }
    ComponentType nodeMass(size_t) { return globalNodeMass; }

    ComponentType springConstant(size_t) const { return globalSpringConstant; }
    ComponentType springLength(size_t) const { return globalSpringLength; }
    ComponentType springDampning(size_t) const { return globalSpringDampning; }

    void constrainPosition(size_t, Vector&) const {}
    void constrainVelocity(size_t, Vector&) const {}

    std::vector<bool> lockedNodes;
    Vector globalExternalForce = Vector{0};
    ComponentType globalNodeMass{1};
    ComponentType globalSpringConstant{1};
    ComponentType globalSpringLength{1};
    ComponentType globalSpringDampning{1};
};

}  // namespace inviwo
