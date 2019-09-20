/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2017-2018 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#include <modules/tensorvisbase/processors/tensorfield3dmetadata.h>
#include <modules/tensorvisbase/util/tensorutil.h>
#include <modules/tensorvisbase/datastructures/tensorfieldmetadata.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorField3DMetaData::processorInfo_{
    "org.inviwo.TensorField3DMetaData",  // Class identifier
    "Tensor Field 3D Meta Data",         // Display name
    "Tensor",                            // Category
    CodeState::Experimental,             // Code state
    Tags::CPU,                           // Tags
};
const ProcessorInfo TensorField3DMetaData::getProcessorInfo() const { return processorInfo_; }

TensorField3DMetaData::TensorField3DMetaData()
    : Processor()
    , inport_("inport")
    , outport_("outport")
    , sigma1_("sigma1", tensorutil::sigma1_str, true)
    , sigma2_("sigma2", tensorutil::sigma2_str, true)
    , sigma3_("sigma3", tensorutil::sigma3_str, true)
    , ev1_("ev1", "Major eigenvectors", true)
    , ev2_("ev2", "Intermediate eigenvectors", true)
    , ev3_("ev3", "Minor eigenvectors", true)
    , i1_("i1", "I1", false)
    , i2_("i2", "I2", false)
    , i3_("i3", "I3", false)
    , j1_("j1", "J1", false)
    , j2_("j2", "J2", false)
    , j3_("j3", "J3", false)
    , lodeAngle_("lodeAngle", "Lode angle", false)
    , anisotropy_("anisotropy", "Anisotropy", false)
    , linearAnisotropy_("linearAnisotropy", "Linear anisotropy", true)
    , planarAnisotropy_("planarAnisotropy", "Planar anisotropy", true)
    , sphericalAnisotropy_("sphericalAnisotropy", "Spherical anisotropy", true)
    , diffusivity_("diffusivity", "Diffusivity", true)
    , shearStress_("shearStress", "Shear stress", true)
    , pureShear_("pureShear", "Pure shear", true)
    , shapeFactor_("shapeFactor", "Shape-Factor", true)
    , isotropicScaling_("isotropicScaling", "Isotropic scaling", true)
    , rotation_("rotation", "Rotation", true)
    , frobeniusNorm_("frobeniusNorm", "Frobenius Norm", true)
    //    , hillYieldCriterion_("hillYieldCriterion", "Hill yield criterion", true)
    , selectAll_("selectAll", "Select all")
    , deselectAll_("deselectAll", "Deselect all")
    , tensorFieldOut_(nullptr) {

    addPort(inport_);
    addPort(outport_);

    addProperty(sigma1_);
    addProperty(sigma2_);
    addProperty(sigma3_);
    addProperty(ev1_);
    addProperty(ev2_);
    addProperty(ev3_);
    addProperty(i1_);
    addProperty(i2_);
    addProperty(i3_);
    addProperty(j1_);
    addProperty(j2_);
    addProperty(j3_);
    addProperty(lodeAngle_);
    addProperty(anisotropy_);
    addProperty(linearAnisotropy_);
    addProperty(planarAnisotropy_);
    addProperty(sphericalAnisotropy_);
    addProperty(diffusivity_);
    addProperty(shearStress_);
    addProperty(pureShear_);
    addProperty(shapeFactor_);
    addProperty(isotropicScaling_);
    addProperty(rotation_);
    addProperty(frobeniusNorm_);
    //    addProperty(hillYieldCriterion_);

    addProperty(selectAll_);
    addProperty(deselectAll_);

    sigma1_.setReadOnly(true);
    sigma2_.setReadOnly(true);
    sigma3_.setReadOnly(true);
    ev1_.setReadOnly(true);
    ev2_.setReadOnly(true);
    ev3_.setReadOnly(true);

    inport_.onChange([this]() { invalidate(InvalidationLevel::InvalidResources); });
    selectAll_.onChange([this]() { selectAll(); });
    deselectAll_.onChange([this]() { deselectAll(); });
}

