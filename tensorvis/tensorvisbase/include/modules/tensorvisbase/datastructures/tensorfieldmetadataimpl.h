template <typename T>
void MetaDataType<T>::serialize(std::ofstream &outFile) const {
    auto id = getId();
    const auto dataPtr = static_cast<const T *>(data_.data());

    outFile.write(reinterpret_cast<const char *>(&id), sizeof(uint64_t));
    outFile.write(reinterpret_cast<const char *>(&type_), sizeof(uint64_t));
    outFile.write(reinterpret_cast<const char *>(dataPtr), sizeof(T) * data_.size());
}

template <typename T>
void MetaDataType<T>::deserialize(std::ifstream &inFile, const size_t numElements) {
    data_.resize(numElements);
    auto data = data_.data();

    inFile.read(reinterpret_cast<char *>(&type_), sizeof(uint64_t));
    inFile.read(reinterpret_cast<char *>(data), sizeof(DataType) * numElements);
}

template <typename T>
std::pair<typename MetaDataType<T>::DataType, typename MetaDataType<T>::DataType>
MetaDataType<T>::getMinMax() const {
    auto minmax = util::dataMinMax(data_.data(), data_.size());
    return std::make_pair(util::glm_convert<DataType>(minmax.first),
                          util::glm_convert<DataType>(minmax.second));
}

template <typename T>
const std::vector<T> &MetaDataType<T>::getData() const {
    return data_;
}

template <typename T>
MetaDataType<T>::MetaDataType(std::vector<T> data, const TensorFeature type)
    : data_(std::move(data)), type_(type) {}

template <typename T>
const void *MetaDataType<T>::getDataPtr() const {
    return data_.data();
}

template <typename T>
size_t MetaDataType<T>::getNumberOfComponents() const {
    return util::extent<DataType>::value;
}