#include "ivw_ttkmergetreeautoencoder.h"
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
#include <ttkMergeTreeAutoencoder.h>
#include <warn/pop>

namespace inviwo {
namespace vtkwrapper {
namespace {

#include <warn/push>
#include <warn/ignore/conversion>

struct Wrapper0 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetNormalizedWasserstein(property.get());
        return true;
    }
    BoolProperty property{
        "NormalizedWasserstein", "Normalized Wasserstein",
        R"(Choose between the normalizd Wasserstein or the usual Wasserstein metric. If merge trees are provided in input, they will automatically be converted to persistence diagrams if this option is disabled.)"_help,
        true};
};

struct Wrapper1 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetNumberOfEncoderLayers(property.get());
        return true;
    }
    IntProperty property{
        "NumberOfEncoderLayers",
        "Number of Encoder Layers",
        R"(Number of layers in the encoder (without counting the latent space layer).)"_help,
        1,
        std::pair{0, ConstraintBehavior::Ignore},
        std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper2 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetScaleLayerAfterLatent(property.get());
        return true;
    }
    BoolProperty property{
        "ScaleLayerAfterLatent", "ScaleLayerAfterLatent",
        R"(By default, the latent layer and the layer after it will have the same number of axes. This option allows to set the number of axes in the layer after the latent one as the average with the layer after it.)"_help,
        false};
};

struct Wrapper3 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetInputNumberOfAxes(property.get());
        return true;
    }
    IntProperty property{"InputNumberOfAxes",
                         "Input Number of Axes",
                         R"(Number of axes in the input layer.)"_help,
                         16,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper4 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetInputOriginPrimeSizePercent(property.get());
        return true;
    }
    DoubleProperty property{
        "InputOriginPrimeSizePercent",
        "Input Origin Prime Size Percent",
        R"(Adjust the maximum size of the origin of the first output basis, given the total number of nodes in the input.)"_help,
        15.0,
        std::pair{0.0, ConstraintBehavior::Ignore},
        std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper5 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetNumberOfAxes(property.get());
        return true;
    }
    IntProperty property{"LatentSpaceNumberOfAxes",
                         "Latent Space Number of Axes",
                         R"(Number of axes in the latent space.)"_help,
                         2,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper6 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetLatentSpaceOriginPrimeSizePercent(property.get());
        return true;
    }
    DoubleProperty property{
        "LatentSpaceOriginPrimeSizePercent",
        "Latent Space Origin Prime Size Percent",
        R"(Adjust the maximum size of the origin of the latent output basis, as a percentage of the total number of nodes in the input.)"_help,
        10.0,
        std::pair{0.0, ConstraintBehavior::Ignore},
        std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper7 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetBarycenterSizeLimitPercent(property.get());
        return true;
    }
    DoubleProperty property{
        "BarycenterSizeLimitPercent",
        "Barycenter Size Limit Percent",
        R"(Adjust the maximum size of the origins of the input bases, as a percentage of the total number of nodes in the input.)"_help,
        20.0,
        std::pair{0.0, ConstraintBehavior::Ignore},
        std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper8 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetMinIteration(property.get());
        return true;
    }
    IntProperty property{"MinIteration",
                         "Min Iteration",
                         R"(Minimum number of iterations during the optimization.)"_help,
                         0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper9 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetMaxIteration(property.get());
        return true;
    }
    IntProperty property{"MaxIteration",
                         "Max Iteration",
                         R"(Maximum number of iterations during the optimization.)"_help,
                         0,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper10 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetIterationGap(property.get());
        return true;
    }
    IntProperty property{
        "IterationGap",
        "Iteration Gap",
        R"(Choose the number of iterations between two displays of information in the console.)"_help,
        100,
        std::pair{0, ConstraintBehavior::Ignore},
        std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper11 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetBatchSize(property.get());
        return true;
    }
    DoubleProperty property{
        "BatchSize",
        "Batch Size",
        R"(Determine the batch size, proportion of the ensemble size (between 0 and 1).)"_help,
        1.0,
        std::pair{0.0, ConstraintBehavior::Ignore},
        std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper12 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetOptimizer(property.get());
        return true;
    }
    OptionPropertyInt property{"Optimizer",
                               "Optimizer",
                               R"(Choose the optimizer method.)"_help,
                               {{"Adam", "Adam", 0}, {"SGD", "SGD", 1}, {"RMSprop", "RMSprop", 2}},
                               0};
};

