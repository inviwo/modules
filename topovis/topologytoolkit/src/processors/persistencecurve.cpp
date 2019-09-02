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

#include <inviwo/topologytoolkit/processors/persistencecurve.h>
#include <inviwo/topologytoolkit/utils/ttkutils.h>
#include <inviwo/core/datastructures/buffer/buffer.h>
#include <inviwo/core/datastructures/buffer/bufferram.h>

#include <warn/push>
#include <warn/ignore/all>
#include <ttk/core/base/persistenceCurve/PersistenceCurve.h>
#include <warn/pop>

#include <tuple>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo PersistenceCurve::processorInfo_{
    "org.inviwo.ttk.PersistenceCurve",                 // Class identifier
    "Persistence Curve",                               // Display name
    "Topology",                                        // Category
    CodeState::Experimental,                           // Code state
    "CPU, Topology, TTK, Triangulation, Persistence",  // Tags
};
const ProcessorInfo PersistenceCurve::getProcessorInfo() const { return processorInfo_; }

PersistenceCurve::PersistenceCurve() : Processor(), inport_("triangulation"), outport_("outport") {

    addPort(inport_);
    addPort(outport_);
}

void PersistenceCurve::process() {
    auto compute = [&](const auto buffer) {
        using ValueType = util::PrecisionValueType<decltype(buffer)>;
        using PrimitiveType = typename DataFormat<ValueType>::primitive;

        std::vector<int> offsets(inport_.getData()->getOffsets());

        // Computing the persistence curve
        ttk::PersistenceCurve curve;
        std::vector<std::pair<PrimitiveType, ttk::SimplexId>> outputCurve;
        curve.setupTriangulation(
            const_cast<ttk::Triangulation*>(&inport_.getData()->getTriangulation()));
        curve.setInputScalars(buffer->getDataContainer().data());
        curve.setInputOffsets(offsets.data());
        curve.setOutputCTPlot(&outputCurve);

        int retVal = curve.execute<PrimitiveType, int>();
        if (retVal < 0) {
            throw TTKException("Error computing ttk::PersistenceCurve");
        }

        // convert result of ttk::PersistenceCurve into a DataFrame
        auto dataFrame = std::make_shared<DataFrame>();

        std::vector<PrimitiveType> persistence;
        std::vector<unsigned int> count;
        persistence.reserve(outputCurve.size());
        count.reserve(outputCurve.size());
        for (const auto& p : outputCurve) {
            persistence.emplace_back(p.first);
            count.emplace_back(static_cast<unsigned int>(p.second));
        }

        dataFrame->addColumnFromBuffer("Persistence",
                                       util::makeBuffer<PrimitiveType>(std::move(persistence)));
        dataFrame->addColumnFromBuffer("Number of Points",
                                       util::makeBuffer<unsigned int>(std::move(count)));
        dataFrame->updateIndexBuffer();

        return dataFrame;
    };

    auto data = inport_.getData()
                    ->getScalarValues()
                    ->getEditableRepresentation<BufferRAM>()
                    ->dispatch<std::shared_ptr<DataFrame>, dispatching::filter::Scalars>(compute);

    outport_.setData(data);
}

}  // namespace inviwo
