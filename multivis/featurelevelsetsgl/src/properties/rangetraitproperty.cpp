#include <inviwo/featurelevelsetsgl/properties/rangetraitproperty.h>
#include <modules/base/algorithm/dataminmax.h>

namespace inviwo {
const std::string RangeTraitProperty::classIdentifier = "org.inviwo.RangeTraitProperty";
std::string RangeTraitProperty::getClassIdentifier() const { return classIdentifier; }

mat4 RangeTraitProperty::getAsMat4() const {
    mat4 minMaxes;

    minMaxes[0][0] = attributes_[0]->get().r;
    minMaxes[0][1] = attributes_[0]->get().g;

    minMaxes[1][0] = attributes_[1]->get().r;
    minMaxes[1][1] = attributes_[1]->get().g;

    minMaxes[2][0] = attributes_[2]->get().r;
    minMaxes[2][1] = attributes_[2]->get().g;

    minMaxes[3][0] = attributes_[3]->get().r;
    minMaxes[3][1] = attributes_[3]->get().g;

    return minMaxes;
}

void RangeTraitProperty::addAttribute(const std::string& name, std::shared_ptr<const Volume> volume,
                                      bool useVolumeDataMap) {
    const auto minVal = useVolumeDataMap
                            ? vec2(volume->dataMap_.valueRange).x
                            : static_cast<float>(util::volumeMinMax(volume.get()).first.x);
    const auto maxVal = useVolumeDataMap
                            ? vec2(volume->dataMap_.valueRange).y
                            : static_cast<float>(util::volumeMinMax(volume.get()).second.x);

    attributes_[numInitializedAttributes()]->setVisible(true);
    attributes_[numInitializedAttributes()]->setStart(minVal);
    attributes_[numInitializedAttributes()]->setEnd(maxVal);
    attributes_[numInitializedAttributes()]->setRangeMin(minVal);
    attributes_[numInitializedAttributes()]->setRangeMax(maxVal);
    attributes_[numInitializedAttributes()]->setDisplayName(name);
}

}  // namespace inviwo
