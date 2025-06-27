#include "ivw_vtkdecimatepolylinefilter.h"
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
#include <vtkDecimatePolylineFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkDecimatePolylineFilter& filter) {
        filter.SetTargetReduction(property.get());
        return true;
    }
    DoubleProperty property{"TargetReduction",
                            "TargetReduction",
                            R"(This property specifies the desired reduction in the
total number of lines in the output dataset. For example, if the
TargetReduction value is 0.9, the Decimate Polyline filter will attempt to
produce an output dataset that is 10% the size of the
input.)"_help,
                            0.9,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(vtkDecimatePolylineFilter& filter) {
        filter.SetMaximumError(property.get());
        return true;
    }
    DoubleProperty property{
        "MaximumError",
        "MaximumError",
        R"(Set the largest decimation error that is allowed during the decimation
process. This may limit the maximum reduction that may be achieved. The
maximum error is specified as a fraction of the maximum length of
the input data bounding box.)"_help,
        1.79769e+308,
        std::pair{0.0, ConstraintBehavior::Ignore},
        std::pair{100.0, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkDecimatePolylineFilter> {
    static constexpr std::string_view uri = "vtk.vtkDecimatePolylineFilter";
    static constexpr std::string_view className = "vtkDecimatePolylineFilter";
    static constexpr std::string_view identifier = "DecimatePolyline";
    static constexpr std::string_view displayName = "Decimate Polyline";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkPolyData", -1, R"(This property specifies the input to the Decimate Polyline
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1> properties;

    static constexpr std::string_view doc =
        R"ivw(Decimate Polyline is a filter to reduce the number of lines in a
polyline. The algorithm functions by evaluating an error metric for each
vertex (i.e., the distance of the vertex to a line defined from the two
vertices on either side of the vertex). Then, these vertices are placed
into a priority queue, and those with smaller errors are deleted first.
The decimation continues until the target reduction is reached. While the
filter will not delete end points, it will decimate closed loops down to a
single line, thereby changing topology.

As this filter works on polylines, you may need to call Triangle Strips before calling
this filter.)ivw";
};

void registervtkDecimatePolylineFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkDecimatePolylineFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
