#include <inviwo/dataframe/datastructures/dataframe.h>
#include <inviwo/core/datastructures/buffer/bufferram.h>
#include <inviwo/core/util/constexprhash.h>
#include <string>
#include <filesystem>
#include <sstream>
#include <ostream>
#include <iomanip>
#include <inviwo/tensorvisio/tensorvisiomoduledefine.h>

#pragma once

namespace inviwo {
namespace util {

enum class FileSizeOrder { KiB = 1, MiB = 2, GiB = 3 };

template <class Elem, class Traits>
std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& os,
                                             FileSizeOrder order) {
    switch (order) {
        case FileSizeOrder::KiB:
            os << "KiB";
            break;
        case FileSizeOrder::MiB:
            os << "MiB";
            break;
        case FileSizeOrder::GiB:
            os << "GiB";
            break;
    }
    return os;
}

inline std::string getFileSizeAsString(const std::string& file, const FileSizeOrder order,
                                       const long long precision = 2) {
    const auto fileSize = std::filesystem::file_size(file);

    std::stringstream ss;

    ss << std::setprecision(precision);
    ss << std::to_string(float(fileSize) / (std::pow(1024.0f, static_cast<float>(order))));
    ss << " ";
    ss << order;

    return ss.str();
}

inline void serializeDataFrame(std::shared_ptr<const DataFrame> dataFrame, std::ofstream& file) {
    for (auto col : *dataFrame) {
        auto buf = col->getBuffer();

        // Omit the index column
        if (col->getHeader() == "index") {
            continue;
        }

        const auto id = util::constexpr_hash(std::string_view(col->getHeader()));
        file.write(reinterpret_cast<const char*>(&id), sizeof(size_t));

        buf->getRepresentation<BufferRAM>()->dispatch<void, dispatching::filter::All>(
            [](auto brprecision, std::ofstream& file) {
                using ValueType = util::PrecisionValueType<decltype(brprecision)>;
                auto& data = brprecision->getDataContainer();

                const auto numItems = data.size();
                file.write(reinterpret_cast<const char*>(&numItems), sizeof(size_t));

                file.write(reinterpret_cast<const char*>(data.data()),
                           sizeof(ValueType) * numItems);
            },
            std::ref(file));
    }
}

struct IVW_MODULE_TENSORVISIO_API DeserializeColumn {
    template <typename T>
    void operator()(size_t id, std::ifstream& inFile, std::shared_ptr<DataFrame> df,
        size_t numItems) {
        using ValueType = typename T::value_type;

        if (id == util::constexpr_hash(T::identifier)) {
            // construct std::vector from binary data
            std::vector<ValueType> data;
            data.resize(numItems);

            inFile.read(reinterpret_cast<char*>(data.data()), sizeof(ValueType)* numItems);

            // Add column to data frame
            df->addColumn(
                std::make_shared<TemplateColumn<ValueType>>(std::string(T::identifier), data));
        }
    }
};

}  // namespace util
}  // namespace inviwo
