/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2017-2018 Inviwo Foundation
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

#include <modules/tensorvisio/tensorvisiomodule.h>

#include <modules/tensorvisio/processors/amiratensorreader.h>
#include <modules/tensorvisio/processors/nrrdreader.h>
#include <modules/tensorvisio/processors/tensorfield2dexport.h>
#include <modules/tensorvisio/processors/tensorfield2dimport.h>
#include <modules/tensorvisio/processors/tensorfield3dexport.h>
#include <modules/tensorvisio/processors/tensorfield3dimport.h>
#include <modules/tensorvisio/processors/vtkdatasettotensorfield3d.h>
#include <modules/tensorvisio/processors/vtkdatasetinformation.h>
#include <modules/tensorvisio/processors/vtkreader.h>
#include <modules/tensorvisio/processors/vtkunstructuredgridtorectilineargrid.h>
#include <modules/tensorvisio/processors/flowguifilereader.h>
#include <modules/tensorvisio/processors/vtkwriter.h>

namespace inviwo {

TensorVisIOModule::TensorVisIOModule(InviwoApplication* app)
    : InviwoModule{app, "TensorVisIO"}{

    registerProcessor<AmiraTensorReader>();
    registerProcessor<NRRDReader>();
    registerProcessor<TensorField2DExport>();
    registerProcessor<TensorField2DImport>();
    registerProcessor<TensorField3DExport>();
    registerProcessor<TensorField3DImport>();
    registerProcessor<VTKDataSetToTensorField3D>();
    registerProcessor<VTKDataSetInformation>();
    registerProcessor<VTKReader>();
    registerProcessor<VTKUnstructuredGridToRectilinearGrid>();
    registerProcessor<FlowGUIFileReader>();
    registerProcessor<VTKWriter>();
}

TensorVisIOModule::~TensorVisIOModule() = default;

}  // namespace inviwo