void TensorField3DMetaData::initializeResources() {
    if (inport_.hasData()) {
        auto tensorField = inport_.getData();

        tensorFieldOut_ = tensorField->clone();

        sigma1_.set(tensorFieldOut_->hasMetaData<MajorEigenValues>());
        sigma2_.set(tensorFieldOut_->hasMetaData<IntermediateEigenValues>());
        sigma3_.set(tensorFieldOut_->hasMetaData<MinorEigenValues>());

        ev1_.set(tensorFieldOut_->hasMetaData<MajorEigenVectors>());
        ev2_.set(tensorFieldOut_->hasMetaData<IntermediateEigenVectors>());
        ev3_.set(tensorFieldOut_->hasMetaData<MinorEigenVectors>());

        i1_.set(tensorFieldOut_->hasMetaData<I1>());
        i2_.set(tensorFieldOut_->hasMetaData<I2>());
        i3_.set(tensorFieldOut_->hasMetaData<I3>());

        j1_.set(tensorFieldOut_->hasMetaData<J1>());
        j2_.set(tensorFieldOut_->hasMetaData<J2>());
        j3_.set(tensorFieldOut_->hasMetaData<J3>());

        lodeAngle_.set(tensorFieldOut_->hasMetaData<LodeAngle>());

        anisotropy_.set(tensorFieldOut_->hasMetaData<Anisotropy>());

        linearAnisotropy_.set(tensorFieldOut_->hasMetaData<LinearAnisotropy>());
        planarAnisotropy_.set(tensorFieldOut_->hasMetaData<PlanarAnisotropy>());
        sphericalAnisotropy_.set(tensorFieldOut_->hasMetaData<SphericalAnisotropy>());
        diffusivity_.set(tensorFieldOut_->hasMetaData<Diffusivity>());
        shearStress_.set(tensorFieldOut_->hasMetaData<ShearStress>());
        pureShear_.set(tensorFieldOut_->hasMetaData<PureShear>());
        shapeFactor_.set(tensorFieldOut_->hasMetaData<ShapeFactor>());
        isotropicScaling_.set(tensorFieldOut_->hasMetaData<IsotropicScaling>());
        rotation_.set(tensorFieldOut_->hasMetaData<Rotation>());
        frobeniusNorm_.set(tensorFieldOut_->hasMetaData<FrobeniusNorm>());
        //        hillYieldCriterion_.set(tensorFieldOut_->hasMetaData<HillYieldCriterion>());
    }
}

