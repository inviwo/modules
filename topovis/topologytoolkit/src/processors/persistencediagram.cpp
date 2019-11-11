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

#include <inviwo/topologytoolkit/processors/persistencediagram.h>
#include <inviwo/topologytoolkit/utils/ttkutils.h>
#include <inviwo/core/datastructures/buffer/bufferram.h>
#include <inviwo/core/util/zip.h>
#include <inviwo/core/util/stdextensions.h>

#include <warn/push>
#include <warn/ignore/all>
#include <ttk/core/base/persistenceDiagram/PersistenceDiagram.h>
#include <warn/pop>

#include <algorithm>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo PersistenceDiagram::processorInfo_{
    "org.inviwo.ttk.PersistenceDiagram",               // Class identifier
    "Persistence Diagram",                             // Display name
    "Topology",                                        // Category
    CodeState::Experimental,                           // Code state
    "CPU, Topology, TTK, Triangulation, Persistence",  // Tags
};
const ProcessorInfo PersistenceDiagram::getProcessorInfo() const { return processorInfo_; }

PersistenceDiagram::PersistenceDiagram()
    : Processor()
    , inport_("triangulation")
    , outport_("outport")
    , dataFrameOutport_("dataframe")
    , computeSaddleConnectors_{"computeSaddleConnectors", "Compute Saddle Connectors", false} {

    addPort(inport_);
    addPort(outport_);
    addPort(dataFrameOutport_);
    addProperties(computeSaddleConnectors_);
}

void PersistenceDiagram::process() {
    if (util::is_future_ready(newData_)) {
        try {
            auto data = newData_.get();
            outport_.setData(data.first);
            dataFrameOutport_.setData(data.second);

            hasNewData_ = false;
            getActivityIndicator().setActive(false);
        } catch (Exception&) {
            outport_.setData(nullptr);
            hasNewData_ = false;
            throw;
        }
    } else {
        if (inport_.isChanged() || computeSaddleConnectors_.isModified()) {
            getActivityIndicator().setActive(true);
            compute();
        }
    }
}

void PersistenceDiagram::compute() {
    // Compute persistence diagram

    auto done = [this]() {
        dispatchFront([this]() {
            hasNewData_ = true;
            invalidate(InvalidationLevel::InvalidOutput);
        });
    };

    auto compute = [done, data = inport_.getData(), css = computeSaddleConnectors_.get()]() {
        return data->getScalarValues()
            ->getEditableRepresentation<BufferRAM>()
            ->dispatch<std::pair<std::shared_ptr<topology::PersistenceDiagramData>,
                                 std::shared_ptr<DataFrame>>,
                       dispatching::filter::Scalars>([&](const auto buffer) {
                using ValueType = util::PrecisionValueType<decltype(buffer)>;
                using DiagramOutput =
                    std::vector<std::tuple<ttk::SimplexId, ttk::CriticalType, ttk::SimplexId,
                                           ttk::CriticalType, ValueType, ttk::SimplexId>>;

                std::vector<int> offsets(buffer->getSize());
                std::iota(offsets.begin(), offsets.end(), 0);

                DiagramOutput output;
                ttk::PersistenceDiagram diagram;
                diagram.setComputeSaddleConnectors(css);
                diagram.setupTriangulation(
                    const_cast<ttk::Triangulation*>(&data->getTriangulation()));
                diagram.setOutputCTDiagram(&output);
                diagram.setInputScalars(buffer->getDataContainer().data());
                diagram.setInputOffsets(offsets.data());

                int retVal = diagram.execute<typename DataFormat<ValueType>::primitive, int>();
                if (retVal != 0) {
                    throw TTKException("Error computing ttk::PersistenceDiagram");
                }

                // convert result of ttk::PersistenceDiagram into a DataFrame
                std::vector<ValueType> birth;
                std::vector<ValueType> death;

                const auto& scalars = buffer->getDataContainer();
                birth.reserve(output.size());
                death.reserve(output.size());
                for (const auto& extremumPair : output) {
                    birth.push_back(scalars[std::get<0>(extremumPair)]);
                    death.push_back(scalars[std::get<2>(extremumPair)]);
                }

                auto dataFrame = std::make_shared<DataFrame>();
                dataFrame->addColumnFromBuffer("Birth",
                                               util::makeBuffer<ValueType>(std::move(birth)));
                dataFrame->addColumnFromBuffer("Death",
                                               util::makeBuffer<ValueType>(std::move(death)));
                dataFrame->updateIndexBuffer();

                // TODO: use proper data structure with a Buffer<ValueType> instead of conversion,
                // needs to match type of scalar buffer!

                // convert diagram output to topology::PersistenceDiagramData, i.e. float
                auto diagramOutput = std::make_shared<topology::PersistenceDiagramData>();
                diagramOutput->reserve(output.size());
                for (auto& elem : output) {
                    diagramOutput->emplace_back(std::make_tuple(
                        std::get<0>(elem), std::get<1>(elem), std::get<2>(elem), std::get<3>(elem),
                        static_cast<float>(std::get<4>(elem)), std::get<5>(elem)));
                }

                done();
                return std::make_pair(diagramOutput, dataFrame);
            });
    };

    newData_ = dispatchPool(compute);
}

void PersistenceDiagram::invalidate(InvalidationLevel invalidationLevel, Property* source) {
    notifyObserversInvalidationBegin(this);
    PropertyOwner::invalidate(invalidationLevel, source);
    if (!isValid() && hasNewData_) {
        for (auto& port : getOutports()) port->invalidate(InvalidationLevel::InvalidOutput);
    }
    notifyObserversInvalidationEnd(this);
}

}  // namespace inviwo
