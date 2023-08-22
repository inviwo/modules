#include "ivw_vtkxmlimagedatawriter.h"

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
#include <vtkXMLImageDataWriter.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkXMLImageDataWriter& filter) {
        if (property.get().empty()) return false;
        filter.SetFileName(property.get().string().c_str());
        return true;
    }
    FileProperty property{"FileName", "FileName",
                          R"(This property specifies the file name for the VTK XML
uniform rectilinear (image/volume) data reader.)"_help,
                          std::filesystem::path{""}};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkXMLImageDataWriter> {
    static constexpr std::string_view className = "vtkXMLImageDataWriter";
    static constexpr std::string_view identifier = "XMLImageDataWriter";
    static constexpr std::string_view displayName = "XMLImageDataWriter";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1, R"(The input filter/source whose output dataset is to
written to the file.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0> properties;
    static constexpr std::string_view doc =
        R"(Writer to write image data in a xml-based vtk data file. Cannot be used
for parallel writing.)";
};

void registervtkXMLImageDataWriter(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkXMLImageDataWriter>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
