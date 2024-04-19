#include "ivw_ttktopologicalcompression.h"
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
#include <ttkTopologicalCompression.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkTopologicalCompression& filter) {
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
    bool set(ttkTopologicalCompression& filter) {
        filter.SetCompressionType(property.get());
        return true;
    }
    OptionPropertyInt property{
        "CompressionType",
        "Compression type",
        R"(Compression Type.)"_help,
        {{"Driven by persistence diagram", "Driven by persistence diagram", 0}},
        0};
};

struct Wrapper2 {
    bool set(ttkTopologicalCompression& filter) {
        filter.SetTolerance(property.get());
        return true;
    }
    DoubleProperty property{"Tolerance",
                            "Topological loss (persistence percentage)",
                            R"(Topological loss expressed as a persistence threshold (normalized
between 0 and 100).)"_help,
                            10.0,
                            std::pair{1.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(ttkTopologicalCompression& filter) {
        filter.SetSubdivide(property.get());
        return true;
    }
    BoolProperty property{"Subdivide", "Enable maximum pointwise error control",
                          R"(Enable maximum pointwise error control.)"_help, false};
};

struct Wrapper4 {
    bool set(ttkTopologicalCompression& filter) {
        filter.SetMaximumError(property.get());
        return true;
    }
    DoubleProperty property{"MaximumError",
                            "Maximum pointwise error (percentage)",
                            R"(Set the maximum allowed pointwise error (in percentage of the
function span).)"_help,
                            10.0,
                            std::pair{1.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkTopologicalCompression& filter) {
        filter.SetUseTopologicalSimplification(property.get());
        return true;
    }
    BoolProperty property{"UseTopologicalSimplification", "Simplify at compression (slower)",
                          R"(Enable topological simplification at compression time (higher
compression factors, slower compression time).)"_help,
                          true};
};

struct Wrapper6 {
    bool set(ttkTopologicalCompression& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper7 {
    bool set(ttkTopologicalCompression& filter) {
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
    bool set(ttkTopologicalCompression& filter) {
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
    bool set(ttkTopologicalCompression& filter) {
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
    bool set(ttkTopologicalCompression& filter) {
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
struct VTKTraits<ttkTopologicalCompression> {
    static constexpr std::string_view uri = "ttk.ttkTopologicalCompression";
    static constexpr std::string_view className = "ttkTopologicalCompression";
    static constexpr std::string_view identifier = "ttkTopologicalCompression";
    static constexpr std::string_view displayName = "TTK TopologicalCompression";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkImageData", 1, R"(Data-set to process.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Input", {"Scalar Field"}},
        Group{"Topology aware compression",
              {"CompressionType", "Tolerance", "Subdivide", "MaximumError",
               "UseTopologicalSimplification", "SQMethod"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc = R"(TTK topologicalCompression plugin documentation.

Online examples:

- https://topology-tool-kit.github.io/examples/persistenceDrivenCompression/)";
};

void registerttkTopologicalCompression(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkTopologicalCompression>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
