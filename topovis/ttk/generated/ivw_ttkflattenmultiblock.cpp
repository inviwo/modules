#include "ivw_ttkflattenmultiblock.h"
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
#include <ttkFlattenMultiBlock.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkFlattenMultiBlock& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper1 {
    bool set(ttkFlattenMultiBlock& filter) {
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

struct Wrapper2 {
    bool set(ttkFlattenMultiBlock& filter) {
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

struct Wrapper3 {
    bool set(ttkFlattenMultiBlock& filter) {
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

struct Wrapper4 {
    bool set(ttkFlattenMultiBlock& filter) {
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
struct VTKTraits<ttkFlattenMultiBlock> {
    static constexpr std::string_view uri = "ttk.ttkFlattenMultiBlock";
    static constexpr std::string_view className = "ttkFlattenMultiBlock";
    static constexpr std::string_view identifier = "ttkFlattenMultiBlock";
    static constexpr std::string_view displayName = "TTK FlattenMultiBlock";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {InputData{
        "Input", "vtkMultiBlockDataSet", -1, R"(Hierarchy of vtkMultiBlockDataSet to process.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 1> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4> properties;
    static constexpr std::string_view doc =
        R"(This filter flattens the top-level hierarchy of a tree
vtkMultiBlockDataSet structure.

The main use-case is to flatten a "Group Datasets" of several
vtkMultiBlockDataSets.

Online examples:

- https://topology-tool-kit.github.io/examples/mergeTreeClustering/

- https://topology-tool-kit.github.io/examples/mergeTreePGA/

- https://topology-tool-kit.github.io/examples/nestedTrackingFromOverlap/)";
};

void registerttkFlattenMultiBlock(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkFlattenMultiBlock>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
