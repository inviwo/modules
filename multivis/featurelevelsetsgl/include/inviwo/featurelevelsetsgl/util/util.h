namespace inviwo {
namespace util {
enum class DistanceMetric { Euclidean, Manhattan, Minkowski, SquaredSum };

/**
 * \brief Calculates Minkowski distance of order n
 *        Input: Two vectors, each of which represent one point in an
 * n-dimensional space Output: Minkowski distance of order n between the two
 * points
 *
 * \param a Point a
 * \param b Point b
 * \param order Order of the Minkowski distance
 * \return Minkowski distance of order n between points a and b
 */
template <typename T>
T minkowskiDistance(const std::vector<T>& a, const std::vector<T>& b, const T order) {
    IVW_ASSERT(a.size() == b.size(), "Minkowski distance requires equal length vectors");
    return std::pow(std::inner_product(a.begin(), a.end(), b.begin(), T(0), std::plus<>(),
                                       [order](T x, T y) { return std::pow(y - x, order); }),
                    T(1) / order);
}

/**
 * \brief Calculates Manhattan distance
 *        Input: Two vectors, each of which represent one point in an
 * n-dimensional space Output: Manhattan distance between the two points
 *
 * \param a Point a
 * \param b Point b
 * \return Manhattan distance between points a and b
 */
template <typename T>
T manhattanDistance(const std::vector<T>& a, const std::vector<T>& b) {
    return minkowskiDistance(a, b, T(1));
}

/**
 * \brief Calculates Euclidean distance
 *        Input: Two vectors, each of which represent one point in an
 * n-dimensional space Output: Euclidean distance between the two points
 *
 * \param a Point a
 * \param b Point b
 * \return Euclidean distance between points a and b
 */
template <typename T>
T euclideanDistance(const std::vector<T>& a, const std::vector<T>& b) {
    return minkowskiDistance(a, b, T(2));
}

/**
 * \brief Calculates squared sum distance
 *        Input: Two vectors, each of which represent one point in an
 * n-dimensional space Output: Squared sum distance between the two points
 *
 * \param a Point a
 * \param b Point b
 * \return Squared sum distance between points a and b
 */
template <typename T>
T squaredSumDistance(const std::vector<T>& a, const std::vector<T>& b) {
    IVW_ASSERT(a.size() == b.size(), "Squared sum distance requires equal length vectors");
    return std::inner_product(a.begin(), a.end(), b.begin(), T(0), std::plus<>(),
                              [](T x, T y) { return std::pow(y - x, T(2)); });
}

template <typename T>
T normalizeValue(const T& val, const T& minVal, const T& maxVal) {
    if (val >= maxVal) return T(1);
    if (val <= minVal) return T(0);
    return (val - minVal) / (maxVal - minVal);
}

template <typename T>
T denormalizeValue(const T& val, const T& minVal, const T& maxVal) {
    if (val <= T(0)) return minVal;
    if (val >= T(1)) return maxVal;
    return minVal + val * (maxVal - minVal);
}
}  // namespace util
}  // namespace inviwo