/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2026 Inviwo Foundation
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

#include <modules/tensorvisbase/processors/vectorfieldlic.h>
#include <algorithm>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VectorFieldLIC::processorInfo_{
    "org.inviwo.VectorFieldLIC",  // Class identifier
    "Vector Field LIC",           // Display name
    "Tensor visualization",       // Category
    CodeState::Experimental,      // Code state
    Tags::GL,                     // Tags
};

const ProcessorInfo& VectorFieldLIC::getProcessorInfo() const { return processorInfo_; }

VectorFieldLIC::VectorFieldLIC()
    : Processor()
    , vectorField_("vectorField")
    , outport_("outport")
    , noiseTexture_("noiseTexture", true)
    , samples_("samples", "Number of steps", 20, 3, 100)
    , stepLength_("stepLength", "Step Length", 0.003f, 0.0001f, 0.01f, 0.0001f)
    , normalizeVectors_("normalizeVectors", "Normalize vectors", true)
    , intensityMapping_("intensityMapping", "Enable intensity remapping", false)
    , shader_("vectorlic2d.frag")
    , useRK4_("useRK4", "Use Runge-Kutta4", true)
    , imageInport_("imageInport") {
    imageInport_.setOptional(true);

    addProperty(samples_);
    addProperty(stepLength_);
    addProperty(normalizeVectors_);
    addProperty(intensityMapping_);
    addProperty(useRK4_);

    shader_.onReload([&]() { invalidate(InvalidationLevel::InvalidOutput); });

    addPort(vectorField_);
    addPort(noiseTexture_);
    addPort(outport_);
    addPort(imageInport_);
}

void VectorFieldLIC::initializeResources() {}

void VectorFieldLIC::process() {
    utilgl::activateAndClearTarget(outport_);

    shader_.activate();
    TextureUnitContainer units;
    // add vector field texture to texture unit container
    utilgl::bindAndSetUniforms(shader_, units, vectorField_, ImageType::ColorOnly);
    // add noise texture to texture unit container
    utilgl::bindAndSetUniforms(shader_, units, noiseTexture_, ImageType::ColorOnly);

    bool hasInputImage = imageInport_.isConnected();

    if (hasInputImage) {
        utilgl::bindAndSetUniforms(shader_, units, imageInport_, ImageType::ColorOnly);
    }

    utilgl::setUniforms(shader_, outport_, samples_, stepLength_, normalizeVectors_,
                        intensityMapping_, useRK4_);
    shader_.setUniform("hasInputImage", hasInputImage);

    utilgl::singleDrawImagePlaneRect();
    shader_.deactivate();
    utilgl::deactivateCurrentTarget();
}

}  // namespace inviwo
