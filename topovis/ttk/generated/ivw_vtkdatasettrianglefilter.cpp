#include "ivw_vtkdatasettrianglefilter.h"

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
#include <vtkDataSetTriangleFilter.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkDataSetTriangleFilter> {
    static constexpr std::string_view className = "vtkDataSetTriangleFilter";
    static constexpr std::string_view identifier = "DataSetTriangleFilter";
    static constexpr std::string_view displayName = "Tetrahedralize";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkDataSet", -1, R"(This property specifies the input to the Tetrahedralize
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<> properties;
    static constexpr std::string_view doc = R"(The
Tetrahedralize filter converts the 3D cells of any type of
dataset to tetrahedrons and the 2D ones to triangles. This
filter always produces unstructured grid
output.)";
};

void registervtkDataSetTriangleFilter(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkDataSetTriangleFilter>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
