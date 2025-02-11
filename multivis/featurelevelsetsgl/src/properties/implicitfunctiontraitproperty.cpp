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

#include <inviwo/featurelevelsetsgl/properties/implicitfunctiontraitproperty.h>
#include <inviwo/core/util/stringconversion.h>
#include <modules/opengl/shader/shader.h>

#include <fmt/base.h>

namespace inviwo {

std::string_view ImplicitFunctionTraitProperty::getClassIdentifier() const {
    return classIdentifier;
}

void ImplicitFunctionTraitProperty::addAttribute(const std::string&, std::shared_ptr<const Volume>,
                                                 bool) {}

void ImplicitFunctionTraitProperty::inject(Shader& shader) const {
    const auto identifier = getIdentifier();
    const auto itIdentifier = std::find_if(identifier.rbegin(), identifier.rend(),
                                           [](char c) { return !std::isdigit(c); });
    std::string_view number(itIdentifier.base(), identifier.end());
    if (number.empty()) {
        number = "1";
    }

    auto* computeShader = shader.getComputeShaderObject();

    StrBuffer buf;
    computeShader->addShaderDefine(buf.replace("ENABLE_IMPLICIT_FUNCTION_{}", number));
    computeShader->addShaderDefine(buf.replace("IMPLICIT_FUNCTION_{}", number),
                                   shaderInjection_.get());
    shader.build();
}

}  // namespace inviwo
