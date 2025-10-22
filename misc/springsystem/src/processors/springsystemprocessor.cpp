/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2025 Inviwo Foundation
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

#include <inviwo/springsystem/processors/springsystemprocessor.h>

#include <inviwo/core/interaction/events/pickingevent.h>
#include <inviwo/core/datastructures/geometry/typedmesh.h>
#include <inviwo/core/util/zip.h>

#include <inviwo/springsystem/datastructures/springsystem.h>
#include <inviwo/springsystem/utils/springsystemutils.h>

#include <chrono>

namespace inviwo {

const ProcessorInfo SpringSystemProcessor::processorInfo_{
    "org.inviwo.SpringSystemExampleProcessor",  // Class identifier
    "Spring System Example",                    // Display name
    "Spring System",                            // Category
    CodeState::Stable,                          // Code state
    Tags::CPU,                                  // Tags
    "A example processor demonstrating the use of the spring system"_help};

const ProcessorInfo& SpringSystemProcessor::getProcessorInfo() const { return processorInfo_; }

SpringSystemProcessor::SpringSystemProcessor()
    : Processor()
    , nodeOutport_("nodes")
    , springOutport_("springs")
    , springLayout_("springLayout", "Spring Layout",
                    {{"line", "Line", 1},
                     {"grid", "Grid", 2},
                     {"gridDiag", "Grid with Diagonals", 3},
                     {"hexGrid", "Hexagonal Grid", 4}})
    , nodeCount_("nodeCount", "Number of Nodes", 10, 1, 100)
    , nodeSpacing_("nodeSpacing", "Node Spacing", 0.15f, 0.0f, 10.0f)
    , dampingCoeff_("dampingCoeff", "Damping Coefficient", 0.05f, 0.0f, 1.0f)
    , nodeMass_("nodeMass", "Mass of Node [kg]", 0.01f, 0.0f, 1.0f, 0.005f)
    , springConst_("springConst", "Spring Elasticity k [N/m]", 20.0f, 0.1f, 100.0f)
    , springRestLength_("springRestLength", "Spring Length [m]", 0.1f, 0.0f, 10.0f)
    , deltaT_("deltaT", "Delta T [s]", 0.01f, 0.0001f, 1.0f)
    , iterationsPerStep_("iterationsPerStep", "Iterations Per Step", 1, 1, 100, 1,
                         InvalidationLevel::Valid)
    , externalForce_("externalForce", "Ext. Force F [N]", dvec2(0.0f, -9.81f), dvec2(-10.0f),
                     dvec2(10.0f))
    , scaleFactor_("scaleFactor", "Scale Factor for Mesh coloring", 1.0f, 0.01f, 1000.0f)
    , advanceButton_("advanceBtn", "Advance")
    , resetButton_("resetBtn", "Reset")
    , startButton_("startBtn", "Start Solver")
    , stopButton_("stopBtn", "Stop Solver")
    , logStatusButton_("logStatusBtn", "Log Status")
    , camera_("camera", "Camera")
    , advance_(false)
    , meshDirty_(false)
    , updateTimer_(std::chrono::milliseconds(20),
                   [this]() {
                       advance_ = true;
                       invalidate(InvalidationLevel::InvalidOutput);
                   })
    , springSystem_{getSystem()}
    , nodePicking_{this, springSystem_.getNumberOfNodes(),
                   [this](PickingEvent* p) { handlePicking(p); }} {

    addPort(nodeOutport_);
    addPort(springOutport_);

    addProperty(springLayout_);
    addProperty(nodeCount_);
    addProperty(nodeSpacing_);
    addProperty(dampingCoeff_);
    addProperty(nodeMass_);
    addProperty(springConst_);
    addProperty(springRestLength_);
    addProperty(deltaT_);
    addProperty(iterationsPerStep_);
    addProperty(externalForce_);
    addProperty(scaleFactor_);

    addProperty(advanceButton_);
    addProperty(resetButton_);
    addProperty(startButton_);
    addProperty(stopButton_);
    addProperty(logStatusButton_);

    addProperty(camera_);

    advanceButton_.onChange([&]() { advance_ = true; });
    resetButton_.onChange([&]() { springSystem_ = getSystem(); });
    logStatusButton_.onChange([&]() { LogInfo(springSystem_); });

    startButton_.onChange([&]() { updateTimer_.start(); });
    stopButton_.onChange([&]() { updateTimer_.stop(); });

    camera_.setInvalidationLevel(InvalidationLevel::Valid);
    camera_.setCollapsed(true);
}

namespace {

template <typename P, typename T, typename Arg>
void sync(T& obj, const P& property, void (T::*setter)(Arg)) {
    if (property.isModified()) {
        (obj.*setter)(property.get());
    }
}
template <typename P, typename T, typename V>
void sync(T& obj, const P& property, V T::* member) {
    if (property.isModified()) {
        obj.*member = property.get();
    }
}

template <typename T>
void syncer(T&) {}
template <typename T, typename Pair, typename... Args>
void syncer(T& obj, Pair&& item, Args&&... args) {
    sync(obj, item.first, item.second);
    syncer(obj, std::forward<Args>(args)...);
}

}  // namespace

void SpringSystemProcessor::process() {
    using Sys = GravitySpringSystem<2, double>;

    if (springLayout_.isModified() || nodeCount_.isModified() || nodeSpacing_.isModified()) {
        springSystem_ = getSystem();
        nodePicking_.resize(springSystem_.getNumberOfNodes());
    }

    syncer(springSystem_, std::make_pair(dampingCoeff_, &Sys::globalSpringDampning),
           std::make_pair(nodeMass_, &Sys::globalNodeMass),
           std::make_pair(springConst_, &Sys::globalSpringConstant),
           std::make_pair(springRestLength_, &Sys::globalSpringLength),
           std::make_pair(externalForce_, &Sys::globalExternalForce));

    if (advance_) {
        advance_ = false;
        springSystem_.integrate(iterationsPerStep_);
        meshDirty_ = true;
    }

    if (meshDirty_ || scaleFactor_.isModified()) {
        updateMesh();
    }
}

void SpringSystemProcessor::deserialize(Deserializer& d) {
    Processor::deserialize(d);
    updateSystemFromProperties();
}

void SpringSystemProcessor::updateMesh() {
    using NodeMesh = TypedMesh<buffertraits::PositionsBuffer, buffertraits::PickingBuffer,
                               buffertraits::ScalarMetaBuffer>;
    using SpringMesh = TypedMesh<buffertraits::PositionsBuffer>;

    const auto& positions = springSystem_.getPositions();
    const auto& forces = springSystem_.getForces();
    const auto& springs = springSystem_.getSprings();

    auto nodeMesh = std::make_shared<NodeMesh>();
    nodeMesh->reserveSizeInVertexBuffer(positions.size());
    auto nodeIndices = nodeMesh->addIndexBuffer(DrawType::Points, ConnectivityType::None);
    nodeIndices->reserve(positions.size());

    auto springMesh = std::make_shared<SpringMesh>();
    springMesh->reserveSizeInVertexBuffer(springs.size() * 2);
    auto springIndices = springMesh->addIndexBuffer(DrawType::Lines, ConnectivityType::None);
    springIndices->reserve(springs.size() * 2);

    for (auto&& item : util::enumerate(positions, forces)) {
        springMesh->addVertex(vec3(item.second(), 0.0f));
        const auto pick = static_cast<uint32_t>(nodePicking_.getPickingId(item.first()));
        const auto meta = static_cast<float>(glm::length(item.third()) / scaleFactor_.get());
        const auto ind = nodeMesh->addVertex(vec3(item.second(), 0.0f), pick, meta);
        nodeIndices->add(ind);
    }
    for (const auto& spring : springs) {
        springIndices->add(static_cast<uint32_t>(spring.first));
        springIndices->add(static_cast<uint32_t>(spring.second));
    }

    nodeOutport_.setData(nodeMesh);
    springOutport_.setData(springMesh);
}

void SpringSystemProcessor::updateSystemFromProperties() { springSystem_ = getSystem(); }

GravitySpringSystem<2, double> SpringSystemProcessor::getSystem() {
    meshDirty_ = true;

    auto grid = [&]() {
        switch (springLayout_.get()) {
            default:
                [[fallthrough]];
            case 1: {
                return springmass::createLineGrid<2, double>(nodeCount_, nodeSpacing_);
            }
            case 2: {
                const double offset = (std::ptrdiff_t{nodeCount_} - 1) * nodeSpacing_ * 0.5;
                const dvec2 origin(-offset, offset);
                return springmass::createRectangularGrid<2, double>(size2_t(nodeCount_), origin,
                                                                    dvec2(nodeSpacing_));
            }
            case 3: {
                const double offset = (nodeCount_ - 1) * nodeSpacing_ * 0.5;
                const dvec2 origin(-offset, offset);
                return springmass::createDiagonalGridDiagonal<2, double>(
                    size2_t(nodeCount_), origin, dvec2(nodeSpacing_));
            }
            case 4: {
                const double offset = (nodeCount_ - 1) * nodeSpacing_ * 0.5;
                const dvec2 origin(-offset, offset);
                return springmass::createHexagonalGrid<2, double>(size2_t(nodeCount_), origin,
                                                                  dvec2(nodeSpacing_));
            }
        }
    }();

    return GravitySpringSystem<2, double>{deltaT_,
                                          std::move(grid.positions),
                                          std::move(grid.springs),
                                          std::move(grid.locked),
                                          externalForce_,
                                          nodeMass_,
                                          springConst_,
                                          springRestLength_,
                                          dampingCoeff_};
}

void SpringSystemProcessor::handlePicking(PickingEvent* p) {
    if (p->getPressState() == PickingPressState::Move &&
        p->getPressItems().count(PickingPressItem::Primary)) {
        const auto worldDelta = p->getWorldSpaceDeltaAtPressDepth(camera_);
        const auto ind = p->getPickedId();

        springSystem_.position(ind) += dvec2{worldDelta};
        p->markAsUsed();
        meshDirty_ = true;
        invalidate(InvalidationLevel::InvalidOutput);
    }
    if (p->getPressState() == PickingPressState::Press &&
        p->getPressItem() == PickingPressItem::Primary) {
        springSystem_.lockedNodes[p->getPickedId()] = true;
    }
    if (p->getPressState() == PickingPressState::Release &&
        p->getPressItem() == PickingPressItem::Primary) {
        springSystem_.lockedNodes[p->getPickedId()] = false;
    }

    if (p->getHoverState() != PickingHoverState::Exit) {
        p->setToolTip(toString(p->getPickedId()) + " : " +
                      toString(springSystem_.position(p->getPickedId())));
    } else {
        p->setToolTip("");
    }
}

}  // namespace inviwo
