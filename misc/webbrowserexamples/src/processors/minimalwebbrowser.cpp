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

#include <inviwo/webbrowserexamples/processors/minimalwebbrowser.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo MinimalWebBrowser::processorInfo_{
    "org.inviwo.MinimalWebBrowser",                   // Class identifier
    "Minimal Web Browser Example",                    // Display name
    "Web",                                            // Category
    CodeState::Stable,                                // Code state
    Tags::CPU | Tag{"Web Browser"} | Tag{"Example"},  // Tags
    R"(Example processor demonstrating the use of the web browser functionality.)"_unindentHelp,
};

const ProcessorInfo& MinimalWebBrowser::getProcessorInfo() const { return processorInfo_; }

MinimalWebBrowser::MinimalWebBrowser(InviwoApplication* app)
    : Processor{}
    , background_{"background", "Background image"_help}
    , outport_{"webpage", "Rendered web page"_help, DataVec4UInt8::get()}
    , fileName_{"fileName", "HTML file", {}, "html", InvalidationLevel::Valid}
    , autoReloadFile_{"autoReloadFile", "Auto Reload", true, InvalidationLevel::Valid}
    , reloadButton_("reloadButton", "Reload")
    , zoom_{"zoom", "Zoom Factor", 1.0, 0.2, 5.0}
    , browser_{new WebBrowserBase(app, this)} {

    addPorts(background_, outport_);
    background_.setOptional(true);
    addProperties(fileName_, autoReloadFile_, reloadButton_, zoom_);

    fileName_.onChange([this]() {
        if (autoReloadFile_) {
            fileObserver_.setFilename(fileName_);
        }
        browser_->load(fileName_);
    });
    autoReloadFile_.onChange([this]() {
        if (autoReloadFile_) {
            fileObserver_.setFilename(fileName_);
        } else {
            fileObserver_.stop();
        }
    });

    reloadButton_.onChange([this]() { browser_->reload(); });

    addInteractionHandler(browser_->getInteractionHandler());
    browser_->load(fileName_);
}

void MinimalWebBrowser::process() {
    if (browser_->isLoading()) {
        return;
    }
    if (zoom_.isModified()) {
        browser_->setZoom(zoom_);
    }
    browser_->render(outport_, &background_);
}

void MinimalWebBrowser::deserialize(Deserializer& d) {
    Processor::deserialize(d);
    browser_->load(fileName_);
}

}  // namespace inviwo
