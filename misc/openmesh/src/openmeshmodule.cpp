/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2017 Inviwo Foundation
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

#include <modules/openmesh/openmeshmodule.h>

#include <modules/openmesh/openmeshreader.h>
#include <modules/openmesh/openmeshwriter.h>

#include <modules/openmesh/processors/trianglestowireframe.h>
#include <modules/openmesh/processors/meshdecimationprocessor.h>


#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES_WAS_DEFINED
#define _USE_MATH_DEFINES
#endif

#include <warn/push>
#include <warn/ignore/all>
#include <OpenMesh/Core/System/config.h>
#include <warn/pop>

#ifdef _USE_MATH_DEFINES_WAS_DEFINED
#undef _USE_MATH_DEFINES
#endif

namespace inviwo {

OpenMeshModule::OpenMeshModule(InviwoApplication* app) : InviwoModule(app, "OpenMesh") {
    registerProcessor<TrianglesToWireframe>();
    registerProcessor<MeshDecimationProcessor>();

    // Readers and writes
    registerDataReader(util::make_unique<OpenMeshReader>());
    registerDataWriter(util::make_unique<OpenMeshWriter>());

    LogInfo("OpenMesh version: " << OM_GET_VER << "." << OM_GET_MAJ << "." << OM_GET_MIN);

}

}  // namespace inviwo
