#include "ivw_ttkgeometrysmoother.h"

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
#include <ttkGeometrySmoother.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkGeometrySmoother& filter) {
        filter.SetNumberOfIterations(property.get());
        return true;
    }
    IntProperty property{"NumberOfIterations",
                         "Iteration Number",
                         R"(Number of iterations for the smoothing filter.)"_help,
                         1,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(ttkGeometrySmoother& filter) {
        filter.SetUseMaskScalarField(property.get());
        return true;
    }
    BoolProperty property{"UseMaskScalarField", "Use a Scalar Field as Mask",
                          R"(Check this box if an input scalar field should be considered as
vertex mask (used to mark vertices to smooth).)"_help,
                          true};
};

struct Wrapper2 {
    bool set(ttkGeometrySmoother& filter) {
        filter.SetForceInputMaskScalarField(property.get());
        return true;
    }
    BoolProperty property{"ForceInputMaskScalarField", "Force Input Mask Scalar Field",
                          R"(Check this box to choose a non-default scalar field as mask
(default mask scalar field is named ttk::MaskScalarFieldName).)"_help,
                          false};
};

struct Wrapper3 : FieldSelection {
    bool set(ttkGeometrySmoother& filter) {
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
        CompositeProperty tmp{"InputMaskNew", "Input Mask Field",
                              R"(Input mask field (used to mark vertices to smooth).)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper4 {
    bool set(ttkGeometrySmoother& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper5 {
    bool set(ttkGeometrySmoother& filter) {
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
    bool set(ttkGeometrySmoother& filter) {
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
    bool set(ttkGeometrySmoother& filter) {
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
    bool set(ttkGeometrySmoother& filter) {
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
struct TTKTraits<ttkGeometrySmoother> {
    static constexpr std::string_view className = "ttkGeometrySmoother";
    static constexpr std::string_view identifier = "ttkGeometrySmoother";
    static constexpr std::string_view displayName = "TTK GeometrySmoother";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkPointSet", -1, R"(Point-set to smooth.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Input options",
              {"Input", "NumberOfIterations", "UseMaskScalarField", "ForceInputMaskScalarField",
               "InputMaskNew"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8>
        properties;
    static constexpr std::string_view doc =
        R"(This filter is a dummy example for the development of TTK packages. It
smooths an input mesh by average the vertex locations on the link of each
vertex.

Online examples:

- https://topology-tool-kit.github.io/examples/1manifoldLearning/

- https://topology-tool-kit.github.io/examples/2manifoldLearning/

- https://topology-tool-kit.github.io/examples/dragon/

- https://topology-tool-kit.github.io/examples/harmonicSkeleton/

- https://topology-tool-kit.github.io/examples/interactionSites/

- https://topology-tool-kit.github.io/examples/mergeTreePGA/

- https://topology-tool-kit.github.io/examples/morseMolecule/)";
};

void registerttkGeometrySmoother(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkGeometrySmoother>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
