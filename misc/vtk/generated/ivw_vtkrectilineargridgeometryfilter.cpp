#include "ivw_vtkrectilineargridgeometryfilter.h"
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
#include <vtkRectilinearGridGeometryFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkRectilinearGridGeometryFilter> {
    static constexpr std::string_view uri = "vtk.vtkRectilinearGridGeometryFilter";
    static constexpr std::string_view className = "vtkRectilinearGridGeometryFilter";
    static constexpr std::string_view identifier = "RectilinearGridGeometryFilter";
    static constexpr std::string_view displayName = "RectilinearGridGeometryFilter";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1, R"(Set the input to the Rectilinear Grid Geometry
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<> properties;

    static constexpr std::string_view doc =
        R"(RectilinearGridGeometryFilter is a filter that extracts
geometry from a rectilinear grid. By specifying
appropriate i-j-k indices, it is possible to extract a
point, a curve, a surface, or a "volume". The volume is
actually a (n x m x o) region of points. The extent
specification is zero-offset. That is, the first k-plane
in a 50x50x50 rectilinear grid is given by (0,49, 0,49,
0,0).)";
};

void registervtkRectilinearGridGeometryFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkRectilinearGridGeometryFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
