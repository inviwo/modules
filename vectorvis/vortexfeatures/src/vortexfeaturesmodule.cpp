/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020 Inviwo Foundation
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

#include <inviwo/vortexfeatures/processors/assemblewindingangle.h>
#include <inviwo/vortexfeatures/processors/floodfillimage.h>
#include <inviwo/vortexfeatures/processors/lineplot.h>
#include <inviwo/vortexfeatures/processors/okuboweiss.h>
#include <inviwo/vortexfeatures/processors/samplecoherence.h>
#include <inviwo/vortexfeatures/processors/vortexdirectionapproximation.h>
#include <inviwo/vortexfeatures/processors/vortexselector.h>
#include <inviwo/vortexfeatures/processors/vortexsettovolumes.h>
#include <inviwo/vortexfeatures/processors/vortexstatistics.h>
#include <inviwo/vortexfeatures/processors/vortextomesh.h>
#include <inviwo/vortexfeatures/processors/windingangle.h>
#include <inviwo/vortexfeatures/vortexfeaturesmodule.h>
#include <modules/opengl/shader/shadermanager.h>
#include <modules/opengl/debugmessages.h>

namespace inviwo {

VortexFeaturesModule::VortexFeaturesModule(InviwoApplication* app)
    : InviwoModule(app, "VortexFeatures") {
    // Add a directory to the search path of the Shadermanager
    ShaderManager::getPtr()->addShaderSearchPath(getPath(ModulePath::GLSL));

    registerProperty<plot::LineAxisProperty>();

    // Processors
    registerProcessor<AssembleWindingAngle>();
    registerProcessor<FloodfillImage>();
    registerProcessor<SampleCoherence>();
    registerProcessor<plot::LinePlot>();
    registerProcessor<OkuboWeiss2D>();
    registerProcessor<OkuboWeiss3D>();
    registerProcessor<VortexDirectionApproximation>();
    registerProcessor<VortexSelector>();
    registerProcessor<VortexSetToVolumes>();
    registerProcessor<VortexStatistics>();
    registerProcessor<VortexToMesh>();
    registerProcessor<WindingAngle>();

    utilgl::setOpenGLDebugMode(utilgl::debug::Mode::DebugSynchronous,
                               utilgl::debug::Severity::Medium);
}

}  // namespace inviwo
