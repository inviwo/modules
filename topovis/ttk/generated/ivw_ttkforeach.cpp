#include "ivw_ttkforeach.h"
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
#include <ttkForEach.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkForEach& filter) {
        filter.SetExtractionMode(property.get());
        return true;
    }
    OptionPropertyInt property{"IterationMode",
                               "IterationMode",
                               R"(Iteration Mode.)"_help,
                               {{"Automatic", "Automatic", -1},
                                {"Block", "Block", 0},
                                {"Rows", "Rows", 1},
                                {"Array Values", "Array Values", 3},
                                {"Arrays", "Arrays", 4},
                                {"Block Tuples", "Block Tuples", 5}},
                               0};
};

struct Wrapper1 : FieldSelection {
    bool set(ttkForEach& filter) {
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
                              R"(The field data array to iterate over.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper2 {
    bool set(ttkForEach& filter) {
        filter.SetArrayAttributeType(property.get());
        return true;
    }
    OptionPropertyInt property{"ArrayAttributeType",
                               "Attribute",
                               R"(Specifies to loop over point, cell, or field data arrays.)"_help,
                               {{"Point Data", "Point Data", 0},
                                {"Cell Data", "Cell Data", 1},
                                {"Field Data", "Field Data", 2}},
                               0};
};

struct Wrapper3 {
    bool set(ttkForEach& filter) {
        filter.SetOutputType(property.get());
        return true;
    }
    OptionPropertyInt property{
        "OutputType",
        "Output Type",
        R"(If mode is set to 'Block', then this parameter is used to set the output type of the filter at the 'RequestInformation' pass.

Auto: Extracted blocks (of any type) are appended to a new 'vtkMultiBlockDataSet'. If the output type is specified, then only one object is returned which is of the specified type (Note: In this case the object is not contained in a 'vtkMultiBlockDataSet').)"_help,
        {{"Auto", "Auto", -1},
         {"vtkMultiBlockDataSet", "vtkMultiBlockDataSet", 13},
         {"vtkImageData", "vtkImageData", 6},
         {"vtkPolyData", "vtkPolyData", 0},
         {"vtkUnstructuredGrid", "vtkUnstructuredGrid", 4},
         {"vtkTable", "vtkTable", 19}},
        0};
};

struct Wrapper4 {
    bool set(ttkForEach& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper5 {
    bool set(ttkForEach& filter) {
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
    bool set(ttkForEach& filter) {
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
    bool set(ttkForEach& filter) {
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
    bool set(ttkForEach& filter) {
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
struct VTKTraits<ttkForEach> {
    static constexpr std::string_view uri = "ttk.ttkForEach";
    static constexpr std::string_view className = "ttkForEach";
    static constexpr std::string_view identifier = "ttkForEach";
    static constexpr std::string_view displayName = "TTK ForEach";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkMultiBlockDataSet", -1, R"(Iteration Source)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Input Parameters",
              {"IterationMode", "OutputType", "ImageExtent", "InputArray", "ArrayAttributeType"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"(This filter works in conjunction with the ttkEndFor filter to iterate over blocks, rows, array values, and arrays.

Online examples:

- https://topology-tool-kit.github.io/examples/cinemaIO/

- https://topology-tool-kit.github.io/examples/contourTreeAlignment/

- https://topology-tool-kit.github.io/examples/nestedTrackingFromOverlap/)";
};

void registerttkForEach(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkForEach>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
