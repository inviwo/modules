# -*- coding: utf-8 -*-
# ********************************************************************************
#
# Inviwo - Interactive Visualization Workshop
#
# Copyright (c) 2024 Inviwo Foundation
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# ********************************************************************************

headerTemplate = """#pragma once
namespace inviwo {{
class InviwoModule;
namespace vtkwrapper {{
void register{className}(InviwoModule* module);
}}  // namespace vtkwrapper
}}  // namespace inviwo
"""

sourceTemplate = """#include <inviwo/core/common/inviwomodule.h>
#include <{processorInclude}>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/buttonproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/fileproperty.h>
{customIncludes}

#include <tuple>
#include <array>
#include <string_view>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkDataObject.h>
#include <{className}.h>
#include <warn/pop>

namespace inviwo {{
namespace vtkwrapper {{
namespace {{

#include <warn/push>
#include <warn/ignore/conversion>

{propClasses}

#include <warn/pop>

}}
template <>
struct VTKTraits<{className}> {{
    static constexpr std::string_view uri = "{uri}";
    static constexpr std::string_view className = "{className}";
    static constexpr std::string_view identifier = "{identifier}";
    static constexpr std::string_view displayName = "{displayName}";
    static constexpr std::string_view category = "{category}";
    static constexpr std::string_view tags = "{tags}";
    inline static std::array<InputData, {nInput}> inports = {{{inputData}}};
    inline static std::array<OutputData, {nOutput}> outports = {{{outputData}}};
    inline static std::array<Group, {nGroup}> groups = {{{groupList}}};
    std::tuple<{proplist}> properties;
    {customTraits}
    static constexpr std::string_view doc = R"({doc})";
}};

void register{className}(InviwoModule* module) {{
    module->registerProcessor<{processorName}<{className}>>();
}}

}}  // namespace vtkwrapper
}}  // namespace inviwo
"""

mainHeader = """#pragma once
namespace inviwo {
class InviwoModule;
namespace vtkwrapper {

void registerVTKFilters(InviwoModule* module);

}  // namespace vtkwrapper
}  // namespace inviwo
"""
mainSourceTemplate = """{includes}

namespace inviwo {{
namespace vtkwrapper {{

void registerVTKFilters(InviwoModule* module) {{
{register}
}}

}}  // namespace vtkwrapper
}}  // namespace inviwo
"""
