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
#include <inviwo/core/util/zip.h>
#include <glm/gtc/epsilon.hpp>

#include <vector>
#include <sstream>
#include <iomanip>
#include <utility>
#include <algorithm>

#if __has_include(<execution>)
#include <execution>
#endif

namespace inviwo {

namespace util {
template <class T, T... Ints>
constexpr T get(std::integer_sequence<T, Ints...>, std::size_t i) {
    constexpr T arr[] = {Ints...};
    return arr[i];
}

template <class Seq>
constexpr auto get(size_t i) {
    return get(Seq{}, i);
}

namespace detail {
template <auto val, typename T>
struct fill_integer_sequence;

template <auto val, size_t... Inds>
struct fill_integer_sequence<val, std::index_sequence<Inds...>> {
    template <size_t N, auto T>
    struct map {
        static constexpr decltype(T) value = T;
    };

    using type = std::integer_sequence<decltype(val), map<Inds, val>::value...>;
};
}  // namespace detail

template <size_t N, auto val>
using fill_integer_sequence =
    typename detail::fill_integer_sequence<val, std::make_integer_sequence<std::size_t, N>>::type;

template <typename I1, typename I2, typename F>
auto for_each_parallel(I1&& begin, I2&& end, F&& fun) {

#if __has_include(<execution>)
    return std::for_each(std::execution::par_unseq, std::forward<I1>(begin), std::forward<I2>(end),
                         std::forward<F>(fun));
#else
    return std::for_each(std::forward<I1>(begin), std::forward<I2>(end), std::forward<F>(fun));
#endif
}

}  // namespace util

/**
 * \class SpringSystem
 *
 * \brief this class is representing a spring mass system including a solver using either the Verlet
 * integration scheme.
 *
 * @see SpringMassConfig
 * @see ConstGravityConfig
 */
template <size_t Components, typename ComponentType, typename Derived,
          typename PBC = util::fill_integer_sequence<Components, false>>
class SpringSystem {
public:
    using SpringIndices = std::pair<std::size_t, std::size_t>;
    using Vector = glm::vec<Components, ComponentType>;

    SpringSystem(ComponentType timeStep, std::vector<Vector> positions,
                 std::vector<SpringIndices> springs, Vector origin = Vector{1},
                 Vector extent = Vector{1});
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

