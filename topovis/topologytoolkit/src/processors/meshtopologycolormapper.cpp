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
#include <inviwo/core/datastructures/geometry/basicmesh.h>
#pragma optimize("", off)


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
	, isolinesOutport_("isolinesOutport_")
    , transferFunction_("transferFunction", "TranferFunction")
	, isoFactor_("isoFactor", "IsoFactor", 0.5f, 0.0f, 1.0f)
    , pickingutil_(this, [&](PickingEvent* p) { this->handlePickingEvent(p); })
    , mscSegmentOptions_("mscSegmentOptions", "MSC Segments")
    , morseSmaleComplex_("morseSmaleComplex", "MSC Options"){

    addPort(meshInport_);
    addPort(contourtreeInport);
    addPort(morseSmaleComplexInport);
    addPort(outport_);
    addPort(isolinesOutport_);

    addProperty(transferFunction_);
	addProperty(isoFactor_);

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

		if (contourtreeInport.getData()) {
			segmentTriangulations_ = topology::ttkSegmentExtraction(contourtreeInport.getData());
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

		//iso contours
		bool showContour = true;
		auto tree = contourtreeInport.getData()->getTree();
		auto triangulation = contourtreeInport.getData()->triangulation.get();


		if (showContour) {		


			std::set<int> s(contourtreeInport.getData()->getSegments().begin(), contourtreeInport.getData()->getSegments().end());
			std::vector<int> usegments(s.begin(), s.end());

			std::vector<float> scalars = topology::ttkExtractScalarValuesFromTriangulation(*triangulation);
			/*auto minmax = std::minmax_element(scalars.begin(), scalars.end());
            auto div = (*minmax.second - *minmax.first);
			auto avg = (*minmax.first + *minmax.second) / 2.0f;*/

			std::shared_ptr<inviwo::Mesh> contour_mesh = nullptr;

			auto numArcs = tree->getNumberOfSuperArcs();

			std::vector<ttk::ftm::idNode> leafNodes;
			for (size_t i = 0; i < tree->getLeaves().size(); i++)
				leafNodes.push_back( tree->getLeave(i));
			

			for (ttk::ftm::idSuperArc i = 0; i <usegments.size(); ++i) {
				
				if (usegments[i] >= numArcs) continue;

				//if (usegments[i] != (int)(isoFactor_.get() * numArcs))
				//	continue;				

				auto arc = tree->getSuperArc(usegments[i]);

				if (std::find(leafNodes.begin(), leafNodes.end(), arc->getDownNodeId()) == leafNodes.end() &&
					std::find(leafNodes.begin(), leafNodes.end(), arc->getUpNodeId()) == leafNodes.end())
						continue;

				auto upscalar = scalars[tree->getNode(arc->getUpNodeId())->getVertexId()];
				auto downscalar =  scalars[tree->getNode(arc->getDownNodeId())->getVertexId()];

				auto iso = (upscalar * isoFactor_.get()) + downscalar *(1.0f - isoFactor_.get());
				auto avg = (upscalar + downscalar) * 0.5f;

				auto col = vec4(1.0f, 0.0f, 0.0f, 1.0f);
				//col = transferFunction_.get().sample(usegments[i] / usegments.size());
				col = glm::clamp(col, vec4(0.0f), vec4(1.0f));

				auto iso_contours = marchingTriangles_from_Triangulation(*segmentTriangulations_[i], iso, col);

				if (!contour_mesh) {
					contour_mesh = iso_contours;
					continue;
				}

				contour_mesh->append(*iso_contours);
			}

			isolinesOutport_.setData(contour_mesh);

		}

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
