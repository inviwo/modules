#include <inviwo/featurelevelsetsgl/properties/pointtraitproperty.h>
#include <modules/base/algorithm/dataminmax.h>

namespace inviwo {
const std::string PointTraitProperty::classIdentifier = "org.inviwo.PointTraitProperty";
std::string PointTraitProperty::getClassIdentifier() const { return classIdentifier; }

vec4 PointTraitProperty::getAsVec4() const {
    return vec4(attribute1_.get(), attribute2_.get(), attribute3_.get(), attribute4_.get());
}

void PointTraitProperty::addAttribute(const std::string& name, std::shared_ptr<const Volume> volume,
                                      bool useVolumeDataMap) {
    const auto minVal = useVolumeDataMap
                            ? vec2(volume->dataMap_.valueRange).x
                            : static_cast<float>(util::volumeMinMax(volume.get()).first.x);
    const auto maxVal = useVolumeDataMap
                            ? vec2(volume->dataMap_.valueRange).y
                            : static_cast<float>(util::volumeMinMax(volume.get()).second.x);

    attributes_[numInitializedAttributes()]->setVisible(true);
    attributes_[numInitializedAttributes()]->set(minVal);
    attributes_[numInitializedAttributes()]->setMinValue(minVal);
    attributes_[numInitializedAttributes()]->setMaxValue(maxVal);
    attributes_[numInitializedAttributes()]->setDisplayName(name);

    inc();
}
}  // namespace inviwo
