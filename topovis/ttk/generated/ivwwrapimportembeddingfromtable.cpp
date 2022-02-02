#include "ivwwrapimportembeddingfromtable.h"

#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/ttk/processors/ttkgenericprocessor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/fileproperty.h>

#include <tuple>
#include <array>
#include <string_view>

#include <warn/push>
#include <warn/ignore/all>
#include "ttkImportEmbeddingFromTable.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkImportEmbeddingFromTable& filter) {
        filter.SetXColumn(property.get().c_str());
        return true;
    }
    StringProperty property{"XColumn", "X Column", ""};
};

struct Wrapper1 {
    bool set(ttkImportEmbeddingFromTable& filter) {
        filter.SetYColumn(property.get().c_str());
        return true;
    }
    StringProperty property{"YColumn", "Y Column", ""};
};

struct Wrapper2 {
    bool set(ttkImportEmbeddingFromTable& filter) {
        filter.SetZColumn(property.get().c_str());
        return true;
    }
    StringProperty property{"ZColumn", "Z Column", ""};
};

struct Wrapper3 {
    bool set(ttkImportEmbeddingFromTable& filter) {
        filter.SetEmbedding2D(property.get());
        return true;
    }
    BoolProperty property{"2DPoints", "2DPoints", false};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<ttkImportEmbeddingFromTable> {
    static constexpr std::string_view identifier = "ttkImportEmbeddingFromTable";
    static constexpr std::string_view displayName = "TTK ImportEmbeddingFromTable";
    inline static std::array<InputData, 2> inports = {InputData{"DataSet", "vtkPointSet", -1},
                                                      InputData{"Table", "vtkTable", -1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3> properties;
};

void registerttkImportEmbeddingFromTable(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkImportEmbeddingFromTable>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
