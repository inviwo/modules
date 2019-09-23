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

#include <inviwo/topologytoolkit/processors/contourtreecolormapper.h>

namespace inviwo {

namespace topology {


}  // namespace topology

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo ContourTreeColorMapper::processorInfo_{
    "org.inviwo.ContourTreeColorMapper",  // Class identifier
    "Contour Tree Color Mapper",          // Display name
    "Undefined",                          // Category
    CodeState::Experimental,              // Code state
    Tags::None,                           // Tags
};
const ProcessorInfo ContourTreeColorMapper::getProcessorInfo() const { return processorInfo_; }

ContourTreeColorMapper::ContourTreeColorMapper()
    : Processor()
    , contourtreeInport("contourtree")
    , morseSmaleComplexInport("morsesmalecomplex")
    , meshInport_("mesh")
    , outport_("outport")
    , transferFunction_("transferFunction", "TranferFunction") {

    addPort(meshInport_);
    addPort(contourtreeInport);
    addPort(morseSmaleComplexInport);
    addPort(outport_);

    addProperty(transferFunction_);

    contourtreeInport.setOptional(true);
    morseSmaleComplexInport.setOptional(true);
}

void ContourTreeColorMapper::process() {
	
	if (contourtreeInport.getData()) {
        auto mesh =
            topology::applyColorMapToMesh(transferFunction_.get(), *meshInport_.getData().get(),
											contourtreeInport.getData()->getSegments(),
                                           *contourtreeInport.getData()->triangulation.get());

        // set output mesh
        outport_.setData(mesh);
        return;
    }

    if (morseSmaleComplexInport.getData()) {
        auto mesh =
            topology::applyColorMapToMesh(transferFunction_.get(), *meshInport_.getData().get(),
                                           morseSmaleComplexInport.getData()->segmentation.msc,
                                           *morseSmaleComplexInport.getData()->triangulation.get());

        // set output mesh
        outport_.setData(mesh);
        return;
    } 


	//default original mesh
    outport_.setData(meshInport_.getData());

    
}

}  // namespace inviwo
