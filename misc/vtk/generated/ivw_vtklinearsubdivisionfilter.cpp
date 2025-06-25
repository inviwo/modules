#include "ivw_vtklinearsubdivisionfilter.h"
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
#include <vtkLinearSubdivisionFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkLinearSubdivisionFilter& filter) {
        filter.SetNumberOfSubdivisions(property.get());
        return true;
    }
    IntProperty property{"NumberOfSubdivisions",
                         "Number of Subdivisions",
                         R"(The value of this property specifies the number of
subdivision iterations to perform.)"_help,
                         1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{4, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkLinearSubdivisionFilter> {
    static constexpr std::string_view uri = "vtk.vtkLinearSubdivisionFilter";
    static constexpr std::string_view className = "vtkLinearSubdivisionFilter";
    static constexpr std::string_view identifier = "Subdivide";
    static constexpr std::string_view displayName = "Subdivide";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkPolyData", -1, R"(This parameter specifies the input to the Subdivide
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;

    static constexpr std::string_view doc =
        R"ivw(The Subdivide filter iteratively divides each triangle in
the input dataset into 4 new triangles. Three new points
are added per triangle -- one at the midpoint of each
edge. This filter operates only on polygonal data
containing triangles, so run your polygonal data through
the Triangulate filter first if it is not composed of
triangles. The output of this filter is also
polygonal.)ivw";
};

void registervtkLinearSubdivisionFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkLinearSubdivisionFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
