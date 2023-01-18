#include "ivwwrapdimensionreduction.h"

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
#include "ttkDimensionReduction.h"
#include <warn/pop>

namespace inviwo {
namespace ttkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkDimensionReduction& filter) {
        filter.SetSelectFieldsWithRegexp(property.get());
        return true;
    }
    BoolProperty property{"SelectFieldsWithRegexp", "Select Fields with a Regexp", false};
};

struct Wrapper1 {
    bool set(ttkDimensionReduction& filter) {
        filter.SetRegexpString(property.get().c_str());
        return true;
    }
    StringProperty property{"Regexp", "Regexp", ".*"};
};

struct Wrapper2 {
    bool set(ttkDimensionReduction& filter) {
        filter.SetMethod(property.get());
        return true;
    }
    OptionPropertyInt property{
        "Method",
        "Method",
        {{"Spectral Embedding", "Spectral Embedding", 0},
         {"Locally Linear Embedding", "Locally Linear Embedding", 1},
         {"Multi-Dimensional Scaling", "Multi-Dimensional Scaling", 2},
         {"t-distributed Stochastic Neighbor Embedding",
          "t-distributed Stochastic Neighbor Embedding", 3},
         {"Isomap Embedding", "Isomap Embedding", 4},
         {"Principal Component Analysis", "Principal Component Analysis", 5}},
        2};
};

