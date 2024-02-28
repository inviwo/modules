#include "ivw_ttkpathcompression.h"
#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/vtk/processors/vtkgenericprocessor.h>
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
#include <ttkPathCompression.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkPathCompression& filter) {
        if (name.size() == 0) return false;
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
        3};

    CompositeProperty property{[&]() {
        CompositeProperty tmp{"ScalarField", "Scalar Field",
                              R"(Select the scalar field to process.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(ttkPathCompression& filter) {
        filter.SetForceInputOffsetScalarField(property.get());
        return true;
    }
    BoolProperty property{"ForceInputOffsetScalarField", "Force Input Offset Field",
                          R"(Check this box to force the usage of a specific input scalar field
as vertex offset (used to disambiguate flat plateaus).)"_help,
                          false};
};

struct Wrapper2 : FieldSelection {
    bool set(ttkPathCompression& filter) {
        if (name.size() == 0) return false;
        filter.SetInputArrayToProcess(1, 0, 0, fieldAssociation.get(), name.get().c_str());
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
        3};

    CompositeProperty property{[&]() {
        CompositeProperty tmp{
            "OffsetField", "Offset Field",
            R"(Select the input offset field (to disambiguate flat plateaus).)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper3 {
    bool set(ttkPathCompression& filter) {
        filter.SetComputeAscendingSegmentation(property.get());
        return true;
    }
    BoolProperty property{
        "ComputeAscendingSegmentation", "Ascending Segmentation",
        R"(Toggles the creation of the ascending segmentation, where each vertex points towards its minimum.)"_help,
        true};
};

struct Wrapper4 {
    bool set(ttkPathCompression& filter) {
        filter.SetComputeDescendingSegmentation(property.get());
        return true;
    }
    BoolProperty property{
        "ComputeDescendingSegmentation", "Descending Segmentation",
        R"(Toggles the creation of the descending segmentation, where each vertex points towards its maximum.)"_help,
        true};
};

struct Wrapper5 {
    bool set(ttkPathCompression& filter) {
        filter.SetComputeMSSegmentationHash(property.get());
        return true;
    }
    BoolProperty property{
        "ComputeFinalSegmentation", "Morse-Smale Complex Hash",
        R"(Toggles the creation of a hash from the Ascending and Descending labels that can still exhibit hash collisions.)"_help,
        true};
};

struct Wrapper6 {
    bool set(ttkPathCompression& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper7 {
    bool set(ttkPathCompression& filter) {
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

struct Wrapper8 {
    bool set(ttkPathCompression& filter) {
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

struct Wrapper9 {
    bool set(ttkPathCompression& filter) {
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

struct Wrapper10 {
    bool set(ttkPathCompression& filter) {
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
struct VTKTraits<ttkPathCompression> {
    static constexpr std::string_view uri = "ttk.ttkPathCompression";
    static constexpr std::string_view className = "ttkPathCompression";
    static constexpr std::string_view identifier = "ttkPathCompression";
    static constexpr std::string_view displayName = "TTK PathCompression";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", 1, R"(Data-set to process.
TTK assumes that the input dataset is made of only one connected component.
If it's not the case, you can use the filter "Connectivity" (and select "Extract Largest Region").)"}};
    inline static std::array<OutputData, 1> outports = {OutputData{"port0", "Segmentation", 0}};
    inline static std::array<Group, 3> groups = {
        Group{"Input options", {"Scalar Field", "ForceInputOffsetScalarField", "Offset Field"}},
        Group{"Output options",
              {"ComputeAscendingSegmentation", "ComputeDescendingSegmentation",
               "ComputeFinalSegmentation"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10>
        properties;
    static constexpr std::string_view doc =
        R"(TTK plugin for the computation of Morse-Smale segmentation. It allows to extract the ascending, descending, and Morse-Smale segmentation hash as point data arrays.
Each array represents the minimum/maximum/minimum-maximum combination a vertex is reaching when following the gradient direction. By using path compression, the computational cost was minimized.

Related publication:
"Parallel Computation of Piecewise Linear Morse-Smale Segmentations"
Robin G. C. Maack, Jonas Lukasczyk, Julien Tierny, Hans Hagen, Ross Maciejewski, Christoph Garth
IEEE Transactions on Visualization and Computer Graphics, 2023

Online examples:

- https://topology-tool-kit.github.io/examples/morseSmaleSegmentation_at/)";
};

void registerttkPathCompression(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkPathCompression>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
