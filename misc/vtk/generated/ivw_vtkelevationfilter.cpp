#include "ivw_vtkelevationfilter.h"
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
#include <vtkElevationFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkElevationFilter& filter) {
        filter.SetScalarRange(property.get(0), property.get(1));
        return true;
    }
    DoubleVec2Property property{"ScalarRange",
                                "ScalarRange",
                                R"(This property determines the range into which scalars
will be mapped.)"_help,
                                dvec2{0.0, 1.0},
                                std::pair{dvec2{0.0, 0.0}, ConstraintBehavior::Ignore},
                                std::pair{dvec2{100.0, 100.0}, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(vtkElevationFilter& filter) {
        filter.SetLowPoint(property.get(0), property.get(1), property.get(2));
        return true;
    }
    DoubleVec3Property property{"LowPoint",
                                "Low Point",
                                R"(This property defines one end of the direction vector
(small scalar values).)"_help,
                                dvec3{0.0, 0.0, 0.0},
                                std::pair{dvec3{0.0, 0.0, 0.0}, ConstraintBehavior::Ignore},
                                std::pair{dvec3{100.0, 100.0, 100.0}, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(vtkElevationFilter& filter) {
        filter.SetHighPoint(property.get(0), property.get(1), property.get(2));
        return true;
    }
    DoubleVec3Property property{"HighPoint",
                                "High Point",
                                R"(This property defines the other end of the direction
vector (large scalar values).)"_help,
                                dvec3{0.0, 0.0, 1.0},
                                std::pair{dvec3{0.0, 0.0, 0.0}, ConstraintBehavior::Ignore},
                                std::pair{dvec3{100.0, 100.0, 100.0}, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkElevationFilter> {
    static constexpr std::string_view uri = "vtk.vtkElevationFilter";
    static constexpr std::string_view className = "vtkElevationFilter";
    static constexpr std::string_view identifier = "ElevationFilter";
    static constexpr std::string_view displayName = "Elevation";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1, R"(This property specifies the input dataset to the
Elevation filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2> properties;
    static constexpr std::string_view doc =
        R"(The Elevation filter generates point scalar values for an
input dataset along a specified direction vector. The
Input menu allows the user to select the data set to which
this filter will be applied. Use the Scalar range entry
boxes to specify the minimum and maximum scalar value to
be generated. The Low Point and High Point define a line
onto which each point of the data set is projected. The
minimum scalar value is associated with the Low Point, and
the maximum scalar value is associated with the High
Point. The scalar value for each point in the data set is
determined by the location along the line to which that
point projects.)";
};

void registervtkElevationFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkElevationFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
