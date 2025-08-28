#include "ivw_ttkripspersistencegenerators.h"
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
#include <ttkRipsPersistenceGenerators.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkRipsPersistenceGenerators& filter) {
        filter.SetSelectFieldsWithRegexp(property.get());
        return true;
    }
    BoolProperty property{"SelectFieldsWithRegexp", "Select Fields with a Regexp",
                          R"(Select input scalar fields matching a regular expression.)"_help,
                          false};
};

struct Wrapper1 {
    bool set(ttkRipsPersistenceGenerators& filter) {
        filter.SetRegexpString(property.get().c_str());
        return true;
    }
    StringProperty property{"Regexp", "Regexp",
                            R"(This regexp will be used to filter the chosen fields. Only
matching ones will be selected.)"_help,
                            ".*"};
};

struct Wrapper2 {
    bool set(ttkRipsPersistenceGenerators& filter) {
        filter.SetInputIsDistanceMatrix(property.get());
        return true;
    }
    BoolProperty property{
        "InputIsDistanceMatrix", "Input is a distance matrix",
        R"(Check this box to read the input table as a distance matrix instead of a point cloud.)"_help,
        false};
};

struct Wrapper3 {
    bool set(ttkRipsPersistenceGenerators& filter) {
        filter.SetSimplexMaximumDiameter(property.get().c_str());
        return true;
    }
    StringProperty property{
        "SimplexMaximumDiameter", "Simplex maximum diameter",
        R"(Simplex maximum diameter in the Rips complex (type "inf" for unlimited diameter).)"_help,
        "inf"};
};

struct Wrapper4 {
    bool set(ttkRipsPersistenceGenerators& filter) {
        filter.SetOutputCascade(property.get());
        return true;
    }
    BoolProperty property{
        "OutputCascade", "Output cascade",
        R"(Check this to output the whole persistent cascade instead of the persistent generators. The edges will be
separated in four classes: the edges killing a 0-cycle (id=0); the edges creating a 1-cycle (id=1); the edges
killing a 1-cycle (id=2); and the other edges of the 1-dimensional persistent cascade (id=3).)"_help,
        false};
};

struct Wrapper5 {
    bool set(ttkRipsPersistenceGenerators& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper6 {
    bool set(ttkRipsPersistenceGenerators& filter) {
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
    bool set(ttkRipsPersistenceGenerators& filter) {
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
    bool set(ttkRipsPersistenceGenerators& filter) {
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
    bool set(ttkRipsPersistenceGenerators& filter) {
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
struct VTKTraits<ttkRipsPersistenceGenerators> {
    static constexpr std::string_view uri = "ttk.ttkRipsPersistenceGenerators";
    static constexpr std::string_view className = "ttkRipsPersistenceGenerators";
    static constexpr std::string_view identifier = "ttkRipsPersistenceGenerators";
    static constexpr std::string_view displayName = "TTK RipsPersistenceGenerators";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 2> inports = {
        InputData{.identifier = "Input",
                  .dataType = "vtkTable",
                  .numComp = -1,
                  .doc = R"(Point cloud to process.)"},
        InputData{.identifier = "Input1",
                  .dataType = "vtkPointSet",
                  .numComp = -1,
                  .doc = R"(Point cloud for geometric representation of persistent generators.)"}};
    inline static std::array<OutputData, 2> outports = {
        OutputData{"port0", "Generators", 0}, OutputData{"port1", "Persistence Diagram", 1}};
    inline static std::array<Group, 3> groups = {
        Group{"Input options",
              {"InputIsDistanceMatrix", "SelectFieldsWithRegexp", "ScalarFields", "Regexp"}},
        Group{"Output options", {"SimplexMaximumDiameter", "OutputCascade"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"ivw(TTK plugin for the computation of the persistence generators of Rips complexes.)ivw";
};

void registerttkRipsPersistenceGenerators(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkRipsPersistenceGenerators>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
