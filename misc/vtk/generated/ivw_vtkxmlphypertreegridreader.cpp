#include "ivw_vtkxmlphypertreegridreader.h"
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
#include <vtkXMLPHyperTreeGridReader.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkXMLPHyperTreeGridReader& filter) {
        if (property.get().empty()) return false;
        filter.SetFileName(property.get().string().c_str());
        return true;
    }
    FileProperty property{"FileName", "FileName", R"(This property specifies the file name for the
partitioned VTK XML htg reader.)"_help,
                          std::filesystem::path{""}};
};

struct Wrapper1 {
    bool set(vtkXMLPHyperTreeGridReader& filter) {
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
struct VTKTraits<vtkXMLPHyperTreeGridReader> {
    static constexpr std::string_view uri = "vtk.vtkXMLPHyperTreeGridReader";
    static constexpr std::string_view className = "vtkXMLPHyperTreeGridReader";
    static constexpr std::string_view identifier = "XMLPHyperTreeGridReaderCore";
    static constexpr std::string_view displayName = "XML Partitioned HyperTree Grid Reader";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK,readers";
    inline static std::array<InputData, 0> inports = {};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1> properties;

    static constexpr std::string_view doc = R"(The XML HyperTree Grid reader reads the
partitioned VTK htg data file format. It
reads the partitioned format's summary file and then the
associated VTK XML htg data files. The
expected file extension is .phtg.)";
};

void registervtkXMLPHyperTreeGridReader(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkXMLPHyperTreeGridReader>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