void TensorField3DMetaData::addMetaData() {
    const auto& tensors = tensorFieldOut_->tensors();

    if (i1_.get() && !tensorFieldOut_->hasMetaData<I1>()) {
        std::vector<double> i1;

        for (const auto& tensor : tensors) {
            i1.emplace_back(tensorutil::calculateI1(tensor));
        }

        tensorFieldOut_->addMetaData<I1>(i1, TensorFeature::I1);
    }
    if (i2_.get() && !tensorFieldOut_->hasMetaData<I2>()) {
        std::vector<double> i2;

        for (const auto& tensor : tensors) {
            i2.emplace_back(tensorutil::calculateI2(tensor));
        }

        tensorFieldOut_->addMetaData<I2>(i2, TensorFeature::I2);
    }
    if (i3_.get() && !tensorFieldOut_->hasMetaData<I3>()) {
        std::vector<double> i3;

        for (const auto& tensor : tensors) {
            i3.emplace_back(tensorutil::calculateI3(tensor));
        }

        tensorFieldOut_->addMetaData<I3>(i3, TensorFeature::I3);
    }
    if (j1_.get() && !tensorFieldOut_->hasMetaData<J1>()) {
        std::vector<double> j1;

        for (const auto& tensor : tensors) {
            j1.emplace_back(tensorutil::calculateJ1(tensor));
        }

        tensorFieldOut_->addMetaData<J1>(j1, TensorFeature::J1);
    }
    if (j2_.get() && !tensorFieldOut_->hasMetaData<J2>()) {
        std::vector<double> j2;

        for (const auto& tensor : tensors) {
            j2.emplace_back(tensorutil::calculateJ2(tensor));
        }

        tensorFieldOut_->addMetaData<J2>(j2, TensorFeature::J2);
    }
    if (j3_.get() && !tensorFieldOut_->hasMetaData<J3>()) {
        std::vector<double> j3;

        for (const auto& tensor : tensors) {
            j3.emplace_back(tensorutil::calculateJ3(tensor));
        }

        tensorFieldOut_->addMetaData<J3>(j3, TensorFeature::J3);
    }
    if (lodeAngle_.get() && !tensorFieldOut_->hasMetaData<LodeAngle>()) {
        std::vector<double> lodeAngle;

        for (const auto& tensor : tensors) {
            lodeAngle.emplace_back(tensorutil::calculateLodeAngle(tensor));
        }

        tensorFieldOut_->addMetaData<LodeAngle>(lodeAngle, TensorFeature::LodeAngle);
    }
    if (anisotropy_.get() && !tensorFieldOut_->hasMetaData<Anisotropy>()) {
        std::vector<double> anisotropy;
        const auto& majorEigenValues = tensorFieldOut_->getMetaData<MajorEigenValues>();
        const auto& minorEigenValues = tensorFieldOut_->getMetaData<MinorEigenValues>();

        for (size_t i = 0; i < tensors.size(); i++) {
            anisotropy.emplace_back(std::abs(majorEigenValues[i] - minorEigenValues[i]));
        }

        tensorFieldOut_->addMetaData<Anisotropy>(anisotropy, TensorFeature::Anisotropy);
    }
    if (linearAnisotropy_.get() && !tensorFieldOut_->hasMetaData<LinearAnisotropy>()) {
        std::vector<double> linearAnisotropy;

        const auto& majorEigenValues = tensorFieldOut_->getMetaData<MajorEigenValues>();
        const auto& intermediateEigenValues =
            tensorFieldOut_->getMetaData<IntermediateEigenValues>();
        const auto& minorEigenValues = tensorFieldOut_->getMetaData<MinorEigenValues>();

        for (size_t i = 0; i < majorEigenValues.size(); i++) {
            std::array<double, 3> eigenValues{majorEigenValues[i], intermediateEigenValues[i],
                                              minorEigenValues[i]};

            std::transform(eigenValues.begin(), eigenValues.end(), eigenValues.begin(),
                           [](const double& val) { return glm::abs(val); });

            std::sort(eigenValues.begin(), eigenValues.end(),
                      [](const double& valA, const double& valB) { return valA > valB; });

            auto denominator = eigenValues[0] + eigenValues[1] + eigenValues[2];
            if (denominator < std::numeric_limits<double>::epsilon())
                denominator = std::numeric_limits<double>::epsilon();

            linearAnisotropy.emplace_back((eigenValues[0] - eigenValues[1]) / denominator);
        }

        tensorFieldOut_->addMetaData<LinearAnisotropy>(linearAnisotropy,
                                                       TensorFeature::LinearAnisotropy);
    }
    if (planarAnisotropy_.get() && !tensorFieldOut_->hasMetaData<PlanarAnisotropy>()) {
        std::vector<double> planarAnisotropy;

        const auto& majorEigenValues = tensorFieldOut_->getMetaData<MajorEigenValues>();
        const auto& intermediateEigenValues =
            tensorFieldOut_->getMetaData<IntermediateEigenValues>();
        const auto& minorEigenValues = tensorFieldOut_->getMetaData<MinorEigenValues>();

        for (size_t i = 0; i < majorEigenValues.size(); i++) {
            std::array<double, 3> eigenValues{majorEigenValues[i], intermediateEigenValues[i],
                                              minorEigenValues[i]};

            std::transform(eigenValues.begin(), eigenValues.end(), eigenValues.begin(),
                           [](const double& val) { return glm::abs(val); });

            std::sort(eigenValues.begin(), eigenValues.end(),
                      [](const double& valA, const double& valB) { return valA > valB; });

            auto denominator = eigenValues[0] + eigenValues[1] + eigenValues[2];
            if (denominator < std::numeric_limits<double>::epsilon())
                denominator = std::numeric_limits<double>::epsilon();

            planarAnisotropy.emplace_back((2.0 * (eigenValues[1] - eigenValues[2])) / denominator);
        }

        tensorFieldOut_->addMetaData<PlanarAnisotropy>(planarAnisotropy,
                                                       TensorFeature::PlanarAnisotropy);
    }
    if (sphericalAnisotropy_.get() && !tensorFieldOut_->hasMetaData<SphericalAnisotropy>()) {
        std::vector<double> sphericalAnisotropy;

        const auto& majorEigenValues = tensorFieldOut_->getMetaData<MajorEigenValues>();
        const auto& intermediateEigenValues =
            tensorFieldOut_->getMetaData<IntermediateEigenValues>();
        const auto& minorEigenValues = tensorFieldOut_->getMetaData<MinorEigenValues>();

        for (size_t i = 0; i < majorEigenValues.size(); i++) {
            std::array<double, 3> eigenValues{majorEigenValues[i], intermediateEigenValues[i],
                                              minorEigenValues[i]};

            std::transform(eigenValues.begin(), eigenValues.end(), eigenValues.begin(),
                           [](const double& val) { return glm::abs(val); });

            std::sort(eigenValues.begin(), eigenValues.end(),
                      [](const double& valA, const double& valB) { return valA > valB; });

            auto denominator = eigenValues[0] + eigenValues[1] + eigenValues[2];
            if (denominator < std::numeric_limits<double>::epsilon())
                denominator = std::numeric_limits<double>::epsilon();

            sphericalAnisotropy.emplace_back((3.0 * eigenValues[2]) / denominator);
        }

        tensorFieldOut_->addMetaData<SphericalAnisotropy>(sphericalAnisotropy,
                                                          TensorFeature::SphericalAnisotropy);
    }
    if (diffusivity_.get() && !tensorFieldOut_->hasMetaData<Diffusivity>()) {
        std::vector<double> diffusivity;

        const auto& majorEigenValues = tensorFieldOut_->getMetaData<MajorEigenValues>();
        const auto& intermediateEigenValues =
            tensorFieldOut_->getMetaData<IntermediateEigenValues>();
        const auto& minorEigenValues = tensorFieldOut_->getMetaData<MinorEigenValues>();

        for (size_t i = 0; i < majorEigenValues.size(); i++) {
            std::array<double, 3> eigenValues{majorEigenValues[i], intermediateEigenValues[i],
                                              minorEigenValues[i]};

            std::transform(eigenValues.begin(), eigenValues.end(), eigenValues.begin(),
                           [](const double& val) { return glm::abs(val); });

            std::sort(eigenValues.begin(), eigenValues.end(),
                      [](const double& valA, const double& valB) { return valA > valB; });

            diffusivity.emplace_back(eigenValues[0] * eigenValues[0] +
                                     eigenValues[1] * eigenValues[1] +
                                     eigenValues[2] * eigenValues[2]);
        }

        tensorFieldOut_->addMetaData<Diffusivity>(diffusivity, TensorFeature::Diffusivity);
    }
    if (shearStress_.get() && !tensorFieldOut_->hasMetaData<ShearStress>()) {
        std::vector<double> shearStress;

        const auto& majorEigenValues = tensorFieldOut_->getMetaData<MajorEigenValues>();
        const auto& minorEigenValues = tensorFieldOut_->getMetaData<MinorEigenValues>();

        for (size_t i = 0; i < majorEigenValues.size(); i++) {
            shearStress.emplace_back((majorEigenValues[i] - minorEigenValues[i]) / 2.0);
        }

        tensorFieldOut_->addMetaData<ShearStress>(shearStress, TensorFeature::ShearStress);
    }
    if (pureShear_.get() && !tensorFieldOut_->hasMetaData<PureShear>()) {
        std::vector<double> pureShear;

        const auto& majorEigenValues = tensorFieldOut_->getMetaData<MajorEigenValues>();

        for (size_t i = 0; i < majorEigenValues.size(); i++) {
            pureShear.emplace_back(0);
        }

        tensorFieldOut_->addMetaData<PureShear>(pureShear, TensorFeature::PureShear);
    }
    if (shapeFactor_.get() && !tensorFieldOut_->hasMetaData<ShapeFactor>()) {
        std::vector<double> shapeFactor;

        const auto& majorEigenValues = tensorFieldOut_->getMetaData<MajorEigenValues>();
        const auto& intermediateEigenValues =
            tensorFieldOut_->getMetaData<IntermediateEigenValues>();
        const auto& minorEigenValues = tensorFieldOut_->getMetaData<MinorEigenValues>();

        for (size_t i = 0; i < majorEigenValues.size(); i++) {
            shapeFactor.emplace_back((majorEigenValues[i] - intermediateEigenValues[i]) /
                                     (majorEigenValues[i] - minorEigenValues[i]));
        }

        tensorFieldOut_->addMetaData<ShapeFactor>(shapeFactor, TensorFeature::ShapeFactor);
    }
    if (isotropicScaling_.get() && !tensorFieldOut_->hasMetaData<IsotropicScaling>()) {
        std::vector<double> isotropicScaling{};

//        const auto& tensors = tensorFieldOut_->tensors();

        /*const auto& majorEigenValues = tensorFieldOut_->getMetaData<MajorEigenValues>();
        const auto& intermediateEigenValues =
            tensorFieldOut_->getMetaData<IntermediateEigenValues>();
        const auto& minorEigenValues = tensorFieldOut_->getMetaData<MinorEigenValues>();*/

        for (size_t i = 0; i < tensors.size(); i++) {
            isotropicScaling.emplace_back(tensorutil::trace(tensors[i]) / 3.0);
            //                (majorEigenValues[i] + intermediateEigenValues[i] +
            //                minorEigenValues[i]) / 3.0);
        }

        tensorFieldOut_->addMetaData<IsotropicScaling>(isotropicScaling,
                                                       TensorFeature::IsotropicScaling);
    }
    if (rotation_.get() && !tensorFieldOut_->hasMetaData<Rotation>()) {
        std::vector<double> rotation;

        rotation.resize(tensors.size(), 0.0);

        tensorFieldOut_->addMetaData<Rotation>(rotation, TensorFeature::Rotation);
    }
    if (frobeniusNorm_.get() && !tensorFieldOut_->hasMetaData<FrobeniusNorm>()) {
        std::vector<double> frobeniusNorm;

        const auto& majorEigenValues = tensorFieldOut_->getMetaData<MajorEigenValues>();
        const auto& intermediateEigenValues =
            tensorFieldOut_->getMetaData<IntermediateEigenValues>();
        const auto& minorEigenValues = tensorFieldOut_->getMetaData<MinorEigenValues>();

        for (size_t i = 0; i < majorEigenValues.size(); i++) {
            auto eigenValues = std::array<double, 3>{
                majorEigenValues[i], intermediateEigenValues[i], minorEigenValues[i]};
            frobeniusNorm.emplace_back(::sqrt(
                std::accumulate(eigenValues.begin(), eigenValues.end(), 0.,
                                [](const double& a, const double& b) { return a + (b * b); })));
        }

        tensorFieldOut_->addMetaData<FrobeniusNorm>(frobeniusNorm, TensorFeature::FrobeniusNorm);
    }
}

