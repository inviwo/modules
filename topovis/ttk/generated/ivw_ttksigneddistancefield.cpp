#include "ivw_ttksigneddistancefield.h"
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
#include <ttkSignedDistanceField.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkSignedDistanceField& filter) {
        filter.SetSamplingDimensions(property.get(0), property.get(1), property.get(2));
        return true;
    }
    IntVec3Property property{"SamplingDimensions",
                             "SamplingDimensions",
                             R"(Resolution of each axis.)"_help,
                             ivec3{100, 100, 100},
                             std::pair{ivec3{1, 1, 1}, ConstraintBehavior::Ignore},
                             std::pair{ivec3{100, 100, 100}, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(ttkSignedDistanceField& filter) {
        filter.SetExpandBox(property.get());
        return true;
    }
    BoolProperty property{
        "ExpandBox", "Expand Box",
        R"(This parameter allows to have an exterior border of resolution 1 in the output bounding box compared to the input surface bounding box.)"_help,
        true};
};

struct Wrapper2 {
    bool set(ttkSignedDistanceField& filter) {
        filter.SetBackend(property.get());
        return true;
    }
    OptionPropertyInt property{
        "Backend",
        "Backend",
        R"(Method for the signed distance field computation, exact or approximation (fast marching and fast marching band, being an even rougher approximation).)"_help,
        {{"Exact", "Exact", 0},
         {"Fast Marching [Sethian, 1996]", "Fast Marching [Sethian, 1996]", 1},
         {"Fast Marching Band", "Fast Marching Band", 2}},
        0};
};

struct Wrapper3 {
    bool set(ttkSignedDistanceField& filter) {
        filter.SetFastMarchingOrder(property.get());
        return true;
    }
    OptionPropertyInt property{"FastMarchingOrder",
                               "Fast Marching Order",
                               R"(Fast marching order.)"_help,
                               {{"1", "1", 1}, {"2", "2", 2}},
                               0};
};

struct Wrapper4 {
    bool set(ttkSignedDistanceField& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper5 {
    bool set(ttkSignedDistanceField& filter) {
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
    bool set(ttkSignedDistanceField& filter) {
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
    bool set(ttkSignedDistanceField& filter) {
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
    bool set(ttkSignedDistanceField& filter) {
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
struct VTKTraits<ttkSignedDistanceField> {
    static constexpr std::string_view uri = "ttk.ttkSignedDistanceField";
    static constexpr std::string_view className = "ttkSignedDistanceField";
    static constexpr std::string_view identifier = "ttkSignedDistanceField";
    static constexpr std::string_view displayName = "TTK SignedDistanceField";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1,
                  R"(This property specifies the dataset whose data will be probed.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Output options", {"SamplingDimensions", "ExpandBox", "Backend"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"ivw(This filter computes a signed distance field given a surface in input.

It implements three backends (accelerated with a BVH data structure):

- The exact backend

- The fast marching backend, this method simulates a "wave" that move starting from the input surface and solve the eikonal equation vertex by vertex to approximate the signed distance field. It corresponds to the the following reference:

J.A. Sethian.
A Fast Marching Level Set Method for Monotonically Advancing Fronts,
Proc. Natl. Acad. Sci., 93, 4, pp.1591--1595, 1996

- The fast marching band backend, a variant of the fast marching for which all the vertices being not yet updated and nearest the input surface are updated (instead of just one in the fast marching backend). It results in a faster method (due to parallelism) but is a rougher approximation.

Online examples:

- https://topology-tool-kit.github.io/examples/topologicalOptimization_pegasus/

- https://topology-tool-kit.github.io/examples/topologicalOptimization_torus/)ivw";
};

void registerttkSignedDistanceField(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkSignedDistanceField>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
