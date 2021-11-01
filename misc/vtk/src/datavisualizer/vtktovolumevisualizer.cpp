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

#include <inviwo/vtk/datavisualizer/vtktovolumevisualizer.h>

#include <modules/opengl/canvasprocessorgl.h>

#include <inviwo/core/processors/processorutils.h>

#include <inviwo/vtk/ports/vtkdatasetport.h>

#include <inviwo/vtk/processors/vtktovolume.h>

#include <inviwo/core/datastructures/light/lightingstate.h>
#include <modules/base/processors/cubeproxygeometryprocessor.h>

#include <modules/base/processors/volumeboundingbox.h>
#include <modules/basegl/processors/background.h>
#include <modules/basegl/processors/entryexitpointsprocessor.h>
#include <modules/basegl/processors/linerendererprocessor.h>
#include <modules/basegl/processors/volumeraycaster.h>

#include <modules/basegl/datavisualizer/volumeraycastvisualizer.h>

namespace inviwo {

using GP = util::GridPos;

VTKToVolumeVisualizer::VTKToVolumeVisualizer(InviwoApplication* app)
    : DataVisualizer{}, app_(app) {}

std::string VTKToVolumeVisualizer::getName() const { return "VTKToVolume"; }

Document VTKToVolumeVisualizer::getDescription() const {
    Document doc;
    auto b = doc.append("html").append("body");
    b.append("", "Convert a vtk data set to an inviwo volume and apply raycasting.");
    return doc;
}

std::vector<FileExtension> VTKToVolumeVisualizer::getSupportedFileExtensions() const {
    return std::vector<FileExtension>{};
}

bool VTKToVolumeVisualizer::isOutportSupported(const Outport* port) const {
    return dynamic_cast<const VTKDataSetOutport*>(port) != nullptr;
}

bool VTKToVolumeVisualizer::hasSourceProcessor() const { return false; }
bool VTKToVolumeVisualizer::hasVisualizerNetwork() const { return true; }

std::pair<Processor*, Outport*> VTKToVolumeVisualizer::addSourceProcessor(const std::string&,
                                                                          ProcessorNetwork*) const {
    return {nullptr, nullptr};
}

std::vector<Processor*> VTKToVolumeVisualizer::addVisualizerNetwork(Outport* outport,
                                                                    ProcessorNetwork* net) const {
    auto con = net->addProcessor(util::makeProcessor<VTKtoVolume>(GP{0, 3}));

    auto cpg = net->addProcessor(util::makeProcessor<CubeProxyGeometry>(GP{1, 6}));
    auto eep = net->addProcessor(util::makeProcessor<EntryExitPoints>(GP{1, 9}));
    auto vrc = net->addProcessor(util::makeProcessor<VolumeRaycaster>(GP{0, 12}));
    auto bak = net->addProcessor(util::makeProcessor<Background>(GP{0, 15}));
    auto cvs = net->addProcessor(util::makeProcessor<CanvasProcessorGL>(GP{0, 18}));

    auto vbb = net->addProcessor(util::makeProcessor<VolumeBoundingBox>(GP{8, 6}));
    auto lrp = net->addProcessor(util::makeProcessor<LineRendererProcessor>(GP{8, 9}));

    trySetProperty<FloatVec4Property>(bak, "bgColor1", vec4(0.443f, 0.482f, 0.600f, 1.0f));
    trySetProperty<FloatVec4Property>(bak, "bgColor2", vec4(0.831f, 0.831f, 0.831f, 1.0f));

    trySetProperty<TemplateOptionProperty<ShadingMode>>(vrc, "shadingMode", ShadingMode::None,
                                                        true);
    trySetProperty<FloatVec3RefProperty>(vrc, "lookFrom", vec3(0.0f, 0.0f, 30.0f), true);
    trySetProperty<FloatProperty>(lrp, "lineSettings.lineWidth", 1.5f);

    net->addConnection(outport, con->getInports()[0]);

    net->addConnection(con->getOutports()[0], cpg->getInports()[0]);
    net->addConnection(cpg->getOutports()[0], eep->getInports()[0]);

    net->addConnection(eep->getOutports()[0], vrc->getInports()[1]);
    net->addConnection(eep->getOutports()[1], vrc->getInports()[2]);

    net->addConnection(con->getOutports()[0], vrc->getInports()[0]);
    net->addConnection(vrc->getOutports()[0], bak->getInports()[0]);
    net->addConnection(bak->getOutports()[0], cvs->getInports()[0]);

    net->addConnection(con->getOutports()[0], vbb->getInports()[0]);
    net->addConnection(vbb->getOutports()[0], lrp->getInports()[0]);
    net->addConnection(lrp->getOutports()[0], vrc->getInports()[3]);

    net->addLink(eep->getPropertyByIdentifier("camera"), vrc->getPropertyByIdentifier("camera"));
    net->addLink(vrc->getPropertyByIdentifier("camera"), eep->getPropertyByIdentifier("camera"));

    net->addLink(eep->getPropertyByIdentifier("camera"), lrp->getPropertyByIdentifier("camera"));
    net->addLink(lrp->getPropertyByIdentifier("camera"), eep->getPropertyByIdentifier("camera"));

    net->evaluateLinksFromProperty(vrc->getPropertyByIdentifier("camera"));

    return {con, cpg, eep, vrc, cvs, vbb, bak, lrp};
}

std::vector<Processor*> VTKToVolumeVisualizer::addSourceAndVisualizerNetwork(
    const std::string&, ProcessorNetwork*) const {

    return {nullptr};
}

}  // namespace inviwo
