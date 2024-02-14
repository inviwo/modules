#include "ivw_ttkgridlayout.h"
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
#include <ttkGridLayout.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkGridLayout& filter) {
        filter.SetColAxis(property.get());
        return true;
    }
    OptionPropertyInt property{"ColAxis",
                               "Column Axis",
                               R"(Axis which will be used to represent columns.)"_help,
                               {{"X", "X", 0}, {"Y", "Y", 1}, {"Z", "Z", 2}},
                               0};
};

struct Wrapper1 {
    bool set(ttkGridLayout& filter) {
        filter.SetColGap(property.get());
        return true;
    }
    DoubleProperty property{
        "ColGap",
        "Column Gap %",
        R"(A constant factor that is used to create gaps between columns.)"_help,
        0.0,
        std::pair{0.0, ConstraintBehavior::Ignore},
        std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(ttkGridLayout& filter) {
        filter.SetRowAxis(property.get());
        return true;
    }
    OptionPropertyInt property{"RowAxis",
                               "Row Axis",
                               R"(Axis which will be used to represent rows.)"_help,
                               {{"X", "X", 0}, {"Y", "Y", 1}, {"Z", "Z", 2}},
                               1};
};

struct Wrapper3 {
    bool set(ttkGridLayout& filter) {
        filter.SetRowGap(property.get());
        return true;
    }
    DoubleProperty property{"RowGap",
                            "Row Gap %",
                            R"(A constant factor that is used to create gaps between rows.)"_help,
                            0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper4 {
    bool set(ttkGridLayout& filter) {
        filter.SetNumberOfRows(property.get());
        return true;
    }
    IntProperty property{"NumberOfRows",
                         "Number of Rows",
                         R"(Number of rows (0: auto).)"_help,
                         0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkGridLayout& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper6 {
    bool set(ttkGridLayout& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber",
                         "Thread Number",
                         R"(The maximum number of threads.)"_help,
                         1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkGridLayout& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel",
                         "Debug Level",
                         R"(Debug level.)"_help,
                         3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper8 {
    bool set(ttkGridLayout& filter) {
        filter.SetCompactTriangulationCacheSize(property.get());
        return true;
    }
    DoubleProperty property{"CompactTriangulationCacheSize",
                            "Cache",
                            R"(Set the cache size for the compact triangulation as a
ratio with respect to the total cluster number.)"_help,
                            0.2,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper9 {
    bool set(ttkGridLayout& filter) {
        filter.Modified();
        return true;
    }
    ButtonProperty property{"Debug_Execute", "Execute",
                            R"(Executes the filter with the last applied parameters, which is
handy to re-start pipeline execution from a specific element
without changing parameters.)"_help};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<ttkGridLayout> {
    static constexpr std::string_view uri = "ttk.ttkGridLayout";
    static constexpr std::string_view className = "ttkGridLayout";
    static constexpr std::string_view identifier = "ttkGridLayout";
    static constexpr std::string_view displayName = "TTK GridLayout";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkMultiBlockDataSet", -1, R"(vtkMultiBlockDataSet.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Output Options", {"ColAxis", "ColGap", "RowAxis", "RowGap", "NumberOfRows"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9>
        properties;
    static constexpr std::string_view doc =
        R"(This filter computes a grid layout for the blocks of a vtkMultiBlockDataSet.

Online examples:

- https://topology-tool-kit.github.io/examples/cinemaIO/

- https://topology-tool-kit.github.io/examples/contourTreeAlignment/)";
};

void registerttkGridLayout(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkGridLayout>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
