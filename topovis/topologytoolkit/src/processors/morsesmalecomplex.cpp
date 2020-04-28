/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2020 Inviwo Foundation
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

#include <inviwo/topologytoolkit/processors/morsesmalecomplex.h>
#include <inviwo/topologytoolkit/utils/ttkutils.h>
#include <inviwo/core/datastructures/buffer/buffer.h>
#include <inviwo/core/datastructures/buffer/bufferram.h>
#include <inviwo/core/datastructures/geometry/mesh.h>
#include <inviwo/core/datastructures/geometry/meshram.h>
#include <inviwo/core/util/stdextensions.h>
#include <inviwo/core/util/clock.h>
#include <inviwo/core/common/inviwoapplication.h>

#include <warn/push>
#include <warn/ignore/all>
#include <ttk/core/base/morseSmaleComplex/MorseSmaleComplex.h>
#include <ttk/core/base/topologicalSimplification/TopologicalSimplification.h>
#include <ttk/core/base/ftmTree/FTMTree_MT.h>
#include <warn/pop>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo MorseSmaleComplex::processorInfo_{
    "org.inviwo.MorseSmaleComplex",      // Class identifier
    "Morse-Smale Complex",               // Display name
    "Topology",                          // Category
    CodeState::Experimental,             // Code state
    "CPU, Topology, TTK, Contour Tree",  // Tags
};
const ProcessorInfo MorseSmaleComplex::getProcessorInfo() const { return processorInfo_; }

MorseSmaleComplex::MorseSmaleComplex()
    : Processor()
    , inport_{"triangulation"}
    , outport_{"outport"}
    , returnSaddleConnectors_{"returnSaddleConnectors", "Return Saddle Connectors", false}
    , computeSaddleConnectors_{"computeSaddleConnectors", "Compute Saddle Connectors", false}
    , saddleConnectorsPersistenceThreshold_{"saddleConnectorsPersistenceThreshold_",
                                            "Saddle Connectors Persistence Threshold", 0.0f, 0.0f,
                                            100.0f} {
    addPort(inport_);
    addPort(outport_);

    addProperties(returnSaddleConnectors_, computeSaddleConnectors_,
                  saddleConnectorsPersistenceThreshold_);
}

void MorseSmaleComplex::process() {
    if (util::is_future_ready(newMsc_)) {
        try {
            auto vol = newMsc_.get();
            outport_.setData(vol);
            hasNewData_ = false;
            getActivityIndicator().setActive(false);
        } catch (Exception&) {
            outport_.setData(nullptr);
            hasNewData_ = false;
            throw;
        }
    } else {
        if (inport_.isChanged() || mscDirty_ || returnSaddleConnectors_.isModified() ||
            computeSaddleConnectors_.isModified() ||
            saddleConnectorsPersistenceThreshold_.isModified()) {
            getActivityIndicator().setActive(true);
            updateOutport();
        }
    }
}

void MorseSmaleComplex::updateOutport() {
    auto done = [this]() {
        dispatchFront([this]() {
            mscDirty_ = false;
            hasNewData_ = true;
            invalidate(InvalidationLevel::InvalidOutput);
        });
    };

    const auto inportData = inport_.getData();

    const auto rsc = *returnSaddleConnectors_;
    const auto csc = *computeSaddleConnectors_;
    const auto scpt = *saddleConnectorsPersistenceThreshold_;

    auto compute = [inportData, done, rsc, csc,
                    scpt]() -> std::shared_ptr<const topology::MorseSmaleComplexData> {
        ScopedClockCPU clock{"MorseSmaleComplex", "Morse-Smale complex calculation",
                             std::chrono::milliseconds(500), LogLevel::Info};
        auto mscData =
            inportData->getScalarValues()
                ->getRepresentation<BufferRAM>()
                ->dispatch<std::shared_ptr<topology::MorseSmaleComplexData>,
                           dispatching::filter::Scalars>(
                    [inportData, rsc, csc, scpt](const auto buffer) {
                        using ValueType = util::PrecisionValueType<decltype(buffer)>;
                        using PrimitiveType = typename DataFormat<ValueType>::primitive;

                        std::vector<int> offsets(inportData->getOffsets());

                        ttk::MorseSmaleComplex morseSmaleComplex;
                        morseSmaleComplex.setupTriangulation(
                            const_cast<ttk::Triangulation*>(&inportData->getTriangulation()));
                        morseSmaleComplex.setInputScalarField(buffer->getDataContainer().data());
                        morseSmaleComplex.setInputOffsets(offsets.data());

                        auto mscData = std::make_shared<topology::MorseSmaleComplexData>(
                            morseSmaleComplex, inportData);

                        morseSmaleComplex.setReturnSaddleConnectors(rsc);
                        morseSmaleComplex.setComputeSaddleConnectors(csc);
                        morseSmaleComplex.setSaddleConnectorsPersistenceThreshold(scpt);

                        morseSmaleComplex.execute<PrimitiveType, ttk::SimplexId>();

                        return mscData;
                    });

        done();
        return mscData;
    };

    newMsc_ = dispatchPool(compute);
}

void MorseSmaleComplex::invalidate(InvalidationLevel invalidationLevel, Property* source) {
    notifyObserversInvalidationBegin(this);
    PropertyOwner::invalidate(invalidationLevel, source);
    if (!isValid() && hasNewData_) {
        for (auto& port : getOutports()) port->invalidate(InvalidationLevel::InvalidOutput);
    }
    notifyObserversInvalidationEnd(this);
}

}  // namespace inviwo
