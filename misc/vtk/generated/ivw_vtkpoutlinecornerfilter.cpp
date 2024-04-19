#include "ivw_vtkpoutlinecornerfilter.h"
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
#include <vtkPOutlineCornerFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkPOutlineCornerFilter& filter) {
        filter.SetCornerFactor(property.get());
        return true;
    }
    DoubleProperty property{"CornerFactor",
                            "CornerFactor",
                            R"(The value of this property sets the size of the corners
as a percentage of the length of the corresponding bounding box
edge.)"_help,
                            0.2,
                            std::pair{0.001, ConstraintBehavior::Ignore},
                            std::pair{0.5, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkPOutlineCornerFilter> {
    static constexpr std::string_view uri = "vtk.vtkPOutlineCornerFilter";
    static constexpr std::string_view className = "vtkPOutlineCornerFilter";
    static constexpr std::string_view identifier = "OutlineCornerFilter";
    static constexpr std::string_view displayName = "Outline Corners";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkDataSet", -1, R"(This property specifies the input to the Outline Corners
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;

    static constexpr std::string_view doc = R"(The
Outline Corners filter generates the corners of a bounding
box for the input dataset. This filter operates on any
type of dataset and produces polygonal
output.)";
};

void registervtkPOutlineCornerFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkPOutlineCornerFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
