#include "ivw_ttkmorsesmalecomplex.h"
#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/vtk/processors/vtkgenericprocessor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/buttonproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/fileproperty.h>
#include <inviwo/ttk/util/ttkprocessorutils.h>

#include <tuple>
#include <array>
#include <string_view>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkDataObject.h>
#include <ttkMorseSmaleComplex.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkMorseSmaleComplex& filter) {
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
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetForceInputOffsetScalarField(property.get());
        return true;
    }
    BoolProperty property{"ForceInputOffsetScalarField", "Force Input Offset Field",
                          R"(Check this box to force the usage of a specific input scalar field
as vertex offset (used to disambiguate flat plateaus).)"_help,
                          false};
};

struct Wrapper2 : FieldSelection {
    bool set(ttkMorseSmaleComplex& filter) {
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
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetComputeCriticalPoints(property.get());
        return true;
    }
    BoolProperty property{"ComputeCriticalPoints", "Critical Points", R"(Documentation.)"_help,
                          true};
};

struct Wrapper4 {
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetComputeAscendingSeparatrices1(property.get());
        return true;
    }
    BoolProperty property{"ComputeAscendingSeparatrices1", "Ascending 1-Separatrices",
                          R"(Documentation.)"_help, true};
};

struct Wrapper5 {
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetComputeDescendingSeparatrices1(property.get());
        return true;
    }
    BoolProperty property{"ComputeDescendingSeparatrices1", "Descending 1-Separatrices",
                          R"(Documentation.)"_help, true};
};

struct Wrapper6 {
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetComputeSaddleConnectors(property.get());
        return true;
    }
    BoolProperty property{"ComputeSaddleConnectors", "Saddle Connectors", R"(Documentation.)"_help,
                          true};
};

struct Wrapper7 {
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetComputeAscendingSeparatrices2(property.get());
        return true;
    }
    BoolProperty property{"ComputeAscendingSeparatrices2", "Ascending 2-Separatrices",
                          R"(Documentation.)"_help, false};
};

struct Wrapper8 {
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetComputeDescendingSeparatrices2(property.get());
        return true;
    }
    BoolProperty property{"ComputeDescendingSeparatrices2", "Descending 2-Separatrices",
                          R"(Documentation.)"_help, false};
};

struct Wrapper9 {
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetComputeAscendingSegmentation(property.get());
        return true;
    }
    BoolProperty property{"ComputeAscendingSegmentation", "Ascending Segmentation",
                          R"(Documentation.)"_help, true};
};

struct Wrapper10 {
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetComputeDescendingSegmentation(property.get());
        return true;
    }
    BoolProperty property{"ComputeDescendingSegmentation", "Descending Segmentation",
                          R"(Documentation.)"_help, true};
};

struct Wrapper11 {
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetComputeFinalSegmentation(property.get());
        return true;
    }
    BoolProperty property{"ComputeFinalSegmentation", "Morse-Smale Complex Segmentation",
                          R"(Documentation.)"_help, true};
};

struct Wrapper12 {
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetReturnSaddleConnectors(property.get());
        return true;
    }
    BoolProperty property{"ReturnSaddleConnectors", "Return Saddle Connectors",
                          R"(Return the saddle connectors.)"_help, false};
};

struct Wrapper13 {
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetSaddleConnectorsPersistenceThreshold(property.get());
        return true;
    }
    DoubleProperty property{
        "SaddleConnectorsPersistenceThreshold",
        "Saddle Connectors Persistence Threshold",
        R"(Saddle Connectors Persistence Threshold (by default relative to the function range, i.e. between 0 and 1).)"_help,
        0.0,
        std::pair{0.0, ConstraintBehavior::Ignore},
        std::pair{100000.0, ConstraintBehavior::Ignore}};
};

struct Wrapper14 {
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetThresholdIsAbsolute(property.get());
        return true;
    }
    BoolProperty property{"ThresholdIsAbsolute", "ThresholdIsAbsolute",
                          R"(This parameter determines if the saddle connectors
persistence threshold is an absolute scalar value, or a
fraction (0-1) of the function range.)"_help,
                          false};
};

