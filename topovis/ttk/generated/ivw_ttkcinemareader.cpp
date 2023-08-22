#include "ivw_ttkcinemareader.h"

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
#include <ttkCinemaReader.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkCinemaReader& filter) {
        if (property.get().empty()) return false;
        filter.SetDatabasePath(property.get().string().c_str());
        return true;
    }
    FileProperty property{"DatabasePath", "Database Path",
                          R"(Path to a Cinema Spec D database folder.)"_help,
                          std::filesystem::path{""}};
};

struct Wrapper1 {
    bool set(ttkCinemaReader& filter) {
        filter.SetFilePathColumnNames(property.get().c_str());
        return true;
    }
    StringProperty property{"FilePathColumnNames", "File Path Columns",
                            R"(Columns that contain file paths separated by comma.)"_help, "FILE"};
};

struct Wrapper2 {
    bool set(ttkCinemaReader& filter) {
        filter.Modified();
        return true;
    }
    ButtonProperty property{"ReloadDatabase", "Reload Database", R"(Reload Cinema Database.)"_help};
};

struct Wrapper3 {
    bool set(ttkCinemaReader& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper4 {
    bool set(ttkCinemaReader& filter) {
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

struct Wrapper5 {
    bool set(ttkCinemaReader& filter) {
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

struct Wrapper6 {
    bool set(ttkCinemaReader& filter) {
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

struct Wrapper7 {
    bool set(ttkCinemaReader& filter) {
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
struct TTKTraits<ttkCinemaReader> {
    static constexpr std::string_view className = "ttkCinemaReader";
    static constexpr std::string_view identifier = "ttkCinemaReader";
    static constexpr std::string_view displayName = "TTK CinemaReader";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 0> inports = {};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Input Options", {"DatabasePath", "FilePathColumnNames"}},
        Group{"Commands", {"ReloadDatabase"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7>
        properties;
    static constexpr std::string_view doc =
        R"(This source reads the content of a Cinema Spec D database by converting the corresponding data.csv file into a vtkTable.

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

void registerttkCinemaReader(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkCinemaReader>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
