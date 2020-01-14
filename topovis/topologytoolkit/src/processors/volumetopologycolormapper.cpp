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

#include <inviwo/topologytoolkit/processors/volumetopologycolormapper.h>
#include <inviwo/core/datastructures/geometry/basicmesh.h>
#include <modules/base/algorithm/volume/marchingcubesopt.h>
#include <inviwo/core/util/stdextensions.h>
#include <inviwo/core/util/clock.h>

#pragma optimize("", off)


namespace inviwo {
	
// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VolumeTopologyColorMapper::processorInfo_{
    "org.inviwo.VolumeTopologyColorMapper",  // Class identifier
    "Volume Topology Color Mapper",          // Display name
    "Topology",                            // Category
    CodeState::Experimental,               // Code state
    "CPU, Topology, TTK, Triangulation",   // Tags
};
const ProcessorInfo VolumeTopologyColorMapper::getProcessorInfo() const { return processorInfo_; }

VolumeTopologyColorMapper::VolumeTopologyColorMapper()
    : Processor()
    , contourtreeInport("contourtree")
    , morseSmaleComplexInport("morsesmalecomplex")
    , volumeInport_("volume")
    , outport_("outport")	
    , transferFunction_("transferFunction", "TranferFunction")
	, isoFactor_("isoFactor", "IsoFactor", 0.5f, 0.0f, 1.0f)
    , pickingutil_(this, [&](PickingEvent* p) { this->handlePickingEvent(p); })
    , mscSegmentOptions_("mscSegmentOptions", "MSC Segments")
    , morseSmaleComplex_("morseSmaleComplex", "MSC Options")
    , updateButton_("updateButton", "Update") {

    addPort(volumeInport_);
    addPort(contourtreeInport);
    addPort(morseSmaleComplexInport);
    addPort(outport_);    

    addProperty(transferFunction_);
	addProperty(isoFactor_);
	addProperty(updateButton_);

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

	morseSmaleComplexInport.onChange([&]() {
		if (morseSmaleComplexInport.hasData()) {
			morseSmaleComplex_.setReadOnly(false);
		}
		else {
			morseSmaleComplex_.setReadOnly(true);
		}
	});

	contourtreeInport.onChange([&]() {
		if (morseSmaleComplexInport.hasData()) {
			morseSmaleComplex_.setReadOnly(false);
		}
		else {
			morseSmaleComplex_.setReadOnly(true);
		}

		if (contourtreeInport.getData()) {
			if (volumeInport_.getData()) {
			if (contourtreeInport.getData()) {
				segmentTriangulations_ = topology::ttkSegmentExtraction(contourtreeInport.getData());
				this->recompute_ = true;
			}
		}
		}
	});

	volumeInport_.onChange([&]() {
		if (volumeInport_.getData()) {
			if (contourtreeInport.getData()) {
				segmentTriangulations_ = topology::ttkSegmentExtraction(contourtreeInport.getData());
				this->recompute_ = true;
			}
		}
	});

	updateButton_.onChange([&]() {
		if (volumeInport_.getData()) {
			if (contourtreeInport.getData()) {
				//segmentTriangulations_ = topology::ttkSegmentExtraction(contourtreeInport.getData());
				this->recompute_ = true;
			}
		}
	});

	/*transferFunction_.onChange([&]() {
		makeDirty();
	});

	isoFactor_.onChange([&]() {
		makeDirty();
	});

	morseSmaleComplex_.onChange([&]() {
		makeDirty();
	});*/

    getProgressBar().hide();
	invalidate(InvalidationLevel::InvalidOutput);

}



void VolumeTopologyColorMapper::process() {

    if (contourtreeInport.getData() && volumeInport_.getData()) {

		if (recompute_) {
			surfaceExtraction();
			this->recompute_ = false;
		}

        /*auto mesh =
            topology::applyColorMapToMesh(transferFunction_.get(), *meshInport_.getData().get(),
                                          contourtreeInport.getData()->getSegments(),
                                          *contourtreeInport.getData()->triangulation.get());

        pickingutil_.addPickingBuffer(*mesh);

		pickingutil_.buildSegmentMap(contourtreeInport.getData()->getSegments());

        // set output mesh
        outport_.setData(mesh);*/

        return;
    }

    if (morseSmaleComplexInport.getData() && volumeInport_.getData()) {

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
        
        return;
    }

    // default original mesh
    outport_.setData(this->meshes_);
}

