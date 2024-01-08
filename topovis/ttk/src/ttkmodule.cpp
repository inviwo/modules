/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2021-2024 Inviwo Foundation
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

#include <inviwo/ttk/processors/ttkgenericprocessor.h>
#include <inviwo/ttk/processors/volumetovtk.h>
#include <inviwo/ttk/processors/vtktovolume.h>
#include <inviwo/ttk/processors/vtkdatasettovtkimagedata.h>
#include <inviwo/ttk/processors/vtktodataframe.h>
#include <inviwo/ttk/processors/vtktomesh.h>
#include <inviwo/ttk/processors/vtktotetramesh.h>
#include <inviwo/ttk/ports/vtkinport.h>
#include <inviwo/ttk/ports/vtkoutport.h>

#include <inviwo/ttk/processors/imagetovtk.h>
#include <inviwo/ttk/processors/vtksource.h>
#include <inviwo/ttk/ttkmodule.h>

#include <registerttkfilters.h>

#include <vtkLogger.h>

#include <fmt/core.h>

namespace inviwo {

void logCallback(void* /*user_data*/, const vtkLogger::Message& message) {

    auto level = [&]() {
        if (message.verbosity <= vtkLogger::VERBOSITY_ERROR) {
            return LogLevel::Error;
        } else if (message.verbosity <= vtkLogger::VERBOSITY_WARNING) {
            return LogLevel::Warn;
        } else {
            return LogLevel::Info;
        }
    }();

    LogCentral::getPtr()->log("VTK", level, LogAudience::Developer, message.filename, "",
                              message.line, fmt::format("{}{}", message.prefix, message.message));
}

ttkModule::ttkModule(InviwoApplication* app) : InviwoModule(app, "ttk") {
    // Add a directory to the search path of the Shadermanager
    // ShaderManager::getPtr()->addShaderSearchPath(getPath(ModulePath::GLSL));

    // Register objects that can be shared with the rest of inviwo here:

    // see https://vtk.org/doc/nightly/html/classvtkLogger.html
    vtkLogger::AddCallback("inviwolog", &logCallback, nullptr, vtkLogger::VERBOSITY_ERROR);
    // vtkObject::GlobalWarningDisplayOn();
    vtkObject::GlobalWarningDisplayOff();

    // Processors
    ttkwrapper::registerTTKFilters(this);
    // registerProcessor < TTKGenericProcessor < ttkMorseSmaleComplex>>();
    registerProcessor<ImageToVTK>();
    registerProcessor<VolumeToVTK>();
    registerProcessor<VTKToVolume>();
    registerProcessor<VTKDowncastData>();
    registerProcessor<VTKToDataFrame>();
    registerProcessor<VTKToMesh>();
    registerProcessor<VTKToTetraMesh>();
    registerProcessor<vtk::VTKSource>();

    // Properties
    // registerProperty<ttkProperty>();

    // Readers and writes
    // registerDataReader(std::make_unique<ttkReader>());
    // registerDataWriter(std::make_unique<ttkWriter>());

    // Data converters
    // registerRepresentationConverter(std::make_unique<ttkDisk2RAMConverter>());

    // Ports
    registerPort<vtk::VtkInport>();
    registerPort<vtk::VtkOutport>();

    // PropertyWidgets
    // registerPropertyWidget<ttkPropertyWidget, ttkProperty>("Default");

    // Dialogs
    // registerDialog<ttkDialog>(ttkOutport);

    // Other things
    // registerCapabilities(std::make_unique<ttkCapabilities>());
    // registerSettings(std::make_unique<ttkSettings>());
    // registerMetaData(std::make_unique<ttkMetaData>());
    // registerPortInspector("ttkOutport", "path/workspace.inv");
    // registerProcessorWidget(std::string processorClassName, std::unique_ptr<ProcessorWidget>
    // processorWidget); registerDrawer(util::make_unique_ptr<ttkDrawer>());
}

}  // namespace inviwo
