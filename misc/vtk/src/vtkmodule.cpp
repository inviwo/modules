/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2024 Inviwo Foundation
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

#include <inviwo/vtk/vtkmodule.h>

#include <inviwo/vtk/ports/vtkinport.h>
#include <inviwo/vtk/ports/vtkoutport.h>
#include <inviwo/vtk/processors/imagetovtk.h>
#include <inviwo/vtk/processors/layertovtk.h>
#include <inviwo/vtk/processors/volumetovtk.h>
#include <inviwo/vtk/processors/vtkdatasettovtkimagedata.h>
#include <inviwo/vtk/processors/vtksource.h>
#include <inviwo/vtk/processors/vtktodataframe.h>
#include <inviwo/vtk/processors/vtktoimage.h>
#include <inviwo/vtk/processors/vtktolayer.h>
#include <inviwo/vtk/processors/vtktomesh.h>
#include <inviwo/vtk/processors/vtktotetramesh.h>
#include <inviwo/vtk/processors/vtktovolume.h>

#include <registerfilters.h>

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

VTKModule::VTKModule(InviwoApplication* app) : InviwoModule(app, "VTK") {
    // Add a directory to the search path of the Shadermanager
    // ShaderManager::getPtr()->addShaderSearchPath(getPath(ModulePath::GLSL));

    // Register objects that can be shared with the rest of inviwo here:

    // see https://vtk.org/doc/nightly/html/classvtkLogger.html
    vtkLogger::AddCallback("inviwolog", &logCallback, nullptr, vtkLogger::VERBOSITY_ERROR);
    // vtkObject::GlobalWarningDisplayOn();
    vtkObject::GlobalWarningDisplayOff();

    // Processors
    registerProcessor<ImageToVTK>();
    registerProcessor<LayerToVTK>();
    registerProcessor<VolumeToVTK>();
    registerProcessor<VTKToVolume>();
    registerProcessor<VTKDowncastData>();
    registerProcessor<VTKToDataFrame>();
    registerProcessor<VTKToImage>();
    registerProcessor<VTKToLayer>();
    registerProcessor<VTKToMesh>();
    registerProcessor<VTKToTetraMesh>();
    registerProcessor<vtk::VTKSource>();

    vtkwrapper::registerVTKFilters(this);
}

}  // namespace inviwo