struct Wrapper3 {
    bool set(ttkDimensionReduction& filter) {
        filter.SetNumberOfComponents(property.get());
        return true;
    }
    IntProperty property{"NumberOfComponents", "Components", 2,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper4 {
    bool set(ttkDimensionReduction& filter) {
        filter.SetNumberOfNeighbors(property.get());
        return true;
    }
    IntProperty property{"NumberOfNeighbors", "Neighbors", 5,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkDimensionReduction& filter) {
        filter.SetKeepAllDataArrays(property.get());
        return true;
    }
    BoolProperty property{"KeepAllDataArrays", "Keep All Data Arrays", true};
};

struct Wrapper6 {
    bool set(ttkDimensionReduction& filter) {
        filter.SetInputIsADistanceMatrix(property.get());
        return true;
    }
    BoolProperty property{"InputDistanceMatrix", "Input Is a Distance Matrix", false};
};

struct Wrapper7 {
    bool set(ttkDimensionReduction& filter) {
        filter.Setse_Affinity(property.get().c_str());
        return true;
    }
    StringProperty property{"se_Affinity", "Affinity", "nearest_neighbors"};
};

struct Wrapper8 {
    bool set(ttkDimensionReduction& filter) {
        filter.Setse_Gamma(property.get());
        return true;
    }
    DoubleProperty property{"se_Gamma", "Gamma", 1.0, std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper9 {
    bool set(ttkDimensionReduction& filter) {
        filter.Setse_EigenSolver(property.get().c_str());
        return true;
    }
    StringProperty property{"se_EigenSolver", "Eigen Solver", "auto"};
};

struct Wrapper10 {
    bool set(ttkDimensionReduction& filter) {
        filter.Setlle_Regularization(property.get());
        return true;
    }
    DoubleProperty property{"lle_Regularization", "Regularization", 0.001,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper11 {
    bool set(ttkDimensionReduction& filter) {
        filter.Setlle_EigenSolver(property.get().c_str());
        return true;
    }
    StringProperty property{"lle_EigenSolver", "Eigen Solver", "auto"};
};

struct Wrapper12 {
    bool set(ttkDimensionReduction& filter) {
        filter.Setlle_Tolerance(property.get());
        return true;
    }
    DoubleProperty property{"lle_Tolerance", "Tolerance", 0.001,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper13 {
    bool set(ttkDimensionReduction& filter) {
        filter.Setlle_MaxIteration(property.get());
        return true;
    }
    IntProperty property{"lle_MaxIteration", "Iteration threshold", 300,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper14 {
    bool set(ttkDimensionReduction& filter) {
        filter.Setlle_Method(property.get().c_str());
        return true;
    }
    StringProperty property{"lle_Method", "Method", "standard"};
};

struct Wrapper15 {
    bool set(ttkDimensionReduction& filter) {
        filter.Setlle_HessianTolerance(property.get());
        return true;
    }
    DoubleProperty property{"lle_HessianTolerance", "Hessian Tolerance", 0.001,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper16 {
    bool set(ttkDimensionReduction& filter) {
        filter.Setlle_ModifiedTolerance(property.get());
        return true;
    }
    DoubleProperty property{"lle_ModifiedTolerance", "Modified Tolerance", 0.001,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper17 {
    bool set(ttkDimensionReduction& filter) {
        filter.Setlle_NeighborsAlgorithm(property.get().c_str());
        return true;
    }
    StringProperty property{"lle_NeighborsAlgorithm", "NN-algorithm", "auto"};
};

struct Wrapper18 {
    bool set(ttkDimensionReduction& filter) {
        filter.Setmds_Metric(property.get());
        return true;
    }
    BoolProperty property{"mds_Metric", "Metric", true};
};

struct Wrapper19 {
    bool set(ttkDimensionReduction& filter) {
        filter.Setmds_Init(property.get());
        return true;
    }
    IntProperty property{"mds_Init", "Number of init runs", 4,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper20 {
    bool set(ttkDimensionReduction& filter) {
        filter.Setmds_MaxIteration(property.get());
        return true;
    }
    IntProperty property{"mds_MaxIteration", "Iteration Threshold", 300,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper21 {
    bool set(ttkDimensionReduction& filter) {
        filter.Setmds_Verbose(property.get());
        return true;
    }
    IntProperty property{"mds_Verbose", "Verbosity Level", 0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper22 {
    bool set(ttkDimensionReduction& filter) {
        filter.Setmds_Epsilon(property.get());
        return true;
    }
    DoubleProperty property{"mds_Epsilon", "Epsilon", 0.001,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper23 {
    bool set(ttkDimensionReduction& filter) {
        filter.Settsne_Perplexity(property.get());
        return true;
    }
    DoubleProperty property{"tsne_Perplexity", "Perplexity", 30.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper24 {
    bool set(ttkDimensionReduction& filter) {
        filter.Settsne_Exaggeration(property.get());
        return true;
    }
    DoubleProperty property{"tsne_Exaggeration", "Early Exaggeration", 12.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper25 {
    bool set(ttkDimensionReduction& filter) {
        filter.Settsne_LearningRate(property.get());
        return true;
    }
    DoubleProperty property{"tsne_LearningRate", "Learning Rate", 200.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper26 {
    bool set(ttkDimensionReduction& filter) {
        filter.Settsne_MaxIteration(property.get());
        return true;
    }
    IntProperty property{"tsne_MaxIteration", "Iteration Threshold", 1000,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper27 {
    bool set(ttkDimensionReduction& filter) {
        filter.Settsne_MaxIterationProgress(property.get());
        return true;
    }
    IntProperty property{"tsne_MaxIterationProgress", "Iteration Without Progress Threshold", 300,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper28 {
    bool set(ttkDimensionReduction& filter) {
        filter.Settsne_GradientThreshold(property.get());
        return true;
    }
    DoubleProperty property{"tsne_GradientThreshold", "Gradient Threshold", 1e-07,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper29 {
    bool set(ttkDimensionReduction& filter) {
        filter.Settsne_Metric(property.get().c_str());
        return true;
    }
    StringProperty property{"tsne_Metric", "Metric", "euclidean"};
};

struct Wrapper30 {
    bool set(ttkDimensionReduction& filter) {
        filter.Settsne_Init(property.get().c_str());
        return true;
    }
    StringProperty property{"tsne_Init", "Init", "random"};
};

struct Wrapper31 {
    bool set(ttkDimensionReduction& filter) {
        filter.Settsne_Verbose(property.get());
        return true;
    }
    IntProperty property{"tsne_Verbose", "Verbosity Level", 0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper32 {
    bool set(ttkDimensionReduction& filter) {
        filter.Settsne_Method(property.get().c_str());
        return true;
    }
    StringProperty property{"tsne_Method", "Method", "barnes_hut"};
};

struct Wrapper33 {
    bool set(ttkDimensionReduction& filter) {
        filter.Settsne_Angle(property.get());
        return true;
    }
    DoubleProperty property{"tsne_Angle", "Angle", 0.5, std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper34 {
    bool set(ttkDimensionReduction& filter) {
        filter.Setiso_EigenSolver(property.get().c_str());
        return true;
    }
    StringProperty property{"iso_EigenSolver", "Eigen Solver", "auto"};
};

struct Wrapper35 {
    bool set(ttkDimensionReduction& filter) {
        filter.Setiso_Tolerance(property.get());
        return true;
    }
    DoubleProperty property{"iso_Tolerance", "Tolerance", 0.001,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper36 {
    bool set(ttkDimensionReduction& filter) {
        filter.Setiso_MaxIteration(property.get());
        return true;
    }
    IntProperty property{"iso_MaxIteration", "Iteration threshold", 300,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper37 {
    bool set(ttkDimensionReduction& filter) {
        filter.Setiso_PathMethod(property.get().c_str());
        return true;
    }
    StringProperty property{"iso_PathMethod", "Path method", "auto"};
};

struct Wrapper38 {
    bool set(ttkDimensionReduction& filter) {
        filter.Setiso_NeighborsAlgorithm(property.get().c_str());
        return true;
    }
    StringProperty property{"iso_NeighborsAlgorithm", "NN-algorithm", "auto"};
};

struct Wrapper39 {
    bool set(ttkDimensionReduction& filter) {
        filter.Setpca_Copy(property.get());
        return true;
    }
    BoolProperty property{"pca_Copy", "Copy", true};
};

struct Wrapper40 {
    bool set(ttkDimensionReduction& filter) {
        filter.Setpca_Whiten(property.get());
        return true;
    }
    BoolProperty property{"pca_Whiten", "Whiten", false};
};

struct Wrapper41 {
    bool set(ttkDimensionReduction& filter) {
        filter.Setpca_SVDSolver(property.get().c_str());
        return true;
    }
    StringProperty property{"pca_SVDSolver", "SVD Solver", "auto"};
};

struct Wrapper42 {
    bool set(ttkDimensionReduction& filter) {
        filter.Setpca_Tolerance(property.get());
        return true;
    }
    DoubleProperty property{"pca_Tolerance", "Tolerance", 0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper43 {
    bool set(ttkDimensionReduction& filter) {
        filter.Setpca_MaxIteration(property.get().c_str());
        return true;
    }
    StringProperty property{"pca_MaxIteration", "Iteration Threshold", "auto"};
};

struct Wrapper44 {
    bool set(ttkDimensionReduction& filter) {
        if (property.get().empty()) return false;
        filter.SetModulePath(property.get().c_str());
        return true;
    }
    FileProperty property{"ModulePath", "Module Path", "default"};
};

struct Wrapper45 {
    bool set(ttkDimensionReduction& filter) {
        filter.SetModuleName(property.get().c_str());
        return true;
    }
    StringProperty property{"ModuleName", "Module Name", "dimensionReduction"};
};

struct Wrapper46 {
    bool set(ttkDimensionReduction& filter) {
        filter.SetFunctionName(property.get().c_str());
        return true;
    }
    StringProperty property{"FunctionName", "Function Name", "doIt"};
};

struct Wrapper47 {
    bool set(ttkDimensionReduction& filter) {
        filter.SetIsDeterministic(property.get());
        return true;
    }
    BoolProperty property{"IsDeterministic", "Is Deterministic", true};
};

struct Wrapper48 {
    bool set(ttkDimensionReduction& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", true};
};

struct Wrapper49 {
    bool set(ttkDimensionReduction& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber", "Thread Number", 1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper50 {
    bool set(ttkDimensionReduction& filter) {
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
struct TTKTraits<ttkDimensionReduction> {
    static constexpr std::string_view identifier = "ttkDimensionReduction";
    static constexpr std::string_view displayName = "TTK DimensionReduction";
    inline static std::array<InputData, 1> inports = {InputData{"Input", "vtkTable", 1}};
    inline static std::array<OutputData, 0> outports = {};
    inline static std::array<Group, 9> groups = {
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel", "Debug_Execute",
               "ModulePath", "ModuleName", "FunctionName", "IsDeterministic"}},
        Group{"Input options", {"SelectFieldsWithRegexp", "ScalarFields", "Regexp"}},
        Group{"Output options",
              {"Method", "NumberOfComponents", "NumberOfNeighbors", "KeepAllDataArrays",
               "InputDistanceMatrix"}},
        Group{"Spectral Embedding", {"se_Affinity", "se_Gamma", "se_EigenSolver"}},
        Group{"Locally Linear Embedding",
              {"lle_Regularization", "lle_EigenSolver", "lle_Tolerance", "lle_MaxIteration",
               "lle_Method", "lle_HessianTolerance", "lle_ModifiedTolerance",
               "lle_NeighborsAlgorithm"}},
        Group{"Multi-Dimensional Scaling",
              {"mds_Metric", "mds_Init", "mds_MaxIteration", "mds_Verbose", "mds_Epsilon"}},
        Group{"t-distributed Stochastic Neighbor Embedding",
              {"tsne_Perplexity", "tsne_Exaggeration", "tsne_LearningRate", "tsne_MaxIteration",
               "tsne_MaxIterationProgress", "tsne_GradientThreshold", "tsne_Metric", "tsne_Init",
               "tsne_Verbose", "tsne_Method", "tsne_Angle"}},
        Group{"Isomap Embedding",
              {"iso_EigenSolver", "iso_Tolerance", "iso_MaxIteration", "iso_PathMethod",
               "iso_NeighborsAlgorithm"}},
        Group{"Principal Component Analysis",
              {"pca_Copy", "pca_Whiten", "pca_SVDSolver", "pca_Tolerance", "pca_MaxIteration"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11, Wrapper12, Wrapper13, Wrapper14, Wrapper15,
               Wrapper16, Wrapper17, Wrapper18, Wrapper19, Wrapper20, Wrapper21, Wrapper22,
               Wrapper23, Wrapper24, Wrapper25, Wrapper26, Wrapper27, Wrapper28, Wrapper29,
               Wrapper30, Wrapper31, Wrapper32, Wrapper33, Wrapper34, Wrapper35, Wrapper36,
               Wrapper37, Wrapper38, Wrapper39, Wrapper40, Wrapper41, Wrapper42, Wrapper43,
               Wrapper44, Wrapper45, Wrapper46, Wrapper47, Wrapper48, Wrapper49, Wrapper50>
        properties;
};

void registerttkDimensionReduction(InviwoModule* module) {
    module->registerProcessor<TTKGenericProcessor<ttkDimensionReduction>>();
}

}  // namespace ttkwrapper
}  // namespace inviwo
