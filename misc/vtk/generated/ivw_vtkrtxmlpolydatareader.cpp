#include "ivw_vtkrtxmlpolydatareader.h"
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
#include <vtkRTXMLPolyDataReader.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkRTXMLPolyDataReader& filter) {
        if (property.get().empty()) return false;
        filter.SetFileName(property.get().string().c_str());
        return true;
    }
    FileProperty property{"FileName", "FileName",
                          R"(Set the file name for the real-time VTK polygonal
dataset reader.)"_help,
                          std::filesystem::path{""}};
};

struct Wrapper1 {
    bool set(vtkRTXMLPolyDataReader& filter) {
        if (property.get().empty()) return false;
        filter.SetLocation(property.get().string().c_str());
        return true;
    }
    FileProperty property{"Location", "Location",
                          R"(Set the data directory containing real time data
files.)"_help,
                          std::filesystem::path{""}};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkRTXMLPolyDataReader> {
    static constexpr std::string_view uri = "vtk.vtkRTXMLPolyDataReader";
    static constexpr std::string_view className = "vtkRTXMLPolyDataReader";
    static constexpr std::string_view identifier = "RTXMLPolyDataReader";
    static constexpr std::string_view displayName = "RTXMLPolyDataReader";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK,readers";
    inline static std::array<InputData, 0> inports = {};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1> properties;

    static constexpr std::string_view doc = R"()";
};

void registervtkRTXMLPolyDataReader(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkRTXMLPolyDataReader>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
