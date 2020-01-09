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

#ifndef IVW_PICKING_UTILS_H
#define IVW_PICKING_UTILS_H

#include <inviwo/topologytoolkit/topologytoolkitmoduledefine.h>
#include <inviwo/topologytoolkit/datastructures/triangulationdata.h>

#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/datastructures/geometry/mesh.h>
#include <inviwo/core/datastructures/volume/volume.h>

#include <inviwo/core/datastructures/geometry/mesh.h>
#include <inviwo/core/properties/transferfunctionproperty.h>

#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/compositeproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/minmaxproperty.h>
#include <inviwo/core/interaction/pickingmapper.h>
#include <inviwo/dataframe/datastructures/dataframe.h>
#include <inviwo/core/properties/buttonproperty.h>
#include <inviwo/core/properties/stringproperty.h>

#include <inviwo/core/interaction/pickingmanager.h>
#include <inviwo/core/interaction/events/pickingevent.h>
#include <inviwo/core/interaction/events/mouseevent.h>
#include <inviwo/core/interaction/events/touchevent.h>
#include <inviwo/core/interaction/events/wheelevent.h>

#include <vector>

namespace inviwo {

namespace topology {
	
class IVW_MODULE_TOPOLOGYTOOLKIT_API PickingUtil {
public:
    PickingUtil(Processor* proc, std::function<void(PickingEvent*)> callback = nullptr);
    BoolProperty enablePicking_;
    PickingMapper pickingMapper_;
    bool highlight_ = false;
    Processor* proc_;

    void addPickingBuffer(Mesh& mesh);

    const std::vector<uint32_t>& getPickingBuffer();

    void handlePickingEvent(PickingEvent* p);

    size_t getIdIndex(size_t s);

    void buildSegmentMap(const std::vector<int>& segments);

	std::vector<int>& getSegmentIndices(size_t pickingId);

private:
    std::shared_ptr<BufferRAMPrecision<uint32_t>> pickingBuffer_;
    size_t pickingBufferSize_ = 0;

    std::vector<int> segments_;
    std::map<int, std::vector<int>> segmentIndicesMap_;

    void resize(size_t s);
};


struct IVW_MODULE_TOPOLOGYTOOLKIT_API PickingProperties {

public:
	PickingProperties();

	CompositeProperty pickingProps_;
	BoolProperty enablePicking_;
    FloatVec4Property pickingColor_;
    FloatProperty pickingIntensity_;
	StringProperty pickingIndicesTxt_;
};

}  // namespace topology

}  // namespace inviwo

#endif  // IVW_TTK_UTILS_H
