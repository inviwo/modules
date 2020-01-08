namespace inviwo {
namespace attributes {
template <unsigned int N>
inline std::vector<typename MajorEigenValue::value_type> MajorEigenValue::calculate(
    std::shared_ptr<const std::vector<glm::mat<N, N, typename MajorEigenValue::scalar_type>>> tensors,
    std::shared_ptr<const DataFrame> metaData) {
    std::vector<MajorEigenValue::value_type> l1{};
    for (const auto& tensor : *tensors) {
        l1.emplace_back(util::eigenvalue<0>(tensor));
    }
    return l1;
}
}  // namespace attributes
}  // namespace inviwo