struct Wrapper13 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetGradientStepSize(property.get());
        return true;
    }
    DoubleProperty property{"GradientStepSize",
                            "Gradient Step Size",
                            R"(Choose the step use for gradient descent.)"_help,
                            0.1,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper14 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetBeta1(property.get());
        return true;
    }
    DoubleProperty property{
        "Beta1",
        "Beta1",
        R"(Coefficients used for computing running averages of gradient and its square.)"_help,
        0.9,
        std::pair{0.0, ConstraintBehavior::Ignore},
        std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper15 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetBeta2(property.get());
        return true;
    }
    DoubleProperty property{
        "Beta2",
        "Beta2",
        R"(Coefficients used for computing running averages of gradient and its square.)"_help,
        0.999,
        std::pair{0.0, ConstraintBehavior::Ignore},
        std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper16 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetReconstructionLossWeight(property.get());
        return true;
    }
    DoubleProperty property{"ReconstructionLossWeight",
                            "ReconstructionLossWeight",
                            R"(Weight of the reconstruction loss in the optimization.)"_help,
                            1.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper17 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetTrackingLossWeight(property.get());
        return true;
    }
    DoubleProperty property{
        "TrackingLossWeight",
        "TrackingLossWeight",
        R"(Weight of the tracking loss in the optimization. It allows to minimize the distance between the output origins of two consecutive layers to facilitate the tracking of persistence pairs through the network. A non-zero value will trigger a specific initialization consisting in copying the output origin of the previous layer, to enable this option without using the loss, a low value like 1e-6 can be used.)"_help,
        0.0,
        std::pair{0.0, ConstraintBehavior::Ignore},
        std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper18 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetMetricLossWeight(property.get());
        return true;
    }
    DoubleProperty property{"MetricLossWeight",
                            "MetricLossWeight",
                            R"(Weight of the metric loss in the optimization.)"_help,
                            0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper19 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetClusteringLossWeight(property.get());
        return true;
    }
    DoubleProperty property{"ClusteringLossWeight",
                            "ClusteringLossWeight",
                            R"(Weight of the clustering loss in the optimization.)"_help,
                            0.0,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper20 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetClusteringLossTemperature(property.get());
        return true;
    }
    DoubleProperty property{
        "ClusteringLossTemperature",
        "ClusteringLossTemperature",
        R"(Temperature of the softmax in clustering loss in the optimization, it corresponds to the β parameter in Appendix D.2 of the reference paper. When this parameter tends to infinity the softmax will corresponds to argmax with hard classes, more this parameter is low more the paritition will be soft/fuzzy. In practice, more this parameter is low more the classes will be forced to be separated.)"_help,
        10.0,
        std::pair{0.0, ConstraintBehavior::Ignore},
        std::pair{100.0, ConstraintBehavior::Ignore}};
};

struct Wrapper21 : FieldSelection {
    bool set(ttkMergeTreeAutoencoder& filter) {
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
        CompositeProperty tmp{"ClusteringArrayName", "Clustering Array Name",
                              R"(Clustering array name.)"_help};
        tmp.addProperties(name, fieldAssociation);
        return tmp;
    }()};

    static constexpr std::string_view inport = "Info";
};

struct Wrapper22 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetCustomLossDynamicWeight(property.get());
        return true;
    }
    BoolProperty property{
        "CustomLossDynamicWeight", "CustomLossDynamicWeight",
        R"(Adapt the weight of the custom losses (tracking, metric, clustering) given the reconstruction loss.)"_help,
        false};
};

struct Wrapper23 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetCustomLossSpace(property.get());
        return true;
    }
    BoolProperty property{"CustomLossSpace", "CustomLossSpace",
                          R"(0 : latent coefficients ; 1 : latent PD/MT.)"_help, false};
};

