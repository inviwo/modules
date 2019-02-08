/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018 Inviwo Foundation
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

#include <modules/openmesh/processors/meshdecimationprocessor.h>
#include <modules/openmesh/utils/meshdecimation.h>
#include <modules/openmesh/utils/openmeshconverters.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo MeshDecimationProcessor::processorInfo_{
    "org.inviwo.MeshDecimationProcessor",  // Class identifier
    "Mesh Decimation",                     // Display name
    "Mesh Processing",                     // Category
    CodeState::Experimental,               // Code state
    Tags::None,                            // Tags
};
const ProcessorInfo MeshDecimationProcessor::getProcessorInfo() const { return processorInfo_; }

MeshDecimationProcessor::MeshDecimationProcessor() : Processor() {
    addPort(inmesh_);
    addPort(outmesh_);

    addProperty(vertDecimation_);
    addProperty(faceDecimation_);
}

void MeshDecimationProcessor::process() {
    auto mesh = openmeshutil::fromInviwo(*inmesh_.getData(),openmeshutil::TransformCoordinates::DataToModel);
    openmeshutil::decimate(mesh, vertDecimation_.get() , faceDecimation_.get());
    auto newMesh = openmeshutil::toInviwo(mesh);
    newMesh->copyMetaDataFrom(*inmesh_.getData());
    newMesh->setWorldMatrix(inmesh_.getData()->getWorldMatrix());
    //newMesh->setModelMatrix(inmesh_.getData()->getModelMatrix());
    outmesh_.setData(newMesh);
}

}  // namespace inviwo
