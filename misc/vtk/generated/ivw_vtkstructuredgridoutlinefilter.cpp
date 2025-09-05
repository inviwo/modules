#include "ivw_vtkstructuredgridoutlinefilter.h"
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
#include <vtkStructuredGridOutlineFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkStructuredGridOutlineFilter> {
    static constexpr std::string_view uri = "vtk.vtkStructuredGridOutlineFilter";
    static constexpr std::string_view className = "vtkStructuredGridOutlineFilter";
    static constexpr std::string_view identifier = "StructuredGridOutlineFilter";
    static constexpr std::string_view displayName = "Outline Curvilinear DataSet";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{.identifier = "Input",
                  .dataType = "vtkStructuredGrid",
                  .numComp = -1,
                  .doc = R"(This property specifies the input to the outline
(curvilinear) filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<> properties;

    static constexpr std::string_view doc = R"ivw(The Outline filter
generates an outline of the outside edges of the input
dataset, rather than the dataset's bounding box. This
filter operates on structured grid datasets and produces
polygonal output.)ivw";
};

void registervtkStructuredGridOutlineFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkStructuredGridOutlineFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
