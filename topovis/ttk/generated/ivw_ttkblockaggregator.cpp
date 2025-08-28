#include "ivw_ttkblockaggregator.h"
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
#include <ttkBlockAggregator.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkBlockAggregator& filter) {
        filter.SetForceReset(property.get());
        return true;
    }
    BoolProperty property{"ForceReset", "Force Reset",
                          R"(Force deletion of previously aggregated data.)"_help, false};
};

struct Wrapper1 {
    bool set(ttkBlockAggregator& filter) {
        filter.SetFlattenInput(property.get());
        return true;
    }
    BoolProperty property{
        "FlattenInput", "Flatten Input",
        R"(If enabled and if the input vtkDataObject is a 'vtkMultiBlockDataSet' then this filter will add its blocks to the output and not the vtkMultiBlockDataSet itself.)"_help,
        true};
};

struct Wrapper2 {
    bool set(ttkBlockAggregator& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper3 {
    bool set(ttkBlockAggregator& filter) {
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
    bool set(ttkBlockAggregator& filter) {
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
    bool set(ttkBlockAggregator& filter) {
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
    bool set(ttkBlockAggregator& filter) {
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
struct VTKTraits<ttkBlockAggregator> {
    static constexpr std::string_view uri = "ttk.ttkBlockAggregator";
    static constexpr std::string_view className = "ttkBlockAggregator";
    static constexpr std::string_view identifier = "ttkBlockAggregator";
    static constexpr std::string_view displayName = "TTK BlockAggregator";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{.identifier = "Input",
                  .dataType = "",
                  .numComp = -1,
                  .doc = R"(vtkDataObjects that will be added as blocks.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 1> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6> properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"ivw(This filter appends every input vtkDataObject as a block to an output vtkMultiBlockDataSet.

Online examples:

- https://topology-tool-kit.github.io/examples/contourTreeAlignment/

- https://topology-tool-kit.github.io/examples/mergeTreeClustering/

- https://topology-tool-kit.github.io/examples/mergeTreeFeatureTracking/

- https://topology-tool-kit.github.io/examples/mergeTreePGA/

- https://topology-tool-kit.github.io/examples/mergeTreeWAE/

- https://topology-tool-kit.github.io/examples/nestedTrackingFromOverlap/

- https://topology-tool-kit.github.io/examples/persistenceDiagramPGA/)ivw";
};

void registerttkBlockAggregator(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkBlockAggregator>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
