/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020-2025 Inviwo Foundation
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

#include <inviwo/molvisgl/molvisglmodule.h>
#include <inviwo/molvisgl/datavisualizer/molecularmeshrendervisualizer.h>
#include <inviwo/molvisgl/datavisualizer/molecularstructurevisualizer.h>
#include <inviwo/molvisgl/processors/molecularmeshrenderer.h>
#include <inviwo/molvisgl/processors/molecularrasterizer.h>
#include <inviwo/molvisgl/processors/molecularrenderer.h>
#include <modules/opengl/shader/shadermanager.h>

namespace inviwo {

MolVisGLModule::MolVisGLModule(InviwoApplication* app) : InviwoModule(app, "MolVisGL") {
    ShaderManager::getPtr()->addShaderSearchPath(getPath(ModulePath::GLSL));

    registerProcessor<MolecularMeshRenderer>();
    registerProcessor<MolecularRasterizer>();
    registerProcessor<MolecularRenderer>();

    registerDataVisualizer(std::make_unique<MolecularStructureVisualizer>(app));
    registerDataVisualizer(std::make_unique<MolecularMeshRenderVisualizer>(app));
}

}  // namespace inviwo
