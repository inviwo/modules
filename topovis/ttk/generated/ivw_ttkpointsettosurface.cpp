#include "ivw_ttkpointsettosurface.h"

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
#include <ttkPointSetToSurface.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkPointSetToSurface& filter) {
        if (property.size() == 0) return false;
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
        0};

    CompositeProperty property{[&]() {
        CompositeProperty tmp{"InputOrderingXArray", "Input Ordering X Array",
                              R"(Select the input ordering X array.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Domain";
};

struct Wrapper1 : FieldSelection {
    bool set(ttkPointSetToSurface& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(1, 0, 0, fieldAssociation.get(), name.get().c_str());
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
        0};

    CompositeProperty property{[&]() {
        CompositeProperty tmp{"InputOrderingYArray", "Input Ordering Y Array",
                              R"(Select the input ordering Y array.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Domain";
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<ttkPointSetToSurface> {
    static constexpr std::string_view className = "ttkPointSetToSurface";
    static constexpr std::string_view identifier = "ttkPointSetToSurface";
    static constexpr std::string_view displayName = "TTK PointSetToSurface";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Domain", "vtkPointSet", 1, R"(Data-set to process.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 1> groups = {
        Group{"Input options", {"InputOrderingXArray", "InputOrderingYArray"}}};
    std::tuple<Wrapper0, Wrapper1> properties;
    static constexpr std::string_view doc =
        R"(This filter generates a surface between points in a Point Set
according to the ordering of two given Point Data arrays.

Online examples:

- https://topology-tool-kit.github.io/examples/mergeTreePGA/)";
};

void registerttkPointSetToSurface(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkPointSetToSurface>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
