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
#include "utils/pickingutils.glsl"

uniform GeometryParameters geometry;
uniform CameraParameters camera;

uniform float radius_ = 1.0;

layout (lines) in;
layout (triangle_strip, max_vertices = 24) out;

in Vertex {
    smooth vec4 worldPosition;
    smooth vec4 viewPosition;
    flat vec4 color;
    flat uint pickID;
    flat float scalarMeta;
} in_vert[];

out Fragment {
    flat vec4 color[2];
    flat vec4 picking_color[2];
    smooth vec3 view_pos;
    flat float scalar[2];

    flat vec4 capsule_center_radius;
    flat vec4 capsule_axis_length;
} out_frag;

vec4 view_vertices[8];
vec4 proj_vertices[8];

void emit_vertex(int i) {
    out_frag.view_pos = view_vertices[i].xyz;
    gl_Position = proj_vertices[i];
    EmitVertex();
}

void emit(int a, int b, int c, int d) {
    emit_vertex(a);
    emit_vertex(b);
    emit_vertex(c);
    emit_vertex(d);
    EndPrimitive(); 
}

vec3 get_ortho_vec(vec3 v, vec3 A, vec3 B) {
    if (abs(1-dot(v,A)) > 0.001){
        return normalize(cross(v, A));
    } else {
        return normalize(cross(v, B));
    }
}

void main() {
    if (in_vert[0].color.a == 0 || in_vert[1].color.a == 0) {
        EndPrimitive();
        return;
    }

    // Compute orientation vectors for the two connecting faces:
    vec3 p0 = in_vert[0].viewPosition.xyz;
    vec3 p1 = in_vert[1].viewPosition.xyz;
    float r = radius_;
    float l = distance(p0, p1);
    vec3 a = (p1 - p0) / l;
    vec3 c = (p0 + p1) * 0.5;

    out_frag.color[0] = in_vert[0].color;
    out_frag.color[1] = in_vert[1].color;
    out_frag.scalar[0] = in_vert[0].scalarMeta;
    out_frag.scalar[1] = in_vert[1].scalarMeta;

    out_frag.picking_color[0] = vec4(pickingIndexToColor(in_vert[0].pickID), in_vert[0].pickID == 0 ? 0.0 : 1.0);
    out_frag.picking_color[1] = vec4(pickingIndexToColor(in_vert[1].pickID), in_vert[1].pickID == 0 ? 0.0 : 1.0);

    out_frag.capsule_center_radius = vec4(c, r);
    out_frag.capsule_axis_length = vec4(a, l);

    // Extend end points to properly fit the sphere caps
    p0 -= a * r;
    p1 += a * r;

    const vec3 B = vec3(0,0,1);
    const vec3 A = vec3(1,0,0);
    vec3 o = get_ortho_vec(a,A,B);

    // Declare scratch variables for basis vectors:
    vec3 i, j, k;

    // Compute vertices of prismoid:
    j = a; i = o; k = normalize(cross(i, j)); i = cross(k, j); i *= r; k *= r;
    view_vertices[0] = vec4(p0 + i + k, 1);
    view_vertices[1] = vec4(p0 + i - k, 1);
    view_vertices[2] = vec4(p0 - i - k, 1);
    view_vertices[3] = vec4(p0 - i + k, 1);
    view_vertices[4] = vec4(p1 + i + k, 1);
    view_vertices[5] = vec4(p1 + i - k, 1);
    view_vertices[6] = vec4(p1 - i - k, 1);
    view_vertices[7] = vec4(p1 - i + k, 1);

    proj_vertices[0] = camera.viewToClip * view_vertices[0];
    proj_vertices[1] = camera.viewToClip * view_vertices[1];
    proj_vertices[2] = camera.viewToClip * view_vertices[2];
    proj_vertices[3] = camera.viewToClip * view_vertices[3];
    proj_vertices[4] = camera.viewToClip * view_vertices[4];
    proj_vertices[5] = camera.viewToClip * view_vertices[5];
    proj_vertices[6] = camera.viewToClip * view_vertices[6];
    proj_vertices[7] = camera.viewToClip * view_vertices[7];

    // Emit the six faces of the prismoid:
    emit(0,1,3,2); emit(5,4,6,7);
    emit(4,5,0,1); emit(3,2,7,6);
    emit(0,3,4,7); emit(2,1,6,5);
}
