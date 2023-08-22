#include "ivw_ttkicosphere.h"

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
#include <ttkIcosphere.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkIcosphere& filter) {
        filter.SetNumberOfSubdivisions(property.get());
        return true;
    }
    IntProperty property{"NumberOfSubdivisions",
                         "Subdivisions",
                         R"(Number of subdevisions.)"_help,
                         0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(ttkIcosphere& filter) {
        filter.SetRadius(property.get());
        return true;
    }
    DoubleProperty property{
        "Radius",
        "Radius",
        R"(Radius in world coordinates. If an input is provided then the radius is used as a scaling factor of the resulting bounding icosphere.)"_help,
        1.0,
        std::pair{0.0, ConstraintBehavior::Ignore},
        std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(ttkIcosphere& filter) {
        filter.SetCenter(property.get(0), property.get(1), property.get(2));
        return true;
    }
    DoubleVec3Property property{
        "Center",
        "Center",
        R"(Center of the icosphere. If an input is provided this parameter will be overwritten.)"_help,
        dvec3{0.0, 0.0, 0.0},
        std::pair{dvec3{0.0, 0.0, 0.0}, ConstraintBehavior::Ignore},
        std::pair{dvec3{100.0, 100.0, 100.0}, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(ttkIcosphere& filter) {
        filter.SetComputeNormals(property.get());
        return true;
    }
    BoolProperty property{"ComputeNormals", "Compute Normals",
                          R"(Controls if the output contains normals at the vertices.)"_help,
                          false};
};

struct Wrapper4 {
    bool set(ttkIcosphere& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper5 {
    bool set(ttkIcosphere& filter) {
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
    bool set(ttkIcosphere& filter) {
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
    bool set(ttkIcosphere& filter) {
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
    bool set(ttkIcosphere& filter) {
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
struct TTKTraits<ttkIcosphere> {
    static constexpr std::string_view className = "ttkIcosphere";
    static constexpr std::string_view identifier = "ttkIcosphere";
    static constexpr std::string_view displayName = "TTK Icosphere";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 0> inports = {};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Output Options", {"NumberOfSubdivisions", "Radius", "Center", "ComputeNormals"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8>
        properties;
    static constexpr std::string_view doc =
        R"(This filter creates an Icosphere with a specified radius, center, and number of subdivisions. Alternatively, by providing an optional input, the filter will automatically determine the radius and center such that the resulting Icosphere encapsulates the input object. In this case, the entered radius parameter is used as a scaling factor.)";
};

void registerttkIcosphere(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkIcosphere>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
