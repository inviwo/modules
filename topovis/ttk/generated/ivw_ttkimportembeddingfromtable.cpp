#include "ivw_ttkimportembeddingfromtable.h"
#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/vtk/processors/vtkgenericprocessor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/buttonproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/fileproperty.h>
#include <inviwo/ttk/util/ttkprocessorutils.h>

#include <tuple>
#include <array>
#include <string_view>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkDataObject.h>
#include <ttkImportEmbeddingFromTable.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkImportEmbeddingFromTable& filter) {
        filter.SetXColumn(property.get().c_str());
        return true;
    }
    StringProperty property{"XColumn", "X Column", R"(Input x column.)"_help, ""};
};

struct Wrapper1 {
    bool set(ttkImportEmbeddingFromTable& filter) {
        filter.SetYColumn(property.get().c_str());
        return true;
    }
    StringProperty property{"YColumn", "Y Column", R"(Input y column.)"_help, ""};
};

struct Wrapper2 {
    bool set(ttkImportEmbeddingFromTable& filter) {
        filter.SetZColumn(property.get().c_str());
        return true;
    }
    StringProperty property{"ZColumn", "Z Column", R"(Input z column.)"_help, ""};
};

struct Wrapper3 {
    bool set(ttkImportEmbeddingFromTable& filter) {
        filter.SetEmbedding2D(property.get());
        return true;
    }
    BoolProperty property{"2DPoints", "2DPoints", R"()"_help, false};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<ttkImportEmbeddingFromTable> {
    static constexpr std::string_view uri = "ttk.ttkImportEmbeddingFromTable";
    static constexpr std::string_view className = "ttkImportEmbeddingFromTable";
    static constexpr std::string_view identifier = "ttkImportEmbeddingFromTable";
    static constexpr std::string_view displayName = "TTK ImportEmbeddingFromTable";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 2> inports = {
        InputData{"DataSet", "vtkPointSet", -1, R"(Data-set to process.)"},
        InputData{"Table", "vtkTable", -1, R"(Data-set to process.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3> properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc = R"ivw(TTK fieldSelector plugin documentation.)ivw";
};

void registerttkImportEmbeddingFromTable(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkImportEmbeddingFromTable>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