    static constexpr bool hasPBC(size_t dim) noexcept { return util::get<PBC>(dim); }

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
    Vector origin_;
    Vector extent_;
};

template <size_t Components, typename ComponentType, typename Derived, typename PBC>
SpringSystem<Components, ComponentType, Derived, PBC>::SpringSystem(
    ComponentType timeStep, std::vector<Vector> positions, std::vector<SpringIndices> springs,
    Vector origin, Vector extent)
    : timeStep_{timeStep}
    , positions_{std::move(positions)}
    , velocities_(positions_.size(), Vector{0})
    , forces_(positions_.size(), Vector{0})
    , springs_{std::move(springs)}
    , origin_{origin}
    , extent_{extent} {}

template <size_t Components, typename ComponentType, typename Derived, typename PBC>
void SpringSystem<Components, ComponentType, Derived, PBC>::integrate(size_t steps) {
    for (size_t i = 0; i < steps; ++i) {
        verletIntegration();
    }
}

template <size_t Components, typename ComponentType, typename Derived, typename PBC>
void SpringSystem<Components, ComponentType, Derived, PBC>::verletIntegration() {
    const std::size_t numNodes = positions_.size();

    // Verlet integration see <https://en.wikipedia.org/wiki/Verlet_integration>

    // 1) calculate pos(t + timeStep_) and first part of v(t + timeStep_) based on current forces

    const auto seq = util::make_sequence(size_t{0}, numNodes, size_t{1});

    util::for_each_parallel(seq.begin(), seq.end(), [&](size_t i) {
        // for (std::size_t i = 0; i < numNodes; ++i) {
        if (derived().isLocked(i)) return;

        const auto a = forces_[i] / derived().nodeMass(i);  // acceleration
        const auto deltaV = ComponentType{0.5} * a * timeStep_;
        const auto posOffset = velocities_[i] * timeStep_ + deltaV * timeStep_;

        auto newPos = positions_[i] + posOffset;

        for (size_t d = 0; d < Components; ++d) {
            if (hasPBC(d)) {
                if (newPos[d] < origin_[d])
                    newPos[d] += extent_[d];
                else if (newPos[d] >= origin_[d] + extent_[d])
                    newPos[d] -= extent_[d];
            }
        }

        derived().constrainPosition(i, newPos);
        positions_[i] = newPos;

        auto newVel = velocities_[i] + deltaV;
        derived().constrainVelocity(i, newVel);
        velocities_[i] = newVel;
    });

    // 2) update forces of all nodes by iterating all springs
    derived().updateForces();

    // 3) adding 0.5 * v based on new forces
    util::for_each_parallel(seq.begin(), seq.end(), [&](size_t i) {
        // for (std::size_t i = 0; i < numNodes; ++i) {
        if (derived().isLocked(i)) return;

        const auto a = forces_[i] / derived().nodeMass(i);  // acceleration
        const auto deltaV = ComponentType{0.5} * a * timeStep_;

        auto newVel = velocities_[i] + deltaV;
        derived().constrainVelocity(i, newVel);
        velocities_[i] = newVel;
    });
}

template <size_t Components, typename ComponentType, typename Derived, typename PBC>
ComponentType SpringSystem<Components, ComponentType, Derived, PBC>::forceMagnitude(
    size_t i, ComponentType displacement) const {

    return displacement * derived().springConstant(i);
}

template <size_t Components, typename ComponentType, typename Derived, typename PBC>
void SpringSystem<Components, ComponentType, Derived, PBC>::updateForces() {

    const auto fseq = util::make_sequence(size_t{0}, forces_.size(), size_t{1});
    util::for_each_parallel(fseq.begin(), fseq.end(), [&](size_t i) {
        // for (std::size_t i = 0; i < forces_.size(); ++i) {
        forces_[i] = derived().externalForce(i);
    });

    const auto& positions = getPositions();

    const auto seq = util::make_sequence(size_t{0}, springs_.size(), size_t{1});
    util::for_each_parallel(seq.begin(), seq.end(), [&](size_t i) {
        // for (size_t i = 0; i < springs_.size(); ++i) {
        const auto& spring = springs_[i];

        auto pos1 = positions[spring.first];
        auto pos2 = positions[spring.second];

        for (size_t d = 0; d < Components; ++d) {
            if (hasPBC(d)) {
                pos1[d] += int{pos1[d] - pos2[d] < -0.5f * extent_[d]} * extent_[d];
                pos2[d] += int{pos1[d] - pos2[d] > 0.5f * extent_[d]} * extent_[d];
            }
        }

        auto dir(pos2 - pos1);
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
    });
}

template <size_t Components, typename ComponentType, typename Derived, typename PBC>
auto SpringSystem<Components, ComponentType, Derived, PBC>::getNumberOfNodes() const -> size_t {
    return positions_.size();
}
template <size_t Components, typename ComponentType, typename Derived, typename PBC>
auto SpringSystem<Components, ComponentType, Derived, PBC>::getNumberOfSprings() const -> size_t {
    return springs_.size();
}

template <size_t Components, typename ComponentType, typename Derived, typename PBC>
auto SpringSystem<Components, ComponentType, Derived, PBC>::getTimeStep() const -> ComponentType {
    return timeStep_;
}

template <size_t Components, typename ComponentType, typename Derived, typename PBC>
void SpringSystem<Components, ComponentType, Derived, PBC>::setTimeStep(ComponentType timestep) {
    timeStep_ = timestep;
}

template <size_t Components, typename ComponentType, typename Derived, typename PBC>
auto SpringSystem<Components, ComponentType, Derived, PBC>::getPositions() const
    -> const std::vector<Vector>& {
    return positions_;
}

template <size_t Components, typename ComponentType, typename Derived, typename PBC>
auto SpringSystem<Components, ComponentType, Derived, PBC>::getVelocities() const
    -> const std::vector<Vector>& {
    return velocities_;
}

template <size_t Components, typename ComponentType, typename Derived, typename PBC>
auto SpringSystem<Components, ComponentType, Derived, PBC>::getForces() const
    -> const std::vector<Vector>& {
    return forces_;
}

template <size_t Components, typename ComponentType, typename Derived, typename PBC>
auto SpringSystem<Components, ComponentType, Derived, PBC>::position(size_t i) const
    -> const Vector& {
    return positions_[i];
}
template <size_t Components, typename ComponentType, typename Derived, typename PBC>
auto SpringSystem<Components, ComponentType, Derived, PBC>::velocity(size_t i) const
    -> const Vector& {
    return velocities_[i];
}
template <size_t Components, typename ComponentType, typename Derived, typename PBC>
auto SpringSystem<Components, ComponentType, Derived, PBC>::force(size_t i) const -> const Vector& {
    return forces_[i];
}
template <size_t Components, typename ComponentType, typename Derived, typename PBC>
auto SpringSystem<Components, ComponentType, Derived, PBC>::position(size_t i) -> Vector& {
    return positions_[i];
}
template <size_t Components, typename ComponentType, typename Derived, typename PBC>
auto SpringSystem<Components, ComponentType, Derived, PBC>::velocity(size_t i) -> Vector& {
    return velocities_[i];
}
template <size_t Components, typename ComponentType, typename Derived, typename PBC>
auto SpringSystem<Components, ComponentType, Derived, PBC>::force(size_t i) -> Vector& {
    return forces_[i];
}

template <size_t Components, typename ComponentType, typename Derived, typename PBC>
auto SpringSystem<Components, ComponentType, Derived, PBC>::getSprings() const
    -> const std::vector<SpringIndices>& {
    return springs_;
}

template <size_t Components, typename ComponentType, typename Derived, typename PBC>
template <class Elem, class Traits>
void SpringSystem<Components, ComponentType, Derived, PBC>::print(
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

template <class Elem, class Traits, size_t Components, typename ComponentType, typename Derived,
          typename PBC>
std::basic_ostream<Elem, Traits>& operator<<(
    std::basic_ostream<Elem, Traits>& ss,
    const SpringSystem<Components, ComponentType, Derived, PBC>& sms) {
    sms.print(ss);
    return ss;
}

}  // namespace inviwo
