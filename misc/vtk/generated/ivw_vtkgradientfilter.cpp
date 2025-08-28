#include "ivw_vtkgradientfilter.h"
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
#include <vtkGradientFilter.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(vtkGradientFilter& filter) {
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
        CompositeProperty tmp{"SelectInputScalars", "Scalar Array",
                              R"(This property lists the name of the scalar array from
which to compute the gradient.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(vtkGradientFilter& filter) {
        filter.SetComputeGradient(property.get());
        return true;
    }
    BoolProperty property{"ComputeGradient", "ComputeGradient",
                          R"(When this flag is on, the gradient filter will compute
the gradient of the input array.)"_help,
                          true};
};

struct Wrapper2 {
    bool set(vtkGradientFilter& filter) {
        filter.SetResultArrayName(property.get().c_str());
        return true;
    }
    StringProperty property{"ResultArrayName", "ResultArrayName",
                            R"(This property provides a name for the output array
containing the gradient vectors.)"_help,
                            "Gradients"};
};

struct Wrapper3 {
    bool set(vtkGradientFilter& filter) {
        filter.SetFasterApproximation(property.get());
        return true;
    }
    BoolProperty property{"FasterApproximation", "FasterApproximation",
                          R"(When this flag is on, the gradient filter will provide a
less accurate (but close) algorithm that performs fewer derivative
calculations (and is therefore faster). The error contains some
smoothing of the output data and some possible errors on the boundary.
This parameter has no effect when performing the gradient of cell
data or when the input grid is not a vtkUnstructuredGrid.)"_help,
                          false};
};

struct Wrapper4 {
    bool set(vtkGradientFilter& filter) {
        filter.SetComputeDivergence(property.get());
        return true;
    }
    BoolProperty property{"ComputeDivergence", "ComputeDivergence",
                          R"(When this flag is on, the gradient filter will compute
the divergence of a 3 component array.)"_help,
                          false};
};

struct Wrapper5 {
    bool set(vtkGradientFilter& filter) {
        filter.SetDivergenceArrayName(property.get().c_str());
        return true;
    }
    StringProperty property{"DivergenceArrayName", "DivergenceArrayName",
                            R"(This property provides a name for the output array
containing the divergence vector.)"_help,
                            "Divergence"};
};

struct Wrapper6 {
    bool set(vtkGradientFilter& filter) {
        filter.SetComputeVorticity(property.get());
        return true;
    }
    BoolProperty property{"ComputeVorticity", "ComputeVorticity",
                          R"(When this flag is on, the gradient filter will compute
the vorticity/curl of a 3 component array.)"_help,
                          false};
};

struct Wrapper7 {
    bool set(vtkGradientFilter& filter) {
        filter.SetVorticityArrayName(property.get().c_str());
        return true;
    }
    StringProperty property{"VorticityArrayName", "VorticityArrayName",
                            R"(This property provides a name for the output array
containing the vorticity vector.)"_help,
                            "Vorticity"};
};

struct Wrapper8 {
    bool set(vtkGradientFilter& filter) {
        filter.SetComputeQCriterion(property.get());
        return true;
    }
    BoolProperty property{"ComputeQCriterion", "ComputeQCriterion",
                          R"(When this flag is on, the gradient filter will compute
the Q-criterion of a 3 component array.)"_help,
                          false};
};

struct Wrapper9 {
    bool set(vtkGradientFilter& filter) {
        filter.SetQCriterionArrayName(property.get().c_str());
        return true;
    }
    StringProperty property{"QCriterionArrayName", "QCriterionArrayName",
                            R"(This property provides a name for the output array
containing Q criterion.)"_help,
                            "Q-criterion"};
};

struct Wrapper10 {
    bool set(vtkGradientFilter& filter) {
        filter.SetContributingCellOption(property.get());
        return true;
    }
    OptionPropertyInt property{
        "ContributingCellOption",
        "ContributingCellOption",
        R"(Specify which dimensions of cells should be used
when computing gradient quantities. Default is to use
the dataset's maximum cell dimension.)"_help,
        {{"All", "All", 0}, {"Patch", "Patch", 1}, {"Dataset Max", "Dataset Max", 2}},
        2};
};

struct Wrapper11 {
    bool set(vtkGradientFilter& filter) {
        filter.SetReplacementValueOption(property.get());
        return true;
    }
    OptionPropertyInt property{"ReplacementValueOption",
                               "ReplacementValueOption",
                               R"(Specify what value to use for when the gradient quantities at a
point can't be computed with the selected **ContributingCellOption**.)"_help,
                               {{"Zero", "Zero", 0},
                                {"NaN", "NaN", 1},
                                {"Data Type Min", "Data Type Min", 2},
                                {"Data Type Max", "Data Type Max", 2}},
                               1};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkGradientFilter> {
    static constexpr std::string_view uri = "vtk.vtkGradientFilter";
    static constexpr std::string_view className = "vtkGradientFilter";
    static constexpr std::string_view identifier = "UnstructuredGradient";
    static constexpr std::string_view displayName = "Gradient Of Unstructured DataSet";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{.identifier = "Input",
                  .dataType = "vtkDataSet",
                  .numComp = -1,
                  .doc = R"(This property specifies the input to the Gradient
(Unstructured) filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11>
        properties;

    static constexpr std::string_view doc =
        R"ivw(The Gradient (Unstructured) filter estimates the gradient
vector at each point or cell. It operates on any type of
vtkDataSet, and the output is the same type as the input.
If the dataset is a vtkImageData, use the Gradient filter
instead; it will be more efficient for this type of
dataset.)ivw";
};

void registervtkGradientFilter(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkGradientFilter>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
