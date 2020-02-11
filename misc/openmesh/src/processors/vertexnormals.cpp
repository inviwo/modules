/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2020 Inviwo Foundation
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

#include <inviwo/openmesh/processors/vertexnormals.h>
#include <inviwo/core/datastructures/geometry/mesh.h>
#include <inviwo/openmesh/utils/openmeshconverters.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VertexNormals::processorInfo_{
    "org.inviwo.openmesh.VertexNormals",  // Class identifier
    "Vertex Normals",                     // Display name
    "Mesh Processing",                    // Category
    CodeState::Stable,                    // Code state
    Tags::CPU,                            // Tags
};
const ProcessorInfo VertexNormals::getProcessorInfo() const { return processorInfo_; }

VertexNormals::VertexNormals() : Processor() {

    addPort(inport_);
    addPort(outport_);
    addProperty(override_);
}

void VertexNormals::process() {
    if (inport_.getData()->findBuffer(BufferType::NormalAttrib).first && !override_) {
        outport_.setData(inport_.getData());
        return;
    }

    auto mesh = openmeshutil::fromInviwo(*inport_.getData(),
                                         openmeshutil::TransformCoordinates::DataToModel);

    // generate face normals in order to update the vertex normals
    mesh.request_face_normals();
    mesh.update_normals();
    mesh.release_face_normals();

    auto newMesh = openmeshutil::toInviwo(mesh);
    newMesh->copyMetaDataFrom(*inport_.getData());
    newMesh->setWorldMatrix(inport_.getData()->getWorldMatrix());
    outport_.setData(newMesh);
}

}  // namespace inviwo
