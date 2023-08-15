#include "ivw_vtkxmlpartitioneddatasetcollectionreader.h"

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
#include "vtkXMLPartitionedDataSetCollectionReader.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkXMLPartitionedDataSetCollectionReader& filter) {
        if (property.get().empty()) return false;
        filter.SetFileName(property.get().string().c_str());
        return true;
    }
    FileProperty property{"FileName", "FileName", ""};
};

struct Wrapper1 {
    bool set(vtkXMLPartitionedDataSetCollectionReader& filter) {
        filter.SetActiveTimeDataArrayName(property.get().c_str());
        return true;
    }
    StringProperty property{"TimeArray", "Time Array", "TimeValue"};
};

struct Wrapper2 {
    bool set(vtkXMLPartitionedDataSetCollectionReader& filter) {
        filter.SetPieceDistribution(property.get());
        return true;
    }
    OptionPropertyInt property{"PieceDistribution",
                               "PieceDistribution",
                               {{"Block", "Block", 0}, {"Interleave", "Interleave", 1}},
                               0};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkXMLPartitionedDataSetCollectionReader> {
    static constexpr std::string_view identifier = "XMLPartitionedDataSetCollectionReaderCore";
    static constexpr std::string_view displayName = "XML Partitioned Dataset Collection Reader";
    inline static std::array<InputData, 0> inports = {};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2> properties;
};

void registervtkXMLPartitionedDataSetCollectionReader(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkXMLPartitionedDataSetCollectionReader>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
