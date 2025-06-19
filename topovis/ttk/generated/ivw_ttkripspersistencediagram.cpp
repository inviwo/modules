#include "ivw_ttkripspersistencediagram.h"
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
#include <ttkRipsPersistenceDiagram.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkRipsPersistenceDiagram& filter) {
        filter.SetSelectFieldsWithRegexp(property.get());
        return true;
    }
    BoolProperty property{"SelectFieldsWithRegexp", "Select Fields with a Regexp",
                          R"(Select input scalar fields matching a regular expression.)"_help,
                          false};
};

struct Wrapper1 {
    bool set(ttkRipsPersistenceDiagram& filter) {
        filter.SetRegexpString(property.get().c_str());
        return true;
    }
    StringProperty property{"Regexp", "Regexp",
                            R"(This regexp will be used to filter the chosen fields. Only
matching ones will be selected.)"_help,
                            ".*"};
};

struct Wrapper2 {
    bool set(ttkRipsPersistenceDiagram& filter) {
        filter.SetInputIsDistanceMatrix(property.get());
        return true;
    }
    BoolProperty property{
        "InputIsDistanceMatrix", "Input is a distance matrix",
        R"(Check this box to read the input table as a distance matrix instead of a point cloud.)"_help,
        false};
};

struct Wrapper3 {
    bool set(ttkRipsPersistenceDiagram& filter) {
        filter.SetBackEnd(property.get());
        return true;
    }
    OptionPropertyInt property{
        "BackEnd",
        "Backend",
        R"(Backend for the computation of the Rips persistence diagram.)"_help,
        {{"Ripser (JACT 2021)", "Ripser (JACT 2021)", 0},
         {"Geometric (only in 2D)", "Geometric (only in 2D)", 1}},
        0};
};

struct Wrapper4 {
    bool set(ttkRipsPersistenceDiagram& filter) {
        filter.SetSimplexMaximumDimension(property.get());
        return true;
    }
    IntProperty property{"SimplexMaximumDimension",
                         "Simplex maximum dimension",
                         R"(Simplex maximum dimension in the Rips complex.)"_help,
                         1,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkRipsPersistenceDiagram& filter) {
        filter.SetSimplexMaximumDiameter(property.get().c_str());
        return true;
    }
    StringProperty property{
        "SimplexMaximumDiameter", "Simplex maximum diameter",
        R"(Simplex maximum diameter in the Rips complex (type "inf" for unlimited diameter).)"_help,
        "inf"};
};

struct Wrapper6 {
    bool set(ttkRipsPersistenceDiagram& filter) {
        filter.SetFieldOfCoefficients(property.get());
        return true;
    }
    IntProperty property{
        "FieldOfCoefficients",
        "Field of coefficients",
        R"(Prime integer p, by default 2. The homology will be computed with coefficients in the field Z/pZ.)"_help,
        2,
        std::pair{0, ConstraintBehavior::Ignore},
        std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkRipsPersistenceDiagram& filter) {
        filter.SetDelaunayRips(property.get());
        return true;
    }
    BoolProperty property{
        "DelaunayRips", "Compute Delaunay-Rips persistence diagram",
        R"(Check this box to compute the persistence diagram of the Delaunay-Rips complex (faster) instead of the Rips complex (slower).)"_help,
        false};
};

struct Wrapper8 {
    bool set(ttkRipsPersistenceDiagram& filter) {
        filter.SetOutputGenerators(property.get());
        return true;
    }
    BoolProperty property{
        "OutputGenerators", "Output 1-generators",
        R"(Check this box to output generators of the 1-dimensional homology (only in 2D).)"_help,
        false};
};

struct Wrapper9 {
    bool set(ttkRipsPersistenceDiagram& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper10 {
    bool set(ttkRipsPersistenceDiagram& filter) {
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

struct Wrapper11 {
    bool set(ttkRipsPersistenceDiagram& filter) {
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

struct Wrapper12 {
    bool set(ttkRipsPersistenceDiagram& filter) {
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

struct Wrapper13 {
    bool set(ttkRipsPersistenceDiagram& filter) {
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
struct VTKTraits<ttkRipsPersistenceDiagram> {
    static constexpr std::string_view uri = "ttk.ttkRipsPersistenceDiagram";
    static constexpr std::string_view className = "ttkRipsPersistenceDiagram";
    static constexpr std::string_view identifier = "ttkRipsPersistenceDiagram";
    static constexpr std::string_view displayName = "TTK RipsPersistenceDiagram";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkTable", -1, R"(Point cloud to process.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Input options",
              {"InputIsDistanceMatrix", "SelectFieldsWithRegexp", "ScalarFields", "Regexp"}},
        Group{"Output options",
              {"BackEnd", "SimplexMaximumDimension", "SimplexMaximumDiameter",
               "FieldOfCoefficients", "DelaunayRips", "OutputGenerators"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11, Wrapper12, Wrapper13>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"ivw(TTK plugin for the computation of the persistence diagram of Rips complexes.

This plugin computes the persistence diagram of the Rips complex of the
input point cloud or distance matrix. The X-coordinate of each pair
corresponds to its birth, while its smallest and highest Y-coordinates
correspond to its birth and death respectively.

In practice, the diagram is represented by a vtkUnstructuredGrid. Each
vertex of this mesh represent a critical point of the input data. It is
associated with point data (vertexId, critical type). Each vertical edge
of this mesh represent a persistence pair. It is associated with cell data
(persistence of the pair, critical index of the extremum of the pair).
The diagonal of the diagram can be filtered out by considering its
PairIdentifier value (set at -1).)ivw";
};

void registerttkRipsPersistenceDiagram(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkRipsPersistenceDiagram>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
