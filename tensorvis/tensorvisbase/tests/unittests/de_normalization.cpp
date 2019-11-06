#include <warn/push>
#include <warn/ignore/all>
#include <gtest/gtest.h>
#include <warn/pop>

#include <inviwo/tensorvisbase/util/misc.h>

namespace inviwo {
TEST(TensorUtilTests, denormalization) {
    const float min{3.2f};
    const float max{10.8f};

    EXPECT_EQ(min, tensorutil::denormalizeValue(0.0f, min, max));
    EXPECT_EQ(max, tensorutil::denormalizeValue(1.0f, min, max));
    EXPECT_TRUE((min + ((max - min) * 0.5f)) - tensorutil::denormalizeValue(0.5f, min, max) <
                std::numeric_limits<float>::epsilon());
}

TEST(TensorUtilTests, normalization) {
    const float min{3.2f};
    const float max{10.8f};

    EXPECT_EQ(0.0f, tensorutil::normalizeValue(min, min, max));
    EXPECT_EQ(1.0f, tensorutil::normalizeValue(max, min, max));
    EXPECT_TRUE(0.5f - tensorutil::normalizeValue(min + ((max - min) * 0.5f), min, max) <
                std::numeric_limits<float>::epsilon());
}
}  // namespace inviwo