void TensorField3DMetaData::removeMetaData() {
    if (!i1_.get()) {
        tensorFieldOut_->removeMetaData<I1>();
    }
    if (!i2_.get()) {
        tensorFieldOut_->removeMetaData<I2>();
    }
    if (!i3_.get()) {
        tensorFieldOut_->removeMetaData<I3>();
    }
    if (!j1_.get()) {
        tensorFieldOut_->removeMetaData<J1>();
    }
    if (!j2_.get()) {
        tensorFieldOut_->removeMetaData<J2>();
    }
    if (!j3_.get()) {
        tensorFieldOut_->removeMetaData<J3>();
    }
    if (!lodeAngle_.get()) {
        tensorFieldOut_->removeMetaData<LodeAngle>();
    }
    if (!anisotropy_.get()) {
        tensorFieldOut_->removeMetaData<Anisotropy>();
    }
    if (!linearAnisotropy_.get()) {
        tensorFieldOut_->removeMetaData<LinearAnisotropy>();
    }
    if (!planarAnisotropy_.get()) {
        tensorFieldOut_->removeMetaData<PlanarAnisotropy>();
    }
    if (!sphericalAnisotropy_.get()) {
        tensorFieldOut_->removeMetaData<SphericalAnisotropy>();
    }
    if (!diffusivity_.get()) {
        tensorFieldOut_->removeMetaData<Diffusivity>();
    }
    if (!shearStress_.get()) {
        tensorFieldOut_->removeMetaData<ShearStress>();
    }
    if (!pureShear_.get()) {
        tensorFieldOut_->removeMetaData<PureShear>();
    }
    if (!shapeFactor_.get()) {
        tensorFieldOut_->removeMetaData<ShapeFactor>();
    }
    if (!isotropicScaling_.get()) {
        tensorFieldOut_->removeMetaData<IsotropicScaling>();
    }
    if (!rotation_.get()) {
        tensorFieldOut_->removeMetaData<Rotation>();
    }
    if (!frobeniusNorm_.get()) {
        tensorFieldOut_->removeMetaData<FrobeniusNorm>();
    }
}

