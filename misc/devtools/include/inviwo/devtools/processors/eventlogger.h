/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2021 Inviwo Foundation
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

#include <inviwo/devtools/devtoolsmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/processors/processortraits.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/boolcompositeproperty.h>
#include <inviwo/core/properties/buttongroupproperty.h>
#include <inviwo/core/ports/imageport.h>
#include <inviwo/core/ports/volumeport.h>
#include <inviwo/core/ports/meshport.h>
#include <inviwo/core/network/networklock.h>
#include <inviwo/core/interaction/events/keyboardevent.h>
#include <inviwo/core/interaction/events/mouseevent.h>
#include <inviwo/core/interaction/events/wheelevent.h>
#include <inviwo/core/interaction/events/gestureevent.h>
#include <inviwo/core/interaction/events/touchevent.h>
#include <inviwo/core/interaction/events/resizeevent.h>
#include <inviwo/core/interaction/events/pickingevent.h>
#include <inviwo/core/util/zip.h>

#include <fmt/format.h>

#include <functional>

namespace inviwo {

/** \docpage{org.inviwo.EventLogger, Event Printer}
 * ![](org.inviwo.EventLogger.png?classIdentifier=org.inviwo.EventLogger)
 * The event printer will listen for events and log them.
 *
 * ### Inports
 *   * __Inport__ The inport, only used for pass through.
 *
 * ### Outports
 *   * __Outport__ The outport.
 *
 * ### Properties
 *   * __Enable__ Enable or disable logging of events.
 */

template <typename Inport, typename Outport>
class EventLogger : public Processor {
public:
    EventLogger();
    virtual ~EventLogger() = default;

    virtual void process() override;
    virtual const ProcessorInfo getProcessorInfo() const override;
    virtual void invokeEvent(Event* event) override;

private:
    Inport inport_;
    Outport outport_;
    BoolCompositeProperty enable_;
    ButtonGroupProperty eventToggle_;

    static constexpr size_t eventCount = 8;
    static constexpr std::array<const char*, eventCount> events_ = {
        "Keyboard", "Mouse", "Wheel", "Gesture", "Touch", "Resize", "Picking"};
    static constexpr std::array<uint64_t, eventCount> eventHash_ = {
        KeyboardEvent::chash(), MouseEvent::chash(),  WheelEvent::chash(),  GestureEvent::chash(),
        TouchEvent::chash(),    ResizeEvent::chash(), PickingEvent::chash()};
    std::array<BoolProperty, eventCount> enableEvents_;
    const std::unordered_map<uint64_t, std::reference_wrapper<BoolProperty>> eventMap_;

    BoolProperty enableOtherEvents_;
};

template <typename Inport, typename Outport>
EventLogger<Inport, Outport>::EventLogger()
    : Processor()
    , inport_("inport")
    , outport_("outport")
    , enable_("enable", "Enable Logging", true)
    , eventToggle_("eventToggle", "Toggle Events",
                   {{"Set All", std::nullopt, "Enable logging of all event types",
                     [this]() {
                         NetworkLock lock(this);
                         for (auto& item : eventMap_) {
                             item.second.get().set(true);
                         }
                         enableOtherEvents_.set(true);
                     }},
                    {"Clear", std::nullopt, "Clear all event types",
                     [this]() {
                         NetworkLock lock(this);
                         for (auto& item : eventMap_) {
                             item.second.get().set(false);
                         }
                         enableOtherEvents_.set(false);
                     }}})

    , enableEvents_{util::make_array<eventCount>([&](auto ind) {
        return BoolProperty(fmt::format("enable{}event", events_[ind]),
                            fmt::format("Enable {} Event", events_[ind]), true);
    })}
    , eventMap_([&]() {
        std::unordered_map<uint64_t, std::reference_wrapper<BoolProperty>> map;
        for (auto&& i : util::zip(eventHash_, enableEvents_)) {
            map.emplace(i.first(), std::ref(i.second()));
        }
        return map;
    }())
    , enableOtherEvents_("enableOtherEvents", "Enable Other Events", true) {

    addPort(inport_);
    addPort(outport_);
    addProperty(enable_);
    enable_.addProperty(eventToggle_);

    for (auto& p : enableEvents_) {
        enable_.addProperty(p);
    }
    enable_.addProperty(enableOtherEvents_);
}

template <typename Inport, typename Outport>
void EventLogger<Inport, Outport>::process() {
    outport_.setData(inport_.getData());
}

template <typename Inport, typename Outport>
void EventLogger<Inport, Outport>::invokeEvent(Event* event) {
    if (!enable_) return;

    const auto it = eventMap_.find(event->hash());
    if ((it == eventMap_.end() && enableOtherEvents_.get()) ||
        (it != eventMap_.end() && it->second.get())) {
        LogProcessorInfo(std::setw(15) << getDisplayName() << " " << *event);
    }
}

template <typename Inport, typename Outport>
const ProcessorInfo EventLogger<Inport, Outport>::getProcessorInfo() const {
    return ProcessorTraits<EventLogger<Inport, Outport>>::getProcessorInfo();
}

using ImageEventLogger = EventLogger<ImageInport, ImageOutport>;
template <>
struct ProcessorTraits<ImageEventLogger> {
    static ProcessorInfo getProcessorInfo() {
        return {
            "org.inviwo.ImageEventLogger",  // Class identifier
            "ImageEventLogger",             // Display name
            "Testing",                      // Category
            CodeState::Stable,              // Code state
            Tags::CPU                       // Tags
        };
    }
};

using VolumeEventLogger = EventLogger<VolumeInport, VolumeOutport>;
template <>
struct ProcessorTraits<VolumeEventLogger> {
    static ProcessorInfo getProcessorInfo() {
        return {
            "org.inviwo.VolumeEventLogger",  // Class identifier
            "VolumeEventLogger",             // Display name
            "Testing",                       // Category
            CodeState::Stable,               // Code state
            Tags::CPU                        // Tags
        };
    }
};

using MeshEventLogger = EventLogger<MeshInport, MeshOutport>;
template <>
struct ProcessorTraits<MeshEventLogger> {
    static ProcessorInfo getProcessorInfo() {
        return {
            "org.inviwo.MeshEventLogger",  // Class identifier
            "MeshEventLogger",             // Display name
            "Testing",                     // Category
            CodeState::Stable,             // Code state
            Tags::CPU                      // Tags
        };
    }
};

}  // namespace inviwo
