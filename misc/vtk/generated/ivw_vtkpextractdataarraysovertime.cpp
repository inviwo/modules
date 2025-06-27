#include "ivw_vtkpextractdataarraysovertime.h"
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
#include <vtkPExtractDataArraysOverTime.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkPExtractDataArraysOverTime& filter) {
        filter.SetFieldAssociation(property.get());
        return true;
    }
    OptionPropertyInt property{"FieldAssociation",
                               "FieldAssociation",
                               R"(Select the attribute data to pass.)"_help,
                               {{"Points", "Points", 0},
                                {"Cells", "Cells", 1},
                                {"Vertices", "Vertices", 4},
                                {"Edges", "Edges", 5},
                                {"Rows", "Rows", 6}},
                               0};
};

struct Wrapper1 {
    bool set(vtkPExtractDataArraysOverTime& filter) {
        filter.SetReportStatisticsOnly(property.get());
        return true;
    }
    BoolProperty property{"OnlyReportSelectionStatistics", "Only Report Selection Statistics",
                          R"(If this property is set to 1, the min, max,
inter-quartile ranges, and (for numeric arrays) mean and standard
deviation of all the selected points or cells within each time step
are reported -- instead of breaking each selected point's or cell's
attributes out into separate time history tables.)"_help,
                          true};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkPExtractDataArraysOverTime> {
    static constexpr std::string_view uri = "vtk.vtkPExtractDataArraysOverTime";
    static constexpr std::string_view className = "vtkPExtractDataArraysOverTime";
    static constexpr std::string_view identifier = "PlotDataOverTime";
    static constexpr std::string_view displayName = "Plot Data Over Time";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1, R"(The input from which the selection is
extracted.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1> properties;

    static constexpr std::string_view doc = R"ivw()ivw";
};

void registervtkPExtractDataArraysOverTime(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkPExtractDataArraysOverTime>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
