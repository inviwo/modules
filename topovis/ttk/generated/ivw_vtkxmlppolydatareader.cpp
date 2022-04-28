#include "ivw_vtkxmlppolydatareader.h"

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
#include "vtkXMLPPolyDataReader.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkXMLPPolyDataReader& filter) {
        if (property.get().empty()) return false;
        filter.SetFileName(property.get().c_str());
        return true;
    }
    FileProperty property{"FileName", "FileName", ""};
};

struct Wrapper1 {
    bool set(vtkXMLPPolyDataReader& filter) {
        filter.SetActiveTimeDataArrayName(property.get().c_str());
        return true;
    }
    StringProperty property{"TimeArray", "Time Array", "TimeValue"};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkXMLPPolyDataReader> {
    static constexpr std::string_view identifier = "XMLPPolyDataReaderCore";
    static constexpr std::string_view displayName = "XML Partitioned Polydata Reader";
    inline static std::array<InputData, 0> inports = {};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1> properties;
};

void registervtkXMLPPolyDataReader(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkXMLPPolyDataReader>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
