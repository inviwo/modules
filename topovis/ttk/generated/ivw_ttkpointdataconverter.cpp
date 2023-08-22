#include "ivw_ttkpointdataconverter.h"

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
#include <ttkPointDataConverter.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkPointDataConverter& filter) {
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
        CompositeProperty tmp{"PointDataScalarField", "Point Data Scalar Field",
                              R"(Select the point data scalar field to process.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(ttkPointDataConverter& filter) {
        filter.SetOutputType(property.get());
        return true;
    }
    OptionPropertyInt property{"OutputType",
                               "Output Type",
                               R"(Data output type.)"_help,
                               {{"Char", "Char", 0},
                                {"Double", "Double", 1},
                                {"Float", "Float", 2},
                                {"Int", "Int", 3},
                                {"IdType", "IdType", 4},
                                {"Short", "Short", 5},
                                {"Unsigned Short", "Unsigned Short", 6},
                                {"Unsigned Char", "Unsigned Char", 7}},
                               0};
};

struct Wrapper2 {
    bool set(ttkPointDataConverter& filter) {
        filter.SetUseNormalization(property.get());
        return true;
    }
    BoolProperty property{"NormalizeToRange", "Normalize to data type range",
                          R"(Normalizes the values to the data type range extent (useful for light
data types such as char or unsigned chars).)"_help,
                          false};
};

struct Wrapper3 {
    bool set(ttkPointDataConverter& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper4 {
    bool set(ttkPointDataConverter& filter) {
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
    bool set(ttkPointDataConverter& filter) {
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
    bool set(ttkPointDataConverter& filter) {
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
    bool set(ttkPointDataConverter& filter) {
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
struct TTKTraits<ttkPointDataConverter> {
    static constexpr std::string_view className = "ttkPointDataConverter";
    static constexpr std::string_view identifier = "ttkPointDataConverter";
    static constexpr std::string_view displayName = "TTK PointDataConverter";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1, R"(Data-set to process.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}},
        Group{"Input options", {"Point Data Scalar Field"}},
        Group{"Output options", {"Output Type", "NormalizeToRange"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7>
        properties;
    static constexpr std::string_view doc = R"(TTK plugin that converts data types for point-based
scalar fields (for instance, from double to float).)";
};

void registerttkPointDataConverter(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkPointDataConverter>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
