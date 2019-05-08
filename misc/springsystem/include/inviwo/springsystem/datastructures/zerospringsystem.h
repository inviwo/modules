/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018-2019 Inviwo Foundation
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
 * \brief this is a template mix-in class, which is responsible for
 * initializing the forces prior to each integration step of the Spring Mass System.
 * This instance will set all initial forces to zero.
 * @see SpringSystem
 */
template <size_t N, typename ComponentType = double>
class ZeroSpringSystem : public SpringSystem<N, ComponentType, ZeroSpringSystem<N, ComponentType>> {
public:
    using Base = SpringSystem<N, ComponentType, ZeroSpringSystem<N, ComponentType>>;
    using SpringIndices = typename Base::SpringIndices;
    static constexpr size_t Components = N;
    using Vector = glm::vec<Components, ComponentType>;

    ZeroSpringSystem(ComponentType timeStep, std::vector<Vector> positions,
                     std::vector<SpringIndices> springs, std::vector<bool> aLockedNodes,
                     ComponentType nodeMass, ComponentType springConstant,
                     ComponentType springLength, ComponentType springDampning)
        : Base(timeStep, std::move(positions), std::move(springs))
        , lockedNodes{std::move(aLockedNodes)}
        , globalNodeMass{nodeMass}
        , globalSpringConstant{springConstant}
        , globalSpringLength{springLength}
        , globalSpringDampning{springDampning} {}

    bool isLocked(size_t i) const { return lockedNodes[i]; }
    Vector externalForce(size_t i) { return Vector{0}; }
    ComponentType nodeMass(size_t i) { return globalNodeMass; }

    ComponentType springConstant(size_t i) const { return globalSpringConstant; }
    ComponentType springLength(size_t i) const { return globalSpringLength; }
    ComponentType springDampning(size_t i) const { return globalSpringDampning; }

    void constrainPosition(size_t, Vector&) const {}
    void constrainVelocity(size_t, Vector&) const {}

    std::vector<bool> lockedNodes;
    ComponentType globalNodeMass{1};
    ComponentType globalSpringConstant{1};
    ComponentType globalSpringLength{1};
    ComponentType globalSpringDampning{1};
};

}  // namespace inviwo
