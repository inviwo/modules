#include "ivw_vtktrianglefilter.h"
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
#include <vtkTriangleFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkTriangleFilter> {
    static constexpr std::string_view uri = "vtk.vtkTriangleFilter";
    static constexpr std::string_view className = "vtkTriangleFilter";
    static constexpr std::string_view identifier = "TriangleFilter";
    static constexpr std::string_view displayName = "Triangulate";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{.identifier = "Input",
                  .dataType = "vtkPolyData",
                  .numComp = -1,
                  .doc = R"(This property specifies the input to the Triangulate
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<> properties;

    static constexpr std::string_view doc = R"ivw(The
Triangulate filter decomposes polygonal data into only
triangles, points, and lines. It separates triangle strips
and polylines into individual triangles and lines,
respectively. The output is polygonal data. Some filters
that take polygonal data as input require that the data be
composed of triangles rather than other polygons, so
passing your data through this filter first is useful in
such situations. You should use this filter in these cases
rather than the Tetrahedralize filter because they produce
different output dataset types. The filters referenced
require polygonal input, and the Tetrahedralize filter
produces unstructured grid output.)ivw";
};

void registervtkTriangleFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkTriangleFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
