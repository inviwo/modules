/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2013-2018 Inviwo Foundation
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

#include "eigen.glsl"

vec2 euler(vec2 posF, bool useMinor, sampler2D tensorFieldColor, bool normalizeVectors) {
    vec4 tensorVector = texture(tensorFieldColor, posF);
    mat2 tensor;
    tensor[0][0] = tensorVector.r;
    tensor[1][0] = tensorVector.g;
    tensor[0][1] = tensorVector.b;
    tensor[1][1] = tensorVector.a;

    float eigenValue = getEigenvalues(tensor, useMinor).r;
    vec2 V0 = getEigenvector(tensor, eigenValue, useMinor);

    if (normalizeVectors) {
        V0 = normalize(V0);
    }

    return V0;
}

vec2 rk4(vec2 p0, float stepsize, bool useMinor, sampler2D tensorFieldColor,
         bool normalizeVectors) {
    vec2 V0 = euler(p0, useMinor, tensorFieldColor, normalizeVectors);

    vec2 p1 = p0 + V0 * (stepsize / 2.0);
    vec2 V1 = euler(p1, useMinor, tensorFieldColor, normalizeVectors);

    vec2 p2 = p0 + V1 * (stepsize / 2.0);
    vec2 V2 = euler(p2, useMinor, tensorFieldColor, normalizeVectors);

    vec2 p3 = p0 + V2 * stepsize;
    vec2 V3 = euler(p3, useMinor, tensorFieldColor, normalizeVectors);

    return (V0 + 2.0 * (V1 + V2) + V3) / 6.0;
}

vec2 euler(vec2 posF, inout vec2 previousV0, bool useMinor, sampler2D tensorFieldColor,
           bool normalizeVectors) {
    vec4 tensorVector = texture(tensorFieldColor, posF);
    mat2 tensor;
    tensor[0][0] = tensorVector.r;
    tensor[1][0] = tensorVector.g;
    tensor[0][1] = tensorVector.b;
    tensor[1][1] = tensorVector.a;

    float eigenValue = getEigenvalues(tensor, useMinor).r;
    vec2 V0 = getEigenvector(tensor, eigenValue, useMinor);

    if (dot(previousV0, V0) < 0 && previousV0 != vec2(0)) {
        V0 = -V0;
    }

    if (normalizeVectors) {
        V0 = normalize(V0);
    }

    return V0;
}

vec2 rk4(vec2 p0, float stepsize, inout vec2 previousV0, bool useMinor, sampler2D tensorFieldColor,
         bool normalizeVectors) {
    vec2 V0 = euler(p0, previousV0, useMinor, tensorFieldColor, normalizeVectors);

    vec2 p1 = p0 + V0 * stepsize / 2;

    previousV0 = V0;

    vec2 V1 = euler(p1, previousV0, useMinor, tensorFieldColor, normalizeVectors);

    vec2 p2 = p0 + V1 * stepsize / 2;

    previousV0 = V1;

    vec2 V2 = euler(p2, previousV0, useMinor, tensorFieldColor, normalizeVectors);

    vec2 p3 = p0 + V2 * stepsize;

    previousV0 = V2;

    vec2 V3 = euler(p3, previousV0, useMinor, tensorFieldColor, normalizeVectors);

    previousV0 = V3;

    return (V0 + 2 * (V1 + V2) + V3) / 6.0;
}

float eigenValueToNormalized(float eigenValue, float eigenValueRange, float minEigenValue,
                             float maxEigenValue) {
    // account for possible numerical differences between CPU and GPU
    eigenValue = clamp(eigenValue, minEigenValue, maxEigenValue);

    float scale = 1.0 / eigenValueRange;
    if (minEigenValue < 0.0) {
        eigenValue = eigenValue + abs(minEigenValue);
    } else {
        eigenValue = eigenValue - minEigenValue;
    }

    float d = scale * eigenValue;

    d = clamp(d, 0.0, 1.0);

    return d;
}

vec4 eigenValueToColor(float eigenValue, float eigenValueRange, float minEigenValue,
                       float maxEigenValue, sampler2D tf) {
    float d = eigenValueToNormalized(eigenValue, eigenValueRange, minEigenValue, maxEigenValue);

    return texture(tf, vec2(d, 0.5));
}

float anisotropyToNormalized(vec2 eigenValues, float anisotropyValueRange, float anisotropyMinVal,
                             float anisotropyMaxVal) {
    float anisotropy = abs(eigenValues.r - eigenValues.g);

    // account for possible numerical differences between CPU and GPU
    anisotropy = clamp(anisotropy, anisotropyMinVal, anisotropyMaxVal);

    float scale = 1.0 / anisotropyValueRange;
    if (anisotropyMinVal < 0.0) {
        anisotropy = anisotropy + abs(anisotropyMinVal);
    } else {
        anisotropy = anisotropy - anisotropyMinVal;
    }

    float d = scale * anisotropy;

    d = clamp(d, 0.0, 1.0);

    return d;
}

vec4 anisotropyToColor(vec2 eigenValues, float anisotropyValueRange, float anisotropyMinVal,
                       float anisotropyMaxVal, sampler2D tf) {
    float d = anisotropyToNormalized(eigenValues, anisotropyValueRange, anisotropyMinVal,
                                     anisotropyMaxVal);

    return texture(tf, vec2(d, 0.5));
}
