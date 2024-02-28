#include "ivw_vtkappendpolydata.h"
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
#include <vtkAppendPolyData.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkAppendPolyData> {
    static constexpr std::string_view uri = "vtk.vtkAppendPolyData";
    static constexpr std::string_view className = "vtkAppendPolyData";
    static constexpr std::string_view identifier = "AppendPolyData";
    static constexpr std::string_view displayName = "Append Geometry";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkPolyData", -1, R"(Set the input to the Append Geometry
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<> properties;
    static constexpr std::string_view doc = R"(The Append
Geometry filter operates on multiple polygonal data sets.
It merges their geometry into a single data set. Only the
point and cell attributes that all of the input data sets
have in common will appear in the output.)";
};

void registervtkAppendPolyData(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkAppendPolyData>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
