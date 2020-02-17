#include <inviwo/dataframe/datastructures/dataframe.h>
#include <inviwo/core/datastructures/buffer/bufferram.h>
#include <inviwo/core/util/constexprhash.h>
#include <string>
//#include <filesystem>
#include <sstream>
#include <ostream>
#include <iomanip>
#include <inviwo/tensorvisio/tensorvisiomoduledefine.h>
#include <inviwo/tensorvisbase/datastructures/tensorfield2d.h>
#include <inviwo/tensorvisbase/datastructures/tensorfield3d.h>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkDataArrayAccessor.h>
#include <vtkAssume.h>
#include <warn/pop>

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

inline std::string getFileSizeAsString(const std::string&, const FileSizeOrder order,
                                       const long long precision = 2) {
    const auto fileSize = 0;  // std::filesystem::file_size(file);

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

            inFile.read(reinterpret_cast<char*>(data.data()), sizeof(ValueType) * numItems);

            // Add column to data frame
            df->addColumn(
                std::make_shared<TemplateColumn<ValueType>>(std::string(T::identifier), data));
        }
    }
};

template <unsigned N>
struct VTKToVector {
    using TensorFieldType =
        std::conditional_t<N == 2, TensorField2D, std::conditional_t<N == 3, TensorField3D, void>>;
    using matN = typename TensorFieldType::matN;

    std::shared_ptr<std::vector<matN>> vec;

    VTKToVector() : vec(std::make_shared<std::vector<matN>>()) {}

    template <typename TensorArray>
    void operator()(TensorArray* tensors) {
        vec->clear();
        // This allows the compiler to optimize for the AOS array stride.
        const auto numComponents = tensors->GetNumberOfComponents();
        VTK_ASSUME(numComponents == N * N);

        vtkDataArrayAccessor<TensorArray> t(tensors);

        const vtkIdType numTensors = tensors->GetNumberOfTuples();

        vec->reserve(numTensors * numComponents);

        for (vtkIdType tupleIdx = 0; tupleIdx < numTensors; ++tupleIdx) {
            // Get compiles to inlined optimizable raw memory accesses for
            // vtkGenericDataArray subclasses.
            if constexpr (N == 2) {
                vec->emplace_back(matN(t.Get(tupleIdx, 0), t.Get(tupleIdx, 1), t.Get(tupleIdx, 2),
                                       t.Get(tupleIdx, 3)));
            }
            if constexpr (N == 3) {
                vec->emplace_back(matN(t.Get(tupleIdx, 0), t.Get(tupleIdx, 1), t.Get(tupleIdx, 2),
                                       t.Get(tupleIdx, 3), t.Get(tupleIdx, 4), t.Get(tupleIdx, 5),
                                       t.Get(tupleIdx, 6), t.Get(tupleIdx, 7), t.Get(tupleIdx, 8)));
            }
        }
    }
};

}  // namespace util
}  // namespace inviwo
