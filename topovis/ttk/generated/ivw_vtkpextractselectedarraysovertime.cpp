#include "ivw_vtkpextractselectedarraysovertime.h"

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
#include <vtkPExtractSelectedArraysOverTime.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkPExtractSelectedArraysOverTime& filter) {
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
struct TTKTraits<vtkPExtractSelectedArraysOverTime> {
    static constexpr std::string_view className = "vtkPExtractSelectedArraysOverTime";
    static constexpr std::string_view identifier = "ExtractSelectionOverTime";
    static constexpr std::string_view displayName = "Plot Selection Over Time";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 2> inports = {
        InputData{"Input", "vtkDataSet", -1, R"(The input from which the selection is
extracted.)"},
        InputData{"Selection", "vtkSelection", -1, R"(The input that provides the selection
object.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;
    static constexpr std::string_view doc =
        R"(This filter extracts the selection over time, i.e. cell
and/or point variables at a cells/point selected are
extracted over time The output multiblock consists of 1D
rectilinear grids where the x coordinate corresponds to
time (the same array is also copied to a point array named
Time or TimeData (if Time exists in the input)). If
selection input is a Location based selection then the
point values are interpolated from the nearby cells, ie
those of the cell the location lies in.)";
};

void registervtkPExtractSelectedArraysOverTime(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkPExtractSelectedArraysOverTime>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
