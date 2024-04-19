#include "ivw_ttktriangulationrequest.h"
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
#include <ttkTriangulationRequest.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkTriangulationRequest& filter) {
        filter.SetSimplexType(property.get());
        return true;
    }
    OptionPropertyInt property{"SimplexType",
                               "Simplex",
                               R"(Output simplex.)"_help,
                               {{"Vertex", "Vertex", 0},
                                {"Edge", "Edge", 1},
                                {"Triangle", "Triangle", 2},
                                {"Tetra", "Tetra", 3}},
                               0};
};

struct Wrapper1 {
    bool set(ttkTriangulationRequest& filter) {
        filter.SetSimplexIdentifier(property.get().c_str());
        return true;
    }
    StringProperty property{"SimplexIdentifier", "Simplex identifiers",
                            R"(Output simplex identifiers (comma-separated list).)"_help, "0"};
};

struct Wrapper2 {
    bool set(ttkTriangulationRequest& filter) {
        filter.SetKeepAllDataArrays(property.get());
        return true;
    }
    BoolProperty property{"KeepAllDataArrays", "Keep All Data Arrays",
                          R"(Keep all data arrays.)"_help, true};
};

struct Wrapper3 {
    bool set(ttkTriangulationRequest& filter) {
        filter.SetRequestType(property.get());
        return true;
    }
    OptionPropertyInt property{"RequestType",
                               "Request type",
                               R"(Output request.)"_help,
                               {{"Simplex", "Simplex", 0},
                                {"Facet", "Facet", 1},
                                {"Cofacet", "Cofacet", 2},
                                {"Star", "Star", 3},
                                {"Link", "Link", 4},
                                {"Domain Boundary", "Domain Boundary", 5}},
                               0};
};

struct Wrapper4 {
    bool set(ttkTriangulationRequest& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper5 {
    bool set(ttkTriangulationRequest& filter) {
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

struct Wrapper6 {
    bool set(ttkTriangulationRequest& filter) {
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

struct Wrapper7 {
    bool set(ttkTriangulationRequest& filter) {
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

struct Wrapper8 {
    bool set(ttkTriangulationRequest& filter) {
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
struct VTKTraits<ttkTriangulationRequest> {
    static constexpr std::string_view uri = "ttk.ttkTriangulationRequest";
    static constexpr std::string_view className = "ttkTriangulationRequest";
    static constexpr std::string_view identifier = "ttkTriangulationRequest";
    static constexpr std::string_view displayName = "TTK TriangulationRequest";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1, R"(Data-set to process.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Input options", {"SimplexType", "SimplexIdentifier", "KeepAllDataArrays"}},
        Group{"Output options", {"RequestType"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc = R"(TTK triangulationRequest plugin documentation.)";
};

void registerttkTriangulationRequest(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkTriangulationRequest>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
