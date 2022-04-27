#include "ivwwrapcinemawriter.h"

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
#include "ttkCinemaWriter.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkCinemaWriter& filter) {
        if (property.get().empty()) return false;
        filter.SetDatabasePath(property.get().c_str());
        return true;
    }
    FileProperty property{"DatabasePath", "DatabasePath", ""};
};

struct Wrapper1 {
    bool set(ttkCinemaWriter& filter) {
        filter.SetCompressionLevel(property.get());
        return true;
    }
    IntProperty property{"CompressionLevel", "CompressionLevel", 5,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{9, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(ttkCinemaWriter& filter) {
        filter.SetFormat(property.get());
        return true;
    }
    OptionPropertyInt property{"Format",
                               "Store as",
                               {{"VTK File", "VTK File", 0},
                                {"PNG Image", "PNG Image", 1},
                                {"TTK Compression", "TTK Compression", 2}},
                               0};
};

struct Wrapper3 {
    bool set(ttkCinemaWriter& filter) {
        filter.SetIterateMultiBlock(property.get());
        return true;
    }
    BoolProperty property{"IterateMultiBlock", "IterateMultiBlock", false};
};

struct Wrapper4 {
    bool set(ttkCinemaWriter& filter) {
        filter.SetForwardInput(property.get());
        return true;
    }
    BoolProperty property{"ForwardInput", "ForwardInput", true};
};

struct Wrapper5 {
    bool set(ttkCinemaWriter& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper6 {
    bool set(ttkCinemaWriter& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkCinemaWriter& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper8 {
    bool set(ttkCinemaWriter& filter) {
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
struct TTKTraits<ttkCinemaWriter> {
    static constexpr std::string_view identifier = "ttkCinemaWriter";
    static constexpr std::string_view displayName = "TTK CinemaWriter";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "", -1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 4> groups = {
        Group{"Output Options",
              {"DatabasePath", "CompressionLevel", "Format", "IterateMultiBlock", "ForwardInput"}},
        Group{"Commands", {"DeleteDatabase"}},
        Group{"Topological Compression",
              {"Scalar Field", "CompressionType", "Tolerance", "Subdivide", "MaximumError",
               "ZFPTolerance", "ZFPOnly", "UseTopologicalSimplification", "SQMethod"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8>
        properties;
};

void registerttkCinemaWriter(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkCinemaWriter>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
