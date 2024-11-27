#include <inviwo/tensorvisbase/processors/invariantspaceselection.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo InvariantSpaceSelection::processorInfo_{
    "org.inviwo.InvariantSpaceSelection",  // Class identifier
    "Invariant Space Selection",           // Display name
    "Tensor",                              // Category
    CodeState::Experimental,               // Code state
    Tags::CPU,                             // Tags
};
const ProcessorInfo& InvariantSpaceSelection::getProcessorInfo() const { return processorInfo_; }

InvariantSpaceSelection::InvariantSpaceSelection()
    : Processor()
    , tensorFieldInport_("tensorFieldInport")
    , outport_("outport")
    , invariantSpaceAxes_("invariantSpaceAxes_", "Invariant space axes")
    , sigma1_("sigma1", tensorutil::sigma1_str, true)
    , sigma2_("sigma2", tensorutil::sigma2_str, true)
    , sigma3_("sigma3", tensorutil::sigma3_str, true)
    , i1_("i1", "I1", false)
    , i2_("i2", "I2", false)
    , i3_("i3", "I3", false)
    , j1_("j1", "J1", false)
    , j2_("j2", "J2", false)
    , j3_("j3", "J3", false)
    , lodeAngle_("lodeAngle", "Lode angle", false)
    , anisotropy_("anisotropy", "Anisotropy", false)
    , linearAnisotropy_("linearAnisotropy", "Linear anisotropy", false)
    , planarAnisotropy_("planarAnisotropy", "Planar anisotropy", false)
    , sphericalAnisotropy_("sphericalAnisotropy", "Spherical anisotropy", false)
    , diffusivity_("diffusivity", "Diffusivity", false)
    , shearStress_("shearStress", "Shear stress", false)
    , pureShear_("pureShear", "Pure shear", false)
    , shapeFactor_("shapeFactor", "Shape-Factor", false)
    , isotropicScaling_("isotropicScaling", "Isotropic scaling", false)
    , rotation_("rotation", "Rotation", false)
    , hill_("hill", "Hill", false) {
    addPort(tensorFieldInport_);
    addPort(outport_);

    invariantSpaceAxes_.addProperty(sigma1_);
    invariantSpaceAxes_.addProperty(sigma2_);
    invariantSpaceAxes_.addProperty(sigma3_);
    invariantSpaceAxes_.addProperty(i1_);
    invariantSpaceAxes_.addProperty(i2_);
    invariantSpaceAxes_.addProperty(i3_);
    invariantSpaceAxes_.addProperty(j1_);
    invariantSpaceAxes_.addProperty(j2_);
    invariantSpaceAxes_.addProperty(j3_);
    invariantSpaceAxes_.addProperty(lodeAngle_);
    invariantSpaceAxes_.addProperty(anisotropy_);
    invariantSpaceAxes_.addProperty(linearAnisotropy_);
    invariantSpaceAxes_.addProperty(planarAnisotropy_);
    invariantSpaceAxes_.addProperty(sphericalAnisotropy_);
    invariantSpaceAxes_.addProperty(diffusivity_);
    invariantSpaceAxes_.addProperty(shearStress_);
    invariantSpaceAxes_.addProperty(pureShear_);
    invariantSpaceAxes_.addProperty(shapeFactor_);
    invariantSpaceAxes_.addProperty(isotropicScaling_);
    invariantSpaceAxes_.addProperty(rotation_);
    invariantSpaceAxes_.addProperty(hill_);

    addProperty(invariantSpaceAxes_);

    invariantSpaceAxes_.onChange([this]() {
        std::unordered_set<size_t> selectedIndices;

        if (sigma1_.get()) {
            selectedIndices.insert(tensor::MajorEigenValues::id());
        }
        if (sigma2_.get()) {
            selectedIndices.insert(tensor::IntermediateEigenValues::id());
        }
        if (sigma3_.get()) {
            selectedIndices.insert(tensor::MinorEigenValues::id());
        }
        if (i1_.get()) {
            selectedIndices.insert(tensor::I1::id());
        }
        if (i2_.get()) {
            selectedIndices.insert(tensor::I2::id());
        }
        if (i3_.get()) {
            selectedIndices.insert(tensor::I3::id());
        }
        if (j1_.get()) {
            selectedIndices.insert(tensor::J1::id());
        }
        if (j2_.get()) {
            selectedIndices.insert(tensor::J2::id());
        }
        if (j3_.get()) {
            selectedIndices.insert(tensor::J3::id());
        }
        if (lodeAngle_.get()) {
            selectedIndices.insert(tensor::LodeAngle::id());
        }
        if (anisotropy_.get()) {
            selectedIndices.insert(tensor::Anisotropy::id());
        }
        if (linearAnisotropy_.get()) {
            selectedIndices.insert(tensor::LinearAnisotropy::id());
        }
        if (planarAnisotropy_.get()) {
            selectedIndices.insert(tensor::PlanarAnisotropy::id());
        }
        if (sphericalAnisotropy_.get()) {
            selectedIndices.insert(tensor::SphericalAnisotropy::id());
        }
        if (diffusivity_.get()) {
            selectedIndices.insert(tensor::Diffusivity::id());
        }
        if (shearStress_.get()) {
            selectedIndices.insert(tensor::ShearStress::id());
        }
        if (pureShear_.get()) {
            selectedIndices.insert(tensor::PureShear::id());
        }
        if (shapeFactor_.get()) {
            selectedIndices.insert(tensor::ShapeFactor::id());
        }
        if (isotropicScaling_.get()) {
            selectedIndices.insert(tensor::IsotropicScaling::id());
        }
        if (rotation_.get()) {
            selectedIndices.insert(tensor::Rotation::id());
        }
        if (hill_.get()) {
            selectedIndices.insert(tensor::HillYieldCriterion::id());
        }
    });
}

