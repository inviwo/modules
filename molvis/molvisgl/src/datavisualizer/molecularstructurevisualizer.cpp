/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020-2023 Inviwo Foundation
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

#include <inviwo/molvisgl/datavisualizer/molecularstructurevisualizer.h>

#include <inviwo/molvisbase/processors/molecularstructuresource.h>
#include <inviwo/molvisbase/ports/molecularstructureport.h>
#include <inviwo/molvisgl/processors/molecularrenderer.h>

#include <inviwo/core/processors/processorutils.h>
#include <inviwo/core/network/processornetwork.h>
#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/io/datareaderfactory.h>
#include <modules/opengl/canvasprocessorgl.h>
#include <modules/basegl/processors/background.h>

namespace inviwo {

using GP = util::GridPos;

MolecularStructureVisualizer::MolecularStructureVisualizer(InviwoApplication* app)
    : DataVisualizer{}, app_(app) {}

std::string MolecularStructureVisualizer::getName() const { return "Molecular Visualization"; }

Document MolecularStructureVisualizer::getDescription() const {
    Document doc;
    auto b = doc.append("html").append("body");
    b.append("", "Construct a network for molecular visualization");
    return doc;
}

std::vector<FileExtension> MolecularStructureVisualizer::getSupportedFileExtensions() const {
    auto rf = app_->getDataReaderFactory();
    auto exts = rf->getExtensionsForType<molvis::MolecularStructure>();
    return exts;
}

bool MolecularStructureVisualizer::isOutportSupported(const Outport* port) const {
    return dynamic_cast<const molvis::MolecularStructureOutport*>(port) != nullptr;
}

bool MolecularStructureVisualizer::hasSourceProcessor() const { return true; }

bool MolecularStructureVisualizer::hasVisualizerNetwork() const { return true; }

std::pair<Processor*, Outport*> MolecularStructureVisualizer::addSourceProcessor(
    const std::string& filename, ProcessorNetwork* net) const {
    auto source =
        net->addProcessor(util::makeProcessor<MolecularStructureSource>(GP{0, 0}, app_, filename));
    auto outport = source->getOutports().front();
    return {source, outport};
}

std::vector<Processor*> MolecularStructureVisualizer::addVisualizerNetwork(
    Outport* outport, ProcessorNetwork* net) const {
    auto mr = net->addProcessor(util::makeProcessor<MolecularRenderer>(GP{0, 3}));
    auto bg = net->addProcessor(util::makeProcessor<Background>(GP{0, 5}));
    auto cvs = net->addProcessor(util::makeProcessor<CanvasProcessorGL>(GP{0, 7}));

    auto bgProc = static_cast<Background*>(bg);
    bgProc->backgroundStyle_.setSelectedValue(Background::BackgroundStyle::Uniform);
    bgProc->bgColor1_ = vec4{1.0f, 1.0f, 1.0f, 1.0f};

    net->addConnection(outport, mr->getInports()[0]);
    net->addConnection(mr->getOutports()[0], bg->getInports()[0]);
    net->addConnection(bg->getOutports()[0], cvs->getInports()[0]);

    return {mr, bg, cvs};
}

std::vector<Processor*> MolecularStructureVisualizer::addSourceAndVisualizerNetwork(
    const std::string& filename, ProcessorNetwork* net) const {

    auto sourceAndOutport = addSourceProcessor(filename, net);
    auto processors = addVisualizerNetwork(sourceAndOutport.second, net);

    processors.push_back(sourceAndOutport.first);
    return processors;
}

}  // namespace inviwo
