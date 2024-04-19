#include "ivw_vtkxmlimagedatareader.h"
#include <inviwo/core/common/inviwomodule.h>
#include <inviwo/vtk/processors/vtkgenericprocessor.h>
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
#include <vtkXMLImageDataReader.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkXMLImageDataReader& filter) {
        if (property.get().empty()) return false;
        filter.SetFileName(property.get().string().c_str());
        return true;
    }
    FileProperty property{"FileName", "FileName",
                          R"(This property specifies the file name for the VTK XML
uniform rectilinear (image/volume) data reader.)"_help,
                          std::filesystem::path{""}};
};

struct Wrapper1 {
    bool set(vtkXMLImageDataReader& filter) {
        filter.SetActiveTimeDataArrayName(property.get().c_str());
        return true;
    }
    StringProperty property{"TimeArray", "Time Array",
                            R"(This property sets which field data to use as time arrays to
read. If set to Default, time steps are incremented integer values starting at zero.)"_help,
                            "TimeValue"};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkXMLImageDataReader> {
    static constexpr std::string_view uri = "vtk.vtkXMLImageDataReader";
    static constexpr std::string_view className = "vtkXMLImageDataReader";
    static constexpr std::string_view identifier = "XMLImageDataReaderCore";
    static constexpr std::string_view displayName = "XML Image Data Reader";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK,readers";
    inline static std::array<InputData, 0> inports = {};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1> properties;

    static constexpr std::string_view doc = R"(The XML Image
Data reader reads the VTK XML image data file format. The
standard extension is .vti.)";
};

void registervtkXMLImageDataReader(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkXMLImageDataReader>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
