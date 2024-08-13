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

#include <inviwo/molvisbase/datavisualizer/molecularsourcevisualizer.h>

#include <inviwo/molvisbase/processors/molecularstructuresource.h>
#include <inviwo/molvisbase/ports/molecularstructureport.h>

#include <inviwo/core/processors/processorutils.h>
#include <inviwo/core/network/processornetwork.h>
#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/io/datareaderfactory.h>

namespace inviwo {

using GP = util::GridPos;

MolecularSourceVisualizer::MolecularSourceVisualizer(InviwoApplication* app)
    : DataVisualizer{}, app_(app) {}

std::string MolecularSourceVisualizer::getName() const { return "Molecular Source"; }

Document MolecularSourceVisualizer::getDescription() const {
    Document doc;
    auto b = doc.append("html").append("body");
    b.append("", "Construct a molecular source processor");
    return doc;
}

std::vector<FileExtension> MolecularSourceVisualizer::getSupportedFileExtensions() const {
    auto rf = app_->getDataReaderFactory();
    auto exts = rf->getExtensionsForType<molvis::MolecularStructure>();
    return exts;
}

bool MolecularSourceVisualizer::isOutportSupported(const Outport* port) const {
    return dynamic_cast<const molvis::MolecularStructureOutport*>(port) != nullptr;
}

bool MolecularSourceVisualizer::hasSourceProcessor() const { return true; }

bool MolecularSourceVisualizer::hasVisualizerNetwork() const { return false; }

std::pair<Processor*, Outport*> MolecularSourceVisualizer::addSourceProcessor(
    const std::filesystem::path& filename, ProcessorNetwork* net, const ivec2& origin) const {
    auto* source = net->addProcessor(
        util::makeProcessor<MolecularStructureSource>(GP{0, 0} + origin, app_, filename));
    auto* outport = source->getOutports().front();
    return {source, outport};
}

std::vector<Processor*> MolecularSourceVisualizer::addVisualizerNetwork(Outport*,
                                                                        ProcessorNetwork*) const {
    return {};
}

std::vector<Processor*> MolecularSourceVisualizer::addSourceAndVisualizerNetwork(
    const std::filesystem::path&, ProcessorNetwork*, const ivec2&) const {
    return {};
}

}  // namespace inviwo
