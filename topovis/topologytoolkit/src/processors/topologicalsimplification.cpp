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
    auto compute = [&](const auto buffer, float threshold, bool invert) {
        using ValueType = util::PrecisionValueType<decltype(buffer)>;

        const auto &persistenceDiagram = *persistenceInport_.getData();

        // select most/least persistent critical point pairs
        std::vector<int> authorizedCriticalPoints;
        for (int i = 0; i < (int)persistenceDiagram.size(); i++) {
            auto persistence = std::get<4>(persistenceDiagram[i]);
            if ((persistence >= threshold) != invert) {
                authorizedCriticalPoints.push_back(std::get<0>(persistenceDiagram[i]));
                authorizedCriticalPoints.push_back(std::get<2>(persistenceDiagram[i]));
            }
        }

        // create a copy of the data values, nth component will be overwritten by simplification
        auto simplifiedDataValues = buffer->getDataContainer();
        auto input = inport_.getData();

        std::vector<int> offsets(input->getOffsets());
        if (!authorizedCriticalPoints.empty()) {
            // perform topological simplification
            ttk::TopologicalSimplification simplification;
            simplification.setupTriangulation(
                const_cast<ttk::Triangulation *>(&input->getTriangulation()));
            simplification.setInputScalarFieldPointer(buffer->getDataContainer().data());
            simplification.setInputOffsetScalarFieldPointer(offsets.data());
            simplification.setOutputScalarFieldPointer(simplifiedDataValues.data());
            simplification.setOutputOffsetScalarFieldPointer(offsets.data());
            simplification.setConstraintNumber(static_cast<int>(authorizedCriticalPoints.size()));
            simplification.setVertexIdentifierScalarFieldPointer(authorizedCriticalPoints.data());

            int retVal = simplification.execute<typename DataFormat<ValueType>::primitive, int>();
            if (retVal < 0) {
                throw TTKException("Error computing ttk::TopologicalSimplification");
            }
        }

        // create a new triangulation based on the old one, but with new scalar values
        auto result = std::make_shared<topology::TriangulationData>(*inport_.getData().get());
        result->setScalarValues(util::makeBuffer(std::move(simplifiedDataValues)));
        result->setOffsets(std::move(offsets));

        return result;
    };

    auto data =
        inport_.getData()
            ->getScalarValues()
            ->getEditableRepresentation<BufferRAM>()
            ->dispatch<std::shared_ptr<topology::TriangulationData>, dispatching::filter::Scalars>(
                compute, threshold_.get(), invert_.get());

    outport_.setData(data);
}

}  // namespace inviwo
