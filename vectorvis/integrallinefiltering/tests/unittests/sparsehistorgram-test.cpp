/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018-2019 Inviwo Foundation
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

#include <inviwo/integrallinefiltering/utils/sparsehistogram.h>

#include <warn/push>
#include <warn/ignore/all>
#include <gtest/gtest.h>
#include <warn/pop>

#include <random>

namespace inviwo {

TEST(SpraseHistgoramTest, TypeDefinition) {
    using SHint = SparseHistogram<int>;
    using SHuint = SparseHistogram<unsigned int>;

    EXPECT_TRUE((std::is_same_v<SHint::index_type, int>));
    EXPECT_TRUE((std::is_same_v<SHint::value_type, int>));

    EXPECT_TRUE((std::is_same_v<SHuint::index_type, unsigned int>));
    EXPECT_TRUE((std::is_same_v<SHuint::value_type, unsigned int>));
}

TEST(SpraseHistgoramTest, GLMTypeDefinition) {
    using SHint2 = SparseHistogram<ivec2>;
    using SHsize3 = SparseHistogram<size3_t>;

    EXPECT_TRUE((std::is_same_v<SHint2::index_type, ivec2>));
    EXPECT_TRUE((std::is_same_v<SHint2::value_type, int>));

    EXPECT_TRUE((std::is_same_v<SHsize3::index_type, size3_t>));
    EXPECT_TRUE((std::is_same_v<SHsize3::value_type, size_t>));
}

TEST(SpraseHistgoramTest, SizeAndCleanup) {
    auto histogram = SparseHistogram<int>();
    
    EXPECT_EQ(histogram.numberOfBins(), 0);

    histogram[1]++;
    histogram[4]++;
    EXPECT_EQ(histogram.numberOfBins(), 2);
        
    histogram[4]++;
    EXPECT_EQ(histogram.numberOfBins(), 2);

    histogram[4] = 0;
    EXPECT_EQ(histogram.numberOfBins(), 2);

    histogram.cleanup();
    EXPECT_EQ(histogram.numberOfBins(), 1);
}

}  // namespace inviwo