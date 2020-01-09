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

#include <inviwo/topologytoolkit/utils/pickingutils.h>

namespace inviwo {

namespace topology {

PickingUtil::PickingUtil(Processor* proc, std::function<void(PickingEvent*)> callback)
    :  // CompositeProperty("topologypicking", "Topology Picking"),
    enablePicking_("enablePicking", "Enable Picking", false)
    , proc_(proc)
    , pickingMapper_(proc, 1, callback ? callback : [&](PickingEvent* p) { handlePickingEvent(p); })

{
    // addProperty(enablePicking_);
}

void PickingUtil::addPickingBuffer(Mesh& mesh) {
    // if (!enablePicking_) return;
    resize(mesh.getBuffer(0)->getSize());
    mesh.addBuffer(Mesh::BufferInfo(BufferType::PickingAttrib),
                   std::make_shared<Buffer<uint32_t>>(pickingBuffer_));
}

const std::vector<uint32_t>& PickingUtil::getPickingBuffer() {
    auto& data = pickingBuffer_->getDataContainer();
    return data;
}

void PickingUtil::handlePickingEvent(PickingEvent* p) {
    if (p->getState() == PickingState::Updated && p->getEvent()->hash() == MouseEvent::chash()) {
        auto me = p->getEventAs<MouseEvent>();
        if ((me->buttonState() & MouseButton::Left) && me->state() == MouseState::Press) {
            LogInfo("Not yet implemented.");
        }
    } else if (p->getState() == PickingState::Updated &&
               p->getEvent()->hash() == TouchEvent::chash()) {

        auto te = p->getEventAs<TouchEvent>();
        if (!te->touchPoints().empty() && te->touchPoints()[0].state() == TouchState::Updated) {
            LogInfo("Not yet implemented.");
        }
    } else if (auto we = p->getEventAs<WheelEvent>()) {
        p->markAsUsed();

        LogInfo("Not yet implemented.");
    }

    if (p->getState() == PickingState::Started) {
        highlight_ = true;
        proc_->invalidate(InvalidationLevel::InvalidOutput);
    } else if (p->getState() == PickingState::Finished) {
        highlight_ = false;
        proc_->invalidate(InvalidationLevel::InvalidOutput);
    }
}

void PickingUtil::resize(size_t s) {
    // if (!enablePicking_) return;
    pickingMapper_.resize(s);
    pickingBuffer_ = std::make_shared<BufferRAMPrecision<uint32_t>>(s);
    auto& data = pickingBuffer_->getDataContainer();
    // fill in picking IDs
    std::iota(data.begin(), data.end(), static_cast<uint32_t>(pickingMapper_.getPickingId(0)));
    pickingBufferSize_ = s;
}

size_t PickingUtil::getIdIndex(size_t s) {
    if (!pickingBuffer_) throw AbortException("Picking Buffer has not been initialized");
    auto data = pickingBuffer_->getDataContainer();

    auto it = std::find(data.begin(), data.end(), s);

    if (it == data.end()) {
        throw AbortException("Picking Buffer has invalid ids");
    }

    return std::distance(data.begin(), it);
}

void PickingUtil::buildSegmentMap(const std::vector<int>& segments) {
   
	for (size_t i = 0; i < segments.size(); i++) {
    
		int segmentId = segments[i];

		if (segmentIndicesMap_.find(segmentId) == segmentIndicesMap_.end()) {

			std::vector<int> selectedSegmentIndices;

			for (size_t i = 0; i < segments.size(); i++)
				if (segments[i] == segmentId) selectedSegmentIndices.push_back(i);

			segmentIndicesMap_[segmentId] = selectedSegmentIndices;
		}
	}

	segments_ = segments;
}


std::vector<int>& PickingUtil::getSegmentIndices(size_t pickingId) {
    auto ind = getIdIndex(pickingId);
    int segmentId = segments_[ind];

	std::vector<int> segs;

	if (segmentIndicesMap_.find(segmentId) == segmentIndicesMap_.end())
            return segs;

    return segmentIndicesMap_[segmentId];
}



PickingProperties::PickingProperties():
	pickingProps_("pickingProps", "Picking Parameters")
	, enablePicking_("enablePicking", "Enable Picking", false)
    , pickingColor_("pickingColor", "PickingColor", vec4(1.000f, 0.333f, 0.000f, 1.000f), vec4(0.0f), vec4(1.0f))
	, pickingIntensity_("pickingIntensity", "Picking Intesity", 1.0f, 0.5f, 1.0f)
	, pickingIndicesTxt_("pickingIndicesTxt", "Picking Index List", "", inviwo::InvalidationLevel::InvalidOutput, inviwo::PropertySemantics::Text)
	{

	pickingColor_.setSemantics(PropertySemantics::Color);
	pickingProps_.addProperty(enablePicking_);
	pickingProps_.addProperty(pickingColor_);
	pickingProps_.addProperty(pickingIntensity_);
	pickingProps_.addProperty(pickingIndicesTxt_);
}

}  // namespace topology

}  // namespace inviwo
