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
 *********************************************************************************/ \
#pragma once

#include <inviwo/featurelevelsetsgl/featurelevelsetsglmoduledefine.h>
#include <inviwo/featurelevelsetsgl/properties/traitproperty.h>
#include <inviwo/core/properties/stringproperty.h>

namespace inviwo {

class Shader;
class Volume;

class IVW_MODULE_FEATURELEVELSETSGL_API ImplicitFunctionTraitProperty : public TraitProperty {
public:
    static constexpr std::string_view stub{
        "// in:  <float> point[NUM_VOLUMES]\n//in:  <vec2> ranges[NUM_VOLUMES]\n// out: <float> "
        "result\nresult = point[0];"};

    ImplicitFunctionTraitProperty() = delete;
    ImplicitFunctionTraitProperty(const ImplicitFunctionTraitProperty& p)
        : ImplicitFunctionTraitProperty(p.getIdentifier(), p.getDisplayName()) {}

    ImplicitFunctionTraitProperty(const std::string& identifier, const std::string& displayName)
        : TraitProperty(identifier, displayName)
        , shaderInjection_("shaderInjection", "Implicit function", std::string(stub),
                           InvalidationLevel::InvalidOutput, PropertySemantics::ShaderEditor) {
        addProperties(shaderInjection_);
    }

    virtual ImplicitFunctionTraitProperty* clone() const final {
        return new ImplicitFunctionTraitProperty(*this);
    }

    std::string_view getClassIdentifier() const override;
    static constexpr std::string_view classIdentifier{"org.inviwo.ImplicitFunctionTraitProperty"};

    virtual ~ImplicitFunctionTraitProperty() = default;

    void addAttribute(const std::string& name, std::shared_ptr<const Volume> volume,
                      bool useVolumeDataMap) final;

    void inject(Shader& shader) const;

private:
    StringProperty shaderInjection_;
};

}  // namespace inviwo
