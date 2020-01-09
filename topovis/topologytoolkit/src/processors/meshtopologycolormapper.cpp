/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019 Inviwo Foundation
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

#include <inviwo/topologytoolkit/processors/meshtopologycolormapper.h>

namespace inviwo {
	
// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo MeshTopologyColorMapper::processorInfo_{
    "org.inviwo.MeshTopologyColorMapper",  // Class identifier
    "Mesh Topology Color Mapper",          // Display name
    "Topology",                            // Category
    CodeState::Experimental,               // Code state
    "CPU, Topology, TTK, Triangulation",   // Tags
};
const ProcessorInfo MeshTopologyColorMapper::getProcessorInfo() const { return processorInfo_; }

MeshTopologyColorMapper::MeshTopologyColorMapper()
    : Processor()
    , contourtreeInport("contourtree")
    , morseSmaleComplexInport("morsesmalecomplex")
    , meshInport_("mesh")
    , outport_("outport")
    , transferFunction_("transferFunction", "TranferFunction")
    , pickingutil_(this, [&](PickingEvent* p) { this->handlePickingEvent(p); })
    , mscSegmentOptions_("mscSegmentOptions", "MSC Segments")
    , morseSmaleComplex_("morseSmaleComplex", "MSC Options"){

    addPort(meshInport_);
    addPort(contourtreeInport);
    addPort(morseSmaleComplexInport);
    addPort(outport_);

    addProperty(transferFunction_);

    contourtreeInport.setOptional(true);
    morseSmaleComplexInport.setOptional(true);

	mscSegmentOptions_.addOption("asc", "Ascending", 0);
	mscSegmentOptions_.addOption("des", "Descending",1);
	mscSegmentOptions_.addOption("msc", "MSC", 2);
	mscSegmentOptions_.setSelectedValue(2);
	mscSegmentOptions_.setCurrentStateAsDefault();
	morseSmaleComplex_.addProperty(mscSegmentOptions_);
	addProperty(morseSmaleComplex_);

    addProperty(pickingProperties_.pickingProps_);

	morseSmaleComplexInport.onChange([this]() {
		if (morseSmaleComplexInport.hasData()) {
			morseSmaleComplex_.setReadOnly(false);
		}
		else {
			morseSmaleComplex_.setReadOnly(true);
		}
	});

	contourtreeInport.onChange([this]() {
		if (morseSmaleComplexInport.hasData()) {
			morseSmaleComplex_.setReadOnly(false);
		}
		else {
			morseSmaleComplex_.setReadOnly(true);
		}
	});
}

void MeshTopologyColorMapper::process() {

    if (contourtreeInport.getData()) {
        auto mesh =
            topology::applyColorMapToMesh(transferFunction_.get(), *meshInport_.getData().get(),
                                          contourtreeInport.getData()->getSegments(),
                                          *contourtreeInport.getData()->triangulation.get());

        pickingutil_.addPickingBuffer(*mesh);

		pickingutil_.buildSegmentMap(contourtreeInport.getData()->getSegments());

        // set output mesh
        outport_.setData(mesh);
        return;
    }

    if (morseSmaleComplexInport.getData()) {

		std::vector<int> segs;

		switch (mscSegmentOptions_.get()) {
			case 0: 
				segs = morseSmaleComplexInport.getData()->segmentation.ascending;
				break;
			case 1: 
				segs = morseSmaleComplexInport.getData()->segmentation.descending;
				break;
			case 2:
			default:
				segs = morseSmaleComplexInport.getData()->segmentation.msc;
		}

        auto mesh =
            topology::applyColorMapToMesh(transferFunction_.get(), *meshInport_.getData().get(),
                                          segs,
                                          *morseSmaleComplexInport.getData()->triangulation.get());

        pickingutil_.addPickingBuffer(*mesh);

		pickingutil_.buildSegmentMap(segs);		

        // set output mesh
        outport_.setData(mesh);
        return;
    }

    // default original mesh
    outport_.setData(meshInport_.getData());
}

void MeshTopologyColorMapper::handlePickingEvent(PickingEvent* p) {
    if (p->getState() == PickingState::Updated && p->getEvent()->hash() == MouseEvent::chash()) {
        auto me = p->getEventAs<MouseEvent>();
        if ((me->buttonState() & MouseButton::Left) && me->state() == MouseState::Press) {
			           
			auto selectedSegmentIndices = pickingutil_.getSegmentIndices(p->getCurrentGlobalPickingId());

			std::string sids;
			for (auto& sid : selectedSegmentIndices) {
				 sids += " ";
				 sids += toString<int>(sid);
			}

			LogInfo("Selecting the picking ID-" +
				toString<size_t>(p->getCurrentGlobalPickingId()) + " with corresponding SegmentID-" + sids);
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

	return;
	
    if (p->getState() == PickingState::Started) {
        pickingutil_.highlight_ = true;
        invalidate(InvalidationLevel::InvalidOutput);
    } else if (p->getState() == PickingState::Finished) {
        pickingutil_.highlight_ = false;
        invalidate(InvalidationLevel::InvalidOutput);
    }
}

}  // namespace inviwo
