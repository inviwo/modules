#pragma once

#include <inviwo/featurelevelsetsgl/featurelevelsetsglmoduledefine.h>
#include <inviwo/featurelevelsetsgl/properties/traitproperty.h>
#include <inviwo/core/properties/ordinalproperty.h>

namespace inviwo {
class IVW_MODULE_FEATURELEVELSETSGL_API PointTraitProperty : public TraitProperty {
public:
    PointTraitProperty() = delete;
    PointTraitProperty(const PointTraitProperty& p)
        : TraitProperty(p)
        , attribute1_(p.attribute1_)
        , attribute2_(p.attribute2_)
        , attribute3_(p.attribute3_)
        , attribute4_(p.attribute4_) {

        attributes_ =
            std::array<FloatProperty*, 4>{&attribute1_, &attribute2_, &attribute3_, &attribute4_};

        addProperties(attribute1_, attribute2_, attribute3_, attribute4_);
    }

    PointTraitProperty(const std::string& identifier, const std::string& displayName)
        : TraitProperty(identifier, displayName)
        , attribute1_("attribute1", "")
        , attribute2_("attribute2", "")
        , attribute3_("attribute3", "")
        , attribute4_("attribute4", "") {

        attributes_ =
            std::array<FloatProperty*, 4>{&attribute1_, &attribute2_, &attribute3_, &attribute4_};

        for (auto attribute : attributes_) {
            attribute->setVisible(false);
            attribute->setSerializationMode(PropertySerializationMode::All);
        }

        addProperties(attribute1_, attribute2_, attribute3_, attribute4_);
    }

    virtual PointTraitProperty* clone() const final { return new PointTraitProperty(*this); }

    virtual std::string getClassIdentifier() const override;
    static const std::string classIdentifier;

    virtual ~PointTraitProperty() final = default;

    vec4 getAsVec4() const;

    void addAttribute(const std::string& name, std::shared_ptr<const Volume> volume,
                      bool useVolumeDataMap) final;

private:
    FloatProperty attribute1_;
    FloatProperty attribute2_;
    FloatProperty attribute3_;
    FloatProperty attribute4_;

    std::array<FloatProperty*, 4> attributes_;
};

}  // namespace inviwo
