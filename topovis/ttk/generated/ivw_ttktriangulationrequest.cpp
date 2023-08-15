#include "ivw_ttktriangulationrequest.h"

#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/ttk/processors/ttkgenericprocessor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/fileproperty.h>

#include <tuple>
#include <array>
#include <string_view>

#include <warn/push>
#include <warn/ignore/all>
#include "ttkTriangulationRequest.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
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
    StringProperty property{"SimplexIdentifier", "Simplex identifiers", "0"};
};

struct Wrapper2 {
    bool set(ttkTriangulationRequest& filter) {
        filter.SetKeepAllDataArrays(property.get());
        return true;
    }
    BoolProperty property{"KeepAllDataArrays", "Keep All Data Arrays", true};
};

struct Wrapper3 {
    bool set(ttkTriangulationRequest& filter) {
        filter.SetRequestType(property.get());
        return true;
    }
    OptionPropertyInt property{"RequestType",
                               "Request type",
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
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper5 {
    bool set(ttkTriangulationRequest& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(ttkTriangulationRequest& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkTriangulationRequest& filter) {
        filter.SetCompactTriangulationCacheSize(property.get());
        return true;
    }
    DoubleProperty property{"CompactTriangulationCacheSize", "Cache", 0.2,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<ttkTriangulationRequest> {
    static constexpr std::string_view identifier = "ttkTriangulationRequest";
    static constexpr std::string_view displayName = "TTK TriangulationRequest";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkDataSet", -1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Input options", {"SimplexType", "SimplexIdentifier", "KeepAllDataArrays"}},
        Group{"Output options", {"RequestType"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7>
        properties;
};

void registerttkTriangulationRequest(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkTriangulationRequest>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
