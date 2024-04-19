#include "ivw_vtkloopsubdivisionfilter.h"
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
#include <vtkLoopSubdivisionFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkLoopSubdivisionFilter& filter) {
        filter.SetNumberOfSubdivisions(property.get());
        return true;
    }
    IntProperty property{"NumberOfSubdivisions",
                         "Number of Subdivisions",
                         R"(Set the number of subdivision iterations to perform.
Each subdivision divides single triangles into four new
triangles.)"_help,
                         1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{4, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkLoopSubdivisionFilter> {
    static constexpr std::string_view uri = "vtk.vtkLoopSubdivisionFilter";
    static constexpr std::string_view className = "vtkLoopSubdivisionFilter";
    static constexpr std::string_view identifier = "LoopSubdivisionFilter";
    static constexpr std::string_view displayName = "Loop Subdivision";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkPolyData", -1, R"(This property specifies the input to the Loop
Subdivision filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;

    static constexpr std::string_view doc =
        R"(The Loop Subdivision filter increases the granularity of a
polygonal mesh. It works by dividing each triangle in the
input into four new triangles. It is named for Charles
Loop, the person who devised this subdivision scheme. This
filter only operates on triangles, so a data set that
contains other types of polygons should be passed through
the Triangulate filter before applying this filter to it.
This filter only operates on polygonal data (specifically
triangle meshes), and it produces polygonal
output.)";
};

void registervtkLoopSubdivisionFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkLoopSubdivisionFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
