#include "ivwwrapobjwriter.h"

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
#include "ttkOBJWriter.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkOBJWriter& filter) {
        if (property.get().empty()) return false;
        filter.SetFilename(property.get().c_str());
        return true;
    }
    FileProperty property{"FileName", "FileName", ""};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<ttkOBJWriter> {
    static constexpr std::string_view identifier = "ttkOBJWriter";
    static constexpr std::string_view displayName = "TTK OBJWriter";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkDataSet", -1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;
};

void registerttkOBJWriter(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkOBJWriter>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
