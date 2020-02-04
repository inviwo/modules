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

#include <inviwo/heightfieldrendering/heightfieldrenderingmodule.h>
#include <modules/opengl/shader/shadermanager.h>
#include <inviwo/heightfieldrendering/processors/cubeheightfieldrenderer.h>
#include <inviwo/heightfieldrendering/processors/imagetopoints.h>

namespace inviwo {

HeightFieldRenderingModule::HeightFieldRenderingModule(InviwoApplication* app) : InviwoModule(app, "HeightFieldRendering") {
    // Add a directory to the search path of the Shadermanager
    ShaderManager::getPtr()->addShaderSearchPath(getPath(ModulePath::GLSL));

    // Register objects that can be shared with the rest of inviwo here:

    // Processors
    registerProcessor<CubeHeightFieldRenderer>();
    registerProcessor<ImageToPoints>();

    // Properties
    // registerProperty<HeightFieldRenderingProperty>();

    // Readers and writes
    // registerDataReader(std::make_unique<HeightFieldRenderingReader>());
    // registerDataWriter(std::make_unique<HeightFieldRenderingWriter>());

    // Data converters
    // registerRepresentationConverter(std::make_unique<HeightFieldRenderingDisk2RAMConverter>());

    // Ports
    // registerPort<HeightFieldRenderingOutport>();
    // registerPort<HeightFieldRenderingInport>();

    // PropertyWidgets
    // registerPropertyWidget<HeightFieldRenderingPropertyWidget, HeightFieldRenderingProperty>("Default");

    // Dialogs
    // registerDialog<HeightFieldRenderingDialog>(HeightFieldRenderingOutport);

    // Other things
    // registerCapabilities(std::make_unique<HeightFieldRenderingCapabilities>());
    // registerSettings(std::make_unique<HeightFieldRenderingSettings>());
    // registerMetaData(std::make_unique<HeightFieldRenderingMetaData>());
    // registerPortInspector("HeightFieldRenderingOutport", "path/workspace.inv");
    // registerProcessorWidget(std::string processorClassName, std::unique_ptr<ProcessorWidget> processorWidget); 
    // registerDrawer(util::make_unique_ptr<HeightFieldRenderingDrawer>());
}

}  // namespace inviwo
