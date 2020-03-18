/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020 Inviwo Foundation
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

/*
 * Initial implemenation from ViaMD by Robin Skånberg, March 2020
 *
 * https://github.com/scanberg/viamd
 */

#include "utils/structs.glsl"

uniform CameraParameters camera;
uniform GeometryParameters geometry;

uniform vec4 defaultColor = vec4(1, 0, 0, 1);
uniform sampler2D metaColor;

uniform float radiusScaling_ = 1.0;

out Vertex {
    smooth vec4 worldPosition;
    smooth vec4 viewPosition;
    flat vec4 color;
    flat uint pickID;
    flat out float scalarMeta;
} out_vert;

void main(void) {
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

// #if defined(HAS_RADII) && !defined(FORCE_RADIUS)
//     radius_ = in_Radii;
// #else 
//     radius_ = defaultRadius;
// #endif
//     radius_ *= radiusScaling_;

#if defined(HAS_PICKING)
    out_vert.pickID = in_Picking;
#else 
    out_vert.pickID = 0;
#endif

    out_vert.worldPosition = geometry.dataToWorld * vec4(in_Position.xyz, 1.0);
    out_vert.viewPosition = camera.worldToView * out_vert.worldPosition;
    gl_Position = out_vert.viewPosition;
}
