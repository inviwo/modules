#include <warn/push>
#include <warn/ignore/all>
#include <gtest/gtest.h>
#include <warn/pop>

#include <inviwo/tensorvisbase/util/distancemetrics.h>

namespace inviwo {
TEST(TensorUtilTests, euclideanDistanceSuccess) {
    std::vector<float> vec1;
    std::vector<float> vec2;
    vec1.resize(3);
    vec2.resize(3);
    std::iota(vec1.begin(), vec1.end(), 0);
    std::iota(vec2.begin(), vec2.end(), 1);

    EXPECT_EQ(std::sqrt(3.0f), util::euclideanDistance(vec1, vec2));
}

TEST(TensorUtilTests, squaredSumDistanceSuccess) {
    std::vector<float> vec1;
    std::vector<float> vec2;
    vec1.resize(3);
    vec2.resize(3);
    std::iota(vec1.begin(), vec1.end(), 0);
    std::iota(vec2.begin(), vec2.end(), 1);

    EXPECT_EQ(3.0f, util::squaredSumDistance(vec1, vec2));
}

}  // namespace inviwo