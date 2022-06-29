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
#include "utils/pickingutils.glsl"

uniform GeometryParameters geometry;
uniform CameraParameters camera;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in Vertex {
    smooth vec4 worldPosition;
    flat vec4 color;
    flat float radius;
    flat uint pickID;
    flat float scalarMeta;
    flat bool visible;
} in_vert[];

out Fragment {
    flat float radius;
    flat vec3 camPos;
    flat vec4 center;
    flat vec4 color;
    flat vec4 pickColor;
    flat float scalar;
} out_vert;

void emitvertex(vec3 pos, float depth) {
    vec4 projPos = camera.worldToClip * vec4(pos, 1.0);
    projPos /= projPos.w;
    gl_Position = vec4(projPos.xy, depth, 1.0);
    EmitVertex();
}

void main(void) {
    if (in_vert[0].radius <= 0 || in_vert[0].color.a <= 0 || !in_vert[0].visible) {
        EndPrimitive();
        return;
    }

    out_vert.center = in_vert[0].worldPosition;
    out_vert.radius = in_vert[0].radius;

    mat4 worldToViewMatrixInv = inverse(camera.worldToView);

    vec3 camDir = normalize((worldToViewMatrixInv[2]).xyz);     
    vec3 camPosModel =  worldToViewMatrixInv[3].xyz;
    // calculate cam position (in model space of the sphere)
    out_vert.camPos = camPosModel - out_vert.center.xyz;

#ifdef DISCARD_CLIPPED_GLYPHS
    if (dot(out_vert.camPos, camDir) < camera.nearPlane + out_vert.radius) {
        // glyph intersects with the near plane of the camera, discard entire glyph, i.e. no output
        EndPrimitive();
        return;
    }
#endif // DISCARD_CLIPPED_GLYPHS

    vec4 centerMVP = camera.worldToClip * out_vert.center;
    float glyphDepth = centerMVP.z / centerMVP.w;

    // send color to fragment shader
    out_vert.color = in_vert[0].color;
    // set picking color
    out_vert.pickColor = vec4(pickingIndexToColor(in_vert[0].pickID), in_vert[0].pickID == 0 ? 0.0 : 1.0);

    out_vert.scalar = in_vert[0].scalarMeta;

    // camera coordinate system in object space
    vec3 camUp = (worldToViewMatrixInv[1]).xyz;
    vec3 camRight = normalize(cross(camDir, camUp));
    camUp = normalize(cross(camDir, camRight));

    // square:
    camRight *= out_vert.radius * 1.41421356;
    camUp *= out_vert.radius * 1.41421356;

    emitvertex(out_vert.center.xyz + camRight - camUp, glyphDepth);
    emitvertex(out_vert.center.xyz - camRight - camUp, glyphDepth);
    emitvertex(out_vert.center.xyz + camRight + camUp, glyphDepth);
    emitvertex(out_vert.center.xyz - camRight + camUp, glyphDepth);

    EndPrimitive();
}
