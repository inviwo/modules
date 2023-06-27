#include "ivw_vtkfileseriesreader.h"

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
#include "vtkFileSeriesReader.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkFileSeriesReader& filter) {
        if (property.get().empty()) return false;
        filter.AddFileName(property.get().c_str());
        return true;
    }
    FileProperty property{"FileName", "FileName", ""};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkFileSeriesReader> {
    static constexpr std::string_view identifier = "XMLPartitionedDataSetCollectionReader";
    static constexpr std::string_view displayName = "XML Partitioned Dataset Collection Reader";
    inline static std::array<InputData, 0> inports = {};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;
};

void registervtkFileSeriesReader(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkFileSeriesReader>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
