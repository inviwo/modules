#include "ivw_vtkpprobefilter.h"

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
#include <vtkPProbeFilter.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkPProbeFilter& filter) {
        filter.SetPassCellArrays(property.get());
        return true;
    }
    BoolProperty property{
        "PassCellArrays", "PassCellArrays",
        R"(When set the input's cell data arrays are shallow copied to the output.)"_help, false};
};

struct Wrapper1 {
    bool set(vtkPProbeFilter& filter) {
        filter.SetPassPointArrays(property.get());
        return true;
    }
    BoolProperty property{
        "PassPointArrays", "PassPointArrays",
        R"(When set the input's point data arrays are shallow copied to the output.)"_help, false};
};

struct Wrapper2 {
    bool set(vtkPProbeFilter& filter) {
        filter.SetPassFieldArrays(property.get());
        return true;
    }
    BoolProperty property{"PassFieldArrays", "PassFieldArrays",
                          R"(Set whether to pass the field-data arrays from the Input i.e. the input
providing the geometry to the output. On by default.)"_help,
                          true};
};

struct Wrapper3 {
    bool set(vtkPProbeFilter& filter) {
        filter.SetComputeTolerance(property.get());
        return true;
    }
    BoolProperty property{"ComputeTolerance", "ComputeTolerance",
                          R"(Set whether to compute the tolerance or to use a user provided
value. On by default.)"_help,
                          true};
};

struct Wrapper4 {
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
struct TTKTraits<vtkPProbeFilter> {
    static constexpr std::string_view className = "vtkPProbeFilter";
    static constexpr std::string_view identifier = "Probe";
    static constexpr std::string_view displayName = "Legacy Resample With Dataset";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 2> inports = {
        InputData{"Input", "vtkDataSet", -1,
                  R"(This property specifies the dataset from which to obtain
probe values. The data attributes come from this dataset.)"},
        InputData{"Source", "vtkDataSet", -1,
                  R"(This property specifies the dataset whose geometry will
be used in determining positions to probe. The mesh comes from this
dataset.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4> properties;
    static constexpr std::string_view doc = R"(Probe is a filter that computes point attributes at
specified point positions. The filter has two inputs: the
Input and Source.  The 'Source' geometric structure is passed
through the filter. The point attributes are computed at
the 'Source' point positions by interpolating into the
'Input' data. For example, we can compute data values on a plane
(plane specified as Source) from a volume (Input). The
cell data of the Input data is copied to the output based
on in which Input cell each Source point is. If an array
of the same name exists both in Input's point and cell
data, only the one from the point data is
probed. This is the implementation of the
'Resample With Dataset' filter available in ParaView
version 5.1 and earlier.)";
};

void registervtkPProbeFilter(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkPProbeFilter>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
