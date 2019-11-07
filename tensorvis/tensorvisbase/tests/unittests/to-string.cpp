#include <warn/push>
#include <warn/ignore/all>
#include <gtest/gtest.h>
#include <warn/pop>

#include <inviwo/tensorvisbase/util/misc.h>

namespace inviwo {
TEST(TensorUtilTests, vectorToStringSuccess) {
    std::vector<uint32_t> vec;
    vec.resize(5);
    std::iota(vec.begin(), vec.end(), 0);
    std::string res{"[ 0, 1, 2, 3, 4 ]"};

    EXPECT_EQ(res, tensorutil::to_string(vec));
}

TEST(TensorUtilTests, vectorToStringFailure) {
    std::vector<uint32_t> vec;
    vec.resize(5);
    std::iota(vec.begin(), vec.end(), 0);
    std::string res{"[ 0, 1, 2, 3, 5 ]"};

    EXPECT_FALSE(res == tensorutil::to_string(vec));
}
}  // namespace inviwo
