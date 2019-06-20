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

#include <inviwo/topologytoolkit/processors/meshtotriangulation.h>
#include <inviwo/topologytoolkit/utils/ttkutils.h>
#include <inviwo/core/datastructures/buffer/buffer.h>

#include <fmt/format.h>
#include <fmt/ostream.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo MeshToTriangulation::processorInfo_{
    "org.inviwo.ttk.MeshToTriangulation",  // Class identifier
    "Mesh To Triangulation",               // Display name
    "Topology",                            // Category
    CodeState::Experimental,               // Code state
    "CPU, Topology, TTK, Triangulation",   // Tags
};
const ProcessorInfo MeshToTriangulation::getProcessorInfo() const { return processorInfo_; }

MeshToTriangulation::MeshToTriangulation()
    : Processor()
    , meshInport_("mesh")
    , outport_("outport")
    , selectedBuffer_("selectedBuffer", "Buffer")
    , component_("component", "Component") {

    addPort(meshInport_);
    addPort(outport_);

    selectedBuffer_.addOption("none", "None", 0);
    selectedBuffer_.setSerializationMode(PropertySerializationMode::All);
    selectedBuffer_.setReadOnly(true);
    selectedBuffer_.setCurrentStateAsDefault();

    component_.addOption("component1", "x component", 0);
    component_.setSerializationMode(PropertySerializationMode::All);
    component_.setReadOnly(true);
    component_.setCurrentStateAsDefault();

    addProperty(selectedBuffer_);
    addProperty(component_);

    auto updateComponents = [this]() {
        component_.setReadOnly(selectedBuffer_.getReadOnly());
        if (meshInport_.hasData()) {
            if (selectedBuffer_.get() >=
                static_cast<int>(meshInport_.getData()->getNumberOfBuffers())) {
                // invalid buffer selection
                component_.setReadOnly(true);
                return;
            }
            const size_t components = meshInport_.getData()
                                          ->getBuffer(selectedBuffer_.get())
                                          ->getDataFormat()
                                          ->getComponents();
            component_.setReadOnly(components == 0);
            if (components == component_.size()) return;

            std::vector<OptionPropertyIntOption> options;
            const char comp[] = "xyzw";
            for (size_t i = 0; i < components; i++) {
                options.emplace_back(fmt::format("component{}", std::toupper(comp[i])),
                                     std::string(1, comp[i]) + " component", static_cast<int>(i));
            }
            component_.replaceOptions(options);
            component_.setCurrentStateAsDefault();
        }
    };

    selectedBuffer_.onChange(updateComponents);

    meshInport_.onChange([this]() {
        if (meshInport_.hasData()) {
            std::vector<OptionPropertyIntOption> options;
            for (size_t i = 0; i < meshInport_.getData()->getNumberOfBuffers(); ++i) {
                meshInport_.getData()->getBufferInfo(i);
                options.emplace_back(
                    "buffer" + std::to_string(i),
                    fmt::format("{} ({})", meshInport_.getData()->getBufferInfo(i).type, i),
                    static_cast<int>(i));
            }
            selectedBuffer_.replaceOptions(options);
            selectedBuffer_.setReadOnly(options.empty());
            selectedBuffer_.setCurrentStateAsDefault();
        } else {
            selectedBuffer_.setReadOnly(true);
        }
    });
}

void MeshToTriangulation::process() {
    auto data = std::make_shared<topology::TriangulationData>(
        topology::meshToTTKTriangulation(*meshInport_.getData().get()));

    // set data associated with vertex positions
    data->setScalarValues(*meshInport_.getData()->getBuffer(selectedBuffer_.get()),
                          component_.get());
    outport_.setData(data);
}

}  // namespace inviwo
