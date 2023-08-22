#include "ivw_ttkcontouraroundpoint.h"

#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/ttk/processors/ttkgenericprocessor.h>
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
#include <ttkContourAroundPoint.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkContourAroundPoint& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, fieldAssociation.get(), name.get().c_str());
        return true;
    }
    OptionPropertyString name{"name", "Name", {}, 0};

    OptionProperty<vtkDataObject::FieldAssociations> fieldAssociation{
        "fieldAssociation",
        "Field Association",
        {{"points", "Points", vtkDataObject::FIELD_ASSOCIATION_POINTS},
         {"cells", "Cells", vtkDataObject::FIELD_ASSOCIATION_CELLS},
         {"none", "None", vtkDataObject::FIELD_ASSOCIATION_NONE},
         {"pointsThenCells", "Points then Cells",
          vtkDataObject::FIELD_ASSOCIATION_POINTS_THEN_CELLS}},
        0};

    CompositeProperty property{[&]() {
        CompositeProperty tmp{"ScalarAttribute", "Scalar Attribute",
                              R"(Select the scalar attribute to consider.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Domain";
};

struct Wrapper1 {
    bool set(ttkContourAroundPoint& filter) {
        filter.Setui_extension(property.get());
        return true;
    }
    DoubleProperty property{
        "ui_extension",
        "Contour extension",
        R"(Extension of the contour. 0 --> only the point, 100 --> touching the nearest contour.)"_help,
        67.0,
        std::pair{0.0, ConstraintBehavior::Ignore},
        std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(ttkContourAroundPoint& filter) {
        filter.Setui_sizeFilter(property.get());
        return true;
    }
    DoubleProperty property{
        "ui_sizeFilter",
        "Size filter",
        R"(Filter out small regions. 0 --> unfiltered, 10000 --> empty output.)"_help,
        7.0,
        std::pair{0.0, ConstraintBehavior::Ignore},
        std::pair{10000.0, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(ttkContourAroundPoint& filter) {
        filter.Setui_spherical(property.get());
        return true;
    }
    BoolProperty property{
        "ui_spherical", "Domain is a sphere",
        R"(Select this if both the scalar field and the points lie on a sphere and the output
shall lie on the same sphere.)"_help,
        false};
};

struct Wrapper4 {
    bool set(ttkContourAroundPoint& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper5 {
    bool set(ttkContourAroundPoint& filter) {
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

struct Wrapper6 {
    bool set(ttkContourAroundPoint& filter) {
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

struct Wrapper7 {
    bool set(ttkContourAroundPoint& filter) {
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

struct Wrapper8 {
    bool set(ttkContourAroundPoint& filter) {
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
struct TTKTraits<ttkContourAroundPoint> {
    static constexpr std::string_view className = "ttkContourAroundPoint";
    static constexpr std::string_view identifier = "ttkContourAroundPoint";
    static constexpr std::string_view displayName = "TTK ContourAroundPoint";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 3> inports = {
        InputData{"Domain", "vtkDataSet", 1, R"(Domain in which the contours are computed.)"},
        InputData{
            "Nodes", "vtkUnstructuredGrid", 1,
            R"(Points (like "Skeleton Nodes" from "TTKMergeandContourTreeFTM") with the following data
arrays: CriticalType: int, Scalar: float.)"},
        InputData{
            "Arcs", "vtkUnstructuredGrid", 1,
            R"(Cells (like "Skeleton Arcs" from "TTKMergeandContourTreeFTM") with the following data
arrays: downNodeId: int, upNodeId: int.)"}};
    inline static std::array<OutputData, 2> outports = {OutputData{"port0", "Contours", 0},
                                                        OutputData{"port1", "Centroids", 1}};
    inline static std::array<Group, 2> groups = {
        Group{"Input Parameters",
              {"ScalarAttribute", "ui_extension", "ui_sizeFilter", "ui_spherical"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8>
        properties;
    static constexpr std::string_view doc = R"(TTK contourAroundPoint plugin.)";
};

void registerttkContourAroundPoint(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkContourAroundPoint>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
