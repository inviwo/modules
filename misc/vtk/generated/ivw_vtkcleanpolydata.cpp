#include "ivw_vtkcleanpolydata.h"
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
#include <vtkCleanPolyData.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(vtkCleanPolyData& filter) {
        filter.SetPieceInvariant(property.get());
        return true;
    }
    BoolProperty property{"PieceInvariant", "PieceInvariant",
                          R"(If this property is set to 1, the whole data set will be
processed at once so that cleaning the data set always produces the
same results. If it is set to 0, the data set can be processed one
piece at a time, so it is not necessary for the entire data set to fit
into memory; however the results are not guaranteed to be the same as
they would be if the Piece invariant option was on. Setting this option
to 0 may produce seams in the output dataset when ParaView is run in
parallel.)"_help,
                          true};
};

struct Wrapper1 {
    bool set(vtkCleanPolyData& filter) {
        filter.SetTolerance(property.get());
        return true;
    }
    DoubleProperty property{"Tolerance",
                            "Tolerance",
                            R"(If merging nearby points (see PointMerging property) and
not using absolute tolerance (see ToleranceIsAbsolute property), this
property specifies the tolerance for performing merging as a fraction
of the length of the diagonal of the bounding box of the input data
set.)"_help,
                            0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(vtkCleanPolyData& filter) {
        filter.SetAbsoluteTolerance(property.get());
        return true;
    }
    DoubleProperty property{"AbsoluteTolerance",
                            "AbsoluteTolerance",
                            R"(If merging nearby points (see PointMerging property) and
using absolute tolerance (see ToleranceIsAbsolute property), this
property specifies the tolerance for performing merging in the spatial
units of the input data set.)"_help,
                            1.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(vtkCleanPolyData& filter) {
        filter.SetToleranceIsAbsolute(property.get());
        return true;
    }
    BoolProperty property{"ToleranceIsAbsolute", "ToleranceIsAbsolute",
                          R"(This property determines whether to use absolute or
relative (a percentage of the bounding box) tolerance when performing
point merging.)"_help,
                          false};
};

struct Wrapper4 {
    bool set(vtkCleanPolyData& filter) {
        filter.SetConvertLinesToPoints(property.get());
        return true;
    }
    BoolProperty property{"ConvertLinesToPoints", "ConvertLinesToPoints",
                          R"(If this property is set to 1, degenerate lines (a "line"
whose endpoints are at the same spatial location) will be converted to
points.)"_help,
                          true};
};

struct Wrapper5 {
    bool set(vtkCleanPolyData& filter) {
        filter.SetConvertPolysToLines(property.get());
        return true;
    }
    BoolProperty property{"ConvertPolysToLines", "ConvertPolysToLines",
                          R"(If this property is set to 1, degenerate polygons (a
"polygon" with only two distinct point coordinates) will be converted
to lines.)"_help,
                          true};
};

struct Wrapper6 {
    bool set(vtkCleanPolyData& filter) {
        filter.SetConvertStripsToPolys(property.get());
        return true;
    }
    BoolProperty property{"ConvertStripsToPolys", "ConvertStripsToPolys",
                          R"(If this property is set to 1, degenerate triangle strips
(a triangle "strip" containing only one triangle) will be converted to
triangles.)"_help,
                          true};
};

struct Wrapper7 {
    bool set(vtkCleanPolyData& filter) {
        filter.SetPointMerging(property.get());
        return true;
    }
    BoolProperty property{"PointMerging", "PointMerging",
                          R"(If this property is set to 1, then points will be merged
if they are within the specified Tolerance or AbsoluteTolerance (see
the Tolerance and AbsoluteTolerance properties), depending on the value
of the ToleranceIsAbsolute property. (See the ToleranceIsAbsolute
property.) If this property is set to 0, points will not be
merged.)"_help,
                          true};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkCleanPolyData> {
    static constexpr std::string_view uri = "vtk.vtkCleanPolyData";
    static constexpr std::string_view className = "vtkCleanPolyData";
    static constexpr std::string_view identifier = "CleanPolyData";
    static constexpr std::string_view displayName = "Clean";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkPolyData", -1, R"(Set the input to the Clean filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7>
        properties;

    static constexpr std::string_view doc = R"(The Clean filter
takes polygonal data as input and generates polygonal data
as output. This filter can merge duplicate points, remove
unused points, and transform degenerate cells into their
appropriate forms (e.g., a triangle is converted into a
line if two of its points are merged).)";
};

void registervtkCleanPolyData(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkCleanPolyData>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
