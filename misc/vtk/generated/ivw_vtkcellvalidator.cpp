#include "ivw_vtkcellvalidator.h"
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
#include <vtkCellValidator.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<vtkCellValidator> {
    static constexpr std::string_view uri = "vtk.vtkCellValidator";
    static constexpr std::string_view className = "vtkCellValidator";
    static constexpr std::string_view identifier = "ValidateCells";
    static constexpr std::string_view displayName = "Validate Cells";
    static constexpr std::string_view category = "vtk";
    static constexpr std::string_view tags = "VTK";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkDataSet", -1,
                  R"(This property specifies the input to the Cell Validation filter.)"}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 0> groups = {};
    std::tuple<> properties;

    static constexpr std::string_view doc =
        R"ivw(vtkCellValidator accepts as input a dataset and adds integral cell data  to it corresponding to the validity of each cell. The validity field  encodes a bitfield for identifying problems that prevent a cell from standard  use, including:     WrongNumberOfPoints: filters assume that a cell has access to the                         appropriate number of points that comprise it. This                         assumption is often tacit, resulting in unexpected                         behavior when the condition is not met. This check                         simply confirms that the cell has the minimum number                         of points needed to describe it.     IntersectingEdges: cells that incorrectly describe the order of their                       points often manifest with intersecting edges or                       intersecting faces. Given a tolerance, this check                       ensures that two edges from a two-dimensional cell                       are separated by at least the tolerance (discounting                       end-to-end connections).     IntersectingFaces: cells that incorrectly describe the order of their                       points often manifest with intersecting edges or                       intersecting faces. Given a tolerance, this check                       ensures that two faces from a three-dimensional cell                       do not intersect.     NoncontiguousEdges: another symptom of incorrect point ordering within a                        cell is the presence of noncontiguous edges where                        contiguous edges are otherwise expected. Given a                        tolerance, this check ensures that edges around the                        perimeter of a two-dimensional cell are contiguous.     Nonconvex: many algorithms implicitly require that all input three-               dimensional cells be convex. This check uses the generic               convexity checkers implemented in vtkPolygon and vtkPolyhedron               to test this requirement.     FacesAreOrientedIncorrectly: All three-dimensional cells have an implicit                                 expectation for the orientation of their                                 faces. While the convention is unfortunately                                 inconsistent across cell types, it is usually                                 required that cell faces point outward. This                                 check tests that the faces of a cell point in                                 the direction required by the cell type,                                 taking into account the cell types with                                 nonstandard orientation requirements. )ivw";
};

void registervtkCellValidator(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<vtkCellValidator>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
