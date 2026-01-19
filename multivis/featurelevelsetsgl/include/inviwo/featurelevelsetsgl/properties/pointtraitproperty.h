/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2022-2025 Inviwo Foundation
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

    virtual std::string_view getClassIdentifier() const override;
    static constexpr std::string_view classIdentifier{"org.inviwo.PointTraitProperty"};

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
