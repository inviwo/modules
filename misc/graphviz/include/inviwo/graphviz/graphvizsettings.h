/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2024-2025 Inviwo Foundation
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

#include <inviwo/graphviz/graphvizmoduledefine.h>

#include <inviwo/core/util/settings/settings.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/buttonproperty.h>
#include <inviwo/core/properties/ordinalproperty.h>

#include <inviwo/core/network/processornetworkobserver.h>
#include <inviwo/core/metadata/processormetadata.h>

namespace inviwo {

class InviwoApplication;

class IVW_MODULE_GRAPHVIZ_API GraphvizSettings : public Settings,
                                                 public ProcessorNetworkObserver,
                                                 public ProcessorMetaDataObserver {
public:
    GraphvizSettings(InviwoApplication* app);

    BoolProperty autoLayout;

    BoolProperty alignSources;
    BoolProperty alignSinks;
    DoubleVec2Property separation;

    ButtonProperty layout;
    ButtonProperty print;

private:
    void doLayoutNetwork();
    void autoLayoutNetwork();

    virtual void onProcessorNetworkDidAddProcessor(Processor*) override;
    virtual void onProcessorNetworkDidRemoveProcessor(Processor*) override;
    virtual void onProcessorNetworkDidAddConnection(const PortConnection&) override;
    virtual void onProcessorNetworkDidRemoveConnection(const PortConnection&) override;

    virtual void onProcessorMetaDataPositionChange() override;

    InviwoApplication* app_;
    bool isApplyingLayout_;
};

}  // namespace inviwo
