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

#include <inviwo/electrostatics/processors/segmentation.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo segmentation::processorInfo_{
    "org.inviwo.segmentation",      // Class identifier
    "Atom volumes",                // Display name
    "Undefined",              // Category
    CodeState::Experimental,  // Code state
    Tags::None,               // Tags
};
const ProcessorInfo segmentation::getProcessorInfo() const { return processorInfo_; }

segmentation::segmentation()
    : Processor()
    , inport_{"inport"}
    , volumePort_("volume")
    , volumeAllPort_("AllVolumes")
    {

    addPort(inport_);
    addPort(volumePort_);
    addPort(volumeAllPort_);
}

void segmentation::process() { 
	auto msc = inport_.getData(); 
	const auto loadedVolume = volumePort_.getData();
    const auto volumeRAM = loadedVolume->getRepresentation<VolumeRAM>();
    const size3_t volumeDims = loadedVolume->getDimensions();
    int totVox = volumeDims.x * volumeDims.y * volumeDims.z;
	
	auto allVol = std::make_shared<std::vector<std::shared_ptr<Volume>>>();
	auto& seg = msc->segmentation.descending;
    int MAX_SEG = *std::max_element(seg.begin(), seg.end());

	for (int j = 0; j < MAX_SEG + 1; j++) {
        auto volOUT = volumeRAM->dispatch<std::shared_ptr<Volume>, dispatching::filter::Scalars>(
            [&](auto inRam) {
                auto ram =
                    std::make_shared<VolumeRAMPrecision<util::PrecisionValueType<decltype(inRam)>>>(
                        loadedVolume->getDimensions());
                auto outdata = ram->getDataTyped();
                auto indata = inRam->getDataTyped();
                for (size_t i = 0; i < totVox; i++) {
          
                    if (seg[i] == j) {
                        outdata[i] = -1 * indata[i];
                    }
                }

                return std::make_shared<Volume>(ram);
            });
        volOUT->setModelMatrix(loadedVolume->getModelMatrix());
        volOUT->setWorldMatrix(loadedVolume->getWorldMatrix());
        volOUT->copyMetaDataFrom(*loadedVolume);
        allVol->push_back(volOUT);
    }
   

    volumeAllPort_.setData(allVol);




}

}  // namespace inviwo
