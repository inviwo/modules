/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018-2025 Inviwo Foundation
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

#include <inviwo/integrallinefiltering/algorithm/shannonentropy.h>

#include <warn/push>
#include <warn/ignore/all>
#include <gtest/gtest.h>
#include <warn/pop>

#include <random>

namespace inviwo {

TEST(ShannonEntroyTest, OneDimension) {

    const size_t N = 10;

    SparseHistogram<size_t> hist1;
    SparseHistogram<size_t> hist2;
    SparseHistogram<size_t> hist3;
    SparseHistogram<size_t> hist4;

    for (size_t i = 0; i < N; i++) {
        hist1[i]++;
        hist2[0]++;
        hist3[i]++;
        hist4[i * 2 % N]++;
    }
    hist3[0]++;
    hist3[1]--;
    auto e1 = entropy::shannonEntropy(hist1);  // should be max
    auto e2 = entropy::shannonEntropy(hist2);  // should be zero
    auto e3 = entropy::shannonEntropy(hist3) / entropy::shannonEntropyMax(N);
    auto e4 = entropy::shannonEntropy(hist4) / entropy::shannonEntropyMax(N);

    ASSERT_EQ(e1, entropy::shannonEntropyMax(N)) << "Equal probablility == Maximum entropy";
    ASSERT_EQ(e2, 0) << "No (zero) entropy if everything is the same";

    EXPECT_NEAR(e3, 0.93, 0.01);
    EXPECT_NEAR(e4, 0.70, 0.01);
}

TEST(ShannonEntroyTest, Vectors2D) {
    std::vector<vec2> pointsInCircle;
    std::vector<vec2> pointsInSquare;
    size_t N = 10000;

    std::mt19937 rand;
    std::uniform_real_distribution<float> dist(-1, 1);

    while (pointsInCircle.size() < N) {
        // Note: Using length less than one on two independent uniform variables instead of using
        // PDF and polar coordinates to avoid misstakes in the generator function
        vec2 p;
        p.x = dist(rand);
        p.y = dist(rand);
        pointsInSquare.push_back(p);
        if (glm::length2(p) <= 1) {
            pointsInCircle.push_back(p);
        }
    }
    {
        ASSERT_GE(pointsInSquare.size(), N);
        auto e0 = entropy::shannonEntropyEuclidean(pointsInSquare, 1.0f / 1);
        auto e1 = entropy::shannonEntropyEuclidean(pointsInSquare, 1.0f / 2);
        auto e2 = entropy::shannonEntropyEuclidean(pointsInSquare, 1.0f / 4);
        auto e3 = entropy::shannonEntropyEuclidean(pointsInSquare, 1.0f / 8);
        auto e4 = entropy::shannonEntropyEuclidean(pointsInSquare, 1.0f / 16);
        EXPECT_NEAR(e0, 1, 0.005);
        EXPECT_NEAR(e1, 1, 0.005);
        EXPECT_NEAR(e2, 1, 0.005);
        EXPECT_NEAR(e3, 1, 0.005);
        EXPECT_NEAR(e4, 1, 0.0052);
    }
    {
        ASSERT_EQ(pointsInCircle.size(), N);
        auto e0 = entropy::shannonEntropyDirectional(pointsInCircle, 2);
        auto e1 = entropy::shannonEntropyDirectional(pointsInCircle, 4);
        auto e2 = entropy::shannonEntropyDirectional(pointsInCircle, 8);
        auto e3 = entropy::shannonEntropyDirectional(pointsInCircle, 16);
        auto e4 = entropy::shannonEntropyDirectional(pointsInCircle, 100);
        EXPECT_NEAR(e0, 1, 0.00008);
        EXPECT_NEAR(e1, 1, 0.0003);
        EXPECT_NEAR(e2, 1, 0.0004);
        EXPECT_NEAR(e3, 1, 0.0004);
        EXPECT_NEAR(e4, 1, 0.001);
    }
}

TEST(ShannonEntroyTest, Vectors3D2) {
    std::vector<vec3> pointsInSphere;
    std::vector<vec3> pointsInCube;
    size_t N = 100000;

    std::mt19937 rand;
    std::uniform_real_distribution<double> dist(-1, 1);
    while (pointsInSphere.size() < N) {
        // Note: Using length less than one on two independent uniform variables instead of using
        // PDF and polar coordinates to avoid misstakes in the generator function
        dvec3 p;
        p.x = dist(rand);
        p.y = dist(rand);
        p.z = dist(rand);
        pointsInCube.push_back(p);
        if (glm::length2(p) <= 1) {
            pointsInSphere.push_back(p);
        }
    }
    {
        ASSERT_GE(pointsInCube.size(), N);
        auto e0 = entropy::shannonEntropyEuclidean(pointsInCube, 1.0f / 1);
        auto e1 = entropy::shannonEntropyEuclidean(pointsInCube, 1.0f / 2);
        auto e2 = entropy::shannonEntropyEuclidean(pointsInCube, 1.0f / 4);
        auto e3 = entropy::shannonEntropyEuclidean(pointsInCube, 1.0f / 8);
        auto e4 = entropy::shannonEntropyEuclidean(pointsInCube, 1.0f / 16);
        EXPECT_NEAR(e0, 1, 0.0005);
        EXPECT_NEAR(e1, 1, 0.0005);
        EXPECT_NEAR(e2, 1, 0.0005);
        EXPECT_NEAR(e3, 1, 0.002);
        EXPECT_NEAR(e4, 1, 0.009);
    }
    {
        ASSERT_EQ(pointsInSphere.size(), N);

        auto e0 = entropy::shannonEntropyDirectional(pointsInSphere, 10);
        auto e1 = entropy::shannonEntropyDirectional(pointsInSphere, 100);
        auto e2 = entropy::shannonEntropyDirectional(pointsInSphere, 1000);

        EXPECT_NEAR(e0, 1, 0.00005);
        EXPECT_NEAR(e1, 1, 0.0002);
        EXPECT_NEAR(e2, 1, 0.0008);
    }
}

}  // namespace inviwo
