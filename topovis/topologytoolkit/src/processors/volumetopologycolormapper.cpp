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
#include <modules/base/algorithm/volume/marchingcubes.h>
#include <inviwo/core/util/stdextensions.h>
#include <inviwo/core/util/clock.h>
#include <inviwo/core/util/logcentral.h>

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
	, isotfComposite_("isotfComposite", "TF & Isovalues", &volumeInport_,
                      InvalidationLevel::InvalidResources)
	, isoFactor_("isoFactor", "IsoFactor", 0.5f, 0.0f, 1.0f)
	, leafnodeOptions_("leafnodeOptions", "Leaf Node Type")
    , pickingutil_(this, [&](PickingEvent* p) { this->handlePickingEvent(p); })
    , mscSegmentOptions_("mscSegmentOptions", "MSC Segments")
    , morseSmaleComplex_("morseSmaleComplex", "MSC Options")
    , updateButton_("updateButton", "Update") {

    addPort(volumeInport_);
    addPort(contourtreeInport);
    addPort(morseSmaleComplexInport);
    addPort(outport_);    

	leafnodeOptions_.addOption("all-leaves", "All Leaves", 0);
	leafnodeOptions_.addOption("maxima-leaves", "Maxima Leaves", 1);
	leafnodeOptions_.addOption("minima-leaves", "Minima Leaves", 2);
	leafnodeOptions_.setSelectedValue(3);
	leafnodeOptions_.setCurrentStateAsDefault();


    addProperty(transferFunction_);
	addProperty(isoFactor_);
	addProperty(leafnodeOptions_);
	addProperty(updateButton_);
	addProperty(isotfComposite_);

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
	std::vector<vec4> segmentColors_;
	std::vector<float> status_;

	auto triangulation = treeData->triangulation.get();
	std::vector<float> scalars = topology::ttkExtractScalarValuesFromTriangulation(*triangulation);
	
	auto tree = treeData->getTree();
	auto numArcs = tree->getNumberOfSuperArcs();
	
	auto nodeType = [](ttk::ftm::FTMTree_MT* tree, int nodeId) {

		auto node = tree->getNode(nodeId);
        const bool up = node->getNumberOfUpSuperArcs() > 0;
        const bool down = node->getNumberOfDownSuperArcs() > 0;

		// 1 saddle
		// 2 maxima
		// 3 minima

		return up && down ? -1 : (up ?  1 : 2);
	};

	std::vector<ttk::ftm::idNode> leafNodes;
	for (size_t i = 0; i < tree->getLeaves().size(); i++) {

		if (leafnodeOptions_.get()) {
			if (nodeType(tree, tree->getLeave(i)) != leafnodeOptions_.get())
				continue;
		}
		leafNodes.push_back(tree->getLeave(i));
	}

	  
	std::vector<int> valid_usegments;	
	for (ttk::ftm::idSuperArc i = 0; i < usegments.size(); ++i) {
		//if (usegments[i] >= numArcs) continue;
		auto arc = tree->getSuperArc(usegments[i]);
		if (!arc) {
			LogWarn("topology::VolumeTopologyColorMapper::surfaceExtraction - Segment Id does not have corresponding Super Arc. SegId = " + std::to_string(usegments[i]));
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
		valid_usegments.push_back(usegments[i]);
	}

	meshes_->resize(data.size());
	status_.resize(data.size());

	auto minMaxIso = std::minmax_element(scalars.begin(), scalars.end());

	isotfComposite_.isovalues_.get().clear();
	isotfComposite_.tf_.get().clear();

	for (size_t i = 0; i < isoFs.size(); i++) {
		auto nIso = (isoFs[i] - *minMaxIso.first) / (*minMaxIso.second - *minMaxIso.first);
		colors_.push_back(transferFunction_.get().sample(nIso)*0.8f);
		segmentColors_.push_back(transferFunction_.get().sample(float(i+1)/isoFs.size())*0.8f);

		auto isoCol = colors_.back();
		isoCol.a = 0.6f;
		isotfComposite_.isovalues_.get().add(nIso, isoCol );
	}
		

	for (size_t i = 0; i < data.size(); ++i)
		status_[i] = 0.0f;
	
    float totalStatus = 0.0f;


	/*const ivec3 vdim = (ivec3) volume->getDimensions();
    const util::IndexMapper3D im(volume->getDimensions());
	auto& segmentsList = treeData->getSegments();
	int tokenShift = 0;

	std::map<int, int> boundaryTokenMap;
	std::map<int, int> boundaryTokenMap_f;
	for (size_t i = 0; i < segmentsList.size(); i++) {		
		if (boundaryTokenMap.find(segmentsList[i]) == boundaryTokenMap.end()) {
			boundaryTokenMap[segmentsList[i]] = 1 << tokenShift;
			tokenShift++;
		}

		boundaryTokenMap_f[segmentsList[i]] += 1;
	}

	auto boundaryTokenList = std::vector<int>(segmentsList.size(), 0);
	for (size_t i = 0; i < segmentsList.size(); i++) {

		auto s_n = im(i);
		for (auto x = -1; x <= 1; x++) {
			for (auto y = -1; y <= 1; y++) {
				for (auto z = -1; z <= 1; z++) {
					auto p = glm::clamp(ivec3(s_n.x + x, s_n.y + y, s_n.z + z), ivec3(0), vdim-1);
					boundaryTokenList[i] |= boundaryTokenMap[segmentsList[im(p)]];
				}
			}
		}

	}*/
	
	
	for (size_t i = 0; i < data.size(); ++i) {

		auto vol = data[i];
		float iso = isoFs[i];
		vec4 color = colors_[i];
		bool invert = invertIso_;
		bool enclose = encloseSurface_;
		int segmentId = valid_usegments[i];

		//color = segmentColors_[i];

		/*auto maskingCallBack = [this, treeData, segmentId, segmentsList, im, vdim, boundaryTokenList, boundaryTokenMap, iso, scalars](const size3_t s){

			auto currInd = im(s);
			auto currSegmentId = segmentsList[currInd];

			if (segmentId == currSegmentId) {
					return true;
			}

			if (glm::abs(iso - scalars[currInd]) <= 0.001f) return true;

			return false;
		};*/

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
			};
			
			std::shared_ptr<Mesh> m;
			m = util::marchingCubesOpt(vol, iso, color, invert, enclose,
				progressCallBack);

			//m = util::marchingcubes(vol, iso, color, false, false,
			//	progressCallBack, maskingCallBack);

			(this->meshes_->at(i)) = m;


			return m;
		};//compute

		auto m = dispatchPool(compute);
		(*meshes_)[i] = m.get();
		//auto m = compute(); (*meshes_)[i] = m;
		//compute();

		if (i==1) break;
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
