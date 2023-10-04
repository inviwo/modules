#include "ivw_ttkpointsettocurve.h"

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
#include <ttkPointSetToCurve.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(ttkPointSetToCurve& filter) {
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
        CompositeProperty tmp{"InputOrderingArray", "Input Ordering Array",
                              R"(Select the input ordering array.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Domain";
};

struct Wrapper1 {
    bool set(ttkPointSetToCurve& filter) {
        filter.SetCloseCurve(property.get());
        return true;
    }
    BoolProperty property{"CloseCurve", "Close Curve",
                          R"(Close the curve by generating a line cell between the last
point and the first point of the point set.)"_help,
                          false};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<ttkPointSetToCurve> {
    static constexpr std::string_view className = "ttkPointSetToCurve";
    static constexpr std::string_view identifier = "ttkPointSetToCurve";
    static constexpr std::string_view displayName = "TTK PointSetToCurve";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Domain", "vtkPointSet", -1, R"(Data-set to process.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 1> groups = {Group{"Input options", {"InputOrderingArray"}}};
    std::tuple<Wrapper0, Wrapper1> properties;
    static constexpr std::string_view doc =
        R"(This filter generates lines between points in a Point Set
according to the ordering of a given Point Data.

Online examples:

- https://topology-tool-kit.github.io/examples/mergeTreePGA/)";
};

void registerttkPointSetToCurve(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkPointSetToCurve>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
