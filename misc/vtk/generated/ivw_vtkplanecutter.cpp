#include "ivw_vtkplanecutter.h"
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
#include <vtkPlaneCutter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkPlaneCutter& filter) {
        filter.SetBuildTree(property.get());
        return true;
    }
    BoolProperty property{
        "BuildTree", "BuildTree",
        R"(If enabled and input is not an Image Data, Sphere Tree will be computed, for a faster slice computation.)"_help,
        true};
};

struct Wrapper1 {
    bool set(vtkPlaneCutter& filter) {
        filter.SetBuildHierarchy(property.get());
        return true;
    }
    BoolProperty property{
        "BuildHierarchy", "BuildHierarchy",
        R"(If enabled and input is not an Image Data, Tree hierarchy will be computed, for a faster slice computation.)"_help,
        true};
};

struct Wrapper2 {
    bool set(vtkPlaneCutter& filter) {
        filter.SetComputeNormals(property.get());
        return true;
    }
    BoolProperty property{
        "ComputeNormals", "ComputeNormals",
        R"(If enabled, compute the normal on each cell. Since all output cells are coplanar,
the normal generated is simply the normal of the plane used to slice with.
By default computing of normals is disabled.)"_help,
        false};
};

struct Wrapper3 {
    bool set(vtkPlaneCutter& filter) {
        filter.SetInterpolateAttributes(property.get());
        return true;
    }
    BoolProperty property{"InterpolateAttributes", "InterpolateAttributes",
                          R"(If enabled, interpolate attribute data. By default this is
enabled. Point data is always interpolated. Cell data is transferred
unless input is an image data.)"_help,
                          true};
};

struct Wrapper4 {
    bool set(vtkPlaneCutter& filter) {
        filter.SetGeneratePolygons(property.get());
        return true;
    }
    BoolProperty property{"GeneratePolygons", "GeneratePolygons",
                          R"(If enabled and input is a Structured Grid or a Rectilinear Grid,
output slice will consist of polygons instead of only triangles.)"_help,
                          true};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkPlaneCutter> {
    static constexpr std::string_view uri = "vtk.vtkPlaneCutter";
    static constexpr std::string_view className = "vtkPlaneCutter";
    static constexpr std::string_view identifier = "SliceWithPlane";
    static constexpr std::string_view displayName = "Slice With Plane";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1, R"(This property specifies the input of the slice
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4> properties;

    static constexpr std::string_view doc = R"ivw(This filter
extracts the portion of the input dataset that lies along
the specified plane. The Slice filter takes any type of
dataset as input. The output of this filter is a multiblock of multipiece
polygonal data. This is a multithreaded implementation.)ivw";
};

void registervtkPlaneCutter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkPlaneCutter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
