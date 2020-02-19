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

#ifndef IVW_VOL_CONTOURTREECOLORMAPPER_H
#define IVW_VOL_CONTOURTREECOLORMAPPER_H

#include <inviwo/topologytoolkit/topologytoolkitmoduledefine.h>
#include <inviwo/topologytoolkit/ports/triangulationdataport.h>

#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/ports/imageport.h>
#include <inviwo/core/ports/meshport.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/compositeproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/minmaxproperty.h>
#include <inviwo/core/interaction/pickingmapper.h>
#include <inviwo/dataframe/datastructures/dataframe.h>
#include <inviwo/core/properties/buttonproperty.h>
#include <inviwo/core/properties/transferfunctionproperty.h>
#include <inviwo/core/properties/isotfproperty.h>

#include <inviwo/topologytoolkit/ports/contourtreeport.h>
#include <inviwo/topologytoolkit/ports/morsesmalecomplexport.h>
#include <inviwo/core/datastructures/geometry/mesh.h>
#include <inviwo/core/datastructures/geometry/meshram.h>
#include <inviwo/topologytoolkit/utils/ttkutils.h>
#include <inviwo/topologytoolkit/utils/ttkexception.h>

#include <inviwo/core/interaction/pickingmanager.h>
#include <inviwo/core/interaction/events/pickingevent.h>
#include <inviwo/core/interaction/events/mouseevent.h>
#include <inviwo/core/interaction/events/touchevent.h>
#include <inviwo/core/interaction/events/wheelevent.h>

#include <inviwo/topologytoolkit/utils/pickingutils.h>
#include <inviwo/core/processors/progressbarowner.h>


#include <warn/push>
#include <warn/ignore/all>
#include <ttk/core/base/ftmTree/FTMTree.h>
#include <warn/pop>

namespace inviwo {

class IVW_MODULE_TOPOLOGYTOOLKIT_API VolumeTopologyColorMapper : public Processor,
                                                                 public ProgressBarOwner {
public:
    VolumeTopologyColorMapper();
    virtual ~VolumeTopologyColorMapper() = default;

    virtual void process() override;

	virtual void invalidate(InvalidationLevel invalidationLevel,
                            Property* source = nullptr) override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

    void handlePickingEvent(PickingEvent* p);

	void surfaceExtraction();

private:

    VolumeInport volumeInport_;
    topology::ContourTreeInport contourtreeInport;
    topology::MorseSmaleComplexInport morseSmaleComplexInport;
    //MeshOutport outport_;
    //MeshOutport isolinesOutport_;
	DataOutport<std::vector<std::shared_ptr<Mesh>>> outport_;

    TransferFunctionProperty transferFunction_;
    inviwo::topology::PickingUtil pickingutil_;
    inviwo::topology::PickingProperties pickingProperties_;
	ButtonProperty updateButton_;
    IsoTFProperty isotfComposite_;
    
	FloatProperty isoFactor_;
	CompositeProperty morseSmaleComplex_;
	OptionPropertyInt mscSegmentOptions_;

	OptionPropertyInt leafnodeOptions_;


	std::vector<std::shared_ptr<topology::TriangulationData>> segmentTriangulations_;
	bool recompute_ = true;
	std::shared_ptr<std::vector<std::shared_ptr<Mesh>>> meshes_ = nullptr;
};

}  // namespace inviwo

#endif  // IVW_VOL_CONTOURTREECOLORMAPPER_H
