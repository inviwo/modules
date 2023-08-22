#include "ivw_ttkconnectedcomponents.h"

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
#include <ttkConnectedComponents.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkConnectedComponents& filter) {
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
        CompositeProperty tmp{
            "FeatureMask", "FeatureMask",
            R"(Feature Mask where values smaller equal than zero correspond to the background.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Segmentation";
};

struct Wrapper1 {
    bool set(ttkConnectedComponents& filter) {
        filter.SetBackgroundThreshold(property.get());
        return true;
    }
    DoubleProperty property{
        "BackgroundThreshold",
        "BackgroundThreshold",
        R"(Values of the mask smaller or equal to this threshold are considered background.)"_help,
        0.0,
        std::pair{0.0, ConstraintBehavior::Ignore},
        std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(ttkConnectedComponents& filter) {
        filter.SetAugmentSegmentationWithComponentSize(property.get());
        return true;
    }
    BoolProperty property{
        "SegmentationSize", "SegmentationSize",
        R"(If yes the segmentation will have an additional point data array that records for each vertex the size of its corresponding component.)"_help,
        false};
};

struct Wrapper3 {
    bool set(ttkConnectedComponents& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper4 {
    bool set(ttkConnectedComponents& filter) {
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

struct Wrapper5 {
    bool set(ttkConnectedComponents& filter) {
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

struct Wrapper6 {
    bool set(ttkConnectedComponents& filter) {
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

struct Wrapper7 {
    bool set(ttkConnectedComponents& filter) {
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
struct TTKTraits<ttkConnectedComponents> {
    static constexpr std::string_view className = "ttkConnectedComponents";
    static constexpr std::string_view identifier = "ConnectedComponents";
    static constexpr std::string_view displayName = "TTK ConnectedComponents";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Segmentation", "vtkDataSet", 1, R"(A vtkDataSet.)"}};
    inline static std::array<OutputData, 2> outports = {OutputData{"port0", "Segmentation", 0},
                                                        OutputData{"port1", "Components", 1}};
    inline static std::array<Group, 3> groups = {
        Group{"Input options", {"FeatureMask", "BackgroundThreshold"}},
        Group{"Output options", {"SegmentationSize"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7>
        properties;
    static constexpr std::string_view doc =
        R"(This filter consumes a scalar field with a feature mask and computes for each edge connected group of vertices with a non-background mask value a so-called connected component via flood-filling, where the background is masked with values smaller-equal zero. The computed components store the size, seed, and center of mass of each component. The flag UseSeedIdAsComponentId controls if the resulting segmentation is either labeled by the index of the component, or by its seed location (which can be used as a deterministic component label).)";
};

void registerttkConnectedComponents(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkConnectedComponents>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
