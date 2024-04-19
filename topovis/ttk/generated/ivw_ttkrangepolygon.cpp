#include "ivw_ttkrangepolygon.h"
#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/vtk/processors/vtkgenericprocessor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/buttonproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/fileproperty.h>
#include <inviwo/ttk/util/ttkprocessorutils.h>

#include <tuple>
#include <array>
#include <string_view>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkDataObject.h>
#include <ttkRangePolygon.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkRangePolygon& filter) {
        filter.SetClosedLoop(property.get());
        return true;
    }
    BoolProperty property{"ClosePolygon", "Closed Polygon",
                          R"(Systematically close the range polygon.)"_help, false};
};

struct Wrapper1 {
    bool set(ttkRangePolygon& filter) {
        filter.SetNumberOfIterations(property.get());
        return true;
    }
    IntProperty property{"NumberOfIterations",
                         "Number Of Smoothing Iterations",
                         R"(Number of smoothing iterations.)"_help,
                         0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(ttkRangePolygon& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper3 {
    bool set(ttkRangePolygon& filter) {
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

struct Wrapper4 {
    bool set(ttkRangePolygon& filter) {
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

struct Wrapper5 {
    bool set(ttkRangePolygon& filter) {
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

struct Wrapper6 {
    bool set(ttkRangePolygon& filter) {
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
struct VTKTraits<ttkRangePolygon> {
    static constexpr std::string_view uri = "ttk.ttkRangePolygon";
    static constexpr std::string_view className = "ttkRangePolygon";
    static constexpr std::string_view identifier = "ttkRangePolygon";
    static constexpr std::string_view displayName = "TTK RangePolygon";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkUnstructuredGrid", -1, R"(Input range cell selection.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Input options", {"ClosePolygon", "NumberOfIterations"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6> properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"(Given an input 2D selection, this plugin produces a polygon to be used as
an input to vtkFiberSurface. Typically, users generate a 2D selection from
the continuous scatterplot and this plugin translates this selection into
a valid range polygon.

The user can either select:

* Cells on the continuous scatterplot ("Select Cells with Polygon")
WARNING: Users may need to zoom in sufficiently so/ that the "Extract
Selection" indeed captures all of the user brushed cells.
WARNING: In this case, the generated polygon may count A LOT of
edges, which will seriously increase run-times. The next alternative is the
default recommendation.

* Points on the continuous scatterplot ("Interactive Select Points on")
WARNING: This feature will only work properly with the TTK-branded ParaView
(ParaView sources need to be patched with TTK fixes, see the documentation).

Related publication
"Fast and Exact Fiber Surface Extraction for Tetrahedral Meshes"
Pavol Klacansky, Julien Tierny, Hamish Carr, Zhao Geng
IEEE Transactions on Visualization and Computer Graphics, 2016.

See also ContinuousScatterplot, FiberSurface, Fiber, ProjectionFromField.

Online examples:

- https://topology-tool-kit.github.io/examples/builtInExample2/)";
};

void registerttkRangePolygon(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkRangePolygon>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
