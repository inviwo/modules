/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2024 Inviwo Foundation
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

#include <inviwo/nanovgutils/nanovgutilsmodule.h>
#include <inviwo/core/util/rendercontext.h>
#include <inviwo/nanovgutils/processors/nanovgexampleprocessor.h>
#include <inviwo/nanovgutils/processors/nanovgpickingexampleprocessor.h>

#include <modules/fontrendering/util/fontutils.h>

namespace inviwo {

NanoVGUtilsModule::NanoVGUtilsModule(InviwoApplication* app) : InviwoModule(app, "NanoVGUtils") {
    registerProcessor<NanoVGExampleProcessor>();
    registerProcessor<NanoVGPickingExampleProcessor>();

    auto fonts = font::getAvailableFonts();
    for (auto font : fonts) {
        context_.createFont(font.first, font.second);
    }

    // If available, set a default font
    if (fonts.size() > 0) {
        context_.fontFace(fonts[0].first);
    }
}

NanoVGUtilsModule::~NanoVGUtilsModule() = default;

int NanoVGUtilsModule::getVersion() const { return 1; }

std::unique_ptr<VersionConverter> NanoVGUtilsModule::getConverter(int version) const {
    return std::make_unique<Converter>(version);
}

NanoVGUtilsModule::Converter::Converter(int version) : version_(version) {}

bool NanoVGUtilsModule::Converter::convert(TxElement* root) {
    bool res = false;
    switch (version_) {
        case 0: {
            TraversingVersionConverter conv{[&](TxElement* node) -> bool {
                std::string key;
                node->GetValue(&key);
                if (key != "Property") return true;
                const auto& type = node->GetAttribute("type");
                if (type != "org.inviwo.NanoVGFontProperty") {
                    return true;
                }

                if (auto elem = xml::getElement(node, "Properties/Property&identifier=fontFace")) {
                    elem->SetAttribute("type", "org.inviwo.FontFaceOptionProperty");
                    res = true;
                }
                return true;
            }};
            conv.convert(root);

            return res;
        }
        default:
            return false;  // No changes
    }
}

NanoVGContext& NanoVGUtilsModule::getNanoVGContext() { return context_; }

}  // namespace inviwo
