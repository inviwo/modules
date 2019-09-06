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

#include <inviwo/topologytoolkit/processors/contourtreetodataframe.h>
#include <inviwo/topologytoolkit/processors/contourtreetomesh.h>
#include <inviwo/topologytoolkit/processors/morsesmalecomplex.h>
#include <inviwo/topologytoolkit/processors/morsesmalecomplextomesh.h>
#include <inviwo/topologytoolkit/processors/separatrixrefiner.h>
#include <inviwo/topologytoolkit/topologytoolkitmodule.h>
#include <inviwo/topologytoolkit/processors/ttktestprocessor.h>
#include <inviwo/topologytoolkit/processors/meshtotriangulation.h>
#include <inviwo/topologytoolkit/processors/volumetotriangulation.h>
#include <inviwo/topologytoolkit/processors/triangulationtomesh.h>
#include <inviwo/topologytoolkit/processors/persistencecurve.h>
#include <inviwo/topologytoolkit/processors/topologicalsimplification.h>
#include <inviwo/topologytoolkit/processors/persistencediagram.h>
#include <inviwo/topologytoolkit/processors/triangulationtovolume.h>
#include <inviwo/topologytoolkit/processors/contourtree.h>
#include <inviwo/topologytoolkit/properties/topologycolorsproperty.h>

namespace inviwo {

TopologyToolKitModule::TopologyToolKitModule(InviwoApplication* app)
    : InviwoModule(app, "TopologyToolKit") {
    registerProcessor<ContourTreeToDataFrame>();
    registerProcessor<ContourTreeToMesh>();
    registerProcessor<MorseSmaleComplex>();
    registerProcessor<MorseSmaleComplexToMesh>();
    registerProcessor<TTKTestProcessor>();
    registerProcessor<MeshToTriangulation>();
    registerProcessor<SeparatrixRefiner>();
    registerProcessor<VolumeToTriangulation>();
    registerProcessor<TriangulationToMesh>();
    registerProcessor<PersistenceCurve>();
    registerProcessor<TopologicalSimplification>();
    registerProcessor<PersistenceDiagram>();
    registerProcessor<TriangulationToVolume>();
    registerProcessor<ContourTree>();

    registerProperty<TopologyColorsProperty>();
}

}  // namespace inviwo
