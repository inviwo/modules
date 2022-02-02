#include "ivwwrapmeshgraph.h"

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
#include "ttkMeshGraph.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkMeshGraph& filter) {
        filter.SetUseVariableSize(property.get());
        return true;
    }
    BoolProperty property{"UseVariableSize", "Use Variable Size", false};
};

struct Wrapper1 : FieldSelection {
    bool set(ttkMeshGraph& filter) {
        if (property.size() == 0) return false;
        filter.SetInputArrayToProcess(0, 0, 0, 0, property.get().c_str());
        return true;
    }
    OptionPropertyString property{"SizeArray", "Size Array", {}, 0};

    static constexpr std::string_view inport = "Input";
};

struct Wrapper2 {
    bool set(ttkMeshGraph& filter) {
        filter.SetSizeScale(property.get());
        return true;
    }
    DoubleProperty property{"SizeScale", "Size Scale", 1.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper3 {
    bool set(ttkMeshGraph& filter) {
        filter.SetSizeAxis(property.get());
        return true;
    }
    OptionPropertyInt property{
        "SizeAxis", "Size Axis", {{"X", "X", 0}, {"Y", "Y", 1}, {"Z", "Z", 2}}, 1};
};

struct Wrapper4 {
    bool set(ttkMeshGraph& filter) {
        filter.SetUseQuadraticCells(property.get());
        return true;
    }
    BoolProperty property{"UseQuadraticCells", "Use Quadratic Cells", true};
};

struct Wrapper5 {
    bool set(ttkMeshGraph& filter) {
        filter.SetSubdivisions(property.get());
        return true;
    }
    IntProperty property{"Subdivisions", "Subdivisions", 0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{10, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(ttkMeshGraph& filter) {
        filter.SetTetrahedralize(property.get());
        return true;
    }
    BoolProperty property{"UseTetrahedralize", "Tetrahedralize Output", true};
};

struct Wrapper7 {
    bool set(ttkMeshGraph& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper8 {
    bool set(ttkMeshGraph& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper9 {
    bool set(ttkMeshGraph& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel", "Debug Level", 3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

#include <warn/pop>

}  // namespace
template <>
struct TTKTraits<ttkMeshGraph> {
    static constexpr std::string_view identifier = "ttkMeshGraph";
    static constexpr std::string_view displayName = "TTK MeshGraph";
    inline static std::array<InputData, 1> inports = {
        InputData{"Input", "vtkUnstructuredGrid", -1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 3> groups = {
        Group{"Input Options", {"UseVariableSize", "SizeArray"}},
        Group{"Output Options",
              {"SizeAxis", "SizeScale", "UseQuadraticCells", "Subdivisions", "UseTetrahedralize"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9>
        properties;
};

void registerttkMeshGraph(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkMeshGraph>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
