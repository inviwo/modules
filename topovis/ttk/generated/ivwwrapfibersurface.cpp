#include "ivwwrapfibersurface.h"

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
#include "ttkFiberSurface.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkFiberSurface& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"DomainUComponent", "Domain U Component", {}, 0};

    static constexpr std::string_view inport = "Input Domain";
};

struct Wrapper1 : FieldSelection {
    bool set(ttkFiberSurface& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(1, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"DomainVComponent", "Domain V Component", {}, 0};

    static constexpr std::string_view inport = "Input Domain";
};

struct Wrapper2 : FieldSelection {
    bool set(ttkFiberSurface& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(2, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"PolygonUComponent", "Polygon U Component", {}, 0};

    static constexpr std::string_view inport = "Range Polygon";
};

struct Wrapper3 : FieldSelection {
    bool set(ttkFiberSurface& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(3, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"PolygonVComponent", "Polygon V Component", {}, 0};

    static constexpr std::string_view inport = "Range Polygon";
};

struct Wrapper4 {
    bool set(ttkFiberSurface& filter) {
        filter.SetRangeCoordinates(property.get());
        return true;
    }
    BoolProperty property{"WithRangeCoordinates", "With Range Coordinates", true};
};

struct Wrapper5 {
    bool set(ttkFiberSurface& filter) {
        filter.SetEdgeParameterization(property.get());
        return true;
    }
    BoolProperty property{"WithEdgeParameterization", "With Edge Parameterization", true};
};

struct Wrapper6 {
    bool set(ttkFiberSurface& filter) {
        filter.SetEdgeIds(property.get());
        return true;
    }
    BoolProperty property{"WithEdgeIds", "With Edge Identifiers", true};
};

struct Wrapper7 {
    bool set(ttkFiberSurface& filter) {
        filter.SetTetIds(property.get());
        return true;
    }
    BoolProperty property{"WithTetIds", "With Tet Identifiers", true};
};

struct Wrapper8 {
    bool set(ttkFiberSurface& filter) {
        filter.SetCaseIds(property.get());
        return true;
    }
    BoolProperty property{"WithCaseIds", "With Case Identifiers", true};
};

struct Wrapper9 {
    bool set(ttkFiberSurface& filter) {
        filter.SetPointMerge(property.get());
        return true;
    }
    BoolProperty property{"WithPointMerge", "With Point Merging", false};
};

struct Wrapper10 {
    bool set(ttkFiberSurface& filter) {
        filter.SetPointMergeDistanceThreshold(property.get());
        return true;
    }
    DoubleProperty property{"PointMergeDistanceThreshold", "Distance Threshold", 1e-06,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{10.0, ConstraintBehavior::Ignore}};
};

struct Wrapper11 {
    bool set(ttkFiberSurface& filter) {
        filter.SetRangeOctree(property.get());
        return true;
    }
    BoolProperty property{"WithOctree", "With Range Driven Octree", true};
};

struct Wrapper12 {
    bool set(ttkFiberSurface& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper13 {
    bool set(ttkFiberSurface& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper14 {
    bool set(ttkFiberSurface& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper15 {
    bool set(ttkFiberSurface& filter) {
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
struct TTKTraits<ttkFiberSurface> {
    static constexpr std::string_view identifier = "ttkFiberSurface";
    static constexpr std::string_view displayName = "TTK FiberSurface";
    inline static std::array<InputData, 2> inports = {
        InputData{"Input Domain", "vtkDataSet", 1},
        InputData{"Range Polygon", "vtkUnstructuredGrid", 1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 4> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Input options",
              {"Domain U Component", "Domain V Component", "Polygon U Component",
               "Polygon V Component"}},
        Group{"Pre-processing", {"WithOctree"}},
        Group{"Output options",
              {"WithPointMerge", "PointMergeDistanceThreshold", "WithRangeCoordinates",
               "WithEdgeParameterization", "WithEdgeIds", "WithTetIds", "WithCaseIds"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11, Wrapper12, Wrapper13, Wrapper14, Wrapper15>
        properties;
};

void registerttkFiberSurface(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkFiberSurface>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
