#include "ivw_vtktemporalarrayoperatorfilter.h"
#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/vtk/processors/vtkgenericprocessor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/buttonproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/fileproperty.h>

#include <tuple>
#include <array>
#include <string_view>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkDataObject.h>
#include <vtkTemporalArrayOperatorFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(vtkTemporalArrayOperatorFilter& filter) {
        if (name.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, fieldAssociation.get(), name.get().c_str());
        return true;
    }
    OptionPropertyString name{"name", "Name", {}, 0};

    OptionProperty<vtkDataObject::FieldAssociations> fieldAssociation{
        "fieldAssociation",
        "Field Association",
        {{"points", "Points", vtkDataObject::FIELD_ASSOCIATION_POINTS},
         {"cells", "Cells", vtkDataObject::FIELD_ASSOCIATION_CELLS},
         {"none", "None", vtkDataObject::FIELD_ASSOCIATION_NONE},
         {"pointsThenCells", "Points then Cells",
          vtkDataObject::FIELD_ASSOCIATION_POINTS_THEN_CELLS}},
        3};

    CompositeProperty property{[&]() {
        CompositeProperty tmp{"SelectInputArray", "SelectInputArray",
                              R"(This property lists the name of the array from which to
use.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(vtkTemporalArrayOperatorFilter& filter) {
        filter.SetFirstTimeStepIndex(property.get());
        return true;
    }
    IntProperty property{
        "FirstTimeStepIndex",
        "FirstTimeStepIndex",
        R"(This property specifies the timestep index to use in the first part of the comparison computation)"_help,
        0,
        std::pair{0, ConstraintBehavior::Ignore},
        std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(vtkTemporalArrayOperatorFilter& filter) {
        filter.SetOperator(property.get());
        return true;
    }
    OptionPropertyInt property{"Operator",
                               "Operator",
                               R"(The property determines the operation to compute
between the first and second timestep data.)"_help,
                               {{"+", "+", 0}, {"-", "-", 1}, {"*", "*", 2}, {"/", "/", 3}},
                               0};
};

struct Wrapper3 {
    bool set(vtkTemporalArrayOperatorFilter& filter) {
        filter.SetSecondTimeStepIndex(property.get());
        return true;
    }
    IntProperty property{
        "SecondTimeStepIndex",
        "SecondTimeStepIndex",
        R"(This property specifies the timestep index to use in the second part of the comparison computation)"_help,
        0,
        std::pair{0, ConstraintBehavior::Ignore},
        std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper4 {
    bool set(vtkTemporalArrayOperatorFilter& filter) {
        filter.SetOutputArrayNameSuffix(property.get().c_str());
        return true;
    }
    StringProperty property{"OutputArrayNameSuffix", "OutputArrayNameSuffix",
                            R"(This property set the suffix to be append to the output array name.
If empty, output will be suffixed with '_' and the operation type.)"_help,
                            ""};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkTemporalArrayOperatorFilter> {
    static constexpr std::string_view uri = "vtk.vtkTemporalArrayOperatorFilter";
    static constexpr std::string_view className = "vtkTemporalArrayOperatorFilter";
    static constexpr std::string_view identifier = "TemporalArrayOperator";
    static constexpr std::string_view displayName = "Temporal Array Operator";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1,
                  R"(This property specifies the input to the TemporalArrayOperator filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4> properties;
    static constexpr std::string_view doc =
        R"(Filter used to perform an operation on a data array at 2 different timesteps.)";
};

void registervtkTemporalArrayOperatorFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkTemporalArrayOperatorFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
