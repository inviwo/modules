#pragma once

#include <vector>
#include <inviwo/core/util/enumtraits.h>

namespace inviwo {
namespace tensorutil {
enum class DistanceMetric { Euclidean, Manhattan, Minkowski, SquaredSum };

// Input: Two vectors, each of which represent one point in an n-dimensional space
// Output: Minkowski distance of order n between the two points
template <typename T>
T minkowskiDistance(const std::vector<T>& a, const std::vector<T>& b, const T order) {
    if (a.size() != b.size())
        throw std::domain_error("Minkowski distance requires equal length vectors");
    return std::pow(std::inner_product(a.begin(), a.end(), b.begin(), T(0), std::plus<>(),
                                       [order](T x, T y) { return std::pow(y - x, order); }),
                    T(1) / order);
}

// Input: Two vectors, each of which represent one point in an n-dimensional space
// Output: Manhattan distance between the two points
template <typename T>
T manhattanDistance(const std::vector<T>& a, const std::vector<T>& b) {
    return minkowskiDistance(a, b, T(1));
}

// Input: Two vectors, each of which represent one point in an n-dimensional space
// Output: Euclidean distance between the two points
template <typename T>
T euclideanDistance(const std::vector<T>& a, const std::vector<T>& b) {
    return minkowskiDistance(a, b, T(2));
}

// Input: Two vectors, each of which represent one point in an n-dimensional space
// Output: Squared sum distance between the two points
template <typename T>
T squaredSumDistance(const std::vector<T>& a, const std::vector<T>& b) {
    if (a.size() != b.size())
        throw std::domain_error("Squared sum distance requires equal length vectors");
    return std::inner_product(a.begin(), a.end(), b.begin(), T(0), std::plus<>(),
                              [](T x, T y) { return std::pow(y - x, T(2)); });
}

}  // namespace tensorutil
template <>
struct EnumTraits<tensorutil::DistanceMetric> {
    static std::string name() { return "DistanceMetric"; }
};
}  // namespace inviwo