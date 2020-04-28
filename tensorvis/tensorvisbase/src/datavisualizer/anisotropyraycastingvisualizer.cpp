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

#include <inviwo/core/io/datareaderfactory.h>
#include <inviwo/core/processors/processorutils.h>

#include <inviwo/tensorvisbase/ports/tensorfieldport.h>
#include <inviwo/tensorvisbase/datavisualizer/anisotropyraycastingvisualizer.h>
#include <inviwo/tensorvisbase/processors/tensorfield3dtovolume.h>
#include <inviwo/core/util/constexprhash.h>
#include <inviwo/tensorvisbase/datastructures/attributes.h>

#include <modules/base/processors/cubeproxygeometryprocessor.h>
#include <modules/basegl/processors/entryexitpointsprocessor.h>
#include <modules/basegl/processors/volumeraycaster.h>
#include <modules/opengl/canvasprocessorgl.h>
#include <modules/basegl/processors/background.h>

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
    const std::string&, ProcessorNetwork*) const {
    return {nullptr, nullptr};
}

std::vector<Processor*> AnisotropyRaycastingVisualizer::addVisualizerNetwork(
    Outport* outport, ProcessorNetwork* net) const {

    auto aniso = net->addProcessor(util::makeProcessor<TensorField3DToVolume>(GP{0, 3}));
    auto cubep = net->addProcessor(util::makeProcessor<CubeProxyGeometry>(GP{1, 6}));
    auto entry = net->addProcessor(util::makeProcessor<EntryExitPoints>(GP{1, 9}));
    auto volra = static_cast<VolumeRaycaster*>(
        net->addProcessor(util::makeProcessor<VolumeRaycaster>(GP{0, 12})));
    auto backg = net->addProcessor(util::makeProcessor<Background>(GP{0, 15}));
    auto canva = net->addProcessor(util::makeProcessor<CanvasProcessorGL>(GP{0, 18}));

    net->addConnection(aniso->getOutports()[0], cubep->getInports()[0]);
    net->addConnection(cubep->getOutports()[0], entry->getInports()[0]);
    net->addConnection(entry->getOutports()[0], volra->getInports()[1]);
    net->addConnection(entry->getOutports()[1], volra->getInports()[2]);
    net->addConnection(aniso->getOutports()[0], volra->getInports()[0]);
    net->addConnection(volra->getOutports()[0], backg->getInports()[0]);
    net->addConnection(backg->getOutports()[0], canva->getInports()[0]);

    net->addConnection(outport, aniso->getInports()[0]);

    if (auto prop =
            static_cast<OptionPropertySize_t*>(aniso->getPropertyByIdentifier("volumeContent"))) {
        const auto& identifiers = prop->getIdentifiers();

        if (std::find(identifiers.begin(), identifiers.end(),
                      std::string(attributes::Anisotropy::identifier)) != identifiers.end()) {
            prop->set(util::constexpr_hash(attributes::Anisotropy::identifier));
        }
    }

    TransferFunction tf;
    tf.load(InviwoApplication::getPtr()->getBasePath() +
            "/data/transferfunctions/matplotlib/viridis.itf");

    volra->getPropertiesByType<IsoTFProperty>().front()->tf_.set(tf);

    return {aniso, cubep, entry, volra, backg, canva};
}

std::vector<Processor*> AnisotropyRaycastingVisualizer::addSourceAndVisualizerNetwork(
    const std::string&, ProcessorNetwork*) const {

    return {nullptr};
}

}  // namespace inviwo
