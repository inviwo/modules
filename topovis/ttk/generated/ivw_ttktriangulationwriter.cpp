#include "ivw_ttktriangulationwriter.h"
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
#include <ttkTriangulationWriter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkTriangulationWriter& filter) {
        if (property.get().empty()) return false;
        filter.SetFilename(property.get().string().c_str());
        return true;
    }
    FileProperty property{
        "FileName", "FileName",
        R"(This property specifies the file name for the Triangulation writer.)"_help,
        std::filesystem::path{""}};
};

struct Wrapper1 {
    bool set(ttkTriangulationWriter& filter) {
        filter.SetUseASCIIFormat(property.get());
        return true;
    }
    BoolProperty property{"UseASCIIFormat", "Use ASCII format (no reader available)",
                          R"(Write Triangulation using ASCII format instead of binary.)"_help,
                          false};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<ttkTriangulationWriter> {
    static constexpr std::string_view uri = "ttk.ttkTriangulationWriter";
    static constexpr std::string_view className = "ttkTriangulationWriter";
    static constexpr std::string_view identifier = "ttkTriangulationWriter";
    static constexpr std::string_view displayName = "TTK Triangulation File Writer";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkUnstructuredGrid", -1, R"()"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 1> groups = {Group{"Output", {"FileName", "UseASCIIFormat"}}};
    std::tuple<Wrapper0, Wrapper1> properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc = R"(Export a TTK (Explicit) Triangulation into a file.)";
};

void registerttkTriangulationWriter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkTriangulationWriter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
