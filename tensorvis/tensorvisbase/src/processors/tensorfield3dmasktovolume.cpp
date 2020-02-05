#include <inviwo/tensorvisbase/processors/tensorfield3dmasktovolume.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/tensorvisbase/tensorvisbasemodule.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorField3DMaskToVolume::processorInfo_{
    "org.inviwo.TensorField3DMaskToVolume",  // Class identifier
    "Tensor Field 3D Mask To Volume",        // Display name
    "Tensor",                                // Category
    CodeState::Experimental,                 // Code state
    tag::OpenTensorVis | Tag::CPU,           // Tags
};
const ProcessorInfo TensorField3DMaskToVolume::getProcessorInfo() const { return processorInfo_; }

TensorField3DMaskToVolume::TensorField3DMaskToVolume()
    : Processor()
    , inport_("inport")
    , outport_("outport")
    , zeroFactor_("zeroMultiplier", "Zero manipulation factor", 0.f, -2.f, 2.f, 0.1f)
    , oneFactor_("oneMultiplier", "One manipulation factor", 1.f, -2.f, 2.f, 0.1f)
    , method_("method", "Method",
              {{"none", "None", 0},
               {"replace", "Replace", 1},
               {"add", "Add", 2},
               {"subtract", "Subtract", 3},
               {"multiply", "Multiply", 4},
               {"divide", "Divide", 5},
               {"invert", "Invert", 6}},
              0) {

    addPort(inport_);
    addPort(outport_);

    addProperty(zeroFactor_);
    addProperty(oneFactor_);

    addProperty(method_);

    method_.onChange([this]() {
        const auto selectedMethod = method_.get();
        const auto showFactors = selectedMethod % 6;

        zeroFactor_.setVisible(showFactors);
        oneFactor_.setVisible(showFactors);
    });
}

void TensorField3DMaskToVolume::process() {
    auto tensorField = inport_.getData();
    auto mask = tensorField->getMask();
    auto dimensions = tensorField->getDimensions();
    auto extends = tensorField->getExtents();

    auto volume = std::make_shared<Volume>(dimensions, DataFloat32::get());
    auto volumeRAM = volume->getEditableRepresentation<VolumeRAM>();
    auto volumeData = static_cast<glm::f32*>(volumeRAM->getData());

    std::vector<glm::f32> f;

    for (const auto val : mask) f.push_back(glm::f32(val));

    std::transform(std::begin(f), std::end(f), std::begin(f), [this](glm::f32 val) {
        float factor = val == 0.f ? zeroFactor_.get() : oneFactor_.get();
        switch (method_.get()) {
                // None
            case 0:
                return val;
                // Replace
            case 1:
                return factor;
                // Add
            case 2:
                return val + factor;
                // Subtract
            case 3:
                return val - factor;
                // Multiply
            case 4:
                return val * factor;
                // Divide
            case 5:
                return val / factor;
                // Invert
            case 6:
                return float(val == 0.f);
            default:
                break;
        }
        return 0.f;
    });

    const auto min = *std::min_element(std::begin(f), std::end(f));
    const auto max = *std::max_element(std::begin(f), std::end(f));

    auto fData = f.data();
    std::memcpy(volumeData, fData, f.size());

    glm::mat3 basis(1);
    basis[0][0] = static_cast<float>(extends.x);
    basis[1][1] = static_cast<float>(extends.y);
    basis[2][2] = static_cast<float>(extends.z);

    volume->setBasis(basis);
    volume->setOffset(vec3(0.f));

    volume->dataMap_.dataRange = vec2(min, max);
    volume->dataMap_.valueRange = vec2(min, max);

    outport_.setData(volume);
}

}  // namespace inviwo
