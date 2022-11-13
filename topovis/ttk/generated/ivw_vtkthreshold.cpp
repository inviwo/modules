#include "ivw_vtkthreshold.h"

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
#include "vtkThreshold.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(vtkThreshold& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS_THEN_CELLS,
                                      property.get().c_str());
        return true;
    }
    OptionPropertyString property{"SelectInputScalars", "Scalars", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(vtkThreshold& filter) {
        filter.SetLowerThreshold(property.get(0));
        filter.SetUpperThreshold(property.get(1));


        filter.SetThresholdFunction(vtkThreshold::THRESHOLD_BETWEEN);

        return true;
    }
    DoubleVec2Property property{"ThresholdBetween", "Threshold Range", dvec2{0.0, 0.0},
                                std::pair{dvec2{0.0}, ConstraintBehavior::Ignore},
                                std::pair{dvec2{100.0}, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(vtkThreshold& filter) {
        filter.SetAllScalars(property.get());
        return true;
    }
    BoolProperty property{"AllScalars", "AllScalars", true};
};

struct Wrapper3 {
    bool set(vtkThreshold& filter) {
        filter.SetUseContinuousCellRange(property.get());
        return true;
    }
    BoolProperty property{"UseContinuousCellRange", "UseContinuousCellRange", false};
};

struct Wrapper4 {
    bool set(vtkThreshold& filter) {
        filter.SetInvert(property.get());
        return true;
    }
    BoolProperty property{"Invert", "Invert", false};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkThreshold> {
    static constexpr std::string_view identifier = "Threshold";
    static constexpr std::string_view displayName = "Threshold";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkDataSet", 1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4> properties;
};

void registervtkThreshold(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkThreshold>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
