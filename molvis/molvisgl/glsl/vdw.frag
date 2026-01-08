/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020-2026 Inviwo Foundation
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
#include "utils/shading.glsl"

uniform CameraParameters camera;
uniform LightParameters lighting;

uniform vec4 viewport; // holds viewport offset x, offset y, 2 / viewport width, 2 / viewport height
uniform float clipShadingFactor = 0.9;

in Fragment {
    flat float radius;
    flat vec3 camPos;
    flat vec4 center;
    flat vec4 color;
    flat vec4 pickColor;
    flat float scalar;
} in_frag;

void clipToSolid(in vec4 coord, in vec3 srcColor, out vec4 dstColor, out float dstDepth) { 
    dstDepth = 0.000001;

#if defined(SHADE_CLIPPED_AREA)
    // need to compute proper ray-near plane intersection for shading.
    // Use initial glyph coordinate for now since this should be precise enough for smaller glyphs.

    // clip surface is orthogonal to view direction of the camera, use viewDir as normal
    vec3 normal = normalize((camera.viewToWorld[2]).xyz);
    vec3 shadedColor = in_frag.color.rgb * clipShadingFactor;
    dstColor.rgb = APPLY_LIGHTING(lighting, srcColor, srcColor, vec3(1.0f), coord.xyz,
                           normal, normalize(in_frag.camPos - coord.xyz));
#else
    dstColor.rgb = in_frag.color.rgb * clipShadingFactor;
#endif // SHADE_CLIPPED_AREA
}

void main() {
    vec4 pixelPos = gl_FragCoord;
    pixelPos.xy -= viewport.xy;
    // transform fragment coordinates from window coordinates to view coordinates.
    vec4 coord = pixelPos
        * vec4(viewport.z, viewport.w, 2.0, 0.0)
        + vec4(vec3(-1.0), 1.0);

    // transform fragment coord into object space
    //coord = gl_ModelViewProjectionMatrixInverse * coord;
    coord = camera.clipToWorld * coord;
    coord /= coord.w;
    coord -= in_frag.center;
    // setup viewing ray
    vec3 ray = normalize(coord.xyz - in_frag.camPos);
    
    // calculate sphere-ray intersection
    // start ray at current coordinate and not at the camera
    float d1 = -dot(coord.xyz, ray);
    float d2s = dot(coord.xyz, coord.xyz) - d1*d1;
    float radicand = in_frag.radius*in_frag.radius - d2s;
    
    if (radicand < 0.0) {
        // no valid intersection found
        discard;
    }

    // calculate intersection point
    vec3 intersection = (d1 - sqrt(radicand))*ray + coord.xyz;
    
    vec3 normal = intersection / in_frag.radius;

    // shading
    vec4 glyphColor;
    glyphColor.rgb = APPLY_LIGHTING(lighting, in_frag.color.rgb, in_frag.color.rgb, vec3(1.0f), intersection,
                               normal, normalize(in_frag.camPos - intersection));
    glyphColor.a = in_frag.color.a;
    glyphColor.rgb *= glyphColor.a;

    // depth correction for glyph
    mat4 mvpTranspose = transpose(camera.worldToClip);

    vec4 pos = vec4(intersection + in_frag.center.xyz, 1.0);
    float depth = dot(mvpTranspose[2], pos);
    float depthW = dot(mvpTranspose[3], pos);

    depth = ((depth / depthW) + 1.0) * 0.5;

    if (depth <= 0.0) {
        // first intersection lies behind the camera
#ifdef DISCARD_CLIPPED_GLYPHS
        discard;
#else
        clipToSolid(coord, in_frag.color.rgb * clipShadingFactor, glyphColor, depth);
#endif // DISCARD_CLIPPED_GLYPHS
    }

    FragData0 = glyphColor;
    gl_FragDepth = depth;
    PickingData = in_frag.pickColor;
}
