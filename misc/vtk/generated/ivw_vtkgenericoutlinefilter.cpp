#include "ivw_vtkgenericoutlinefilter.h"
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
#include <vtkGenericOutlineFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkGenericOutlineFilter> {
    static constexpr std::string_view uri = "vtk.vtkGenericOutlineFilter";
    static constexpr std::string_view className = "vtkGenericOutlineFilter";
    static constexpr std::string_view identifier = "GenericOutlineFilter";
    static constexpr std::string_view displayName = "Outline Generic DataSet";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkGenericDataSet", -1, R"(Set the input to the Generic Outline
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<> properties;

    static constexpr std::string_view doc = R"ivw(The Generic Outline
filter generates an axis-aligned bounding box for the
input data set. The Input menu specifies the data set for
which to create a bounding box. This filter operates on
generic data sets and produces polygonal
output.)ivw";
};

void registervtkGenericOutlineFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkGenericOutlineFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