void TensorField3DMetaData::selectAll() {
    i1_.set(true);
    i2_.set(true);
    i3_.set(true);
    j1_.set(true);
    j2_.set(true);
    j3_.set(true);
    lodeAngle_.set(true);
    anisotropy_.set(true);
    linearAnisotropy_.set(true);
    planarAnisotropy_.set(true);
    sphericalAnisotropy_.set(true);
    diffusivity_.set(true);
    shearStress_.set(true);
    pureShear_.set(true);
    shapeFactor_.set(true);
    isotropicScaling_.set(true);
    rotation_.set(true);
    frobeniusNorm_.set(true);
}

void TensorField3DMetaData::deselectAll() {
    i1_.set(false);
    i2_.set(false);
    i3_.set(false);
    j1_.set(false);
    j2_.set(false);
    j3_.set(false);
    lodeAngle_.set(false);
    anisotropy_.set(false);
    linearAnisotropy_.set(false);
    planarAnisotropy_.set(false);
    sphericalAnisotropy_.set(false);
    diffusivity_.set(false);
    shearStress_.set(false);
    pureShear_.set(false);
    shapeFactor_.set(false);
    isotropicScaling_.set(false);
    rotation_.set(false);
    frobeniusNorm_.set(false);
}

void TensorField3DMetaData::process() {
    if (!tensorFieldOut_) return;

    addMetaData();

    removeMetaData();

    outport_.setData(tensorFieldOut_);
}

}  // namespace inviwo
