#include "ivw_vtkxmlmultiblockdatareader.h"

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
#include "vtkXMLMultiBlockDataReader.h"
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
    FileProperty property{"FileName", "FileName", ""};
};

struct Wrapper1 {
    bool set(vtkXMLMultiBlockDataReader& filter) {
        filter.SetPieceDistribution(property.get());
        return true;
    }
    OptionPropertyInt property{"PieceDistribution",
                               "PieceDistribution",
                               {{"Block", "Block", 0}, {"Interleave", "Interleave", 1}},
                               0};
};

struct Wrapper2 {
    bool set(vtkXMLMultiBlockDataReader& filter) {
        filter.SetActiveTimeDataArrayName(property.get().c_str());
        return true;
    }
    StringProperty property{"TimeArray", "Time Array", "TimeValue"};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkXMLMultiBlockDataReader> {
    static constexpr std::string_view identifier = "XMLMultiBlockDataReaderCore";
    static constexpr std::string_view displayName = "XML Multi-Block Data reader";
    inline static std::array<InputData, 0> inports = {};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2> properties;
};

void registervtkXMLMultiBlockDataReader(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkXMLMultiBlockDataReader>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
