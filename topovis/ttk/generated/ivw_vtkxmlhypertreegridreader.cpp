#include "ivw_vtkxmlhypertreegridreader.h"

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
#include <vtkXMLHyperTreeGridReader.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkXMLHyperTreeGridReader& filter) {
        if (property.get().empty()) return false;
        filter.SetFileName(property.get().string().c_str());
        return true;
    }
    FileProperty property{"FileName", "FileName", R"(This property specifies the file name for the
Hyper Tree grid reader.)"_help,
                          std::filesystem::path{""}};
};

struct Wrapper1 {
    bool set(vtkXMLHyperTreeGridReader& filter) {
        filter.SetActiveTimeDataArrayName(property.get().c_str());
        return true;
    }
    StringProperty property{"TimeArray", "Time Array",
                            R"(This property sets which field data to use as time arrays to
read. If set to Default, time steps are incremented integer values starting at zero.)"_help,
                            "TimeValue"};
};

struct Wrapper2 {
    bool set(vtkXMLHyperTreeGridReader& filter) {
        filter.SetFixedLevel(property.get());
        return true;
    }
    IntProperty property{"MaximumNumberOfLevels",
                         "MaximumNumberOfLevels",
                         R"(This property sets the maximum level to load. Setting it to
1 shows the coarse grid of trees, and setting it at or past maximum level
of the data will load the data at finest resolution. When set to
a value between (excluded) 1 and the maximum level of the trees, data will
be loaded at the required lower resolution, considerably faster.)"_help,
                         128,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{128, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkXMLHyperTreeGridReader> {
    static constexpr std::string_view className = "vtkXMLHyperTreeGridReader";
    static constexpr std::string_view identifier = "XMLHyperTreeGridReaderCore";
    static constexpr std::string_view displayName = "XML HyperTree Grid Reader";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK,readers";
    inline static std::array<InputData, 0> inports = {};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2> properties;
    static constexpr std::string_view doc = R"()";
};

void registervtkXMLHyperTreeGridReader(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkXMLHyperTreeGridReader>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
