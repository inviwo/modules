/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018-2020 Inviwo Foundation
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

#include <inviwo/openmesh/processors/meshdecimationprocessor.h>
#include <inviwo/openmesh/utils/meshdecimation.h>
#include <inviwo/openmesh/utils/openmeshconverters.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo MeshDecimationProcessor::processorInfo_{
    "org.inviwo.openmesh.MeshDecimationProcessor",  // Class identifier
    "Mesh Decimation",                              // Display name
    "Mesh Processing",                              // Category
    CodeState::Experimental,                        // Code state
    Tags::CPU,                                      // Tags
};
const ProcessorInfo MeshDecimationProcessor::getProcessorInfo() const { return processorInfo_; }

MeshDecimationProcessor::MeshDecimationProcessor() : Processor() {
    addPort(inmesh_);
    addPort(outmesh_);

    addProperty(vertDecimation_);
    addProperty(faceDecimation_);
}

void MeshDecimationProcessor::process() {
    //! [OpenMesh Decimation]
    using namespace openmeshutil;
    auto mesh = fromInviwo(*inmesh_.getData(), TransformCoordinates::DataToModel);
    decimate(mesh, vertDecimation_.get(), faceDecimation_.get());
    auto newMesh = toInviwo(mesh);
    newMesh->copyMetaDataFrom(*inmesh_.getData());
    newMesh->setWorldMatrix(inmesh_.getData()->getWorldMatrix());
    //! [OpenMesh Decimation]
    outmesh_.setData(newMesh);
}

}  // namespace inviwo
