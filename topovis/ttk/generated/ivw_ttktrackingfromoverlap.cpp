#include "ivw_ttktrackingfromoverlap.h"
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
#include <ttkTrackingFromOverlap.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkTrackingFromOverlap& filter) {
        filter.SetLabelFieldName(property.get().c_str());
        return true;
    }
    StringProperty property{"LabelFieldName", "Labels",
                            R"(Point data that associates a label with each point.)"_help, ""};
};

struct Wrapper1 {
    bool set(ttkTrackingFromOverlap& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper2 {
    bool set(ttkTrackingFromOverlap& filter) {
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

struct Wrapper3 {
    bool set(ttkTrackingFromOverlap& filter) {
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

struct Wrapper4 {
    bool set(ttkTrackingFromOverlap& filter) {
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

struct Wrapper5 {
    bool set(ttkTrackingFromOverlap& filter) {
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
struct VTKTraits<ttkTrackingFromOverlap> {
    static constexpr std::string_view uri = "ttk.ttkTrackingFromOverlap";
    static constexpr std::string_view className = "ttkTrackingFromOverlap";
    static constexpr std::string_view identifier = "ttkTrackingFromOverlap";
    static constexpr std::string_view displayName = "TTK TrackingFromOverlap";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {InputData{
        .identifier = "Input",
        .dataType = "",
        .numComp = -1,
        .doc =
            R"(A vtkMultiBlockDataSet that represents a sequence of vtkPointSets. Optionally: a vtkMultiBlockDataSet that contains multiple sequences, where each sequence corresponds to a level.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Input Options", {"LabelFieldName"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5> properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"ivw(This filter identifies and tracks labeled vtkPointSets across time (and optionally levels) based on spatial overlap, where two points overlap iff their corresponding coordinates are equal. This filter can be executed iteratively and can generate nested tracking graphs.

Related publication:

'Nested Tracking Graphs'.
Jonas Lukasczyk, Gunther Weber, Ross Maciejewski, Christoph Garth, and Heike Leitte.
Computer Graphics Forum (Special Issue, Proceedings Eurographics / IEEE Symposium on Visualization). Vol. 36. No. 3. 2017.

Type:
* 0 - tracking graph,
* 1 - nesting tree

Online examples:

- https://topology-tool-kit.github.io/examples/nestedTrackingFromOverlap/)ivw";
};

void registerttkTrackingFromOverlap(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkTrackingFromOverlap>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
