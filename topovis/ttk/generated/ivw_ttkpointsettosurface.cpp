#include "ivw_ttkpointsettosurface.h"

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
#include "ttkPointSetToSurface.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkPointSetToSurface& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"InputOrderingXArray", "Input Ordering X Array", {}, 0};

    static constexpr std::string_view inport = "Domain";
};

struct Wrapper1 : FieldSelection {
    bool set(ttkPointSetToSurface& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(1, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"InputOrderingYArray", "Input Ordering Y Array", {}, 0};

    static constexpr std::string_view inport = "Domain";
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<ttkPointSetToSurface> {
    static constexpr std::string_view identifier = "ttkPointSetToSurface";
    static constexpr std::string_view displayName = "TTK PointSetToSurface";
    inline static std::array<InputData, 1> inports = {InputData{"Domain", "vtkPointSet", 1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 1> groups = {
        Group{"Input options", {"InputOrderingXArray", "InputOrderingYArray"}}};
    std::tuple<Wrapper0, Wrapper1> properties;
};

void registerttkPointSetToSurface(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkPointSetToSurface>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
