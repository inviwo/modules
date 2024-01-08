/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2013-2024 Inviwo Foundation
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
uniform sampler2D tf;
uniform ImageParameters tensorFieldParameters;

uniform int valueType;
uniform bool grayscale;

uniform bool useMinor;
uniform float eigenValueRange;
uniform float minEigenValue;
uniform float maxEigenValue;

uniform float anisotropyValueRange;
uniform float anisotropyMinVal;
uniform float anisotropyMaxVal;

in vec3 texCoord_;

void main() {
    vec2 n = tensorFieldParameters.dimensions;
    vec2 delta = vec2(1.0 / (2 * n));
    vec2 newTex = texCoord_.xy;//mix(delta, 1.0 - delta, texCoord_.xy);

    vec4 tensorVector = texture(tensorFieldColor, newTex);
    mat2 tensor;
    tensor[0][0] = tensorVector.r;
    tensor[1][0] = tensorVector.g;
    tensor[0][1] = tensorVector.b;
    tensor[1][1] = tensorVector.a;

    vec2 eigenValues = getEigenvalues(tensor, useMinor);

    if (valueType == 0) {
        if (grayscale) {
            float d = eigenValueToNormalized(eigenValues.r, eigenValueRange, minEigenValue,
                                             maxEigenValue);
            FragData0 = vec4(d, d, d, 1.0);
        } else {
            FragData0 =
                eigenValueToColor(eigenValues.r, eigenValueRange, minEigenValue, maxEigenValue, tf);
        }
    }

    if (valueType == 1) {
        if (grayscale) {
            float d = anisotropyToNormalized(eigenValues, anisotropyValueRange, anisotropyMinVal,
                                             anisotropyMaxVal);
            FragData0 = vec4(d, d, d, 1.0);
        } else {
            FragData0 = anisotropyToColor(eigenValues, anisotropyValueRange, anisotropyMinVal,
                                          anisotropyMaxVal, tf);
        }
    }
}
