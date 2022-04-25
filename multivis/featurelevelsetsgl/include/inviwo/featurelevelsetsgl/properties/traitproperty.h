#pragma once

#include <inviwo/featurelevelsetsgl/featurelevelsetsglmoduledefine.h>
#include <inviwo/core/properties/compositeproperty.h>
#include <inviwo/core/datastructures/volume/volume.h>

namespace inviwo {
class IVW_MODULE_FEATURELEVELSETSGL_API TraitProperty : public CompositeProperty {
public:
    TraitProperty() = delete;
    TraitProperty(const std::string& identifier, const std::string& displayName)
        : CompositeProperty(identifier, displayName), numInitializedAttributes_(0) {}
    TraitProperty(const TraitProperty& p) : CompositeProperty(p), numInitializedAttributes_(0) {}

    virtual ~TraitProperty() override = default;

    /*
     * Enforce clone method to enable usage as prefab in list property
     */
    TraitProperty* clone() const override = 0;

    std::string getClassIdentifier() const override;
    static const std::string classIdentifier;

    virtual void addAttribute(const std::string& name, std::shared_ptr<const Volume> volume,
                              bool useVolumeDataMap) = 0;

    size_t numInitializedAttributes() const { return numInitializedAttributes_; }
    void inc() { numInitializedAttributes_++; }
    void reset() { numInitializedAttributes_ = 0; }

private:
    size_t numInitializedAttributes_;
};

}  // namespace inviwo
