#include <inviwo/tensorvisbase/processors/tensorfield3dbasismanipulation.h>
#include <inviwo/tensorvisbase/tensorvisbasemodule.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorField3DBasisManipulation::processorInfo_{
    "org.inviwo.TensorField3DBasisManipulation",  // Class identifier
    "Tensor Field 3D Basis Manipulation",         // Display name
    "Tensor",                                     // Category
    CodeState::Experimental,                      // Code state
    tag::OpenTensorVis | Tag::CPU,                // Tags
};
const ProcessorInfo TensorField3DBasisManipulation::getProcessorInfo() const {
    return processorInfo_;
}

TensorField3DBasisManipulation::TensorField3DBasisManipulation()
    : Processor()
    , inport_("inport")
    , outport_("outport")
    , basis_("basis", "Basis", mat3(0.0f), mat3(std::numeric_limits<float>::lowest()),
             mat3(std::numeric_limits<float>::max()))
    , offset_("offset", "Offset", vec3(0.f), vec3(std::numeric_limits<float>::lowest()),
              vec3(std::numeric_limits<float>::max())) {

    addPort(inport_);
    addPort(outport_);
    addProperty(basis_);
    addProperty(offset_);
}

void TensorField3DBasisManipulation::initializeResources() {
    if (!inport_.hasData()) return;

    auto tensorField = inport_.getData();

    auto basis = tensorField->getBasis();

    basis_.set(basis);

    offset_.set(tensorField->getOffset());
}

void TensorField3DBasisManipulation::process() {
    auto outTensorField = inport_.getData()->clone();

    outTensorField->setBasis(basis_.get());

    outTensorField->setOffset(offset_.get());

    outport_.setData(outTensorField);
}

}  // namespace inviwo
