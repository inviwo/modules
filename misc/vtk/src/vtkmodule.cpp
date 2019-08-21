/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2014-2019 Inviwo Foundation
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

#include <warn/push>
#include <warn/ignore/all>
#include <vtkVersion.h>
#include <warn/pop>

#include <inviwo/vtk/ports/vtkdatasetport.h>
#include <inviwo/vtk/processors/vtkdatasetinformation.h>
#include <inviwo/vtk/processors/vtkreader.h>
#include <inviwo/vtk/processors/vtkunstructuredgridtorectilineargrid.h>
#include <inviwo/vtk/processors/vtkwriter.h>

namespace inviwo {

VTKModule::VTKModule(InviwoApplication* app)
    : InviwoModule(app, "VTK"), vtkoutput_{std::make_unique<VtkOutputLogger>()} {

    LogInfo("VTK Version: " << vtkVersion::GetVTKVersion());
    LogInfo("VTK Version: " << vtkVersion::GetVTKSourceVersion());

    registerProcessor<VTKDataSetInformation>();
    registerProcessor<VTKReader>();
    registerProcessor<VTKUnstructuredGridToRectilinearGrid>();
    registerProcessor<VTKWriter>();

    registerPort<VTKDataSetInport>();
    registerPort<VTKDataSetOutport>();
}

}  // namespace inviwo
