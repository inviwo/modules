#include "ivw_vtkcountvertices.h"
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
#include <vtkCountVertices.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkCountVertices& filter) {
        filter.SetOutputArrayName(property.get().c_str());
        return true;
    }
    StringProperty property{
        "OutputArrayName", "Vertices Array Name",
        R"(This is the name of the array in the output containing the vertex counts.)"_help,
        "Vertex Count"};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkCountVertices> {
    static constexpr std::string_view uri = "vtk.vtkCountVertices";
    static constexpr std::string_view className = "vtkCountVertices";
    static constexpr std::string_view identifier = "CountCellVertices";
    static constexpr std::string_view displayName = "Count Cell Vertices";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1, R"(The input.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;

    static constexpr std::string_view doc =
        R"(Adds a new cell data array containing the number of vertices per cell.)";
};

void registervtkCountVertices(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkCountVertices>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
