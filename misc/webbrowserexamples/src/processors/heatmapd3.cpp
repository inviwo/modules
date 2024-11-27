/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2024 Inviwo Foundation
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

#include <inviwo/webbrowserexamples/processors/heatmapd3.h>

#include <inviwo/dataframe/jsondataframeconversion.h>

#include <fmt/core.h>

#include <warn/push>
#include <warn/ignore/all>
#include <nlohmann/json.hpp>
#include <warn/pop>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo HeatmapD3::processorInfo_{
    "org.inviwo.HeatmapD3",                           // Class identifier
    "Heatmap D3 Example",                             // Display name
    "Web",                                            // Category
    CodeState::Stable,                                // Code state
    Tags::CPU | Tag{"Web Browser"} | Tag{"Example"},  // Tags
    R"(Example processor demonstrating the use of a web browser with D3.

Example network:
+ [webbrowserexamples/d3-heatmap-example.inv](file:~modulePath~/data/workspaces/d3-heatmap-example.inv)
)"_unindentHelp,
};

const ProcessorInfo& HeatmapD3::getProcessorInfo() const { return processorInfo_; }

HeatmapD3::HeatmapD3(InviwoApplication* app)
    : Processor{}
    , dataframe_{"dataframe", "Input DataFrame"_help}
    , background_{"background", "Background image"_help}
    , outport_{"webpage", "Rendered web page"_help, DataVec4UInt8::get()}
    , fileName_{"fileName", "HTML file", {}, "html", InvalidationLevel::Valid}
    , reloadButton_("reloadButton", "Reload")
    , zoom_{"zoom", "Zoom Factor", 1.0, 0.2, 5.0}
    , browser_{new WebBrowserBase(app, this)} {

    addPorts(dataframe_, background_, outport_);
    background_.setOptional(true);
    addProperties(fileName_, reloadButton_, zoom_);

    fileName_.onChange([this]() { browser_->load(fileName_.get()); });
    reloadButton_.onChange([this]() { browser_->reload(); });

    addInteractionHandler(browser_->getInteractionHandler());
    loadingDoneCallback_ =
        browser_->addLoadingDoneCallback([this]() { pageLoaded_ = !browser_->isLoading(); });
}

void HeatmapD3::process() {
    if (browser_->isLoading()) {
        return;
    }
    if (zoom_.isModified()) {
        browser_->setZoom(zoom_);
    }
    // load data only if the port has changed or the web page has just loaded
    if (pageLoaded_ || dataframe_.isChanged()) {
        // convert DataFrame input to json and assign it to a JavaScript variable `jsonData`
        const json jsonDataFrame = *dataframe_.getData();
        const std::string js = fmt::format("setData({0});", jsonDataFrame.dump());

        browser_->executeJavaScript(js);
    }

    browser_->render(outport_, &background_);
    pageLoaded_ = false;
}

void HeatmapD3::deserialize(Deserializer& d) {
    Processor::deserialize(d);
    browser_->load(fileName_.get());
}

}  // namespace inviwo
