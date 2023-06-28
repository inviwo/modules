#include "ivw_vtkrtxmlpolydatareader.h"

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
#include "vtkRTXMLPolyDataReader.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkRTXMLPolyDataReader& filter) {
        if (property.get().empty()) return false;
        filter.SetFileName(property.get().string().c_str());
        return true;
    }
    FileProperty property{"FileName", "FileName", ""};
};

struct Wrapper1 {
    bool set(vtkRTXMLPolyDataReader& filter) {
        if (property.get().empty()) return false;
        filter.SetLocation(property.get().string().c_str());
        return true;
    }
    FileProperty property{"Location", "Location", ""};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkRTXMLPolyDataReader> {
    static constexpr std::string_view identifier = "RTXMLPolyDataReader";
    static constexpr std::string_view displayName = "RTXMLPolyDataReader";
    inline static std::array<InputData, 0> inports = {};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1> properties;
};

void registervtkRTXMLPolyDataReader(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkRTXMLPolyDataReader>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
