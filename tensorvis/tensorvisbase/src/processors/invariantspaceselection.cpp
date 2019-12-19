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
const ProcessorInfo InvariantSpaceSelection::getProcessorInfo() const { return processorInfo_; }

InvariantSpaceSelection::InvariantSpaceSelection()
    : Processor()
    , tensorFieldInport_("tensorFieldInport")
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

        /*if (sigma1_.get()) {
            selectedIndices.insert(MajorEigenValues::id());
        }
        if (sigma2_.get()) {
            selectedIndices.insert(IntermediateEigenValues::id());
        }
        if (sigma3_.get()) {
            selectedIndices.insert(MinorEigenValues::id());
        }
        if (i1_.get()) {
            selectedIndices.insert(I1::id());
        }
        if (i2_.get()) {
            selectedIndices.insert(I2::id());
        }
        if (i3_.get()) {
            selectedIndices.insert(I3::id());
        }
        if (j1_.get()) {
            selectedIndices.insert(J1::id());
        }
        if (j2_.get()) {
            selectedIndices.insert(J2::id());
        }
        if (j3_.get()) {
            selectedIndices.insert(J3::id());
        }
        if (lodeAngle_.get()) {
            selectedIndices.insert(LodeAngle::id());
        }
        if (anisotropy_.get()) {
            selectedIndices.insert(Anisotropy::id());
        }
        if (linearAnisotropy_.get()) {
            selectedIndices.insert(LinearAnisotropy::id());
        }
        if (planarAnisotropy_.get()) {
            selectedIndices.insert(PlanarAnisotropy::id());
        }
        if (sphericalAnisotropy_.get()) {
            selectedIndices.insert(SphericalAnisotropy::id());
        }
        if (diffusivity_.get()) {
            selectedIndices.insert(Diffusivity::id());
        }
        if (shearStress_.get()) {
            selectedIndices.insert(ShearStress::id());
        }
        if (pureShear_.get()) {
            selectedIndices.insert(PureShear::id());
        }
        if (shapeFactor_.get()) {
            selectedIndices.insert(ShapeFactor::id());
        }
        if (isotropicScaling_.get()) {
            selectedIndices.insert(IsotropicScaling::id());
        }
        if (rotation_.get()) {
            selectedIndices.insert(Rotation::id());
        }
        if (hill_.get()) {
            selectedIndices.insert(HillYieldCriterion::id());
        }*/
    });
}

