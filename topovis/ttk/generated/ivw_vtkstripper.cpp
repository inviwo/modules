#include "ivw_vtkstripper.h"

#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/ttk/processors/ttkgenericprocessor.h>
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
#include <vtkStripper.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkStripper& filter) {
        filter.SetMaximumLength(property.get());
        return true;
    }
    IntProperty property{"MaximumLength",
                         "MaximumLength",
                         R"(This property specifies the maximum number of
triangles/lines to include in a triangle strip or
polyline.)"_help,
                         1000,
                         std::pair{4, ConstraintBehavior::Ignore},
                         std::pair{100000, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkStripper> {
    static constexpr std::string_view className = "vtkStripper";
    static constexpr std::string_view identifier = "Stripper";
    static constexpr std::string_view displayName = "Triangle Strips";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkPolyData", -1, R"(This property specifies the input to the Triangle Strips
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;
    static constexpr std::string_view doc = R"(The
Triangle Strips filter converts triangles into triangle
strips and lines into polylines. This filter operates on
polygonal data sets and produces polygonal
output.)";
};

void registervtkStripper(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkStripper>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
