#include "ivwwrapstringarrayconverter.h"

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
#include "ttkStringArrayConverter.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkStringArrayConverter& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"InputStringArray", "Input String Array", {}, 0};

    static constexpr std::string_view inport = "Domain";
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<ttkStringArrayConverter> {
    static constexpr std::string_view identifier = "ttkStringArrayConverter";
    static constexpr std::string_view displayName = "TTK StringArrayConverter";
    inline static std::array<InputData, 1> inports = {InputData{"Domain", "vtkDataSet", -1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 1> groups = {Group{"Input options", {"Input String Array"}}};
    std::tuple<Wrapper0> properties;
};

void registerttkStringArrayConverter(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkStringArrayConverter>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