struct Wrapper24 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetCustomLossActivate(property.get());
        return true;
    }
    BoolProperty property{"CustomLossActivate", "CustomLossActivate",
                          R"(Use activation function on the coefficient for custom losses.)"_help,
                          false};
};

struct Wrapper25 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetNormalizeMetricLoss(property.get());
        return true;
    }
    BoolProperty property{"NormalizeMetricLoss", "NormalizeMetricLoss",
                          R"(Normalize the metric loss by dividing by the maximum distance.)"_help,
                          true};
};

struct Wrapper26 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetNumberOfInit(property.get());
        return true;
    }
    IntProperty property{"NumberOfInit",
                         "Number of init.",
                         R"(Number of initialization steps, the best one will be kept.)"_help,
                         4,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper27 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetEuclideanVectorsInit(property.get());
        return true;
    }
    BoolProperty property{
        "EuclideanVectorsInit", "Euclidean Vectors Init",
        R"(Initialize with Euclidean distance instead of the Wasserstein one. It will be faster but less accurate.)"_help,
        false};
};

struct Wrapper28 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetInitOriginPrimeStructByCopy(property.get());
        return true;
    }
    BoolProperty property{
        "InitOriginPrimeStructByCopy", "InitOriginPrimeStructByCopy",
        R"(Initialize the tree structure of origins of the output bases by copying the structure of the origin of the corresponding input basis.)"_help,
        true};
};

struct Wrapper29 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetTrackingLossDecoding(property.get());
        return true;
    }
    BoolProperty property{"TrackingLossDecoding", "TrackingLossDecoding",
                          R"(Choose to use the tracking loss in the decoding layers.)"_help, false};
};

struct Wrapper30 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetTrackingLossInitRandomness(property.get());
        return true;
    }
    DoubleProperty property{
        "TrackingLossInitRandomness",
        "TrackingLossInitRandomness",
        R"(When the tracking loss is used, the origin of each output basis is the strict copy of the one in the input basis. This parameter allows to add randomess in this copy.)"_help,
        0.0,
        std::pair{0.0, ConstraintBehavior::Ignore},
        std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper31 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetNumberOfProjectionSteps(property.get());
        return true;
    }
    IntProperty property{
        "NumberOfProjectionSteps",
        "Number Of Projection Steps",
        R"(Number of steps to compute the projection of each input on a basis, it corresponds to the n_it parameter in Appendix C of the reference paper.)"_help,
        2,
        std::pair{0, ConstraintBehavior::Ignore},
        std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper32 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetDeterministic(property.get());
        return true;
    }
    BoolProperty property{"Deterministic", "Deterministic", R"()"_help, true};
};

struct Wrapper33 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetActivate(property.get());
        return true;
    }
    BoolProperty property{"Activate", "Activate", R"(Choose to use activation functions.)"_help,
                          true};
};

struct Wrapper34 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetActivationFunction(property.get());
        return true;
    }
    OptionPropertyInt property{"ActivationFunction",
                               "Activation Function",
                               R"(Choose the activation function to use.)"_help,
                               {{"ReLU", "ReLU", 0}, {"Leaky ReLU", "Leaky ReLU", 1}},
                               1};
};

struct Wrapper35 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetFullSymmetricAE(property.get());
        return true;
    }
    BoolProperty property{
        "FullSymmetricAE", "FullSymmetricAE",
        R"(Initialize the decoding layers symmetrically to the encoding layers.)"_help, false};
};

struct Wrapper36 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetActivateOutputInit(property.get());
        return true;
    }
    BoolProperty property{"ActivateOutputInit", "Activate Output Init",
                          R"(Use activation function in the initialization steps.)"_help, false};
};

