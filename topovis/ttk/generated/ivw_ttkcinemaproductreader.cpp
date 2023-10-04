#include "ivw_ttkcinemaproductreader.h"

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
#include <ttkCinemaProductReader.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkCinemaProductReader& filter) {
        filter.SetFilepathColumnName(property.get().c_str());
        return true;
    }
    StringProperty property{"SelectColumn", "Filepath Column",
                            R"(Name of the column containing data product references.)"_help,
                            "FILE"};
};

struct Wrapper1 {
    bool set(ttkCinemaProductReader& filter) {
        filter.SetAddFieldDataRecursively(property.get());
        return true;
    }
    BoolProperty property{
        "AddFieldDataRecursively", "Add Field Data Recursively",
        R"(Controls if row data should be added to all children of a vtkMultiBlockDataSet.)"_help,
        false};
};

struct Wrapper2 {
    bool set(ttkCinemaProductReader& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper3 {
    bool set(ttkCinemaProductReader& filter) {
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
    bool set(ttkCinemaProductReader& filter) {
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
    bool set(ttkCinemaProductReader& filter) {
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
    bool set(ttkCinemaProductReader& filter) {
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
struct TTKTraits<ttkCinemaProductReader> {
    static constexpr std::string_view className = "ttkCinemaProductReader";
    static constexpr std::string_view identifier = "ttkCinemaProductReader";
    static constexpr std::string_view displayName = "TTK CinemaProductReader";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkTable", -1,
                  R"(vtkTable containing product references in at least one column.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Input Options", {"SelectColumn", "AddFieldDataRecursively"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6> properties;
    static constexpr std::string_view doc =
        R"(This filter reads the products that are referenced in a vtkTable. The results are stored in a vtkMultiBlockDataSet where each block corresponds to a row of the table with consistent ordering.

Online examples:

- https://topology-tool-kit.github.io/examples/cinemaIO/

- https://topology-tool-kit.github.io/examples/clusteringKelvinHelmholtzInstabilities/

- https://topology-tool-kit.github.io/examples/contourTreeAlignment/

- https://topology-tool-kit.github.io/examples/mergeTreeClustering/

- https://topology-tool-kit.github.io/examples/mergeTreeFeatureTracking/

- https://topology-tool-kit.github.io/examples/mergeTreePGA/

- https://topology-tool-kit.github.io/examples/mergeTreeTemporalReduction/

- https://topology-tool-kit.github.io/examples/nestedTrackingFromOverlap/

- https://topology-tool-kit.github.io/examples/persistenceDiagramClustering/

- https://topology-tool-kit.github.io/examples/persistenceDiagramDistance/

- https://topology-tool-kit.github.io/examples/persistentGenerators_periodicPicture/)";
};

void registerttkCinemaProductReader(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkCinemaProductReader>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
