/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2017-2025 Inviwo Foundation
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

#include <inviwo/tensorvisio/tensorvisiomodule.h>

#include <inviwo/tensorvisio/processors/amiratensorreader.h>
#include <inviwo/tensorvisio/processors/nrrdreader.h>
#include <inviwo/tensorvisio/processors/tensorfield2dexport.h>
#include <inviwo/tensorvisio/processors/tensorfield2dimport.h>
#include <inviwo/tensorvisio/processors/tensorfield2dtovtk.h>
#include <inviwo/tensorvisio/processors/tensorfield3dexport.h>
#include <inviwo/tensorvisio/processors/tensorfield3dimport.h>
#include <inviwo/tensorvisio/processors/flowguifilereader.h>
#include <inviwo/tensorvisio/processors/vtktotensorfield2d.h>
#include <inviwo/tensorvisio/processors/vtktotensorfield3d.h>

#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/io/serialization/ticpp.h>

namespace inviwo {

TensorVisIOModule::TensorVisIOModule(InviwoApplication* app) : InviwoModule{app, "TensorVisIO"} {

    registerProcessor<AmiraTensorReader>();
    registerProcessor<NRRDReader>();
    registerProcessor<TensorField2DExport>();
    registerProcessor<TensorField2DImport>();
    registerProcessor<TensorField2DToVTK>();
    registerProcessor<TensorField3DExport>();
    registerProcessor<TensorField3DImport>();
    registerProcessor<FlowGUIFileReader>();
    registerProcessor<VTKToTensorField2D>();
    registerProcessor<VTKToTensorField3D>();
}

int TensorVisIOModule::getVersion() const { return 1; }

std::unique_ptr<VersionConverter> TensorVisIOModule::getConverter(int version) const {
    return std::make_unique<Converter>(version);
}

TensorVisIOModule::Converter::Converter(int version) : version_(version) {}

bool TensorVisIOModule::Converter::convert(TxElement* root) {
    bool res = false;
    switch (version_) {
        case 0: {
            res |= xml::changeAttribute(
                root, {{xml::Kind::processor("org.inviwo.VTKDataSetToTensorField3D")}}, "type",
                "org.inviwo.VTKDataSetToTensorField3D", "org.inviwo.VTKToTensorField3D");
        }
            return res;

        default:
            return false;  // No changes
    }
}

}  // namespace inviwo
