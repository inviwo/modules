#include <inviwo/tensorvisbase/processors/volumeactualdataandvaluerange.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/tensorvisbase/tensorvisbasemodule.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VolumeActualDataAndValueRange::processorInfo_{
    "org.inviwo.VolumeActualDataAndValueRange",  // Class identifier
    "Volume Actual Data And Value Range",        // Display name
    "Tensor",                                    // Category
    CodeState::Experimental,                     // Code state
    tag::OpenTensorVis | Tag::CPU,               // Tags
};
const ProcessorInfo VolumeActualDataAndValueRange::getProcessorInfo() const {
    return processorInfo_;
}

VolumeActualDataAndValueRange::VolumeActualDataAndValueRange()
    : Processor(), inport_("inport"), outport_("outport") {
    addPort(inport_);
    addPort(outport_);
}

void VolumeActualDataAndValueRange::process() {
    auto inVolume = inport_.getData();

    auto inVolumeRAM = inVolume->getRepresentation<VolumeRAM>();

    auto inVolumeData = static_cast<const glm::f32*>(inVolumeRAM->getData());

    auto dimensions = inVolume->getDimensions();

    auto numElements = dimensions.x * dimensions.y * dimensions.z;

    auto outVolume = inVolume->clone();

    auto min = std::numeric_limits<float>::max();
    auto max = std::numeric_limits<float>::lowest();

    for (size_t i = 0; i < numElements; i++) {
        min = std::min(min, inVolumeData[i]);
        max = std::max(max, inVolumeData[i]);
    }

    outVolume->dataMap_.dataRange = vec2(min, max);
    outVolume->dataMap_.valueRange = vec2(min, max);

    outport_.setData(std::make_shared<Volume>(*outVolume));
}

}  // namespace inviwo
