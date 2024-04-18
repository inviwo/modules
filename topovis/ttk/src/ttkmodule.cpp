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

#include <inviwo/ttk/ttkmodule.h>
#include <inviwo/ttk/processors/mergetreetonodes.h>

#include <registerfilters.h>

namespace inviwo {

ttkModule::ttkModule(InviwoApplication* app) : InviwoModule(app, "ttk") {
    // Add a directory to the search path of the Shadermanager
    // ShaderManager::getPtr()->addShaderSearchPath(getPath(ModulePath::GLSL));

    // Register objects that can be shared with the rest of inviwo here:

    // Processors
    vtkwrapper::registerVTKFilters(this);
    registerProcessor<MergeTreeToNodes>();
}

int ttkModule::getVersion() const { return 1; }

std::unique_ptr<VersionConverter> ttkModule::getConverter(int version) const {
    return std::make_unique<Converter>(version);
}

ttkModule::Converter::Converter(int version) : version_(version) {}

bool ttkModule::Converter::convert(TxElement* root) {
    bool res = false;
    switch (version_) {
        case 0: {
            TraversingVersionConverter conv{[&](TxElement* node) -> bool {
                const auto& key = node->Value();
                if (key != "Processor") return true;
                const auto& type = node->GetAttribute("type");
                if (!type.starts_with("org.inviwo.tkk.")) return true;

                // fix typo in URI
                std::string s{node->GetAttribute("type")};
                replaceInString(s, "org.inviwo.tkk.ttk", "org.inviwo.ttk.ttk");
                // vtk filters have been moved to VTK module
                replaceInString(s, "org.inviwo.tkk.vtk", "org.inviwo.vtk.vtk");

                node->SetAttribute("type", s);
                res = true;

                return true;
            }};

            conv.convert(root);

            return res;
        }
        default:
            return false;  // No changes
    }
}

}  // namespace inviwo
