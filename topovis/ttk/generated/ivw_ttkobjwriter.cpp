#include "ivw_ttkobjwriter.h"
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
#include <ttkOBJWriter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkOBJWriter& filter) {
        if (property.get().empty()) return false;
        filter.SetFilename(property.get().string().c_str());
        return true;
    }
    FileProperty property{"FileName", "FileName",
                          R"(This property specifies the file name for the OBJ writer.)"_help,
                          std::filesystem::path{""}};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<ttkOBJWriter> {
    static constexpr std::string_view uri = "ttk.ttkOBJWriter";
    static constexpr std::string_view className = "ttkOBJWriter";
    static constexpr std::string_view identifier = "ttkOBJWriter";
    static constexpr std::string_view displayName = "TTK OBJWriter";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{.identifier = "Input", .dataType = "vtkDataSet", .numComp = -1, .doc = R"()"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"ivw(Export a VTK Unstructured Grid into a Wavefront OBJ file.)ivw";
};

void registerttkOBJWriter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkOBJWriter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
