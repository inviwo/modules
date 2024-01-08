/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018-2024 Inviwo Foundation
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

#include <inviwo/integrallinefiltering/integrallinefilteringmodule.h>
#include <inviwo/integrallinefiltering/processors/integrallinestodataframe.h>

namespace inviwo {

IntegralLineFilteringModule::IntegralLineFilteringModule(InviwoApplication* app)
    : InviwoModule(app, "IntegralLineFiltering") {
    registerProcessor<IntegralLinesToDataFrame>();

    registerProperty<IntegralLinesToDataFrame::MetaDataSettings>();
}

int IntegralLineFilteringModule::getVersion() const { return 1; }

std::unique_ptr<VersionConverter> IntegralLineFilteringModule::getConverter(int version) const {
    return std::make_unique<Converter>(version);
}

IntegralLineFilteringModule::Converter::Converter(int version) : version_(version) {}

bool IntegralLineFilteringModule::Converter::convert(TxElement* root) {
    std::vector<xml::IdentifierReplacement> repl = {};

    repl.push_back(xml::IdentifierReplacement{
        {xml::Kind::processor("org.inviwo.IntegralLineMetricsToDataFrame")},
        "org.inviwo.IntegralLineMetricsToDataFrame",
        "org.inviwo.IntegralLinesToDataFrame"});

    bool res = false;
    switch (version_) {
        case 0: {
            for (const auto& fromTO : std::vector<std::pair<std::string, std::string>>{
                     {"IntegralLineMetricsToDataFrame", "IntegralLinesToDataFrame"}}) {
                res |= xml::changeAttribute(
                    root, {{xml::Kind::processor("org.inviwo." + fromTO.first)}}, "type",
                    "org.inviwo." + fromTO.first, "org.inviwo." + fromTO.second);
            }
        }
            return res;

        default:
            return false;  // No changes
    }
    return true;
}

}  // namespace inviwo