struct Wrapper37 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetDiagramPairTypes(property.get());
        return true;
    }
    OptionPropertyInt property{
        "DiagramPairTypes",
        "Diagram Pair Types",
        R"(The types of pairs to use when the input are persistence diagrams. This can be weigthed with the "Pair Type Mixture Coefficient" parameter (0 for only the second type of pairs and 1 for the first type of pairs).)"_help,
        {{"Min-Saddle and Saddle-Max", "Min-Saddle and Saddle-Max", 0},
         {"Min-Saddle and Saddle-Saddle", "Min-Saddle and Saddle-Saddle", 1},
         {"Saddle-Saddle Saddle-Max", "Saddle-Saddle Saddle-Max", 2}},
        0};
};

struct Wrapper38 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetJoinSplitMixtureCoefficient(property.get());
        return true;
    }
    DoubleProperty property{
        "JoinSplitMixtureCoefficient",
        "Pair Type Mixture Coefficient",
        R"(If input are merge trees, this parameter allows to weight between the first input and the second input, typically join and split trees (0 for only the second input and 1 for only the first one).
If input are persistence diagrams, this parameter allows to weight between different types of pairs, determined by the "Diagram Pair Types" parameter (default is min-sad and sad-max pairs), 0 for only the second type and 1 for only the first type.)"_help,
        0.5,
        std::pair{0.0, ConstraintBehavior::Ignore},
        std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper39 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetNodePerTask(property.get());
        return true;
    }
    IntProperty property{"NodePerTask",
                         "NodePerTask",
                         R"()"_help,
                         32,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{100, ConstraintBehavior::Ignore}};
};

struct Wrapper40 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetCreateOutput(property.get());
        return true;
    }
    BoolProperty property{
        "CreateOutput", "CreateOutput",
        R"(This parameter will trigger the creation of the vtk output. It can be useful to disable this option when running many execution in python for which the only information needed is printed in the console.)"_help,
        true};
};

struct Wrapper41 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetUseAllCores(property.get());
        return true;
    }
    BoolProperty property{"Debug_UseAllCores", "Use All Cores", R"(Use all available cores.)"_help,
                          true};
};

struct Wrapper42 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetThreadNumber(property.get());
        return true;
    }
    IntProperty property{"Debug_ThreadNumber",
                         "Thread Number",
                         R"(The maximum number of threads.)"_help,
                         1,
                         std::pair{1, ConstraintBehavior::Ignore},
                         std::pair{256, ConstraintBehavior::Ignore}};
};

struct Wrapper43 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetDebugLevel(property.get());
        return true;
    }
    IntProperty property{"Debug_DebugLevel",
                         "Debug Level",
                         R"(Debug level.)"_help,
                         3,
                         std::pair{0, ConstraintBehavior::Ignore},
                         std::pair{5, ConstraintBehavior::Ignore}};
};

struct Wrapper44 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.SetCompactTriangulationCacheSize(property.get());
        return true;
    }
    DoubleProperty property{"CompactTriangulationCacheSize",
                            "Cache",
                            R"(Set the cache size for the compact triangulation as a
ratio with respect to the total cluster number.)"_help,
                            0.2,
                            std::pair{0.0, ConstraintBehavior::Ignore},
                            std::pair{1.0, ConstraintBehavior::Ignore}};
};

struct Wrapper45 {
    bool set(ttkMergeTreeAutoencoder& filter) {
        filter.Modified();
        return true;
    }
    ButtonProperty property{"Debug_Execute", "Execute",
                            R"(Executes the filter with the last applied parameters, which is
handy to re-start pipeline execution from a specific element
without changing parameters.)"_help};
};