void InvariantSpaceSelection::process() {
    if (!tensorFieldInport_.hasData()) return;

    auto invariantSpace = std::make_shared<InvariantSpace>();

    auto tensorField = tensorFieldInport_.getData();

    if (sigma1_.get()) {
        if (tensorField->hasMetaData<tensor::MajorEigenValues>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<tensor::MajorEigenValues>());
        else
            LogWarn(
                "Requested meta data MajorEigenValues not available. Consider adding a meta data "
                "processor.")
    }
    if (sigma2_.get()) {
        if (tensorField->hasMetaData<tensor::IntermediateEigenValues>())
            invariantSpace->addAxis(
                tensorField->getMetaDataContainer<tensor::IntermediateEigenValues>());
        else
            LogWarn(
                "Requested meta data IntermediateEigenValues not available. Consider adding a meta "
                "data processor.")
    }
    if (sigma3_.get()) {
        if (tensorField->hasMetaData<tensor::MinorEigenValues>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<tensor::MinorEigenValues>());
        else
            LogWarn(
                "Requested meta data MinorEigenValues not available. Consider adding a meta data "
                "processor.")
    }
    if (i1_.get()) {
        if (tensorField->hasMetaData<tensor::I1>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<tensor::I1>());
        else
            LogWarn("Requested meta data I1 not available. Consider adding a meta data processor.")
    }
    if (i2_.get()) {
        if (tensorField->hasMetaData<tensor::I2>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<tensor::I2>());
        else
            LogWarn("Requested meta data I2 not available. Consider adding a meta data processor.")
    }
    if (i3_.get()) {
        if (tensorField->hasMetaData<tensor::I3>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<tensor::I3>());
        else
            LogWarn("Requested meta data I3 not available. Consider adding a meta data processor.")
    }
    if (j1_.get()) {
        if (tensorField->hasMetaData<tensor::J1>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<tensor::J1>());
        else
            LogWarn("Requested meta data J1 not available. Consider adding a meta data processor.")
    }
    if (j2_.get()) {
        if (tensorField->hasMetaData<tensor::J2>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<tensor::J2>());
        else
            LogWarn("Requested meta data J2 not available. Consider adding a meta data processor.")
    }
    if (j3_.get()) {
        if (tensorField->hasMetaData<tensor::J3>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<tensor::J3>());
        else
            LogWarn("Requested meta data J3 not available. Consider adding a meta data processor.")
    }
    if (lodeAngle_.get()) {
        if (tensorField->hasMetaData<tensor::LodeAngle>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<tensor::LodeAngle>(),
                                    lodeAngle_.getDisplayName());
        else
            LogWarn(
                "Requested meta data LodeAngle not available. Consider adding a meta data "
                "processor.")
    }
    if (anisotropy_.get()) {
        if (tensorField->hasMetaData<tensor::Anisotropy>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<tensor::Anisotropy>(),
                                    anisotropy_.getDisplayName());
        else
            LogWarn(
                "Requested meta data Anisotropy not available. Consider adding a meta data "
                "processor.")
    }
    if (linearAnisotropy_.get()) {
        if (tensorField->hasMetaData<tensor::LinearAnisotropy>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<tensor::LinearAnisotropy>());
        else
            LogWarn(
                "Requested meta data LinearAnisotropy not available. Consider adding a meta data "
                "processor.")
    }
    if (planarAnisotropy_.get()) {
        if (tensorField->hasMetaData<tensor::PlanarAnisotropy>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<tensor::PlanarAnisotropy>());
        else
            LogWarn(
                "Requested meta data PlanarAnisotropy not available. Consider adding a meta data "
                "processor.")
    }
    if (sphericalAnisotropy_.get()) {
        if (tensorField->hasMetaData<tensor::SphericalAnisotropy>())
            invariantSpace->addAxis(
                tensorField->getMetaDataContainer<tensor::SphericalAnisotropy>());
        else
            LogWarn(
                "Requested meta data SphericalAnisotropy not available. Consider adding a meta "
                "data processor.")
    }
    if (diffusivity_.get()) {
        if (tensorField->hasMetaData<tensor::Diffusivity>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<tensor::Diffusivity>());
        else
            LogWarn(
                "Requested meta data Diffusivity not available. Consider adding a meta data "
                "processor.")
    }
    if (shearStress_.get()) {
        if (tensorField->hasMetaData<tensor::ShearStress>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<tensor::ShearStress>());
        else
            LogWarn(
                "Requested meta data ShearStress not available. Consider adding a meta data "
                "processor.")
    }
    if (pureShear_.get()) {
        if (tensorField->hasMetaData<tensor::PureShear>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<tensor::PureShear>());
        else
            LogWarn(
                "Requested meta data PureShear not available. Consider adding a meta data "
                "processor.")
    }
    if (shapeFactor_.get()) {
        if (tensorField->hasMetaData<tensor::ShapeFactor>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<tensor::ShapeFactor>(),
                                    shapeFactor_.getDisplayName());
        else
            LogWarn(
                "Requested meta data ShapeFactor not available. Consider adding a meta data "
                "processor.")
    }
    if (isotropicScaling_.get()) {
        if (tensorField->hasMetaData<tensor::IsotropicScaling>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<tensor::IsotropicScaling>(),
                                    isotropicScaling_.getDisplayName());
        else
            LogWarn(
                "Requested meta data IsotropicScaling not available. Consider adding a meta data "
                "processor.")
    }
    if (rotation_.get()) {
        if (tensorField->hasMetaData<tensor::Rotation>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<tensor::Rotation>(),
                                    rotation_.getDisplayName());
        else
            LogWarn(
                "Requested meta data Rotation not available. Consider adding a meta data "
                "processor.")
    }
    if (hill_.get()) {
        if (tensorField->hasMetaData<tensor::HillYieldCriterion>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<tensor::HillYieldCriterion>(),
                                    hill_.getDisplayName());
        else
            LogWarn(
                "Requested meta data HillYieldCriterion not available. Consider adding a meta data "
                "processor.")
    }

    outport_.setData(invariantSpace);
}

}  // namespace inviwo
