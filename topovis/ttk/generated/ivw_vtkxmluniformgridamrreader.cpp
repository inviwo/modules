#include "ivw_vtkxmluniformgridamrreader.h"

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
#include "vtkXMLUniformGridAMRReader.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkXMLUniformGridAMRReader& filter) {
        filter.SetMaximumLevelsToReadByDefault(property.get());
        return true;
    }
    IntProperty property{"DefaultNumberOfLevels", "DefaultNumberOfLevels", 1,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{10, ConstraintBehavior::Ignore}};
};

struct Wrapper1 {
    bool set(vtkXMLUniformGridAMRReader& filter) {
        filter.SetActiveTimeDataArrayName(property.get().c_str());
        return true;
    }
    StringProperty property{"TimeArray", "Time Array", "TimeValue"};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkXMLUniformGridAMRReader> {
    static constexpr std::string_view identifier = "XMLUniformGridAMRReaderCore";
    static constexpr std::string_view displayName = "XML UniformGrid AMR reader";
    inline static std::array<InputData, 0> inports = {};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1> properties;
};

void registervtkXMLUniformGridAMRReader(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkXMLUniformGridAMRReader>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
