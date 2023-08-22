#include "ivw_ttkdistancefield.h"

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
#include <ttkDistanceField.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkDistanceField& filter) {
        filter.SetForceInputVertexScalarField(property.get());
        return true;
    }
    BoolProperty property{"ForceInputVertexScalarField", "Force Input Vertex ScalarField",
                          R"()"_help, false};
};

struct Wrapper1 : FieldSelection {
    bool set(ttkDistanceField& filter) {
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
        CompositeProperty tmp{"InputVertexScalarFieldNameNew", "Vertex Identifier Field",
                              R"(Select the vertex identifier scalar field in the sources.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Sources";
};

struct Wrapper2 {
    bool set(ttkDistanceField& filter) {
        filter.SetOutputScalarFieldType(property.get());
        return true;
    }
    OptionPropertyInt property{"OutputScalarFieldType",
                               "Output field type",
                               R"(Select the type of the output scalar field.)"_help,
                               {{"Float", "Float", 0}, {"Double", "Double", 1}},
                               0};
};

struct Wrapper3 {
    bool set(ttkDistanceField& filter) {
        filter.SetOutputScalarFieldName(property.get().c_str());
        return true;
    }
    StringProperty property{"OutputScalarFieldName", "Output field name",
                            R"(Select the name of the output scalar field.)"_help,
                            "OutputDistanceField"};
};

struct Wrapper4 {
    bool set(ttkDistanceField& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper5 {
    bool set(ttkDistanceField& filter) {
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
    bool set(ttkDistanceField& filter) {
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
    bool set(ttkDistanceField& filter) {
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
    bool set(ttkDistanceField& filter) {
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
struct TTKTraits<ttkDistanceField> {
    static constexpr std::string_view className = "ttkDistanceField";
    static constexpr std::string_view identifier = "ttkDistanceField";
    static constexpr std::string_view displayName = "TTK DistanceField";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 2> inports = {
        InputData{"Domain", "vtkDataSet", 1, R"(Data-set to process.)"},
        InputData{"Sources", "vtkPointSet", 1, R"(Sources for the distance fields.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Input options",
              {"Sources", "ForceInputVertexScalarField", "InputVertexScalarFieldNameNew"}},
        Group{"Output options", {"OutputScalarFieldType", "OutputScalarFieldName"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8>
        properties;
    static constexpr std::string_view doc =
        R"(This plugin takes a list of sources (a set of points with their global
identifiers attached to them) and produces a distance field to the closest
source.

Related publication: "A note on two problems in connexion with graphs",
Edsger W. Dijkstra, Numerische Mathematik, 1959.)";
};

void registerttkDistanceField(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkDistanceField>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
