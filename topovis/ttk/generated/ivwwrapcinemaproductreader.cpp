#include "ivwwrapcinemaproductreader.h"

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
#include "ttkCinemaProductReader.h"
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
    StringProperty property{"SelectColumn", "Filepath Column", "FILE"};
};

struct Wrapper1 {
    bool set(ttkCinemaProductReader& filter) {
        filter.SetAddFieldDataRecursively(property.get());
        return true;
    }
    BoolProperty property{"AddFieldDataRecursively", "Add Field Data Recursively", false};
};

struct Wrapper2 {
    bool set(ttkCinemaProductReader& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper3 {
    bool set(ttkCinemaProductReader& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper4 {
    bool set(ttkCinemaProductReader& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkCinemaProductReader& filter) {
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
struct TTKTraits<ttkCinemaProductReader> {
    static constexpr std::string_view identifier = "ttkCinemaProductReader";
    static constexpr std::string_view displayName = "TTK CinemaProductReader";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkTable", -1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Input Options", {"SelectColumn", "AddFieldDataRecursively"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5> properties;
};

void registerttkCinemaProductReader(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkCinemaProductReader>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
