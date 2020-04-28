#include <warn/push>
#include <warn/ignore/all>
#include <gtest/gtest.h>
#include <warn/pop>
#include <list>

#include <inviwo/tensorvisbase/util/misc.h>

namespace inviwo {
TEST(TensorUtilTests, concatenateVectorSuccess) {
    std::vector<int> vec1;
    std::vector<int> vec2;
    std::vector<int> vec3;
    std::vector<int> vec4{3, 4, 5, 0, 1, 2};
    vec1.resize(3);
    vec2.resize(3);
    vec3.resize(6);
    std::iota(vec1.begin(), vec1.end(), 0);
    std::iota(vec2.begin(), vec2.end(), 3);
    std::iota(vec3.begin(), vec3.end(), 0);

    EXPECT_EQ(vec3, tensorutil::concatenate(vec1, vec2));
    EXPECT_EQ(vec4, tensorutil::concatenate(vec2, vec1));
}

TEST(TensorUtilTests, concatenateListSuccess) {
    std::list<int> list1;
    std::list<int> list2;
    std::list<int> list3;
    std::list<int> list4{3, 4, 5, 0, 1, 2};
    list1.resize(3);
    list2.resize(3);
    list3.resize(6);
    std::iota(list1.begin(), list1.end(), 0);
    std::iota(list2.begin(), list2.end(), 3);
    std::iota(list3.begin(), list3.end(), 0);

    EXPECT_EQ(list3, tensorutil::concatenate(list1, list2));
    EXPECT_EQ(list4, tensorutil::concatenate(list2, list1));
}

TEST(TensorUtilTests, unionSuccess) {
    std::vector<int> vec1;
    std::vector<int> vec2;
    std::vector<int> vec3{0, 1, 2, 3, 4, 5};
    vec1.resize(3);
    vec2.resize(3);
    std::iota(vec1.begin(), vec1.end(), 0);
    std::iota(vec2.begin(), vec2.end(), 3);

    EXPECT_EQ(vec3, tensorutil::union_set(vec1, vec2));
}

TEST(TensorUtilTests, unionSuccessVectorized) {
    std::vector<std::vector<int>> vecs{{0, 1, 2, 3}, {2, 3, 4, 5}};
    std::vector<int> res{0, 1, 2, 3, 4, 5};

    EXPECT_EQ(res, tensorutil::union_set(vecs));
}

TEST(TensorUtilTests, intersectionSuccessVectorized) {
    std::vector<std::vector<int>> vecs{{0, 1, 2, 3}, {2, 3, 4}};
    const std::vector<int> res{2, 3};

    EXPECT_EQ(res, tensorutil::intersection(vecs));
}
}  // namespace inviwo
