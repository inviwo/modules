#include "ivwwrapcontouraroundpoint.h"

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
#include "ttkContourAroundPoint.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkContourAroundPoint& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"ScalarAttribute", "Scalar Attribute", {}, 0};

    static constexpr std::string_view inport = "Domain";
};

struct Wrapper1 {
    bool set(ttkContourAroundPoint& filter) {
        filter.Setui_extension(property.get());
        return true;
    }
    DoubleProperty property{"ui_extension", "Contour extension", 67.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(ttkContourAroundPoint& filter) {
        filter.Setui_sizeFilter(property.get());
        return true;
    }
    DoubleProperty property{"ui_sizeFilter", "Size filter", 7.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{10000.0, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(ttkContourAroundPoint& filter) {
        filter.Setui_spherical(property.get());
        return true;
    }
    BoolProperty property{"ui_spherical", "Domain is a sphere", false};
};

struct Wrapper4 {
    bool set(ttkContourAroundPoint& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper5 {
    bool set(ttkContourAroundPoint& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(ttkContourAroundPoint& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkContourAroundPoint& filter) {
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
struct TTKTraits<ttkContourAroundPoint> {
    static constexpr std::string_view identifier = "ttkContourAroundPoint";
    static constexpr std::string_view displayName = "TTK ContourAroundPoint";
    inline static std::array<InputData, 3> inports = {InputData{"Domain", "vtkDataSet", 1},
                                                      InputData{"Nodes", "vtkUnstructuredGrid", 1},
                                                      InputData{"Arcs", "vtkUnstructuredGrid", 1}};
    inline static std::array<OutputData, 2> outports = {OutputData{"port0", "Contours", 0},
                                                        OutputData{"port1", "Centroids", 1}};
    inline static std::array<Group, 2> groups = {
        Group{"Input Parameters",
              {"ScalarAttribute", "ui_extension", "ui_sizeFilter", "ui_spherical"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7>
        properties;
};

void registerttkContourAroundPoint(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkContourAroundPoint>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
