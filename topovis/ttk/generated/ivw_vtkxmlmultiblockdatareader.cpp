#include "ivw_vtkxmlmultiblockdatareader.h"

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
#include <vtkXMLMultiBlockDataReader.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkXMLMultiBlockDataReader& filter) {
        if (property.get().empty()) return false;
        filter.SetFileName(property.get().string().c_str());
        return true;
    }
    FileProperty property{"FileName", "FileName",
                          R"(This property specifies the file name for the VTK
multiblock reader.)"_help,
                          std::filesystem::path{""}};
};

struct Wrapper1 {
    bool set(vtkXMLMultiBlockDataReader& filter) {
        filter.SetPieceDistribution(property.get());
        return true;
    }
    OptionPropertyInt property{"PieceDistribution",
                               "PieceDistribution",
                               R"(Control how datasets are loaded in parallel.)"_help,
                               {{"Block", "Block", 0}, {"Interleave", "Interleave", 1}},
                               0};
};

struct Wrapper2 {
    bool set(vtkXMLMultiBlockDataReader& filter) {
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
struct TTKTraits<vtkXMLMultiBlockDataReader> {
    static constexpr std::string_view className = "vtkXMLMultiBlockDataReader";
    static constexpr std::string_view identifier = "XMLMultiBlockDataReaderCore";
    static constexpr std::string_view displayName = "XML Multi-Block Data reader";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK,readers";
    inline static std::array<InputData, 0> inports = {};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2> properties;
    static constexpr std::string_view doc = R"(Internal proxy used by
XMLMultiBlockDataWriter.)";
};

void registervtkXMLMultiBlockDataReader(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkXMLMultiBlockDataReader>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
