#include "ivw_vtkxmlpartitioneddatasetreader.h"

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
#include "vtkXMLPartitionedDataSetReader.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkXMLPartitionedDataSetReader& filter) {
        if (property.get().empty()) return false;
        filter.SetFileName(property.get().c_str());
        return true;
    }
    FileProperty property{"FileName", "FileName", ""};
};

struct Wrapper1 {
    bool set(vtkXMLPartitionedDataSetReader& filter) {
        filter.SetPieceDistribution(property.get());
        return true;
    }
    OptionPropertyInt property{"PieceDistribution",
                               "PieceDistribution",
                               {{"Block", "Block", 0}, {"Interleave", "Interleave", 1}},
                               0};
};

struct Wrapper2 {
    bool set(vtkXMLPartitionedDataSetReader& filter) {
        filter.SetActiveTimeDataArrayName(property.get().c_str());
        return true;
    }
    StringProperty property{"TimeArray", "Time Array", "TimeValue"};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkXMLPartitionedDataSetReader> {
    static constexpr std::string_view identifier = "XMLPartitionedDataSetReaderCore";
    static constexpr std::string_view displayName = "XML Partitioned Dataset Reader";
    inline static std::array<InputData, 0> inports = {};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2> properties;
};

void registervtkXMLPartitionedDataSetReader(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkXMLPartitionedDataSetReader>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
