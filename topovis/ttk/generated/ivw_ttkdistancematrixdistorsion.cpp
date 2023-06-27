#include "ivw_ttkdistancematrixdistorsion.h"

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
#include "ttkDistanceMatrixDistorsion.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkDistanceMatrixDistorsion& filter) {
        filter.SetSelectFieldsWithRegexpHigh(property.get());
        return true;
    }
    BoolProperty property{"SelectFieldsWithRegexpHigh", "Select Fields with a Regexp (High matrix)",
                          false};
};

struct Wrapper1 {
    bool set(ttkDistanceMatrixDistorsion& filter) {
        filter.SetRegexpStringHigh(property.get().c_str());
        return true;
    }
    StringProperty property{"RegexpHigh", "RegexpHigh", ".*"};
};

struct Wrapper2 {
    bool set(ttkDistanceMatrixDistorsion& filter) {
        filter.SetSelectFieldsWithRegexpLow(property.get());
        return true;
    }
    BoolProperty property{"SelectFieldsWithRegexpLow", "Select Fields with a Regexp (Low matrix)",
                          false};
};

struct Wrapper3 {
    bool set(ttkDistanceMatrixDistorsion& filter) {
        filter.SetRegexpStringLow(property.get().c_str());
        return true;
    }
    StringProperty property{"RegexpLow", "RegexpLow", ".*"};
};

struct Wrapper4 {
    bool set(ttkDistanceMatrixDistorsion& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper5 {
    bool set(ttkDistanceMatrixDistorsion& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(ttkDistanceMatrixDistorsion& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkDistanceMatrixDistorsion& filter) {
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
struct TTKTraits<ttkDistanceMatrixDistorsion> {
    static constexpr std::string_view identifier = "ttkDistanceMatrixDistorsion";
    static constexpr std::string_view displayName = "TTK DistanceMatrixDistorsion";
    inline static std::array<InputData, 2> inports = {
        InputData{"HighDistanceMatrix", "vtkTable", 1},
        InputData{"LowDistanceMatrix", "vtkTable", -1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Input options",
              {"SelectFieldsWithRegexpHigh", "ScalarFieldsHigh", "RegexpHigh",
               "SelectFieldsWithRegexpLow", "ScalarFieldsLow", "RegexpLow"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7>
        properties;
};

void registerttkDistanceMatrixDistorsion(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkDistanceMatrixDistorsion>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
