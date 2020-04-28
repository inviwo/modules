#include <inviwo/tensorvisio/processors/flowguifilereader.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/tensorvisbase/tensorvisbasemodule.h>
#include <fstream>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo FlowGUIFileReader::processorInfo_{
    "org.inviwo.FlowGUIFileReader",  // Class identifier
    "Wito File Reader",              // Display name
    "Tensor",                        // Category
    CodeState::Experimental,         // Code state
    tag::OpenTensorVis | Tag::CPU,   // Tags
};
const ProcessorInfo FlowGUIFileReader::getProcessorInfo() const { return processorInfo_; }

FlowGUIFileReader::FlowGUIFileReader()
    : Processor()
    , inFile_("inFile", "File", "")
    , fieldType_("fieldType", "Field type", {{"scalar", "Scalar", 0}, {"vec4", "Vec4", 1}})
    , outport_("outport")
    , outportVec3_("outportVec3") {
    addProperty(inFile_);
    addProperty(fieldType_);
    addPort(outport_);
    addPort(outportVec3_);
}

void FlowGUIFileReader::process() {
    std::ifstream inFile(inFile_.get(), std::ios::in | std::ios::binary);

    if (!inFile) {
        LogError("Couldn't open file");
        return;
    }

    // Length of Name
    int length;
    inFile.read(reinterpret_cast<char*>(&length), sizeof(int));

    std::string name;
    name.resize(length);

    inFile.read(&name[0], length * sizeof(char));

    LogInfo(name);

    // Bounds
    vec4 minBounds;
    inFile.read(reinterpret_cast<char*>(&minBounds), sizeof(float) * 4);

    vec4 maxBounds;
    inFile.read(reinterpret_cast<char*>(&maxBounds), sizeof(float) * 4);

    ivec4 dimensions;
    inFile.read(reinterpret_cast<char*>(&dimensions), sizeof(int) * 4);

    LogInfo(minBounds);
    LogInfo(maxBounds);
    LogInfo(dimensions);

    const auto numElements = dimensions.x * dimensions.y * dimensions.z;

    auto volumes = std::make_shared<std::vector<std::shared_ptr<Volume>>>();
    auto volumes_vec3 = std::make_shared<std::vector<std::shared_ptr<Volume>>>();

    if (fieldType_.get() == 1) {
        for (int i = 0; i < dimensions.w; i++) {
            volumes->emplace_back(
                std::make_shared<Volume>(size3_t(dimensions), DataVec4Float32::get()));

            auto volume = volumes->back();

            auto volumeRAM = volume->getEditableRepresentation<VolumeRAM>();
            auto volumeData = static_cast<vec4*>(volumeRAM->getData());

            inFile.read(reinterpret_cast<char*>(volumeData), sizeof(float) * 4 * numElements);

            volume->setOffset(minBounds);
            auto basis = mat3(1.f);
            auto extents = maxBounds - minBounds;
            basis[0][0] = extents.x;
            basis[1][1] = extents.y;
            basis[2][2] = extents.z;
            volume->setBasis(basis);

            volumes_vec3->emplace_back(
                std::make_shared<Volume>(size3_t(dimensions), DataVec3Float32::get()));

            auto volume_vec3 = volumes_vec3->back();

            auto volumeRAM_vec3 = volume_vec3->getEditableRepresentation<VolumeRAM>();
            auto volumeData_vec3 = static_cast<vec3*>(volumeRAM_vec3->getData());

            for (int j = 0; j < numElements; j++) {
                volumeData_vec3[j] = vec3(volumeData[j]);
            }
            volume_vec3->setOffset(minBounds);
            volume_vec3->setBasis(basis);
        }

        outport_.setData(volumes);

        outportVec3_.setData(volumes_vec3);
    } else {
        for (int i = 0; i < dimensions.w; i++) {
            volumes->emplace_back(
                std::make_shared<Volume>(size3_t(dimensions), DataFloat32::get()));

            auto volume = volumes->back();

            auto volumeRAM = volume->getEditableRepresentation<VolumeRAM>();
            auto volumeData = static_cast<glm::f32*>(volumeRAM->getData());

            inFile.read(reinterpret_cast<char*>(volumeData), sizeof(float) * numElements);

            auto min = std::numeric_limits<float>::max();
            auto max = std::numeric_limits<float>::lowest();

            for (int j = 0; j < numElements; j++) {
                min = std::min(min, volumeData[j]);
                max = std::max(max, volumeData[j]);
            }

            volume->setOffset(minBounds);
            auto basis = mat3(1);
            auto extents = maxBounds - minBounds;
            basis[0][0] = extents.x;
            basis[1][1] = extents.y;
            basis[2][2] = extents.z;
            volume->setBasis(basis);

            volume->dataMap_.valueRange = vec2(min, max);
            volume->dataMap_.dataRange = vec2(min, max);
        }

        outport_.setData(volumes);
    }
}

}  // namespace inviwo
