/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018-2019 Inviwo Foundation
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

#include <inviwo/topologytoolkit/processors/triangulationtomesh.h>
#include <inviwo/topologytoolkit/utils/ttkutils.h>
#include <inviwo/core/util/exception.h>
#include <inviwo/core/datastructures/buffer/bufferram.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TriangulationToMesh::processorInfo_{
    "org.inviwo.ttk.TriangulationToMesh",  // Class identifier
    "Triangulation To Mesh",               // Display name
    "Topology",                            // Category
    CodeState::Experimental,               // Code state
    "CPU, Topology, TTK, Triangulation",   // Tags
};
const ProcessorInfo TriangulationToMesh::getProcessorInfo() const { return processorInfo_; }

TriangulationToMesh::TriangulationToMesh()
    : Processor()
    , inport_("triangulation")
    , outport_("outport")
    , color_("color", "Mesh Color", vec4(1.0f), vec4(0.0f), vec4(1.0f))
    , mapScalars_("mapScalars", "Map Scalars to Position", false)
    , component_("component", "Map to Component",
                 {{"componentX", "x component", 0},
                  {"componentY", "y component", 1},
                  {"componentZ", "z component", 2}},
                 0) {

    addPort(inport_);
    addPort(outport_);

    color_.setSemantics(PropertySemantics::Color);

    addProperty(color_);
    addProperty(mapScalars_);
    addProperty(component_);

    // scalar mapping is only available if input triangulation features scalars
    mapScalars_.setReadOnly(true);
    component_.setReadOnly(true);

    inport_.onChange([this]() {
        if (inport_.hasData()) {
            const bool readonly = !inport_.getData()->getScalarValues();
            mapScalars_.setReadOnly(readonly);
            component_.setReadOnly(readonly);
        } else {
            mapScalars_.setReadOnly(true);
            component_.setReadOnly(true);
        }
    });
}

void TriangulationToMesh::process() {
    // set output mesh
    auto mesh = topology::ttkTriangulationToMesh(*inport_.getData().get(), color_.get(),
                                                 mapScalars_.get(), component_.get());

    outport_.setData(mesh);
}

}  // namespace inviwo
