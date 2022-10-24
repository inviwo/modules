/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020-2021 Inviwo Foundation
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

#include "utils/structs.glsl"
#include "utils/selectioncolor.glsl"
#include "utils/vertexflags.glsl"

uniform GeometryParameters geometry;

uniform vec4 defaultColor = vec4(1, 0, 0, 1);
uniform float defaultRadius = 0.1;

uniform SelectionColor showFiltered;
uniform SelectionColor showSelected;
uniform SelectionColor showHighlighted;
uniform sampler2D metaColor;

uniform float radiusScaling_ = 1.0;

out Vertex {
    smooth vec4 worldPosition;
    flat vec4 color;
    flat float radius;
    flat uint pickID;
    flat float scalarMeta;
    flat int visible;
} out_vert;

void main(void) {
    out_vert.visible = 1;
    
#if defined(HAS_SCALARMETA) && defined(USE_SCALARMETACOLOR) && !defined(FORCE_COLOR)
    out_vert.scalarMeta = in_ScalarMeta;
    out_vert.color = texture(metaColor, vec2(in_ScalarMeta, 0.5));
#elif defined(HAS_COLOR) && !defined(FORCE_COLOR)
    out_vert.color = in_Color;
    out_vert.scalarMeta = 0.0;
#else
    out_vert.color = defaultColor;
    out_vert.scalarMeta = 0.0;
#endif

#if defined(HAS_RADII) && !defined(FORCE_RADIUS)
    out_vert.radius = in_Radii;
#else
    out_vert.radius = defaultRadius;
#endif
    out_vert.radius *= radiusScaling_;

#if defined(HAS_TEXCOORD)
    VertexFlags flags = extractFlags(in_TexCoord);

    out_vert.visible = int(!flags.filtered || showFiltered.visible);

    if (flags.filtered) {
        out_vert.color = applySelectionColor(out_vert.color, showFiltered);
    } else if (flags.highlighted) {
        out_vert.color = applySelectionColor(out_vert.color, showHighlighted);
    } else  if (flags.selected) {
        out_vert.color = applySelectionColor(out_vert.color, showSelected);
    }
#endif

#if defined(HAS_PICKING)
    out_vert.pickID = in_Picking;
#else
    out_vert.pickID = 0;
#endif

    out_vert.worldPosition = geometry.dataToWorld * vec4(in_Position.xyz, 1.0);
    gl_Position = out_vert.worldPosition;
}
