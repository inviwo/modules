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

/*
 * Initial implemenation from ViaMD by Robin Skånberg, March 2020
 *
 * https://github.com/scanberg/viamd
 */

#include "utils/structs.glsl"
#include "utils/shading.glsl"
#include "intersection/raycapsule.glsl"

#ifdef USE_FRAGMENT_LIST
#include "oit/abufferlinkedlist.glsl"

// this is important for the occlusion query
layout(early_fragment_tests) in;

layout(pixel_center_integer) in vec4 gl_FragCoord;
#endif

uniform CameraParameters camera;
uniform LightParameters lighting;
uniform float uniformAlpha;

in Fragment {
    flat vec4 color[2];
    flat vec4 picking_color[2];
    smooth vec3 view_pos;
    flat float scalar[2];

    flat vec4 capsule_center_radius;
    flat vec4 capsule_axis_length;
} in_frag;

void main() {
#ifdef ORTHO
    vec3 ro = vec3(in_frag.view_pos.xy, 0);
    vec3 rd = vec3(0,0,-1);
#else
    vec3 ro = vec3(0);
    vec3 rd = normalize(in_frag.view_pos);
#endif
    vec3 cc = in_frag.capsule_center_radius.xyz;
    float cr = in_frag.capsule_center_radius.w;
    vec3 ca = in_frag.capsule_axis_length.xyz;
    float ch = in_frag.capsule_axis_length.w;

    vec3 view_normal;
    float seg_t;
    float t = intersect_capsule(ro, rd, cc, ca, cr, ch, view_normal, seg_t);
    if (t < 0.0) {
        discard;
    }

    int side = int(seg_t + 0.5);

    vec3 view_coord = rd * t;
    
    vec3 campos = camera.viewToWorld[3].xyz;
    vec3 intersection = (camera.viewToWorld * vec4(view_coord, 1.0)).xyz;

    vec3 normal = inverse(transpose(mat3(camera.viewToWorld))) * view_normal;

    vec4 color;
    color.rgb = APPLY_LIGHTING(lighting, in_frag.color[side].rgb, in_frag.color[side].rgb, vec3(1.0f), 
                               intersection, normal, normalize(campos - intersection));
#if defined(UNIFORM_ALPHA)
    color.a = uniformAlpha;
#else
    color.a = in_frag.color[side].a;
#endif // UNIFORM_ALPHA
    color.rgb *= color.a;

    if (color.a < 0.01) discard;

    vec4 curr_clip_coord = camera.viewToClip * vec4(view_coord, 1);

    float depth = (curr_clip_coord.z / curr_clip_coord.w) * 0.5 + 0.5;

#if defined(USE_FRAGMENT_LIST)
    // fragment list rendering
    if (color.a > 0.0) {
        ivec2 coords = ivec2(gl_FragCoord.xy);
        abufferRender(coords, depth, color);
    }
    discard;
#else
    FragData0 = color;
    gl_FragDepth = depth;
    PickingData = in_frag.picking_color[side];
#endif  // USE_FRAGMENT_LIST
}
