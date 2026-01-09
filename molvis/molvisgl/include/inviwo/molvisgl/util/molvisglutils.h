/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2022-2026 Inviwo Foundation
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
#pragma once

#include <inviwo/molvisgl/molvisglmoduledefine.h>

#include <cstddef>

namespace inviwo {

class Mesh;
class BrushingAndLinkingManager;

namespace molvis {

/**
 * @brief update the brushing state of the molecular structure @p mesh
 *
 * The brushing information for selection, hightlighting, and filtering is stored per atom encoded
 * as a bit mask using @c VertexFlags. Filtered indices are always included. The @c
 * BufferType::TexCoordAttrib buffer of @p mesh will be updated or created,
 * if it does not exist yet. The buffer will contain the updated bit mask.
 *
 * @param manager  Brushing & Linking manager holding atom indices of multiple meshes/molecular
 * structures
 * @param mesh     mesh representing a molecular structure
 * @param offset   starting index of first atom in @p mesh
 * @param showSelected     if true, included selected indices in bit mask
 * @param showHighlighted  if true, included highlighted indices in bit mask
 *
 * @see VertexFlags BufferType::TexCoordAttrib
 */
IVW_MODULE_MOLVISGL_API void updateBrushing(const BrushingAndLinkingManager& manager, Mesh& mesh,
                                            size_t offset, bool showSelected, bool showHighlighted);

}  // namespace molvis

}  // namespace inviwo
