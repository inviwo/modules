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

#include <inviwo/electrostatics/electrostaticsmodule.h>
#include <inviwo/electrostatics/processors/electrostatics.h>
#include <inviwo/electrostatics/processors/electrostatics_test.h>
#include <inviwo/electrostatics/processors/segmentation.h>

namespace inviwo {

electrostaticsModule::electrostaticsModule(InviwoApplication* app) : InviwoModule(app, "electrostatics") {
    // Add a directory to the search path of the Shadermanager
    // ShaderManager::getPtr()->addShaderSearchPath(getPath(ModulePath::GLSL));

    // Register objects that can be shared with the rest of inviwo here:

    // Processors
    registerProcessor<electrostatics>();
    // registerProcessor<electrostaticsProcessor>();
    registerProcessor<electrostatics_test>();
    registerProcessor<segmentation>();

    // Properties
    // registerProperty<electrostaticsProperty>();

    // Readers and writes
    // registerDataReader(std::make_unique<electrostaticsReader>());
    // registerDataWriter(std::make_unique<electrostaticsWriter>());

    // Data converters
    // registerRepresentationConverter(std::make_unique<electrostaticsDisk2RAMConverter>());

    // Ports
    // registerPort<electrostaticsOutport>();
    // registerPort<electrostaticsInport>();

    // PropertyWidgets
    // registerPropertyWidget<electrostaticsPropertyWidget, electrostaticsProperty>("Default");

    // Dialogs
    // registerDialog<electrostaticsDialog>(electrostaticsOutport);

    // Other things
    // registerCapabilities(std::make_unique<electrostaticsCapabilities>());
    // registerSettings(std::make_unique<electrostaticsSettings>());
    // registerMetaData(std::make_unique<electrostaticsMetaData>());
    // registerPortInspector("electrostaticsOutport", "path/workspace.inv");
    // registerProcessorWidget(std::string processorClassName, std::unique_ptr<ProcessorWidget> processorWidget); 
    // registerDrawer(util::make_unique_ptr<electrostaticsDrawer>());
}

}  // namespace inviwo
