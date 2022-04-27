#include "ivwwrapreebspace.h"

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
#include "ttkReebSpace.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkReebSpace& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"UComponent", "U Component", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 : FieldSelection {
    bool set(ttkReebSpace& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(1, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"VComponent", "V Component", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper2 {
    bool set(ttkReebSpace& filter) {
        filter.SetForceInputOffsetScalarField(property.get());
        return true;
    }
    BoolProperty property{"Withpredefinedoffset", "Force Input Offset Field", false};
};

struct Wrapper3 : FieldSelection {
    bool set(ttkReebSpace& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(2, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"UOffsetField", "U Offset Field", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper4 : FieldSelection {
    bool set(ttkReebSpace& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(3, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"VOffsetField", "V Offset Field", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper5 {
    bool set(ttkReebSpace& filter) {
        filter.SetSimplificationCriterion(property.get());
        return true;
    }
    OptionPropertyInt property{"SimplificationCriterion",
                               "SimplificationCriterion",
                               {{"Domain Volume", "Domain Volume", 0},
                                {"Range Area", "Range Area", 1},
                                {"Hyper Volume", "Hyper Volume", 2}},
                               1};
};

struct Wrapper6 {
    bool set(ttkReebSpace& filter) {
        filter.SetSimplificationThreshold(property.get());
        return true;
    }
    DoubleProperty property{"SimplificationThreshold", "Simplification Threshold", 0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkReebSpace& filter) {
        filter.SetUseOctreeAcceleration(property.get());
        return true;
    }
    BoolProperty property{"OctreeAcceleration", "With Range Query Acceleration", true};
};

struct Wrapper8 {
    bool set(ttkReebSpace& filter) {
        filter.SetZeroSheetValue(property.get());
        return true;
    }
    BoolProperty property{"0-scalar", "With field values", true};
};

struct Wrapper9 {
    bool set(ttkReebSpace& filter) {
        filter.SetZeroSheetVertexId(property.get());
        return true;
    }
    BoolProperty property{"0-vertexId", "With vertex Ids", true};
};

struct Wrapper10 {
    bool set(ttkReebSpace& filter) {
        filter.SetZeroSheetType(property.get());
        return true;
    }
    BoolProperty property{"0-type", "With sheet type", true};
};

struct Wrapper11 {
    bool set(ttkReebSpace& filter) {
        filter.SetZeroSheetId(property.get());
        return true;
    }
    BoolProperty property{"0-sheetId", "With sheet Ids", true};
};

struct Wrapper12 {
    bool set(ttkReebSpace& filter) {
        filter.SetOneSheetValue(property.get());
        return true;
    }
    BoolProperty property{"1-scalar", "With field values", true};
};

struct Wrapper13 {
    bool set(ttkReebSpace& filter) {
        filter.SetOneSheetVertexId(property.get());
        return true;
    }
    BoolProperty property{"1-vertexId", "With vertex Ids", true};
};

struct Wrapper14 {
    bool set(ttkReebSpace& filter) {
        filter.SetOneSheetEdgeId(property.get());
        return true;
    }
    BoolProperty property{"1-edgeId", "With edge Ids", true};
};

struct Wrapper15 {
    bool set(ttkReebSpace& filter) {
        filter.SetOneSheetType(property.get());
        return true;
    }
    BoolProperty property{"1-type", "With sheet type", true};
};

struct Wrapper16 {
    bool set(ttkReebSpace& filter) {
        filter.SetOneSheetId(property.get());
        return true;
    }
    BoolProperty property{"1-sheetId", "With sheet Ids", true};
};

struct Wrapper17 {
    bool set(ttkReebSpace& filter) {
        filter.SetTwoSheets(property.get());
        return true;
    }
    BoolProperty property{"2-sheets", "With 2-sheets", false};
};

struct Wrapper18 {
    bool set(ttkReebSpace& filter) {
        filter.SetTwoSheetValue(property.get());
        return true;
    }
    BoolProperty property{"2-sheetValue", "With field values", true};
};

struct Wrapper19 {
    bool set(ttkReebSpace& filter) {
        filter.SetTwoSheetParameterization(property.get());
        return true;
    }
    BoolProperty property{"2-sheetParameterization", "With edge length parameterization", true};
};

struct Wrapper20 {
    bool set(ttkReebSpace& filter) {
        filter.SetTwoSheetId(property.get());
        return true;
    }
    BoolProperty property{"2-sheetId", "With sheet Ids", true};
};

struct Wrapper21 {
    bool set(ttkReebSpace& filter) {
        filter.SetTwoSheetEdgeId(property.get());
        return true;
    }
    BoolProperty property{"2-sheetEdgeId", "With edge Ids", true};
};

struct Wrapper22 {
    bool set(ttkReebSpace& filter) {
        filter.SetTwoSheetTetId(property.get());
        return true;
    }
    BoolProperty property{"2-sheetTetId", "With tet Ids", true};
};

struct Wrapper23 {
    bool set(ttkReebSpace& filter) {
        filter.SetTwoSheetCaseId(property.get());
        return true;
    }
    BoolProperty property{"2-sheetCaseId", "With case Ids", true};
};

struct Wrapper24 {
    bool set(ttkReebSpace& filter) {
        filter.SetTwoSheetEdgeType(property.get());
        return true;
    }
    BoolProperty property{"2-sheetEdgeType", "With edge type", true};
};

struct Wrapper25 {
    bool set(ttkReebSpace& filter) {
        filter.SetThreeSheetVertexNumber(property.get());
        return true;
    }
    BoolProperty property{"3-sheetVertexNumber", "3-sheet vertex number", true};
};

struct Wrapper26 {
    bool set(ttkReebSpace& filter) {
        filter.SetThreeSheetTetNumber(property.get());
        return true;
    }
    BoolProperty property{"3-sheetTetNumber", "3-sheet tet number", true};
};

struct Wrapper27 {
    bool set(ttkReebSpace& filter) {
        filter.SetThreeSheetExpansion(property.get());
        return true;
    }
    BoolProperty property{"3-sheetExpansion", "3-sheet expansion", true};
};

struct Wrapper28 {
    bool set(ttkReebSpace& filter) {
        filter.SetThreeSheetDomainVolume(property.get());
        return true;
    }
    BoolProperty property{"3-sheetDomainVolume", "3-sheet domain volume", true};
};

struct Wrapper29 {
    bool set(ttkReebSpace& filter) {
        filter.SetThreeSheetRangeArea(property.get());
        return true;
    }
    BoolProperty property{"3-sheetRangeArea", "3-sheet range area", true};
};

struct Wrapper30 {
    bool set(ttkReebSpace& filter) {
        filter.SetThreeSheetHyperVolume(property.get());
        return true;
    }
    BoolProperty property{"3-sheetHyperVolume", "3-sheet hyper volume", true};
};

struct Wrapper31 {
    bool set(ttkReebSpace& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper32 {
    bool set(ttkReebSpace& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper33 {
    bool set(ttkReebSpace& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper34 {
    bool set(ttkReebSpace& filter) {
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
struct TTKTraits<ttkReebSpace> {
    static constexpr std::string_view identifier = "ttkReebSpace";
    static constexpr std::string_view displayName = "TTK ReebSpace";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkDataSet", 1}};
    inline static std::array<OutputData, 4> outports = {
        OutputData{"port0", "0-sheets", 0}, OutputData{"port1", "1-sheets", 1},
        OutputData{"port2", "2-sheets", 2}, OutputData{"port3", "3-sheets", 3}};
    inline static std::array<Group, 6> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Input options",
              {"U Component", "V Component", "With predefined offset", "U Offset Field",
               "V Offset Field", "SimplificationCriterion", "SimplificationThreshold",
               "OctreeAcceleration"}},
        Group{"Output options - 0-sheets", {"0-scalar", "0-vertexId", "0-type", "0-sheetId"}},
        Group{"Output options - 1-sheets",
              {"1-scalar", "1-vertexId", "1-edgeId", "1-type", "1-sheetId"}},
        Group{"Output options - 2-sheets",
              {"2-sheets", "2-sheetValue", "2-sheetParameterization", "2-sheetId", "2-sheetEdgeId",
               "2-sheetTetId", "2-sheetCaseId", "2-sheetEdgeType"}},
        Group{"Output options - 3-sheets",
              {"3-sheetVertexNumber", "3-sheetTetNumber", "3-sheetExpansion", "3-sheetDomainVolume",
               "3-sheetRangeArea", "3-sheetHyperVolume"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11, Wrapper12, Wrapper13, Wrapper14, Wrapper15,
               Wrapper16, Wrapper17, Wrapper18, Wrapper19, Wrapper20, Wrapper21, Wrapper22,
               Wrapper23, Wrapper24, Wrapper25, Wrapper26, Wrapper27, Wrapper28, Wrapper29,
               Wrapper30, Wrapper31, Wrapper32, Wrapper33, Wrapper34>
        properties;
};

void registerttkReebSpace(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkReebSpace>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
