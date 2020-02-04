#pragma once

namespace inviwo {
namespace attributes {
template <glm::length_t N>
inline std::vector<typename Trace::value_type> Trace::calculate(
    std::shared_ptr<const std::vector<glm::mat<N, N, typename Trace::scalar_type>>> tensors,
    std::shared_ptr<const DataFrame> metaData) {
    std::vector<typename Trace::value_type> trace{};
    for (const auto& tensor : *tensors) {
        trace.emplace_back(util::trace(tensor));
    }
    return trace;
}

template <glm::length_t N>
inline std::vector<typename Norm::value_type> Norm::calculate(
    std::shared_ptr<const std::vector<glm::mat<N, N, typename Norm::scalar_type>>> tensors,
    std::shared_ptr<const DataFrame> metaData) {
    std::vector<typename Norm::value_type> norm{};
    for (const auto& tensor : *tensors) {
        typename Norm::value_type sum{0};
        for (unsigned int i{0}; i < N; ++i) {
            for (unsigned int j{0}; j < N; ++j) {
                sum += tensor[i][j] * tensor[i][j];
            }
        }
        norm.push_back(std::sqrt(sum));
    }
    return norm;
}

template <glm::length_t N>
inline std::vector<typename MajorEigenValue::value_type> MajorEigenValue::calculate(
    std::shared_ptr<const std::vector<glm::mat<N, N, typename MajorEigenValue::scalar_type>>>
        tensors,
    std::shared_ptr<const DataFrame> metaData) {
    std::vector<typename MajorEigenValue::value_type> l1{};
    for (const auto& tensor : *tensors) {
        l1.emplace_back(util::eigenvalue<0>(tensor));
    }
    return l1;
}

template <glm::length_t N>
inline std::vector<typename IntermediateEigenValue::value_type> IntermediateEigenValue::calculate(
    std::shared_ptr<const std::vector<glm::mat<N, N, typename IntermediateEigenValue::scalar_type>>>
    tensors,
    std::shared_ptr<const DataFrame> metaData) {
    if constexpr (N % 2) {
        std::vector<typename IntermediateEigenValue::value_type> l2{};
        for (const auto& tensor : *tensors) {
            l2.emplace_back(util::eigenvalue<N / 2>(tensor));
        }
        return l2;
    }
}

template <glm::length_t N>
inline std::vector<typename MinorEigenValue::value_type> MinorEigenValue::calculate(
    std::shared_ptr<const std::vector<glm::mat<N, N, typename MinorEigenValue::scalar_type>>>
    tensors,
    std::shared_ptr<const DataFrame> metaData) {
    std::vector<typename MinorEigenValue::value_type> ln{};
    for (const auto& tensor : *tensors) {
        ln.emplace_back(util::eigenvalue<N - 1>(tensor));
    }
    return ln;
}
}  // namespace attributes
}  // namespace inviwo