void InvariantSpaceSelection::process() {
    /*if (!tensorFieldInport_.hasData()) return;

    auto invariantSpace = std::make_shared<InvariantSpace>();

    auto tensorField = tensorFieldInport_.getData();

    if (sigma1_.get()) {
        if (tensorField->hasMetaData<MajorEigenValues>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<MajorEigenValues>());
        else
            LogWarn(
                "Requested meta data MajorEigenValues not available. Consider adding a meta data "
                "processor.")
    }
    if (sigma2_.get()) {
        if (tensorField->hasMetaData<IntermediateEigenValues>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<IntermediateEigenValues>());
        else
            LogWarn(
                "Requested meta data IntermediateEigenValues not available. Consider adding a meta "
                "data processor.")
    }
    if (sigma3_.get()) {
        if (tensorField->hasMetaData<MinorEigenValues>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<MinorEigenValues>());
        else
            LogWarn(
                "Requested meta data MinorEigenValues not available. Consider adding a meta data "
                "processor.")
    }
    if (i1_.get()) {
        if (tensorField->hasMetaData<I1>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<I1>());
        else
            LogWarn("Requested meta data I1 not available. Consider adding a meta data processor.")
    }
    if (i2_.get()) {
        if (tensorField->hasMetaData<I2>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<I2>());
        else
            LogWarn("Requested meta data I2 not available. Consider adding a meta data processor.")
    }
    if (i3_.get()) {
        if (tensorField->hasMetaData<I3>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<I3>());
        else
            LogWarn("Requested meta data I3 not available. Consider adding a meta data processor.")
    }
    if (j1_.get()) {
        if (tensorField->hasMetaData<J1>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<J1>());
        else
            LogWarn("Requested meta data J1 not available. Consider adding a meta data processor.")
    }
    if (j2_.get()) {
        if (tensorField->hasMetaData<J2>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<J2>());
        else
            LogWarn("Requested meta data J2 not available. Consider adding a meta data processor.")
    }
    if (j3_.get()) {
        if (tensorField->hasMetaData<J3>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<J3>());
        else
            LogWarn("Requested meta data J3 not available. Consider adding a meta data processor.")
    }
    if (lodeAngle_.get()) {
        if (tensorField->hasMetaData<LodeAngle>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<LodeAngle>(),
                                    lodeAngle_.getDisplayName());
        else
            LogWarn(
                "Requested meta data LodeAngle not available. Consider adding a meta data "
                "processor.")
    }
    if (anisotropy_.get()) {
        if (tensorField->hasMetaData<Anisotropy>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<Anisotropy>(),
                                    anisotropy_.getDisplayName());
        else
            LogWarn(
                "Requested meta data Anisotropy not available. Consider adding a meta data "
                "processor.")
    }
    if (linearAnisotropy_.get()) {
        if (tensorField->hasMetaData<LinearAnisotropy>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<LinearAnisotropy>());
        else
            LogWarn(
                "Requested meta data LinearAnisotropy not available. Consider adding a meta data "
                "processor.")
    }
    if (planarAnisotropy_.get()) {
        if (tensorField->hasMetaData<PlanarAnisotropy>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<PlanarAnisotropy>());
        else
            LogWarn(
                "Requested meta data PlanarAnisotropy not available. Consider adding a meta data "
                "processor.")
    }
    if (sphericalAnisotropy_.get()) {
        if (tensorField->hasMetaData<SphericalAnisotropy>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<SphericalAnisotropy>());
        else
            LogWarn(
                "Requested meta data SphericalAnisotropy not available. Consider adding a meta "
                "data processor.")
    }
    if (diffusivity_.get()) {
        if (tensorField->hasMetaData<Diffusivity>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<Diffusivity>());
        else
            LogWarn(
                "Requested meta data Diffusivity not available. Consider adding a meta data "
                "processor.")
    }
    if (shearStress_.get()) {
        if (tensorField->hasMetaData<ShearStress>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<ShearStress>());
        else
            LogWarn(
                "Requested meta data ShearStress not available. Consider adding a meta data "
                "processor.")
    }
    if (pureShear_.get()) {
        if (tensorField->hasMetaData<PureShear>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<PureShear>());
        else
            LogWarn(
                "Requested meta data PureShear not available. Consider adding a meta data "
                "processor.")
    }
    if (shapeFactor_.get()) {
        if (tensorField->hasMetaData<ShapeFactor>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<ShapeFactor>(),
                                    shapeFactor_.getDisplayName());
        else
            LogWarn(
                "Requested meta data ShapeFactor not available. Consider adding a meta data "
                "processor.")
    }
    if (isotropicScaling_.get()) {
        if (tensorField->hasMetaData<IsotropicScaling>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<IsotropicScaling>(),
                                    isotropicScaling_.getDisplayName());
        else
            LogWarn(
                "Requested meta data IsotropicScaling not available. Consider adding a meta data "
                "processor.")
    }
    if (rotation_.get()) {
        if (tensorField->hasMetaData<Rotation>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<Rotation>(),
                                    rotation_.getDisplayName());
        else
            LogWarn(
                "Requested meta data Rotation not available. Consider adding a meta data "
                "processor.")
    }
    if (hill_.get()) {
        if (tensorField->hasMetaData<HillYieldCriterion>())
            invariantSpace->addAxis(tensorField->getMetaDataContainer<HillYieldCriterion>(),
                                    hill_.getDisplayName());
        else
            LogWarn(
                "Requested meta data HillYieldCriterion not available. Consider adding a meta data "
                "processor.")
    }

    outport_.setData(invariantSpace);*/
}

}  // namespace inviwo
