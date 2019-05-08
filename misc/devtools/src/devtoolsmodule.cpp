/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019 Inviwo Foundation
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

#include <inviwo/devtools/devtoolsmodule.h>
#include <inviwo/devtools/processors/eventlogger.h>

namespace inviwo {

DevToolsModule::DevToolsModule(InviwoApplication* app) : InviwoModule(app, "DevTools") {
    // Add a directory to the search path of the Shadermanager
    // ShaderManager::getPtr()->addShaderSearchPath(getPath(ModulePath::GLSL));

    // Register objects that can be shared with the rest of inviwo here:

    // Processors
    // registerProcessor<DevToolsProcessor>();
    registerProcessor<ImageEventLogger>();
    registerProcessor<VolumeEventLogger>();
    registerProcessor<MeshEventLogger>();

    // Properties
    // registerProperty<DevToolsProperty>();

    // Readers and writes
    // registerDataReader(util::make_unique<DevToolsReader>());
    // registerDataWriter(util::make_unique<DevToolsWriter>());

    // Data converters
    // registerRepresentationConverter(util::make_unique<DevToolsDisk2RAMConverter>());

    // Ports
    // registerPort<DevToolsOutport>();
    // registerPort<DevToolsInport>();

    // PropertyWidgets
    // registerPropertyWidget<DevToolsPropertyWidget, DevToolsProperty>("Default");

    // Dialogs
    // registerDialog<DevToolsDialog>(DevToolsOutport);

    // Other things
    // registerCapabilities(util::make_unique<DevToolsCapabilities>());
    // registerSettings(util::make_unique<DevToolsSettings>());
    // registerMetaData(util::make_unique<DevToolsMetaData>());
    // registerPortInspector("DevToolsOutport", "path/workspace.inv");
    // registerProcessorWidget(std::string processorClassName, std::unique_ptr<ProcessorWidget> processorWidget); 
    // registerDrawer(util::make_unique_ptr<DevToolsDrawer>());
}

}  // namespace inviwo
