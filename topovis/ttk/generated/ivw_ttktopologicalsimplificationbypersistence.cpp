#include "ivw_ttktopologicalsimplificationbypersistence.h"
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
#include <ttkTopologicalSimplificationByPersistence.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkTopologicalSimplificationByPersistence& filter) {
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
        CompositeProperty tmp{"InputArray", "Input Array",
                              R"(The scalar array that will be simplified.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(ttkTopologicalSimplificationByPersistence& filter) {
        filter.SetPairType(property.get());
        return true;
    }
    OptionPropertyInt property{
        "PairType",
        "PairType",
        R"(Determines which type of persistence pairs will be removed.)"_help,
        {{"Extremum-Saddle", "Extremum-Saddle", 0},
         {"Minimum-Saddle", "Minimum-Saddle", 1},
         {"Maximum-Saddle", "Maximum-Saddle", 2}},
        0};
};

struct Wrapper2 {
    bool set(ttkTopologicalSimplificationByPersistence& filter) {
        filter.SetPersistenceThreshold(property.get());
        return true;
    }
    DoubleProperty property{
        "PersistenceThreshold",
        "PersistenceThreshold",
        R"(The persistence threshold all persistence pairs of the output scalar field must strictly exceed (by default relative to the function range, i.e. between 0 and 1). Note, a persistence threshold of 0 will remove all flat plateau artifacts.)"_help,
        0.0,
        std::pair{0.0, ConstraintBehavior::Ignore},
        std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(ttkTopologicalSimplificationByPersistence& filter) {
        filter.SetThresholdIsAbsolute(property.get());
        return true;
    }
    BoolProperty property{
        "ThresholdIsAbsolute", "ThresholdIsAbsolute",
        R"(This parameter determines if the persistence threshold is an absolute scalar value, or a fraction (0-1) of the function range.)"_help,
        false};
};

struct Wrapper4 {
    bool set(ttkTopologicalSimplificationByPersistence& filter) {
        filter.SetComputePerturbation(property.get());
        return true;
    }
    BoolProperty property{
        "NumericalPerturbation", "NumericalPerturbation",
        R"(Numerically perturb the output (to avoid the usage of an order array for flat plateau disambiguation).)"_help,
        false};
};

struct Wrapper5 {
    bool set(ttkTopologicalSimplificationByPersistence& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper6 {
    bool set(ttkTopologicalSimplificationByPersistence& filter) {
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

struct Wrapper7 {
    bool set(ttkTopologicalSimplificationByPersistence& filter) {
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

struct Wrapper8 {
    bool set(ttkTopologicalSimplificationByPersistence& filter) {
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

struct Wrapper9 {
    bool set(ttkTopologicalSimplificationByPersistence& filter) {
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
struct VTKTraits<ttkTopologicalSimplificationByPersistence> {
    static constexpr std::string_view uri = "ttk.ttkTopologicalSimplificationByPersistence";
    static constexpr std::string_view className = "ttkTopologicalSimplificationByPersistence";
    static constexpr std::string_view identifier = "ttkTopologicalSimplificationByPersistence";
    static constexpr std::string_view displayName = "TTK TopologicalSimplificationByPersistence";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {InputData{
        .identifier = "Input",
        .dataType = "vtkDataSet",
        .numComp = 1,
        .doc =
            R"(A vtkDataSet that has at least one point data scalar array that will be simplified.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Input Options", {"InputArray"}},
        Group{"Output Options",
              {"PairType", "PersistenceThreshold", "ThresholdIsAbsolute", "NumericalPerturbation"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"ivw(Given an input scalar field and a persistence threshold (either as an absolute value or a fraction of the scalar range), this filter modifies the scalar field such that it no longer exhibits persistence pairs below the given threshold. All other pairs are unaffected. To this end the filter uses the persistence-sensitive specialization of localized topological simplification (PLTS). Note that this filter will also compute an unambiguous global vertex order that can be used in subsequent topological data analysis.

Related publications:
"Generalized Topological Simplification of Scalar Fields on Surfaces"
Julien Tierny, Valerio Pascucci
Proc. of IEEE VIS 2012.
IEEE Transactions on Visualization and Computer Graphics, 2012.

"Localized Topological Simplification of Scalar Data"
Jonas Lukasczyk, Christoph Garth, Ross Maciejewski, Julien Tierny
Proc. of IEEE VIS 2020.
IEEE Transactions on Visualization and Computer Graphics

Online examples:

- https://topology-tool-kit.github.io/examples/cosmicWeb/

- https://topology-tool-kit.github.io/examples/contourTreeAlignment/)ivw";
};

void registerttkTopologicalSimplificationByPersistence(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkTopologicalSimplificationByPersistence>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
