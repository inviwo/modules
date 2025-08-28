#include "ivw_ttkmergetreeautoencoderdecoding.h"
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
#include <ttkMergeTreeAutoencoderDecoding.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkMergeTreeAutoencoderDecoding& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper1 {
    bool set(ttkMergeTreeAutoencoderDecoding& filter) {
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
    bool set(ttkMergeTreeAutoencoderDecoding& filter) {
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
    bool set(ttkMergeTreeAutoencoderDecoding& filter) {
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
    bool set(ttkMergeTreeAutoencoderDecoding& filter) {
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
struct VTKTraits<ttkMergeTreeAutoencoderDecoding> {
    static constexpr std::string_view uri = "ttk.ttkMergeTreeAutoencoderDecoding";
    static constexpr std::string_view className = "ttkMergeTreeAutoencoderDecoding";
    static constexpr std::string_view identifier = "ttkMergeTreeAutoencoderDecoding";
    static constexpr std::string_view displayName = "TTK MergeTreeAutoencoderDecoding";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 3> inports = {
        InputData{.identifier = "Origins",
                  .dataType = "vtkMultiBlockDataSet",
                  .numComp = 1,
                  .doc = R"(Origin of each basis of each layer.)"},
        InputData{.identifier = "Bases Vectors",
                  .dataType = "vtkTable",
                  .numComp = 1,
                  .doc = R"(Vectors defining each basis.)"},
        InputData{.identifier = "Coefficients",
                  .dataType = "vtkTable",
                  .numComp = 1,
                  .doc = R"(Coefficients of the input trees in each basis.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Input options", {}}, Group{"Output options", {}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4> properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"ivw(This VTK filter uses the ttk::MergeTreeAutoencoderDecoding module to compute a decoding of merge trees or persistence diagrams given the parameters of a Wasserstein Auto-Encoder.

Related Publication:
"Wasserstein Auto-Encoders of Merge Trees (and Persistence Diagrams)"
Mathieu Pont,  Julien Tierny.
IEEE Transactions on Visualization and Computer Graphics, 2023

Online examples:

- https://topology-tool-kit.github.io/examples/mergeTreeWAE/

- https://topology-tool-kit.github.io/examples/persistenceDiagramWAE/)ivw";
};

void registerttkMergeTreeAutoencoderDecoding(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkMergeTreeAutoencoderDecoding>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
