#include "ivw_ttkstringarrayconverter.h"
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
#include <ttkStringArrayConverter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkStringArrayConverter& filter) {
        if (name.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, fieldAssociation.get(), name.get().c_str());
        return true;
    }
    OptionPropertyString name{"name", "Name", {}, 0};

    OptionProperty<vtkDataObject::FieldAssociations> fieldAssociation{
        "fieldAssociation",
        "Field Association",
        {{"points", "Points", vtkDataObject::FIELD_ASSOCIATION_POINTS},
         {"cells", "Cells", vtkDataObject::FIELD_ASSOCIATION_CELLS},
         {"none", "None", vtkDataObject::FIELD_ASSOCIATION_NONE},
         {"pointsThenCells", "Points then Cells",
          vtkDataObject::FIELD_ASSOCIATION_POINTS_THEN_CELLS}},
        3};

    CompositeProperty property{[&]() {
        CompositeProperty tmp{"InputStringArray", "Input String Array",
                              R"(Select the input string array.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Domain";
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<ttkStringArrayConverter> {
    static constexpr std::string_view uri = "ttk.ttkStringArrayConverter";
    static constexpr std::string_view className = "ttkStringArrayConverter";
    static constexpr std::string_view identifier = "ttkStringArrayConverter";
    static constexpr std::string_view displayName = "TTK StringArrayConverter";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {InputData{.identifier = "Domain",
                                                                .dataType = "vtkDataSet",
                                                                .numComp = -1,
                                                                .doc = R"(Data-set to process.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 1> groups = {Group{"Input options", {"Input String Array"}}};
    std::tuple<Wrapper0> properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"ivw(This filter converts an input vtkStringArray into an
vtkIntArray to make it easier to apply Threshold on the data
set. The correspondence between a string value and an int is
to be stored in the output Field Data.)ivw";
};

void registerttkStringArrayConverter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkStringArrayConverter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
