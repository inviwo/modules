#include "ivwwrappointsettocurve.h"

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
#include "ttkPointSetToCurve.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkPointSetToCurve& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"InputOrderingArray", "Input Ordering Array", {}, 0};

    static constexpr std::string_view inport = "Domain";
};

struct Wrapper1 {
    bool set(ttkPointSetToCurve& filter) {
        filter.SetCloseCurve(property.get());
        return true;
    }
    BoolProperty property{"CloseCurve", "Close Curve", false};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<ttkPointSetToCurve> {
    static constexpr std::string_view identifier = "ttkPointSetToCurve";
    static constexpr std::string_view displayName = "TTK PointSetToCurve";
    inline static std::array<InputData, 1> inports = {InputData{"Domain", "vtkPointSet", -1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 1> groups = {Group{"Input options", {"InputOrderingArray"}}};
    std::tuple<Wrapper0, Wrapper1> properties;
};

void registerttkPointSetToCurve(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkPointSetToCurve>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
