/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018-2024 Inviwo Foundation
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

#include <inviwo/core/io/datareaderfactory.h>

#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/processors/processorutils.h>
#include <inviwo/core/network/processornetwork.h>

#include <inviwo/tensorvisbase/ports/tensorfieldport.h>
#include <inviwo/tensorvisbase/datavisualizer/anisotropyraycastingvisualizer.h>
#include <inviwo/tensorvisbase/processors/tensorfield3danisotropy.h>

#include <modules/base/processors/cubeproxygeometryprocessor.h>
#include <modules/basegl/processors/entryexitpointsprocessor.h>
#include <modules/basegl/processors/volumeraycaster.h>
#include <modules/opengl/canvasprocessorgl.h>

namespace inviwo {

using GP = util::GridPos;

AnisotropyRaycastingVisualizer::AnisotropyRaycastingVisualizer(InviwoApplication* app)
    : DataVisualizer{}, app_(app) {}

std::string AnisotropyRaycastingVisualizer::getName() const { return "Anisotropy Raycasting"; }

Document AnisotropyRaycastingVisualizer::getDescription() const {
    Document doc;
    auto b = doc.append("html").append("body");
    b.append("", "Construct a anisotropy raycasting network");
    return doc;
}

std::vector<FileExtension> AnisotropyRaycastingVisualizer::getSupportedFileExtensions() const {
    return std::vector<FileExtension>{};
}

bool AnisotropyRaycastingVisualizer::isOutportSupported(const Outport* port) const {
    return dynamic_cast<const TensorField3DOutport*>(port) != nullptr;
}

bool AnisotropyRaycastingVisualizer::hasSourceProcessor() const { return false; }
bool AnisotropyRaycastingVisualizer::hasVisualizerNetwork() const { return true; }

std::pair<Processor*, Outport*> AnisotropyRaycastingVisualizer::addSourceProcessor(
    const std::filesystem::path&, ProcessorNetwork*, const ivec2&) const {
    return {nullptr, nullptr};
}

std::vector<Processor*> AnisotropyRaycastingVisualizer::addVisualizerNetwork(
    Outport* outport, ProcessorNetwork* net) const {
    const ivec2 initialPos = util::getPosition(outport->getProcessor());

    auto* aniso =
        net->addProcessor(util::makeProcessor<TensorField3DAnisotropy>(GP{0, 3} + initialPos));
    auto* cubep = net->addProcessor(util::makeProcessor<CubeProxyGeometry>(GP{1, 6} + initialPos));
    auto* entry = net->addProcessor(util::makeProcessor<EntryExitPoints>(GP{1, 9} + initialPos));
    auto* volra = net->addProcessor(util::makeProcessor<VolumeRaycaster>(GP{0, 12} + initialPos));
    auto* canva = net->addProcessor(util::makeProcessor<CanvasProcessorGL>(GP{0, 15} + initialPos));

    net->addConnection(aniso->getOutports()[0], cubep->getInports()[0]);
    net->addConnection(cubep->getOutports()[0], entry->getInports()[0]);
    net->addConnection(entry->getOutports()[0], volra->getInports()[1]);
    net->addConnection(entry->getOutports()[1], volra->getInports()[2]);
    net->addConnection(aniso->getOutports()[0], volra->getInports()[0]);
    net->addConnection(volra->getOutports()[0], canva->getInports()[0]);

    net->addConnection(outport, aniso->getInports()[0]);

    return {aniso, cubep, entry, volra, canva};
}

std::vector<Processor*> AnisotropyRaycastingVisualizer::addSourceAndVisualizerNetwork(
    const std::filesystem::path&, ProcessorNetwork*, const ivec2&) const {
    return {};
}

}  // namespace inviwo
