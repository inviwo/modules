/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2019 Inviwo Foundation
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

#include <vector>
#include <glm/gtc/epsilon.hpp>
#include <sstream>
#include <iomanip>

namespace inviwo {

/**
 * \class SpringSystem
 *
 * \brief this class is representing a spring mass system including a solver using either the Verlet
 * integration scheme.
 *
 * @see SpringMassConfig
 * @see ConstGravityConfig
 */
template <size_t Components, typename ComponentType, typename Derived>
class SpringSystem {
public:
    using SpringIndices = std::pair<std::size_t, std::size_t>;
    using Vector = glm::vec<Components, ComponentType>;

    SpringSystem(ComponentType timeStep, std::vector<Vector> positions,
                 std::vector<SpringIndices> springs);
    ~SpringSystem() = default;

    ComponentType getTimeStep() const;
    void setTimeStep(ComponentType timeStep);

    void integrate(size_t steps = 1);

    size_t getNumberOfNodes() const;
    size_t getNumberOfSprings() const;

    const std::vector<Vector>& getPositions() const;
    const std::vector<Vector>& getVelocities() const;
    const std::vector<Vector>& getForces() const;

    const Vector& position(size_t i) const;
    const Vector& velocity(size_t i) const;
    const Vector& force(size_t i) const;

    Vector& position(size_t i);
    Vector& velocity(size_t i);
    Vector& force(size_t i);

    const std::vector<SpringIndices>& getSprings() const;

    template <class Elem, class Traits>
    void print(std::basic_ostream<Elem, Traits>& ss) const;

protected:
    inline Derived& derived() { return *static_cast<Derived*>(this); }
    inline const Derived& derived() const { return *static_cast<const Derived*>(this); }

    void updateForces();
    void verletIntegration();

    ComponentType forceMagnitude(size_t i, ComponentType displacement) const;

