#include "ivw_vtkpprobefilter.h"
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
#include <vtkPProbeFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkPProbeFilter& filter) {
        filter.SetPassPartialArrays(property.get());
        return true;
    }
    BoolProperty property{"PassPartialArrays", "PassPartialArrays",
                          R"(When dealing with composite datasets, partial arrays are
common i.e. data-arrays that are not available in all of the blocks. By
default, this filter only passes those point and cell data-arrays that
are available in all the blocks i.e. partial array are removed. When
PassPartialArrays is turned on, this behavior is changed to take a
union of all arrays present thus partial arrays are passed as well.
However, for composite dataset input, this filter still produces a
non-composite output. For all those locations in a block of where a
particular data array is missing, this filter uses vtkMath::Nan() for
double and float arrays, while 0 for all other types of arrays i.e int,
char etc.)"_help,
                          true};
};

struct Wrapper1 {
    bool set(vtkPProbeFilter& filter) {
        filter.SetPassCellArrays(property.get());
        return true;
    }
    BoolProperty property{
        "PassCellArrays", "PassCellArrays",
        R"(When set the input's cell data arrays are shallow copied to the output.)"_help, false};
};

struct Wrapper2 {
    bool set(vtkPProbeFilter& filter) {
        filter.SetPassPointArrays(property.get());
        return true;
    }
    BoolProperty property{
        "PassPointArrays", "PassPointArrays",
        R"(When set the input's point data arrays are shallow copied to the output.)"_help, false};
};

struct Wrapper3 {
    bool set(vtkPProbeFilter& filter) {
        filter.SetPassFieldArrays(property.get());
        return true;
    }
    BoolProperty property{"PassFieldArrays", "PassFieldArrays",
                          R"(Set whether to pass the field-data arrays from the Input i.e. the input
providing the geometry to the output. On by default.)"_help,
                          true};
};

struct Wrapper4 {
    bool set(vtkPProbeFilter& filter) {
        filter.SetComputeTolerance(property.get());
        return true;
    }
    BoolProperty property{"ComputeTolerance", "ComputeTolerance",
                          R"(Set whether to compute the tolerance or to use a user provided
value. On by default.)"_help,
                          true};
};

struct Wrapper5 {
    bool set(vtkPProbeFilter& filter) {
        filter.SetTolerance(property.get());
        return true;
    }
    DoubleProperty property{"Tolerance",
                            "Tolerance",
                            R"(Set the tolerance to use for
vtkDataSet::FindCell)"_help,
                            2.220446049250313e-16,
                            std::pair{2.220446049250313e-16, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkPProbeFilter> {
    static constexpr std::string_view uri = "vtk.vtkPProbeFilter";
    static constexpr std::string_view className = "vtkPProbeFilter";
    static constexpr std::string_view identifier = "ProbeLine";
    static constexpr std::string_view displayName = "ProbeLine";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 2> inports = {
        InputData{"Input", "vtkDataSet", -1,
                  R"(This property specifies the dataset from which to obtain
probe values.)"},
        InputData{"Source", "", -1, R"(This property specifies the dataset whose geometry will
be used in determining positions to probe.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5> properties;
    static constexpr std::string_view doc =
        R"(Internal filter used by (filters, ProbeLine). The Plot
Over Line filter samples the data set attributes of the current data set
at the points along a line. The values of the point-centered variables
along that line will be displayed in an XY Plot. This filter uses
interpolation to determine the values at the selected point, whether or
not it lies at an input point. The Probe filter operates on any type of
data and produces polygonal output (a line).)";
};

void registervtkPProbeFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkPProbeFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
