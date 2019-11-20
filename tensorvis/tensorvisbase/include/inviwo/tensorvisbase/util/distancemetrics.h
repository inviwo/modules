#pragma once

#include <vector>
#include <inviwo/core/util/enumtraits.h>

namespace inviwo {
namespace tensorutil {
enum class DistanceMetric { Euclidean, SquaredSum, Minkowski };

template <typename T>
T minkowskiDistance(const std::vector<T>& a, const std::vector<T>& b, const T order) {
    if (a.size() != b.size())
        throw std::domain_error("Minkowski distance requires equal length vectors");
    return std::pow(std::inner_product(a.begin(), a.end(), b.begin(), T(0), std::plus<>(),
                                       [](T x, T y) { return (y - x) * (y - x); }),
                    order);
}

// Input: Two vectors, each of which represent one point in an n-dimensional space
// Output: Squared distance between the two points
template <typename T>
T squaredDistance(const std::vector<T>& a, const std::vector<T>& b) {
    return minkowskiDistance(a, b, T(1));
}

// Input: Two vectors, each of which represent one point in an n-dimensional space
// Output: Euclidean distance between the two points
template <typename T>
T euclideanDistance(const std::vector<T>& a, const std::vector<T>& b) {
    return minkowskiDistance(a, b, T(0.5));
}
}  // namespace tensorutil
template <>
struct EnumTraits<tensorutil::DistanceMetric> {
    static std::string name() { return "DistanceMetric"; }
};
}  // namespace inviwo