#include <warn/pop>

}  // namespace
template <>
struct VTKTraits<ttkMergeTreeAutoencoder> {
    static constexpr std::string_view uri = "ttk.ttkMergeTreeAutoencoder";
    static constexpr std::string_view className = "ttkMergeTreeAutoencoder";
    static constexpr std::string_view identifier = "ttkMergeTreeAutoencoder";
    static constexpr std::string_view displayName = "TTK MergeTreeAutoencoder";
    static constexpr std::string_view category = "topology";
    static constexpr std::string_view tags = "TTK";
    inline static std::array<InputData, 3> inports = {
        InputData{"Input", "vtkMultiBlockDataSet", 1,
                  R"(Merge trees or persistence diagrams to process.)"},
        InputData{
            "Optional Input", "vtkMultiBlockDataSet", 1,
            R"(If input are merge trees, then this input can be used to process join and split trees together. Pass as input either join or split trees in the first input and the other type of trees in the second input.
If input are persistence diagrams, then this has no effect to use this input.)"},
        InputData{"Info", "vtkTable", 1,
                  R"(Additionnal information such as cluster assignment for cluster loss.)"}};
    inline static std::array<OutputData, 4> outports = {
        OutputData{"port0", "Origins", 0}, OutputData{"port1", "Axes Vectors", 1},
        OutputData{"port2", "Coefficients", 2}, OutputData{"port3", "Data", 3}};
    inline static std::array<Group, 5> groups = {
        Group{"Input options",
              {"NormalizedWasserstein", "Deterministic", "DiagramPairTypes",
               "JoinSplitMixtureCoefficient"}},
        Group{"Architecture options",
              {"NumberOfEncoderLayers", "ScaleLayerAfterLatent", "InputNumberOfAxes",
               "InputOriginPrimeSizePercent", "LatentSpaceNumberOfAxes",
               "LatentSpaceOriginPrimeSizePercent", "BarycenterSizeLimitPercent", "Activate",
               "ActivationFunction", "FullSymmetricAE", "ActivateOutputInit"}},
        Group{"Optimization options",
              {"MinIteration",
               "MaxIteration",
               "IterationGap",
               "BatchSize",
               "Optimizer",
               "GradientStepSize",
               "Beta1",
               "Beta2",
               "ReconstructionLossWeight",
               "TrackingLossWeight",
               "MetricLossWeight",
               "ClusteringLossWeight",
               "ClusteringLossTemperature",
               "ClusteringArrayName",
               "CustomLossDynamicWeight",
               "CustomLossSpace",
               "CustomLossActivate",
               "NormalizeMetricLoss",
               "NumberOfInit",
               "EuclideanVectorsInit",
               "InitOriginPrimeStructByCopy",
               "TrackingLossDecoding",
               "TrackingLossInitRandomness",
               "NumberOfProjectionSteps",
               "NodePerTask"}},
        Group{"Output options", {"CreateOutput"}},
        Group{"Testing",
              {"Debug_UseAllCores", "Debug_ThreadNumber", "Debug_DebugLevel",
               "CompactTriangulationCacheSize", "Debug_Execute"}}};
    std::tuple<Wrapper0, Wrapper1, Wrapper2, Wrapper3, Wrapper4, Wrapper5, Wrapper6, Wrapper7,
               Wrapper8, Wrapper9, Wrapper10, Wrapper11, Wrapper12, Wrapper13, Wrapper14, Wrapper15,
               Wrapper16, Wrapper17, Wrapper18, Wrapper19, Wrapper20, Wrapper21, Wrapper22,
               Wrapper23, Wrapper24, Wrapper25, Wrapper26, Wrapper27, Wrapper28, Wrapper29,
               Wrapper30, Wrapper31, Wrapper32, Wrapper33, Wrapper34, Wrapper35, Wrapper36,
               Wrapper37, Wrapper38, Wrapper39, Wrapper40, Wrapper41, Wrapper42, Wrapper43,
               Wrapper44, Wrapper45>
        properties;
    ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
    static constexpr std::string_view doc =
        R"ivw(This VTK filter uses the ttk::MergeTreeAutoencoder module to compute an auto-encoder of merge trees or persistence diagrams.

Related Publication:
"Wasserstein Auto-Encoders of Merge Trees (and Persistence Diagrams)"
Mathieu Pont,  Julien Tierny.
IEEE Transactions on Visualization and Computer Graphics, 2023

Online examples:

- https://topology-tool-kit.github.io/examples/mergeTreeWAE/

- https://topology-tool-kit.github.io/examples/persistenceDiagramWAE/)ivw";
};

void registerttkMergeTreeAutoencoder(InviwoModule* module) {
    module->registerProcessor<VTKGenericProcessor<ttkMergeTreeAutoencoder>>();
}

}  // namespace vtkwrapper
}  // namespace inviwo
