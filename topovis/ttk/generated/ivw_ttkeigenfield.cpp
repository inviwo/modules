#include "ivw_ttkeigenfield.h"

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
#include "ttkEigenField.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkEigenField& filter) {
        filter.SetEigenNumber(property.get());
        return true;
    }
    IntProperty property{"EigenNumber", "Number of eigenfunctions", 500,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(ttkEigenField& filter) {
        filter.SetComputeStatistics(property.get());
        return true;
    }
    BoolProperty property{"ComputeStatistics", "Compute statistics", false};
};

struct Wrapper2 {
    bool set(ttkEigenField& filter) {
        filter.SetOutputFieldName(property.get().c_str());
        return true;
    }
    StringProperty property{"OutputFieldName", "Output scalar field name", "OutputEigenFunctions"};
};

struct Wrapper3 {
    bool set(ttkEigenField& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper4 {
    bool set(ttkEigenField& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkEigenField& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(ttkEigenField& filter) {
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
struct TTKTraits<ttkEigenField> {
    static constexpr std::string_view identifier = "ttkEigenField";
    static constexpr std::string_view displayName = "TTK EigenField";
    inline static std::array<InputData, 1> inports = {InputData{"Domain", "vtkDataSet", -1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Input options", {"EigenNumber"}},
        Group{"Output options", {"OutputFieldName", "ComputeStatistics"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6> properties;
};

void registerttkEigenField(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkEigenField>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
