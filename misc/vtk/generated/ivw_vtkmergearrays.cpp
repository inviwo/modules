#include "ivw_vtkmergearrays.h"
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
#include <vtkMergeArrays.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkMergeArrays> {
    static constexpr std::string_view uri = "vtk.vtkMergeArrays";
    static constexpr std::string_view className = "vtkMergeArrays";
    static constexpr std::string_view identifier = "AppendAttributes";
    static constexpr std::string_view displayName = "Append Attributes";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataObject", -1, R"(This property specifies the input to the Append
Attributes filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<> properties;
    static constexpr std::string_view doc =
        R"(The Append Attributes filter takes multiple input data
sets with the same geometry and merges their point and
cell attributes to produce a single output containing all
the point and cell attributes of the inputs. Any inputs
without the same number of points and cells as the first
input are ignored. The input data sets must already be
collected together, either as a result of a reader that
loads multiple parts (e.g., EnSight reader) or because the
Group Parts filter has been run to form a collection of
data sets.)";
};

void registervtkMergeArrays(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkMergeArrays>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
