#include "ivw_vtkimagemedian3d.h"

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
#include <vtkImageMedian3D.h>
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 : FieldSelection {
    bool set(vtkImageMedian3D& filter) {
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
        CompositeProperty tmp{"SelectInputScalars", "SelectInputScalars",
                              R"(The value of this property lists the name of the scalar
array to use in computing the median.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper1 {
    bool set(vtkImageMedian3D& filter) {
        filter.SetKernelSize(property.get(0), property.get(1), property.get(2));
        return true;
    }
    IntVec3Property property{"KernelSize",
                             "KernelSize",
                             R"(The value of this property specifies the number of
pixels/voxels in each dimension to use in computing the median to
assign to each pixel/voxel. If the kernel size in a particular
dimension is 1, then the median will not be computed in that
direction.)"_help,
                             ivec3{1, 1, 1},
                             std::pair{ivec3{0, 0, 0}, ConstraintBehavior::Ignore},
                             std::pair{ivec3{100, 100, 100}, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<vtkImageMedian3D> {
    static constexpr std::string_view className = "vtkImageMedian3D";
    static constexpr std::string_view identifier = "Median";
    static constexpr std::string_view displayName = "Median";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkImageData", 1, R"(This property specifies the input to the Median
filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1> properties;
    static constexpr std::string_view doc =
        R"(The Median filter operates on uniform rectilinear (image
or volume) data and produces uniform rectilinear output.
It replaces the scalar value at each pixel / voxel with
the median scalar value in the specified surrounding
neighborhood. Since the median operation removes outliers,
this filter is useful for removing high-intensity,
low-probability noise (shot noise).)";
};

void registervtkImageMedian3D(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<vtkImageMedian3D>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
