#include "ivw_ttktopologicalcompressionreader.h"
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
#include <ttkTopologicalCompressionReader.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkTopologicalCompressionReader& filter) {
        if (property.get().empty()) return false;
        filter.SetFileName(property.get().string().c_str());
        return true;
    }
    FileProperty property{"FileName", "FileName",
                          R"(This property specifies the file name for the OFF reader.)"_help,
                          std::filesystem::path{""}};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<ttkTopologicalCompressionReader> {
    static constexpr std::string_view uri = "ttk.ttkTopologicalCompressionReader";
    static constexpr std::string_view className = "ttkTopologicalCompressionReader";
    static constexpr std::string_view identifier = "ttkTopologicalCompressionReader";
    static constexpr std::string_view displayName = "TTK TopologicalCompressionReader";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 0> inports = {};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 1> groups = {Group{"Select file", {"FileName"}}};
    std::tuple<Wrapper0> properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"ivw(This plugin specifies the file name for the TTK reader.

Online examples:

- https://topology-tool-kit.github.io/examples/persistenceDrivenCompression/)ivw";
};

void registerttkTopologicalCompressionReader(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkTopologicalCompressionReader>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