void VolumeTopologyColorMapper::handlePickingEvent(PickingEvent* p) {

	return;

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

void VolumeTopologyColorMapper::surfaceExtraction()
{	 
    this->meshes_ = std::make_shared<std::vector<std::shared_ptr<Mesh>>>();

    bool invertIso_ = false;
    bool encloseSurface_ = false;
	float isoValue = isoFactor_;	

	auto done = [this]() {
        dispatchFront([this]() {            
            invalidate(InvalidationLevel::InvalidOutput);
			if (!this->meshes_->empty()) {
                outport_.setData(this->meshes_);
            } else {
                outport_.setData(nullptr);
            }
			//getProgressBar().hide();
        });
    };

	//getProgressBar().show();

	auto treeData = contourtreeInport.getData();
	std::set<int> s(treeData->getSegments().begin(),treeData->getSegments().end());
	std::vector<int> usegments(s.begin(), s.end());

	auto volume = volumeInport_.getData();
	std::vector<std::shared_ptr<const Volume>> data;
	std::vector<float> isoFs;
	std::vector<vec4> colors_;
	std::vector<float> status_;

	auto triangulation = treeData->triangulation.get();
	std::vector<float> scalars = topology::ttkExtractScalarValuesFromTriangulation(*triangulation);
	
	auto tree = treeData->getTree();
	auto numArcs = tree->getNumberOfSuperArcs();

	std::vector<ttk::ftm::idNode> leafNodes;
	for (size_t i = 0; i < tree->getLeaves().size(); i++)
		leafNodes.push_back( tree->getLeave(i));
	  

	for (ttk::ftm::idSuperArc i = 0; i < usegments.size(); ++i) {
		//if (usegments[i] >= numArcs) continue;
		auto arc = tree->getSuperArc(usegments[i]);
		if (!arc) {
			LogWarn("topology::VolumeTopologyColorMapper::surfaceExtraction",
			"Segment Id does not have corresponding Super Arc. SegId = " + std::to_string(usegments[i]));
			continue;
		}

		if (std::find(leafNodes.begin(), leafNodes.end(), arc->getDownNodeId()) == leafNodes.end() &&
			std::find(leafNodes.begin(), leafNodes.end(), arc->getUpNodeId()) == leafNodes.end())
			continue;

		auto upscalar = scalars[tree->getNode(arc->getUpNodeId())->getVertexId()];
		auto downscalar = scalars[tree->getNode(arc->getDownNodeId())->getVertexId()];

		auto iso = (upscalar * isoValue) + downscalar * (1.0f - isoValue);

		isoFs.push_back(iso);
		data.push_back(volume);
	}

	meshes_->resize(data.size());
	status_.resize(data.size());

	auto minMaxIso = std::minmax_element(scalars.begin(), scalars.end());

	for (auto& iso : isoFs) {
		auto nIso = (iso - *minMaxIso.first) / (*minMaxIso.second - *minMaxIso.first);
		colors_.push_back(transferFunction_.get().sample(nIso)*0.8f);
	}

	for (size_t i = 0; i < data.size(); ++i)
		status_[i] = 0.0f;
	
    float totalStatus = 0.0f;


	for (size_t i = 0; i < data.size(); ++i) {

		//if (!(i >= 5 && i <= 7)) continue;

		auto vol = data[i];
		float iso = isoFs[i];
		vec4 color = colors_[i];
		bool invert = invertIso_;
		bool enclose = encloseSurface_;

		// construction of iso surface using marchingCubesOpt
		auto compute = [vol, iso, color, invert, enclose, i, done, this, &status_, &totalStatus]() -> std::shared_ptr<Mesh> {

			ScopedClockCPU clock{"VolumeTopologyColorMapper", "VolumeTopologyColorMapper ISO Surface calculation",
							    std::chrono::milliseconds(500), LogLevel::Info};

			//progress call back
			auto progressCallBack = [this, iso, i, &status_, &totalStatus](float s) mutable {
				status_[i] = s;
                for (const auto& e :status_) totalStatus += e;
                totalStatus /= status_.size();

				if (int(totalStatus * 100) % 20 == 0) {
					LogInfo("VolumeTopologyColorMapper::surfaceExtraction " + inviwo::toString(i) + " for iso vlaue: " + inviwo::toString(iso) +
						" has completed " + inviwo::toString((int)(totalStatus * 100)) + "% ");
				}

				/*dispatchFront(
					[this, &totalStatus, iso, i](ProgressBar& pb) {
						pb.updateProgress(totalStatus);
						if (totalStatus < 1.0f) {
							pb.show();							
						}
						else
							pb.hide();

						if (int(totalStatus * 100) % 5 == 0) {
							LogInfo("VolumeTopologyColorMapper::surfaceExtraction " + inviwo::toString(i) + " for iso vlaue: " + inviwo::toString(iso) +
								" has completed " + inviwo::toString((int)(totalStatus * 100)) + "% ");
						}
					}
					,	std::ref(this->getProgressBar())
					);*/

			};

			std::shared_ptr<Mesh> m;
			m = util::marchingCubesOpt(vol, iso, color, invert, enclose,
				progressCallBack);

			return m;
		};//compute

		auto m = dispatchPool(compute);
		(*meshes_)[i] = m.get();

	}

	done();
    
}

void VolumeTopologyColorMapper::invalidate(InvalidationLevel invalidationLevel, Property *source) {
    notifyObserversInvalidationBegin(this);
    PropertyOwner::invalidate(invalidationLevel, source);
    if (!isValid()) {
        for (auto &port : getOutports()) port->invalidate(InvalidationLevel::InvalidOutput);
    }
    notifyObserversInvalidationEnd(this);
}



}  // namespace inviwo
