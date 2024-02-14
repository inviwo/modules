#include "ivw_ttkgaussianpointcloud.h"
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
#include <ttkGaussianPointCloud.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkGaussianPointCloud& filter) {
        filter.SetNumberOfSamples(property.get());
        return true;
    }
    IntProperty property{"NumberOfSamples",
                         "Number of Samples",
                         R"(Number of samples to cast.)"_help,
                         1000,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{1000000, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(ttkGaussianPointCloud& filter) {
        filter.SetDimension(property.get());
        return true;
    }
    OptionPropertyInt property{"Dimension",
                               "Dimension",
                               R"(Dimension of the point cloud.)"_help,
                               {{"1", "1", 1}, {"2", "2", 2}, {"3", "3", 3}},
                               1};
};

struct Wrapper2 {
    bool set(ttkGaussianPointCloud& filter) {
        filter.SetRandomSeed(property.get());
        return true;
    }
    IntProperty property{"Seed",
                         "Random Seed",
                         R"()"_help,
                         0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(ttkGaussianPointCloud& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper4 {
    bool set(ttkGaussianPointCloud& filter) {
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
    bool set(ttkGaussianPointCloud& filter) {
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
    bool set(ttkGaussianPointCloud& filter) {
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
    bool set(ttkGaussianPointCloud& filter) {
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
struct VTKTraits<ttkGaussianPointCloud> {
    static constexpr std::string_view uri = "ttk.ttkGaussianPointCloud";
    static constexpr std::string_view className = "ttkGaussianPointCloud";
    static constexpr std::string_view identifier = "ttkGaussianPointCloud";
    static constexpr std::string_view displayName = "TTK GaussianPointCloud";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 0> inports = {};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Output Options", {"NumberOfSamples", "Dimension", "Seed"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7>
        properties;
    static constexpr std::string_view doc =
        R"(This module generates a 1D, 2D or 3D point cloud by randomly
casting samples from a Gaussian distribution.)";
};

void registerttkGaussianPointCloud(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkGaussianPointCloud>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
