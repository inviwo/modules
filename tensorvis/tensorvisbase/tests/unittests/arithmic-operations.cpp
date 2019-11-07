#include <warn/push>
#include <warn/ignore/all>
#include <gtest/gtest.h>
#include <warn/pop>

#include <glm/gtx/component_wise.hpp>
#include <inviwo/tensorvisbase/util/misc.h>

namespace inviwo {
TEST(TensorUtilTests, divideOperatorSuccess) {
    std::vector<float> vec;
    vec.resize(10);
    std::iota(vec.begin(), vec.end(), 0.0f);

    auto divided = tensorutil::operator/(vec, 5.0f);

    divided = tensorutil::operator/(divided, 1.0f / 5.0f);

    EXPECT_EQ(vec, divided);
}

TEST(TensorUtilTests, divideFunctionSuccess) {
    std::vector<float> vec;
    vec.resize(10);
    std::iota(vec.begin(), vec.end(), 0.0f);

    auto divided = tensorutil::divide(vec, 5.0f);

    divided = tensorutil::divide(divided, 1.0f / 5.0f);

    EXPECT_EQ(vec, divided);
}

TEST(TensorUtilTests, numberOfElements) {
    size3_t dimensions{128, 32, 64};
    EXPECT_EQ(262144, glm::compMul(dimensions));
}
}  // namespace inviwo
