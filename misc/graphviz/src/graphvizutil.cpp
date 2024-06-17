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

#include <inviwo/graphviz/graphvizutil.h>


#include <graphviz/gvc.h>
#include <graphviz/gvplugin.h>

namespace inviwo::util {

extern "C" {

#ifdef _WIN32
#define IMPORT_API __declspec(dllimport)
#else
#define IMPORT_API
#endif

IMPORT_API extern gvplugin_library_t gvplugin_dot_layout_LTX_library;
// extern gvplugin_library_t gvplugin_neato_layout_LTX_library;
// extern gvplugin_library_t gvplugin_core_LTX_library;
// extern gvplugin_library_t gvplugin_quartz_LTX_library;
// extern gvplugin_library_t gvplugin_visio_LTX_library;
}

void loadGraphvizLibraries(GVC_t* gvc) {
    gvAddLibrary(gvc, &gvplugin_dot_layout_LTX_library);
    // gvAddLibrary(gvc, &gvplugin_neato_layout_LTX_library);
    // gvAddLibrary(gvc, &gvplugin_core_LTX_library);
    // gvAddLibrary(gvc, &gvplugin_quartz_LTX_library);
    // gvAddLibrary(gvc, &gvplugin_visio_LTX_library);
}

}  // namespace inviwo
