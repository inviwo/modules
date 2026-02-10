/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2026 Inviwo Foundation
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

#include <infravis/astrophysics/processors/fitsvolumeraycaster.h>

#include <inviwo/core/algorithm/boundingbox.h>
#include <modules/basegl/shadercomponents/shadercomponentutil.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo FitsVolumeRaycaster::processorInfo_{
    "org.infravis.FitsVolumeRaycaster",                         // Class identifier
    "Fits Volume Raycaster",                                    // Display name
    "Volume Rendering",                                         // Category
    CodeState::Experimental,                                    // Code state
    Tags::GL | Tag{"Volume"} | Tag{"Raycaster"} | Tag{"Fits"},  // Tags
    R"(Visualizing volumetric astrophysical observation data by also taking non-linear 
    depth into consideration.)"_unindentHelp,
};

const ProcessorInfo& FitsVolumeRaycaster::getProcessorInfo() const { return processorInfo_; }

FitsVolumeRaycaster::FitsVolumeRaycaster(std::string_view identifier, std::string_view displayName)
    : VolumeRaycasterBase{identifier, displayName}
    //, volume_{"volume", VolumeComponent::Gradients::Single,
    //          "input volume (Only one channel will be rendered)"_help}
    , fits_{"volume"}
    , entryExit_{}
    , background_{*this}
    , isoTF_{fits_.volumePort_}
    , raycasting_{fits_.getName(), isoTF_.isotfs[0]}
    , camera_{"camera", util::boundingBox(fits_.volumePort_)}
    , light_{&camera_.camera}
{

    registerComponents(fits_, entryExit_, background_, raycasting_, isoTF_, camera_,
                       light_);
}

void FitsVolumeRaycaster::process() {
    util::checkValidChannel(raycasting_.selectedChannel(), fits_.channelsForVolume().value_or(0));
    VolumeRaycasterBase::process();
}

}  // namespace inviwo
