#include "ivw_ttkdistancefield.h"

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
#include "ttkDistanceField.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkDistanceField& filter) {
        filter.SetForceInputVertexScalarField(property.get());
        return true;
    }
    BoolProperty property{"ForceInputVertexScalarField", "Force Input Vertex ScalarField", false};
};

struct Wrapper1 : FieldSelection {
    bool set(ttkDistanceField& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{
        "InputVertexScalarFieldNameNew", "Vertex Identifier Field", {}, 0};

    static constexpr std::string_view inport = "Sources";
};

struct Wrapper2 {
    bool set(ttkDistanceField& filter) {
        filter.SetOutputScalarFieldType(property.get());
        return true;
    }
    OptionPropertyInt property{"OutputScalarFieldType",
                               "Output field type",
                               {{"Float", "Float", 0}, {"Double", "Double", 1}},
                               0};
};

struct Wrapper3 {
    bool set(ttkDistanceField& filter) {
        filter.SetOutputScalarFieldName(property.get().c_str());
        return true;
    }
    StringProperty property{"OutputScalarFieldName", "Output field name", "OutputDistanceField"};
};

struct Wrapper4 {
    bool set(ttkDistanceField& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper5 {
    bool set(ttkDistanceField& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(ttkDistanceField& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkDistanceField& filter) {
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
struct TTKTraits<ttkDistanceField> {
    static constexpr std::string_view identifier = "ttkDistanceField";
    static constexpr std::string_view displayName = "TTK DistanceField";
    inline static std::array<InputData, 2> inports = {InputData{"Domain", "vtkDataSet", 1},
                                                      InputData{"Sources", "vtkPointSet", 1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Input options",
              {"Sources", "ForceInputVertexScalarField", "InputVertexScalarFieldNameNew"}},
        Group{"Output options", {"OutputScalarFieldType", "OutputScalarFieldName"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7>
        properties;
};

void registerttkDistanceField(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkDistanceField>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
