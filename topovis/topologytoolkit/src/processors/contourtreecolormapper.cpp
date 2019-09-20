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

std::shared_ptr<Mesh> mapMeshToContourTree(const TransferFunction& tf, const Mesh& mesh,
										   const std::vector<int> &segments,
                                           const TriangulationData& triangulationData,
                                           MeshColorOption colorOption) {

    auto newMesh = std::make_shared<Mesh>(mesh);

    auto buffers = newMesh->getBuffers();
    auto isColorBuffer =
        [](const std::pair<Mesh::BufferInfo, std::shared_ptr<BufferBase>>& buffer) -> bool {
        return buffer.first.type == BufferType::ColorAttrib;
    };

    auto bufferIt = std::find_if(buffers.begin(), buffers.end(), isColorBuffer);
    if (bufferIt == buffers.end()) {
        throw TTKConversionException("Mesh does not contain a color buffer");
    } else if (std::find_if(bufferIt + 1, buffers.end(), isColorBuffer) != buffers.end()) {
        LogWarnCustom("topology::mapMeshToContourTree",
                      "Mesh features multiple color buffer, using only first one.");
    }

    auto colorBuffer = std::dynamic_pointer_cast<Buffer<vec4>>(bufferIt->second);
    std::vector<vec4>& colors = colorBuffer->getEditableRAMRepresentation()->getDataContainer();
    // auto minmax = std::minmax_element(colors.begin(), colors.end());

    if (colorOption == MeshColorOption::SEGMENT_COLORMAP) {
        //auto& segments = triangulationData.getSegments();

        auto minmax = std::minmax_element(segments.begin(), segments.end());
        auto div = (float)(*minmax.second - *minmax.first);

        for (size_t i = 0; i < std::min(segments.size(), colors.size()); ++i) {
            colors[i] = tf.sample(static_cast<float>((segments[i] - *minmax.first) / div));
        }
    } else {

        triangulationData.getScalarValues()
            ->getRepresentation<BufferRAM>()
            ->dispatch<void, dispatching::filter::Scalars>([&](auto bufferpr) {
                auto& scalars = bufferpr->getDataContainer();

                auto minmax = std::minmax_element(scalars.begin(), scalars.end());
                auto div = (float)(*minmax.second - *minmax.first);

                for (size_t i = 0; i < std::min(bufferpr->getSize(), colors.size()); ++i) {
                    colors[i] = tf.sample(static_cast<float>((scalars[i] - *minmax.first) / div));
                }
            });
    }

    return newMesh;
}

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
            topology::mapMeshToContourTree(transferFunction_.get(), *meshInport_.getData().get(),
											contourtreeInport.getData()->getSegments(),
                                           *contourtreeInport.getData()->triangulation.get());

        // set output mesh
        outport_.setData(mesh);
        return;
    }

    if (morseSmaleComplexInport.getData()) {
        auto mesh =
            topology::mapMeshToContourTree(transferFunction_.get(), *meshInport_.getData().get(),
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
