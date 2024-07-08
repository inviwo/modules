/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020-2024 Inviwo Foundation
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

#include <inviwo/molvisgl/datavisualizer/molecularmeshrendervisualizer.h>

#include <inviwo/molvisbase/processors/molecularstructuresource.h>
#include <inviwo/molvisbase/processors/molecularstructuretomesh.h>
#include <inviwo/molvisbase/ports/molecularstructureport.h>
#include <inviwo/molvisgl/processors/molecularmeshrenderer.h>

#include <inviwo/core/processors/processorutils.h>
#include <inviwo/core/network/processornetwork.h>
#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/io/datareaderfactory.h>
#include <modules/opengl/canvasprocessorgl.h>
#include <modules/basegl/processors/background.h>

namespace inviwo {

using GP = util::GridPos;

MolecularMeshRenderVisualizer::MolecularMeshRenderVisualizer(InviwoApplication* app)
    : DataVisualizer{}, app_(app) {}

std::string MolecularMeshRenderVisualizer::getName() const { return "Molecular Mesh Renderer"; }

Document MolecularMeshRenderVisualizer::getDescription() const {
    Document doc;
    auto b = doc.append("html").append("body");
    b.append("", "Construct a network for molecular visualization using a mesh");
    return doc;
}

std::vector<FileExtension> MolecularMeshRenderVisualizer::getSupportedFileExtensions() const {
    auto rf = app_->getDataReaderFactory();
    auto exts = rf->getExtensionsForType<molvis::MolecularStructure>();
    return exts;
}

bool MolecularMeshRenderVisualizer::isOutportSupported(const Outport* port) const {
    return dynamic_cast<const molvis::MolecularStructureOutport*>(port) != nullptr;
}

bool MolecularMeshRenderVisualizer::hasSourceProcessor() const { return true; }

bool MolecularMeshRenderVisualizer::hasVisualizerNetwork() const { return true; }

std::pair<Processor*, Outport*> MolecularMeshRenderVisualizer::addSourceProcessor(
    const std::filesystem::path& filename, ProcessorNetwork* net, const ivec2& origin) const {
    auto* source = net->addProcessor(
        util::makeProcessor<MolecularStructureSource>(GP{0, 0} + origin, app_, filename));
    auto* outport = source->getOutports().front();
    return {source, outport};
}

std::vector<Processor*> MolecularMeshRenderVisualizer::addVisualizerNetwork(
    Outport* outport, ProcessorNetwork* net) const {
    const ivec2 origin = util::getPosition(outport->getProcessor());

    auto* msm = net->addProcessor(util::makeProcessor<MolecularStructureToMesh>(GP{0, 3} + origin));
    auto* mmr = net->addProcessor(util::makeProcessor<MolecularMeshRenderer>(GP{0, 6} + origin));
    auto* bg = net->addProcessor(util::makeProcessor<Background>(GP{0, 8} + origin));
    auto* cvs = net->addProcessor(util::makeProcessor<CanvasProcessorGL>(GP{0, 10} + origin));

    auto* bgProc = static_cast<Background*>(bg);
    bgProc->backgroundStyle_.setSelectedValue(Background::BackgroundStyle::Uniform);
    bgProc->bgColor1_ = vec4{1.0f, 1.0f, 1.0f, 1.0f};

    net->addConnection(outport, msm->getInports()[0]);
    net->addConnection(msm->getOutports()[0], mmr->getInports()[0]);
    net->addConnection(mmr->getOutports()[0], bg->getInports()[0]);
    net->addConnection(bg->getOutports()[0], cvs->getInports()[0]);

    return {msm, mmr, bg, cvs};
}

std::vector<Processor*> MolecularMeshRenderVisualizer::addSourceAndVisualizerNetwork(
    const std::filesystem::path& filename, ProcessorNetwork* net, const ivec2& origin) const {

    auto sourceAndOutport = addSourceProcessor(filename, net, origin);
    auto processors = addVisualizerNetwork(sourceAndOutport.second, net);

    processors.push_back(sourceAndOutport.first);
    return processors;
}

}  // namespace inviwo
