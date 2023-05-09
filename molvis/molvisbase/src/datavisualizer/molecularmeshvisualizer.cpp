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

#include <inviwo/molvisbase/datavisualizer/molecularmeshvisualizer.h>

#include <inviwo/molvisbase/processors/molecularstructuresource.h>
#include <inviwo/molvisbase/processors/molecularstructuretomesh.h>
#include <inviwo/molvisbase/ports/molecularstructureport.h>

#include <inviwo/core/processors/processorutils.h>
#include <inviwo/core/network/processornetwork.h>
#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/io/datareaderfactory.h>

namespace inviwo {

using GP = util::GridPos;

MolecularMeshVisualizer::MolecularMeshVisualizer(InviwoApplication* app)
    : DataVisualizer{}, app_(app) {}

std::string MolecularMeshVisualizer::getName() const { return "Molecular Mesh"; }

Document MolecularMeshVisualizer::getDescription() const {
    Document doc;
    auto b = doc.append("html").append("body");
    b.append("", "Construct a MolecularStructure mesh");
    return doc;
}

std::vector<FileExtension> MolecularMeshVisualizer::getSupportedFileExtensions() const {
    auto rf = app_->getDataReaderFactory();
    auto exts = rf->getExtensionsForType<molvis::MolecularStructure>();
    return exts;
}

bool MolecularMeshVisualizer::isOutportSupported(const Outport* port) const {
    return dynamic_cast<const molvis::MolecularStructureOutport*>(port) != nullptr;
}

bool MolecularMeshVisualizer::hasSourceProcessor() const { return true; }

bool MolecularMeshVisualizer::hasVisualizerNetwork() const { return true; }

std::pair<Processor*, Outport*> MolecularMeshVisualizer::addSourceProcessor(
    const std::filesystem::path& filename, ProcessorNetwork* net) const {
    auto source =
        net->addProcessor(util::makeProcessor<MolecularStructureSource>(GP{0, 0}, app_, filename));
    auto outport = source->getOutports().front();
    return {source, outport};
}

std::vector<Processor*> MolecularMeshVisualizer::addVisualizerNetwork(Outport* outport,
                                                                      ProcessorNetwork* net) const {
    auto msm = net->addProcessor(util::makeProcessor<MolecularStructureToMesh>(GP{0, 3}));

    net->addConnection(outport, msm->getInports()[0]);

    return {msm};
}

std::vector<Processor*> MolecularMeshVisualizer::addSourceAndVisualizerNetwork(
    const std::filesystem::path& filename, ProcessorNetwork* net) const {

    auto sourceAndOutport = addSourceProcessor(filename, net);
    auto processors = addVisualizerNetwork(sourceAndOutport.second, net);

    processors.push_back(sourceAndOutport.first);
    return processors;
}

}  // namespace inviwo
