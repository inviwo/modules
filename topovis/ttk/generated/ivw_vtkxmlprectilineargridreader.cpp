#include "ivw_vtkxmlprectilineargridreader.h"

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
#include <vtkXMLPRectilinearGridReader.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkXMLPRectilinearGridReader& filter) {
        if (property.get().empty()) return false;
        filter.SetFileName(property.get().string().c_str());
        return true;
    }
    FileProperty property{"FileName", "FileName", R"(This property specifies the file name for the
partitioned VTK XML non-uniform rectilinear grid
reader.)"_help,
                          std::filesystem::path{""}};
};

struct Wrapper1 {
    bool set(vtkXMLPRectilinearGridReader& filter) {
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
struct TTKTraits<vtkXMLPRectilinearGridReader> {
    static constexpr std::string_view className = "vtkXMLPRectilinearGridReader";
    static constexpr std::string_view identifier = "XMLPRectilinearGridReaderCore";
    static constexpr std::string_view displayName = "XML Partitioned Rectilinear Grid Reader";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK,readers";
    inline static std::array<InputData, 0> inports = {};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1> properties;
    static constexpr std::string_view doc =
        R"(The XML Partitioned Rectilinear Grid reader reads the
partitioned VTK rectilinear grid file format. It reads the
partitioned format's summary file and then the associated
VTK XML rectilinear grid files. The expected file
extension is .pvtr.)";
};

void registervtkXMLPRectilinearGridReader(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkXMLPRectilinearGridReader>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
