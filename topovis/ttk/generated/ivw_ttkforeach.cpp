#include "ivw_ttkforeach.h"

#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/ttk/processors/ttkgenericprocessor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/fileproperty.h>

#include <tuple>
#include <array>
#include <string_view>

#include <warn/push>
#include <warn/ignore/all>
#include "ttkForEach.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
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
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"InputArray", "Input Array", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper2 {
    bool set(ttkForEach& filter) {
        filter.SetArrayAttributeType(property.get());
        return true;
    }
    OptionPropertyInt property{"ArrayAttributeType",
                               "Attribute",
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
    OptionPropertyInt property{"OutputType",
                               "Output Type",
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
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper5 {
    bool set(ttkForEach& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(ttkForEach& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkForEach& filter) {
        filter.SetCompactTriangulationCacheSize(property.get());
        return true;
    }
    DoubleProperty property{"CompactTriangulationCacheSize", "Cache", 0.2,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<ttkForEach> {
    static constexpr std::string_view identifier = "ttkForEach";
    static constexpr std::string_view displayName = "TTK ForEach";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkMultiBlockDataSet", -1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 2> groups = {
        Group{"Input Parameters",
              {"IterationMode", "OutputType", "ImageExtent", "InputArray", "ArrayAttributeType"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7>
        properties;
};

void registerttkForEach(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkForEach>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
