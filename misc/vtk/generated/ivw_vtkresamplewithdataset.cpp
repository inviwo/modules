#include "ivw_vtkresamplewithdataset.h"
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
#include <vtkResampleWithDataSet.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkResampleWithDataSet& filter) {
        filter.SetCategoricalData(property.get());
        return true;
    }
    BoolProperty property{"CategoricalData", "CategoricalData",
                          R"(Control whether the source point data is to be
treated as categorical. If the data is categorical, then the
resultant data will be determined by a nearest neighbor
interpolation scheme rather than by linear interpolation.)"_help,
                          false};
};

struct Wrapper1 {
    bool set(vtkResampleWithDataSet& filter) {
        filter.SetPassCellArrays(property.get());
        return true;
    }
    BoolProperty property{
        "PassCellArrays", "PassCellArrays",
        R"(When set the input's cell data arrays are shallow copied to the output.)"_help, false};
};

struct Wrapper2 {
    bool set(vtkResampleWithDataSet& filter) {
        filter.SetPassPointArrays(property.get());
        return true;
    }
    BoolProperty property{
        "PassPointArrays", "PassPointArrays",
        R"(When set the input's point data arrays are shallow copied to the output.)"_help, false};
};

struct Wrapper3 {
    bool set(vtkResampleWithDataSet& filter) {
        filter.SetPassFieldArrays(property.get());
        return true;
    }
    BoolProperty property{"PassFieldArrays", "PassFieldArrays",
                          R"(Set whether to pass the field-data arrays from the Input i.e. the input
providing the geometry to the output. On by default.)"_help,
                          true};
};

struct Wrapper4 {
    bool set(vtkResampleWithDataSet& filter) {
        filter.SetComputeTolerance(property.get());
        return true;
    }
    BoolProperty property{"ComputeTolerance", "ComputeTolerance",
                          R"(Set whether to compute the tolerance or to use a user provided
value. On by default.)"_help,
                          true};
};

struct Wrapper5 {
    bool set(vtkResampleWithDataSet& filter) {
        filter.SetTolerance(property.get());
        return true;
    }
    DoubleProperty property{"Tolerance",
                            "Tolerance",
                            R"(Set the tolerance to use for
vtkDataSet::FindCell)"_help,
                            2.220446049250313e-16,
                            std::pair{2.220446049250313e-16, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(vtkResampleWithDataSet& filter) {
        filter.SetMarkBlankPointsAndCells(property.get());
        return true;
    }
    BoolProperty property{"MarkBlankPointsAndCells", "MarkBlankPointsAndCells",
                          R"(When set, points that did not get valid values during resampling, and
cells containing such points, are marked as blank.)"_help,
                          false};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkResampleWithDataSet> {
    static constexpr std::string_view uri = "vtk.vtkResampleWithDataSet";
    static constexpr std::string_view className = "vtkResampleWithDataSet";
    static constexpr std::string_view identifier = "ResampleWithDataset";
    static constexpr std::string_view displayName = "Resample With Dataset";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 2> inports = {
        InputData{"SourceDataArrays", "vtkDataSet", -1,
                  R"(This property specifies the dataset from which to obtain
probe values. The data attributes come from this dataset.)"},
        InputData{"DestinationMesh", "vtkDataSet", -1,
                  R"(This property specifies the dataset whose geometry will
be used in determining positions to probe. The mesh comes from this
dataset.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6> properties;
    static constexpr std::string_view doc =
        R"(This filter takes two inputs - **Source Data Arrays**, and samples the
point and cell values of this input onto the point locations of the
**Destination Geometry** input. The output has the same structure as the
**Destination Geometry** input, but its point data has the resampled values from
the **Source Data Arrays**.")";
};

void registervtkResampleWithDataSet(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkResampleWithDataSet>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
