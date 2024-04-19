#include "ivw_ttkeigenfield.h"
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
#include <ttkEigenField.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkEigenField& filter) {
        filter.SetEigenNumber(property.get());
        return true;
    }
    IntProperty property{"EigenNumber",
                         "Number of eigenfunctions",
                         R"(Select the number of eigenfunctions to compute.)"_help,
                         500,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(ttkEigenField& filter) {
        filter.SetComputeStatistics(property.get());
        return true;
    }
    BoolProperty property{"ComputeStatistics", "Compute statistics",
                          R"(Also compute statistics (min, max, sum, average) on
generated eigenfunctions.)"_help,
                          false};
};

struct Wrapper2 {
    bool set(ttkEigenField& filter) {
        filter.SetOutputFieldName(property.get().c_str());
        return true;
    }
    StringProperty property{"OutputFieldName", "Output scalar field name",
                            R"(Select the name of the output eigenfunctions field.)"_help,
                            "OutputEigenFunctions"};
};

struct Wrapper3 {
    bool set(ttkEigenField& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper4 {
    bool set(ttkEigenField& filter) {
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
    bool set(ttkEigenField& filter) {
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
    bool set(ttkEigenField& filter) {
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
    bool set(ttkEigenField& filter) {
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
struct VTKTraits<ttkEigenField> {
    static constexpr std::string_view uri = "ttk.ttkEigenField";
    static constexpr std::string_view className = "ttkEigenField";
    static constexpr std::string_view identifier = "ttkEigenField";
    static constexpr std::string_view displayName = "TTK EigenField";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Domain", "vtkDataSet", -1, R"(Data-set to process.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Input options", {"EigenNumber"}},
        Group{"Output options", {"OutputFieldName", "ComputeStatistics"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"(This plugin computes the first eigenfunctions of a given
triangular surface mesh.
Related publication
"Spectral surface quadrangulation"
Shen Dong, Peer-Timo Bremer, Michael Garland, Valerio Pascucci, John C. Hart
SIGGRAPH 2006

Online examples:

- https://topology-tool-kit.github.io/examples/morseSmaleQuadrangulation/

- https://topology-tool-kit.github.io/examples/persistentGenerators_casting/

- https://topology-tool-kit.github.io/examples/persistentGenerators_fertility/

- https://topology-tool-kit.github.io/examples/persistentGenerators_skull/)";
};

void registerttkEigenField(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkEigenField>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
