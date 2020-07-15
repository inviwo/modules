
/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020 Inviwo Foundation
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

#include <inviwo/vortexfeatures/vortexfeaturesmoduledefine.h>
#include <inviwo/vortexfeatures/algorithms/calculatemeasure.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/ports/datainport.h>
#include <inviwo/core/ports/dataoutport.h>
#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/core/datastructures/volume/volumeramprecision.h>
#include <inviwo/core/datastructures/image/image.h>
#include <inviwo/core/datastructures/image/layer.h>
#include <inviwo/core/datastructures/image/layerram.h>
#include <inviwo/core/datastructures/image/layerramprecision.h>
#include <inviwo/core/util/volumeramutils.h>

#include <type_traits>

namespace inviwo {
template <unsigned SpatialDims>
class IVW_MODULE_VORTEXFEATURES_API MeasureProcessor : public Processor {
public:
    using Dispatcher = DispatchMeasure<SpatialDims>;
    using BaseData = typename Dispatcher::BaseData;
    using BaseDataOwner = typename Dispatcher::BaseDataOwner;
    using PortType = std::conditional_t<(SpatialDims == 3), Volume, Image>;

    // Should give warning if not orthogonal!!!
    MeasureProcessor();
    virtual ~MeasureProcessor() = default;

    template <template <class> class Predicate = dispatching::filter::All, typename Callable>
    void dispatchMeasure(Callable&& measure, double invalidValue = NAN);

private:
    DataInport<PortType> inport_;
    DataOutport<PortType> outport_;
};
template <unsigned SpatialDims>
MeasureProcessor<SpatialDims>::MeasureProcessor() : inport_("dataIn"), outport_("dataOut") {
    addPort(inport_);
    addPort(outport_);
}

template <unsigned SpatialDims>
template <template <class> class Predicate, typename Callable>
void MeasureProcessor<SpatialDims>::dispatchMeasure(Callable&& measure, double invalidValue) {
    if (!inport_.hasData()) {
        outport_.setData(nullptr);
        return;
    }
    std::shared_ptr<const PortType> inData = inport_.getData();
    const BaseData* baseRep = inData->template getRepresentation<BaseData>();

    DispatchMeasure<SpatialDims> dispatcher;

    auto result = baseRep->template dispatch<std::shared_ptr<BaseDataOwner>, Predicate>(
        dispatcher, std::forward<Callable&&>(measure), invalidValue);

    outport_.setData(result);
}

template <>
template <template <class> class Predicate, typename Callable>
void MeasureProcessor<2>::dispatchMeasure(Callable&& measure, double invalidValue) {
    if (!inport_.hasData()) {
        outport_.setData(nullptr);
        return;
    }
    std::shared_ptr<const PortType> inData = inport_.getData();
    const BaseData* baseRep = inData->getColorLayer()->template getRepresentation<BaseData>();

    DispatchMeasure<2> dispatcher;

    auto result = baseRep->template dispatch<std::shared_ptr<BaseDataOwner>, Predicate>(
        dispatcher, std::forward<Callable&&>(measure), invalidValue);

    outport_.setData(std::make_shared<PortType>(result));
}
}  // namespace inviwo