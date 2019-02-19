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

 vec2 getEigenvalues(mat2 tensor, bool useMinor) {
    float xx = tensor[0][0];
    float xy = tensor[0][1];
    float yx = tensor[1][0];
    float yy = tensor[1][1];

    float lambda1 = 0.0;
    float lambda2 = 0.0;
    
    float determinant = xx * yy - xy * yx;
    float trace = xx + yy;

    lambda1 = trace / 2.0 + (sqrt(abs(pow(trace, 2) / 4.0 - determinant)));
    lambda2 = trace / 2.0 - (sqrt(abs(pow(trace, 2) / 4.0 - determinant)));

    float bigger; 
    float smaller; 

    if (lambda1 > lambda2) {
        bigger = lambda1;
        smaller = lambda2;
    } else {
        bigger = lambda2;
        smaller = lambda1;
    }
    
    if (useMinor) {
        return vec2(smaller, bigger);
    } else {
        return vec2(bigger, smaller);
    }
}

// http://www.math.harvard.edu/archive/21b_fall_04/exhibits/2dmatrices/index.html
vec2 getEigenvector(mat2 tensor, float eigenValue, bool useMinor) {
    vec2 eigenVector;
    float a = tensor[0][0];
    float b = tensor[0][1];
    float c = tensor[1][0];
    float d = tensor[1][1];

    if(abs(c) > 10e-7) {
        eigenVector = vec2(eigenValue - d, c);
        
        return eigenVector;
    }
    if(abs(c) < 10e-7 && abs(c) < 10e-7) {
        if (!useMinor) {
            if (a >= d)
                eigenVector = vec2(1, 0);
            else
                eigenVector = vec2(0, 1);
        } else {
            if (a <= d)
                eigenVector = vec2(1, 0);
            else
                eigenVector = vec2(0, 1);
        }

        return eigenVector;
    }
    
    return vec2(0.0);
}