struct Wrapper15 {
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetForceLoopFreeGradient(property.get());
        return true;
    }
    BoolProperty property{
        "ForceLoopFreeGradient", "Force loop-free gradient",
        R"(If set to true, the saddle connectors that would create a loop in the whole gradient after returning them will not be processed.
Otherwise, only the saddle connectors that will create a loop in the wall of the corresponding 2-saddle will not be processed.)"_help,
        true};
};

struct Wrapper16 {
    bool set(ttkMorseSmaleComplex& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper17 {
    bool set(ttkMorseSmaleComplex& filter) {
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

struct Wrapper18 {
    bool set(ttkMorseSmaleComplex& filter) {
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

struct Wrapper19 {
    bool set(ttkMorseSmaleComplex& filter) {
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

struct Wrapper20 {
    bool set(ttkMorseSmaleComplex& filter) {
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
struct VTKTraits<ttkMorseSmaleComplex> {
    static constexpr std::string_view uri = "ttk.ttkMorseSmaleComplex";
    static constexpr std::string_view className = "ttkMorseSmaleComplex";
    static constexpr std::string_view identifier = "ttkMorseSmaleComplex";
    static constexpr std::string_view displayName = "TTK MorseSmaleComplex";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", 1, R"(Data-set to process.
TTK assumes that the input dataset is made of only one connected component.
If it's not the case, you can use the filter "Connectivity" (and select "Extract Largest Region").)"}};
    inline static std::array<OutputData, 4> outports = {
        OutputData{"port0", "Critical Points", 0}, OutputData{"port1", "1-Separatrices", 1},
        OutputData{"port2", "2-Separatrices", 2}, OutputData{"port3", "Segmentation", 3}};
    inline static std::array<Group, 3> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Input options", {"Scalar Field", "ForceInputOffsetScalarField", "Offset Field"}},
        Group{"Output options",
              {"ComputeCriticalPoints", "ComputeAscendingSeparatrices1",
               "ComputeDescendingSeparatrices1", "ComputeSaddleConnectors",
               "ComputeAscendingSeparatrices2", "ComputeDescendingSeparatrices2",
               "ComputeAscendingSegmentation", "ComputeDescendingSegmentation",
               "ComputeFinalSegmentation", "ReturnSaddleConnectors",
               "SaddleConnectorsPersistenceThreshold", "ThresholdIsAbsolute",
               "ForceLoopFreeGradient"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11, Wrapper12, Wrapper13, Wrapper14, Wrapper15,
               Wrapper16, Wrapper17, Wrapper18, Wrapper19, Wrapper20>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"ivw(TTK plugin for the computation of Morse-Smale complexes.

Morse-Smale complexes are useful topological abstractions of scalar
fields for data segmentation, feature extraction, etc.

Related publication:
"Parallel Computation of 3D Morse-Smale Complexes"
Nithin Shivashankar, Vijay Natarajan
Proc. of EuroVis 2012.
Computer Graphics Forum, 2012.

Online examples:

- https://topology-tool-kit.github.io/examples/1manifoldLearning/

- https://topology-tool-kit.github.io/examples/1manifoldLearningCircles/

- https://topology-tool-kit.github.io/examples/2manifoldLearning/

- https://topology-tool-kit.github.io/examples/cosmicWeb/

- https://topology-tool-kit.github.io/examples/imageProcessing/

- https://topology-tool-kit.github.io/examples/karhunenLoveDigits64Dimensions/

- https://topology-tool-kit.github.io/examples/morseMolecule/

- https://topology-tool-kit.github.io/examples/morsePersistence/

- https://topology-tool-kit.github.io/examples/morseSmaleQuadrangulation/

- https://topology-tool-kit.github.io/examples/persistenceClustering0/

- https://topology-tool-kit.github.io/examples/persistenceClustering1/

- https://topology-tool-kit.github.io/examples/persistenceClustering2/

- https://topology-tool-kit.github.io/examples/persistenceClustering3/

- https://topology-tool-kit.github.io/examples/persistenceClustering4/

- https://topology-tool-kit.github.io/examples/persistentGenerators_at/

- https://topology-tool-kit.github.io/examples/persistentGenerators_darkSky/

- https://topology-tool-kit.github.io/examples/tectonicPuzzle/

- https://topology-tool-kit.github.io/examples/topologicalOptimization_darkSky/

- https://topology-tool-kit.github.io/examples/tribute/)ivw";
};

void registerttkMorseSmaleComplex(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkMorseSmaleComplex>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
