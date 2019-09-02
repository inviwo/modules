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

#include <inviwo/topologytoolkit/processors/topologicalsimplification.h>
#include <inviwo/topologytoolkit/utils/ttkutils.h>
#include <inviwo/core/datastructures/buffer/bufferram.h>
#include <inviwo/core/datastructures/buffer/buffer.h>
#include <inviwo/core/util/zip.h>
#include <inviwo/core/util/clock.h>

#include <warn/push>
#include <warn/ignore/all>
#include <ttk/core/base/topologicalSimplification/TopologicalSimplification.h>
#include <warn/pop>
#include <inviwo/core/util/formats.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TopologicalSimplification::processorInfo_{
    "org.inviwo.ttk.TopologicalSimplification",  // Class identifier
    "Topological Simplification",                // Display name
    "Topology",                                  // Category
    CodeState::Experimental,                     // Code state
    "CPU, Topology, TTK, Simplification",        // Tags
};
const ProcessorInfo TopologicalSimplification::getProcessorInfo() const { return processorInfo_; }

TopologicalSimplification::TopologicalSimplification()
    : Processor()
    , inport_("triangulation")
    , persistenceInport_("persistence")
    , outport_("outport")
    , threshold_("threshold", "Threshold", 0.0f, 0.0f, 1000.0f)
    , invert_("invert", "Invert", false) {

    addPort(inport_);
    addPort(persistenceInport_);
    addPort(outport_);

    addProperty(threshold_);
    addProperty(invert_);

    persistenceInport_.onChange([this]() {
        if (persistenceInport_.hasData()) {
            // Adjust max value to highest persistence value
            auto endIt = std::end(*persistenceInport_.getData());
            auto maxIt = std::max_element(
                std::begin(*persistenceInport_.getData()), endIt,
                [](const auto &a, const auto &b) { return std::get<4>(a) < std::get<4>(b); });
            if (maxIt != endIt) {
                threshold_.setMaxValue(std::get<4>(*maxIt));
            }
        }
    });
}

void TopologicalSimplification::process() {
    if (util::is_future_ready(newTriangulation_)) {
        try {
            auto triangulation = newTriangulation_.get();
            outport_.setData(triangulation);
            hasNewData_ = false;
            getActivityIndicator().setActive(false);
        } catch (Exception &) {
            // Need to reset the future, VS bug:
            // http://stackoverflow.com/questions/33899615/stdfuture-still-valid-after-calling-get-which-throws-an-exception
            newTriangulation_ = {};
            outport_.setData(nullptr);
            hasNewData_ = false;
            throw;
        }
    } else if (!newTriangulation_.valid()) {  // We are not waiting for a calculation
        if (inport_.isChanged() || persistenceInport_.isChanged() || threshold_.isModified() ||
            invert_.isModified() || simplificationDirty_) {
            getActivityIndicator().setActive(true);
            updateOutport();
        }
    }
}

void TopologicalSimplification::updateOutport() {
    auto done = [this]() {
        dispatchFront([this]() {
            simplificationDirty_ = false;
            hasNewData_ = true;
            invalidate(InvalidationLevel::InvalidOutput);
        });
    };

    // Save input and properties needed to calculate ttk contour tree to local variables
    const auto inportData = inport_.getData();
    const auto persistenceDiagram = persistenceInport_.getData();

    auto compute = [this, inportData, persistenceDiagram, done, threshold = threshold_.get(),
                    invert =
                        invert_.get()]() -> std::shared_ptr<const topology::TriangulationData> {
        ScopedClockCPU clock{"TopologicalSimplification",
                             "Topological simplification for " + std::to_string(threshold_.get()),
                             std::chrono::milliseconds(500), LogLevel::Info};

        auto triangulation =
            inportData->getScalarValues()
                ->getEditableRepresentation<BufferRAM>()
                ->dispatch<std::shared_ptr<topology::TriangulationData>,
                           dispatching::filter::Scalars>([inportData,
                                                          &diagramData = *persistenceDiagram,
                                                          threshold, invert](const auto buffer) {
                    using ValueType = util::PrecisionValueType<decltype(buffer)>;

                    // select most/least persistent critical point pairs
                    std::vector<int> authorizedCriticalPoints;
                    for (int i = 0; i < (int)diagramData.size(); i++) {
                        auto persistence = std::get<4>(diagramData[i]);
                        if ((persistence >= threshold) != invert) {
                            authorizedCriticalPoints.push_back(std::get<0>(diagramData[i]));
                            authorizedCriticalPoints.push_back(std::get<2>(diagramData[i]));
                        }
                    }

                    // create a copy of the data values, nth component will be overwritten by
                    // simplification
                    auto simplifiedDataValues = buffer->getDataContainer();

                    std::vector<int> offsets(inportData->getOffsets());
                    if (!authorizedCriticalPoints.empty()) {
                        // perform topological simplification
                        ttk::TopologicalSimplification simplification;
                        simplification.setupTriangulation(
                            const_cast<ttk::Triangulation *>(&inportData->getTriangulation()));
                        simplification.setInputScalarFieldPointer(
                            buffer->getDataContainer().data());
                        simplification.setInputOffsetScalarFieldPointer(offsets.data());
                        simplification.setOutputScalarFieldPointer(simplifiedDataValues.data());
                        simplification.setOutputOffsetScalarFieldPointer(offsets.data());
                        simplification.setConstraintNumber(
                            static_cast<int>(authorizedCriticalPoints.size()));
                        simplification.setVertexIdentifierScalarFieldPointer(
                            authorizedCriticalPoints.data());

                        int retVal = simplification
                                         .execute<typename DataFormat<ValueType>::primitive, int>();
                        if (retVal < 0) {
                            throw TTKException("Error computing ttk::TopologicalSimplification");
                        }
                    }

                    // create a new triangulation based on the old one, but with new scalar values
                    auto result = std::make_shared<topology::TriangulationData>(*inportData);
                    result->setScalarValues(util::makeBuffer(std::move(simplifiedDataValues)));
                    result->setOffsets(std::move(offsets));

                    return result;
                });

        done();
        return triangulation;
    };

    newTriangulation_ = dispatchPool(compute);
}

void TopologicalSimplification::invalidate(InvalidationLevel invalidationLevel, Property *source) {
    notifyObserversInvalidationBegin(this);
    PropertyOwner::invalidate(invalidationLevel, source);
    if (!isValid() && hasNewData_) {
        for (auto &port : getOutports()) port->invalidate(InvalidationLevel::InvalidOutput);
    }
    notifyObserversInvalidationEnd(this);
}

}  // namespace inviwo
