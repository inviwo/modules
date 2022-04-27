#include "ivwwraptopologicalcompression.h"

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
#include "ttkTopologicalCompression.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkTopologicalCompression& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"ScalarField", "Scalar Field", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(ttkTopologicalCompression& filter) {
        filter.SetCompressionType(property.get());
        return true;
    }
    OptionPropertyInt property{
        "CompressionType",
        "Compression type",
        {{"Driven by persistence diagram", "Driven by persistence diagram", 0}},
        0};
};

struct Wrapper2 {
    bool set(ttkTopologicalCompression& filter) {
        filter.SetTolerance(property.get());
        return true;
    }
    DoubleProperty property{"Tolerance", "Topological loss (persistence percentage)", 10.0,
                            std::pair{1.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(ttkTopologicalCompression& filter) {
        filter.SetSubdivide(property.get());
        return true;
    }
    BoolProperty property{"Subdivide", "Enable maximum pointwise error control", false};
};

struct Wrapper4 {
    bool set(ttkTopologicalCompression& filter) {
        filter.SetMaximumError(property.get());
        return true;
    }
    DoubleProperty property{"MaximumError", "Maximum pointwise error (percentage)", 10.0,
                            std::pair{1.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkTopologicalCompression& filter) {
        filter.SetUseTopologicalSimplification(property.get());
        return true;
    }
    BoolProperty property{"UseTopologicalSimplification", "Simplify at compression (slower)", true};
};

struct Wrapper6 {
    bool set(ttkTopologicalCompression& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper7 {
    bool set(ttkTopologicalCompression& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper8 {
    bool set(ttkTopologicalCompression& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper9 {
    bool set(ttkTopologicalCompression& filter) {
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
struct TTKTraits<ttkTopologicalCompression> {
    static constexpr std::string_view identifier = "ttkTopologicalCompression";
    static constexpr std::string_view displayName = "TTK TopologicalCompression";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkImageData", 1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Input", {"Scalar Field"}},
        Group{"Topology aware compression",
              {"CompressionType", "Tolerance", "Subdivide", "MaximumError",
               "UseTopologicalSimplification", "SQMethod"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9>
        properties;
};

void registerttkTopologicalCompression(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkTopologicalCompression>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
