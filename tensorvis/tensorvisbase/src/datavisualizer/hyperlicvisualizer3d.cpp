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

#include <inviwo/tensorvisbase/datavisualizer/hyperlicvisualizer3d.h>

#include <modules/opengl/canvasprocessorgl.h>
#include <modules/base/processors/noiseprocessor.h>
#include <inviwo/core/processors/processorutils.h>
#include <inviwo/tensorvisbase/ports/tensorfieldport.h>
#include <inviwo/tensorvisbase/processors/tensorfield3dslice.h>
#include <inviwo/tensorvisbase/processors/tensorfield2dlic.h>

#include <inviwo/core/io/datareaderfactory.h>

namespace inviwo {

using GP = util::GridPos;

HyperLICVisualizer3D::HyperLICVisualizer3D(InviwoApplication* app) : DataVisualizer{}, app_(app) {}

std::string HyperLICVisualizer3D::getName() const { return "HyperLIC"; }

Document HyperLICVisualizer3D::getDescription() const {
    Document doc;
    auto b = doc.append("html").append("body");
    b.append("", "Construct a HyperLIC network");
    return doc;
}

std::vector<FileExtension> HyperLICVisualizer3D::getSupportedFileExtensions() const {
    return std::vector<FileExtension>{};
}

bool HyperLICVisualizer3D::isOutportSupported(const Outport* port) const {
    return dynamic_cast<const TensorField3DOutport*>(port) != nullptr;
}

bool HyperLICVisualizer3D::hasSourceProcessor() const { return false; }
bool HyperLICVisualizer3D::hasVisualizerNetwork() const { return true; }

std::pair<Processor*, Outport*> HyperLICVisualizer3D::addSourceProcessor(const std::string&,
                                                                         ProcessorNetwork*) const {
    return {nullptr, nullptr};
}

std::vector<Processor*> HyperLICVisualizer3D::addVisualizerNetwork(Outport* outport,
                                                                   ProcessorNetwork* net) const {

    auto slicer = net->addProcessor(util::makeProcessor<TensorField3DSlice>(GP{0, 3}));
    auto noiser = net->addProcessor(util::makeProcessor<NoiseProcessor>(GP{1, 6}));
    auto hlicer = net->addProcessor(util::makeProcessor<TensorField2DLIC>(GP{0, 9}));
    auto canvas = net->addProcessor(util::makeProcessor<CanvasProcessorGL>(GP{0, 12}));

    net->addConnection(slicer->getOutports()[0], hlicer->getInports()[0]);
    net->addConnection(noiser->getOutports()[0], hlicer->getInports()[1]);
    net->addConnection(hlicer->getOutports()[0], canvas->getInports()[0]);

    net->addConnection(outport, slicer->getInports()[0]);

    return {slicer, hlicer, noiser, canvas};
}

std::vector<Processor*> HyperLICVisualizer3D::addSourceAndVisualizerNetwork(
    const std::string&, ProcessorNetwork*) const {

    return {nullptr};
}

}  // namespace inviwo