    ComponentType timeStep_;
    std::vector<Vector> positions_;
    std::vector<Vector> velocities_;
    std::vector<Vector> forces_;
    std::vector<SpringIndices> springs_;
};

template <size_t Components, typename ComponentType, typename Derived>
SpringSystem<Components, ComponentType, Derived>::SpringSystem(ComponentType timeStep,
                                                               std::vector<Vector> positions,
                                                               std::vector<SpringIndices> springs)
    : timeStep_{timeStep}
    , positions_{std::move(positions)}
    , velocities_(positions_.size(), Vector{0})
    , forces_(positions_.size(), Vector{0})
    , springs_{std::move(springs)} {}

template <size_t Components, typename ComponentType, typename Derived>
void SpringSystem<Components, ComponentType, Derived>::integrate(size_t steps) {
    for (size_t i = 0; i < steps; ++i) {
        verletIntegration();
    }
}

template <size_t Components, typename ComponentType, typename Derived>
void SpringSystem<Components, ComponentType, Derived>::verletIntegration() {
    const std::size_t numNodes = positions_.size();

    // Verlet integration see <https://en.wikipedia.org/wiki/Verlet_integration>

    // 1) calculate pos(t + timeStep_) and first part of v(t + timeStep_) based on current forces
    for (std::size_t i = 0; i < numNodes; ++i) {
        if (derived().isLocked(i)) continue;

        const auto a = forces_[i] / derived().nodeMass(i);  // acceleration
        const auto deltaV = ComponentType{0.5} * a * timeStep_;
        const auto posOffset = velocities_[i] * timeStep_ + deltaV * timeStep_;

        auto newPos = positions_[i] + posOffset;
        derived().constrainPosition(i, newPos);
        positions_[i] = newPos;

        auto newVel = velocities_[i] + deltaV;
        derived().constrainVelocity(i, newVel);
        velocities_[i] = newVel;
    }

    // 2) update forces of all nodes by iterating all springs
    updateForces();

    // 3) adding 0.5 * v based on new forces
    for (std::size_t i = 0; i < numNodes; ++i) {
        if (derived().isLocked(i)) continue;

        const auto a = forces_[i] / derived().nodeMass(i);  // acceleration
        const auto deltaV = ComponentType{0.5} * a * timeStep_;

        auto newVel = velocities_[i] + deltaV;
        derived().constrainVelocity(i, newVel);
        velocities_[i] = newVel;
    }
}

template <size_t Components, typename ComponentType, typename Derived>
ComponentType SpringSystem<Components, ComponentType, Derived>::forceMagnitude(
    size_t i, ComponentType displacement) const {

    return displacement * derived().springConstant(i);
}

template <size_t Components, typename ComponentType, typename Derived>
void SpringSystem<Components, ComponentType, Derived>::updateForces() {
    for (std::size_t i = 0; i < forces_.size(); ++i) {
        forces_[i] = derived().externalForce(i);
    }

    const auto& positions = getPositions();
    for (std::size_t i = 0; i < springs_.size(); ++i) {
        const auto& spring = springs_[i];
        auto dir(positions[spring.second] - positions[spring.first]);
        ComponentType dist = glm::length(dir);
        if (std::abs(dist) < glm::epsilon<ComponentType>()) {
            // positions are identical, choose "random" direction
            dir = Vector(0);
            // dir.x = 1;
        } else {
            dir /= dist;
        }

        const auto displacement = dist - derived().springLength(i);
        const auto force = -derived().forceMagnitude(i, displacement) * dir;
        const auto dampning = derived().springDampning(i);

        // add force to both nodes
        forces_[spring.first] += -force - dampning * velocities_[spring.first];
        forces_[spring.second] += force - dampning * velocities_[spring.second];
    }
}

template <size_t Components, typename ComponentType, typename Derived>
auto SpringSystem<Components, ComponentType, Derived>::getNumberOfNodes() const -> size_t {
    return positions_.size();
}
template <size_t Components, typename ComponentType, typename Derived>
auto SpringSystem<Components, ComponentType, Derived>::getNumberOfSprings() const -> size_t {
    return springs_.size();
}

template <size_t Components, typename ComponentType, typename Derived>
auto SpringSystem<Components, ComponentType, Derived>::getTimeStep() const -> ComponentType {
    return timeStep_;
}

template <size_t Components, typename ComponentType, typename Derived>
void SpringSystem<Components, ComponentType, Derived>::setTimeStep(ComponentType timestep) {
    timeStep_ = timestep;
}

template <size_t Components, typename ComponentType, typename Derived>
auto SpringSystem<Components, ComponentType, Derived>::getPositions() const
    -> const std::vector<Vector>& {
    return positions_;
}

template <size_t Components, typename ComponentType, typename Derived>
auto SpringSystem<Components, ComponentType, Derived>::getVelocities() const
    -> const std::vector<Vector>& {
    return velocities_;
}

template <size_t Components, typename ComponentType, typename Derived>
auto SpringSystem<Components, ComponentType, Derived>::getForces() const
    -> const std::vector<Vector>& {
    return forces_;
}

template <size_t Components, typename ComponentType, typename Derived>
auto SpringSystem<Components, ComponentType, Derived>::position(size_t i) const -> const Vector& {
    return positions_[i];
}
template <size_t Components, typename ComponentType, typename Derived>
auto SpringSystem<Components, ComponentType, Derived>::velocity(size_t i) const -> const Vector& {
    return velocities_[i];
}
template <size_t Components, typename ComponentType, typename Derived>
auto SpringSystem<Components, ComponentType, Derived>::force(size_t i) const -> const Vector& {
    return forces_[i];
}
template <size_t Components, typename ComponentType, typename Derived>
auto SpringSystem<Components, ComponentType, Derived>::position(size_t i) -> Vector& {
    return positions_[i];
}
template <size_t Components, typename ComponentType, typename Derived>
auto SpringSystem<Components, ComponentType, Derived>::velocity(size_t i) -> Vector& {
    return velocities_[i];
}
template <size_t Components, typename ComponentType, typename Derived>
auto SpringSystem<Components, ComponentType, Derived>::force(size_t i) -> Vector& {
    return forces_[i];
}

template <size_t Components, typename ComponentType, typename Derived>
auto SpringSystem<Components, ComponentType, Derived>::getSprings() const
    -> const std::vector<SpringIndices>& {
    return springs_;
}

template <size_t Components, typename ComponentType, typename Derived>
template <class Elem, class Traits>
void SpringSystem<Components, ComponentType, Derived>::print(
    std::basic_ostream<Elem, Traits>& ss) const {
    ss << "Nodes:";
    for (std::size_t i = 0; i < positions_.size(); ++i) {
        ss << "\n"
           << std::setw(6) << (i + 1) << ":  p = " << positions_[i] << ", v = " << velocities_[i]
           << ", F = " << forces_[i] << ", fixed = " << std::boolalpha << derived().isLocked(i);
    }
    ss << "\nSprings:";
    for (std::size_t i = 0; i < springs_.size(); ++i) {
        const auto& spring = springs_[i];
        auto dir(positions_[spring.second] - positions_[spring.first]);
        auto dist = glm::length(dir);
        dir /= dist;
        auto displacement = dir * (dist - derived().springLength(i));

        ss << "\n"
           << std::setw(6) << (i + 1) << ":  l0 = " << derived().springLength(i) << ", l = " << dist
           << ", s = " << displacement;
    }
    ss << "\n-----------------------------";
}

template <class Elem, class Traits, size_t Components, typename ComponentType, typename Derived>
std::basic_ostream<Elem, Traits>& operator<<(
    std::basic_ostream<Elem, Traits>& ss,
    const SpringSystem<Components, ComponentType, Derived>& sms) {
    sms.print(ss);
    return ss;
}

}  // namespace inviwo
