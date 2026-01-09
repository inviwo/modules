/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2013-2026 Inviwo Foundation
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
#include "tensorutil.glsl"

uniform sampler2D tensorFieldColor;
uniform sampler2D noiseTexture;
uniform sampler2D tf;
uniform ImageParameters tensorFieldParameters;
uniform ImageParameters noiseTextureParameters;

uniform int samples;
uniform float stepLength;
uniform bool normalizeVectors;
uniform bool useRK4;
uniform bool useMinor;
uniform float eigenValueRange;
uniform float minEigenValue;
uniform float maxEigenValue;
uniform float ratio;
uniform vec4 backgroundColor;

uniform bool postProcessing = false;
uniform bool intensityMapping = false;
uniform float brightness = 0.0; // in [-1, 1]
uniform float contrast = 0.0;   // in [-1, 1]
uniform float gamma = 1.0;

in vec3 texCoord_;

void traverse(inout float v, inout int c, vec2 posForTensorFieldSampling, float stepSize,
              int steps) {
    vec2 previousV0 =
        rk4(posForTensorFieldSampling, stepSize, useMinor, tensorFieldColor, normalizeVectors);

    for (int i = 0; i < steps; i++) {
        vec2 V0;
        if (useRK4) {
            V0 = rk4(posForTensorFieldSampling, stepSize, previousV0, useMinor, tensorFieldColor,
                     normalizeVectors);
        } else {
            V0 = euler(posForTensorFieldSampling, previousV0, useMinor, tensorFieldColor,
                       normalizeVectors);
        }

        if (dot(previousV0, V0) < 0) {
            V0 = -V0;
        }

        V0.x = ratio * V0.x;

        V0 = normalize(V0);

        previousV0 = V0;

        posForTensorFieldSampling += V0 * stepSize;

        if (posForTensorFieldSampling.x < 0) break;
        if (posForTensorFieldSampling.y < 0) break;

        if (posForTensorFieldSampling.x > 1) break;
        if (posForTensorFieldSampling.y > 1) break;

        // vec2 n = noiseTextureParameters.dimensions;
        // vec2 delta = vec2(1.0 / (2.0 * n));
        // vec2 newTexForNoiseTextureSampling = mix(delta, 1.0 - delta, posForTensorFieldSampling);

        v += texture(noiseTexture, posForTensorFieldSampling).r;
        c += 1;
    }
}

void main() {
    vec2 n = tensorFieldParameters.dimensions;
    vec2 delta = vec2(1.0 / (2.0 * n));
    vec2 newTexForTensorFieldSampling = texCoord_.xy;  // mix(delta, 1.0 - delta, texCoord_.xy);

    n = noiseTextureParameters.dimensions;
    delta = vec2(1.0 / (2.0 * n));
    vec2 newTexForNoiseTextureSampling = texCoord_.xy;  // mix(delta, 1.0 - delta, texCoord_.xy);

    vec4 tensorVector = texture(tensorFieldColor, newTexForNoiseTextureSampling);
    if (tensorVector[0] == 0 && tensorVector[1] == 0 && tensorVector[2] == 0 &&
        tensorVector[3] == 0) {
        FragData0 = backgroundColor;
        return;
    }

    float v = texture(noiseTexture, newTexForNoiseTextureSampling).r;

    int c = 1;
    traverse(v, c, newTexForTensorFieldSampling, stepLength, samples / 2);
    traverse(v, c, newTexForTensorFieldSampling, -stepLength, samples / 2);

    v /= c;

    if (postProcessing) {
        v = mix(v, pow(v, 5.0 / pow(v + 1.0, 4)), intensityMapping);
        // brightness-contrast
        v = clamp((v - 0.5) * (contrast + 1.0) + 0.5 + brightness, 0, 1);
        // gamma correction
        v = pow(v, gamma);
    }

    FragData0 = vec4(v);
}