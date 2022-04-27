#include "ivwwraptriangulationwriter.h"

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
#include "ttkTriangulationWriter.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkTriangulationWriter& filter) {
        if (property.get().empty()) return false;
        filter.SetFilename(property.get().c_str());
        return true;
    }
    FileProperty property{"FileName", "FileName", ""};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<ttkTriangulationWriter> {
    static constexpr std::string_view identifier = "ttkTriangulationWriter";
    static constexpr std::string_view displayName = "TTK Triangulation File Writer";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkUnstructuredGrid", -1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;
};

void registerttkTriangulationWriter(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkTriangulationWriter>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
