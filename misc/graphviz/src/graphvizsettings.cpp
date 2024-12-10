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

#include <inviwo/graphviz/graphvizsettings.h>

#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/network/processornetwork.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/metadata/processormetadata.h>
#include <inviwo/core/util/stringconversion.h>
#include <inviwo/core/network/networkutils.h>
#include <inviwo/graphviz/graphvizutil.h>

#include <graphviz/cgraph.h>
#include <graphviz/gvc.h>

#include <algorithm>
#include <ranges>

namespace inviwo {

namespace {

constexpr std::string_view graph = R"(digraph G {{
graph [pad="0", ranksep="{}", nodesep="{}"]
node [shape=plain,
label=<
<TABLE BORDER="0" CELLBORDER="1" CELLSPACING="4" CELLPADDING="0" WIDTH="150" HEIGHT="50" FIXEDSIZE="TRUE">
    <TR>
        <TD WIDTH="2" HEIGHT="9" BORDER="0"></TD>
        <TD WIDTH="9" HEIGHT="9" PORT="i0"></TD>
        <TD WIDTH="9" HEIGHT="9" PORT="i1"></TD>
        <TD WIDTH="9" HEIGHT="9" PORT="i2"></TD>
        <TD WIDTH="9" HEIGHT="9" PORT="i3"></TD>
        <TD WIDTH="9" HEIGHT="9" PORT="i4"></TD>
        <TD WIDTH="9" HEIGHT="9" PORT="i5"></TD>
        <TD WIDTH="9" HEIGHT="9" PORT="i6"></TD>
        <TD WIDTH="9" HEIGHT="9" PORT="i7"></TD>
        <TD WIDTH="9" HEIGHT="9" PORT="i8"></TD>
        <TD WIDTH="9" HEIGHT="9" PORT="i9"></TD>
        <TD WIDTH="2" HEIGHT="9" BORDER="0"></TD>
    </TR>
    <TR>
        <TD WIDTH="2" HEIGHT="15" PORT="l1"></TD>
        <TD COLSPAN="10" BORDER="0"></TD>
        <TD WIDTH="2" HEIGHT="15" PORT="l2"></TD>
    </TR>
    <TR>
        <TD WIDTH="2" HEIGHT="9" BORDER="0"></TD>
        <TD WIDTH="9" HEIGHT="9" PORT="o0"></TD>
        <TD WIDTH="9" HEIGHT="9" PORT="o1"></TD>
        <TD WIDTH="9" HEIGHT="9" PORT="o2"></TD>
        <TD WIDTH="9" HEIGHT="9" PORT="o3"></TD>
        <TD WIDTH="9" HEIGHT="9" PORT="o4"></TD>
        <TD WIDTH="9" HEIGHT="9" PORT="o5"></TD>
        <TD WIDTH="9" HEIGHT="9" PORT="o6"></TD>
        <TD WIDTH="9" HEIGHT="9" PORT="o7"></TD>
        <TD WIDTH="9" HEIGHT="9" PORT="o8"></TD>
        <TD WIDTH="9" HEIGHT="9" PORT="o9"></TD>
        <TD WIDTH="2" HEIGHT="9" BORDER="0"></TD>
    </TR>
</TABLE>>
]
overlap = true;
splines = true;
)";

ivec2 snap(dvec2 pos) {
    const int spacing = 25;
    auto ox = pos.x > 0.0 ? 0.5 : -0.5;
    auto oy = pos.y > 0.0 ? 0.5 : -0.5;

    return {static_cast<int>(pos.x / spacing + ox) * spacing,
            static_cast<int>(pos.y / spacing + oy) * spacing};
}

StrBuffer& createDotGraph(StrBuffer& buff, ProcessorNetwork* net, glm::dvec2 sep, bool alignSources,
                          bool alignSinks) {
    buff.append(graph, sep.y, sep.x);

    if (alignSources) {
        buff.append("{{rank=same;");
        net->forEachProcessor([&](Processor* p) {
            if (p->getInports().empty()) {
                buff.append("{};", p->getIdentifier());
            }
        });
        buff.append("}}\n");
    }

    if (alignSinks) {
        buff.append("{{rank=same;");
        net->forEachProcessor([&](Processor* p) {
            if (p->getOutports().empty()) {
                buff.append("{};", p->getIdentifier());
            }
        });
        buff.append("}}\n");
    }
    net->forEachConnection([&](PortConnection connection) {
        const auto* src = connection.getOutport()->getProcessor();
        const auto* dst = connection.getInport()->getProcessor();
        const auto sit = std::ranges::find(src->getOutports(), connection.getOutport());
        const auto sPos = std::distance(src->getOutports().begin(), sit);
        const auto dit = std::ranges::find(dst->getInports(), connection.getInport());
        const auto dPos = std::distance(dst->getInports().begin(), dit);

        buff.append("{}:o{}:s -> {}:i{}:n;\n",
                    connection.getOutport()->getProcessor()->getIdentifier(), sPos,
                    connection.getInport()->getProcessor()->getIdentifier(), dPos);
    });

    buff.append("}}");

    return buff;
}

template <typename Func>
void calculateLayout(StrBuffer& buff, ProcessorNetwork* net, const Func& func) {
    auto* gvc = gvContext();
    util::loadGraphvizLibraries(gvc);

    Agraph_t* G = agmemread(buff.c_str());
    gvLayout(gvc, G, "dot");
    util::OnScopeExit freeG{[&]() {
        gvFreeLayout(gvc, G);
        agclose(G);
    }};

    const auto oldCenter = util::getCenterPosition(net);
    const auto newCenter = [&]() {
        ivec2 center{0};
        int count = 0;
        net->forEachProcessor([&](Processor* p) {
            if (auto* n = agnode(G, const_cast<char*>(p->getIdentifier().c_str()), 0)) {
                const auto& coord = ND_coord(n);
                center += ivec2{coord.x, -coord.y};
                ++count;
            }
        });
        if (count == 0) {
            return center;
        } else {
            return center / count;
        }
    }();
    const dvec2 offset = oldCenter - newCenter;

    net->forEachProcessor([&](Processor* p) {
        if (auto* n = agnode(G, const_cast<char*>(p->getIdentifier().c_str()), 0)) {
            const auto& coord = ND_coord(n);
            func(p, dvec2{coord.x, -coord.y} + offset);
        }
    });
}

void layoutNetwork(ProcessorNetwork* net, glm::dvec2 sep, bool alignSources, bool alignSinks) {
    StrBuffer buff;
    createDotGraph(buff, net, sep, alignSources, alignSinks);

    calculateLayout(buff, net, [](Processor* p, const dvec2 pos) {
        if (auto* meta = p->getMetaData<ProcessorMetaData>(ProcessorMetaData::classIdentifier)) {
            meta->setPosition(snap(pos));
        }
    });
}

}  // namespace

GraphvizSettings::GraphvizSettings(InviwoApplication* app)
    : Settings("Graphviz Settings")
    , autoLayout{"autoLayout", "Auto Layout Network", false}
    , alignSources{"alignSources", "Align Source Processors", false}
    , alignSinks{"alignSinks", "Align Sink Processors", true}
    , separation{"separation", "Separation", dvec2{0.35, 0.35}}
    , layout{"layout", "Layout Network"}
    , print{"print", "Print"}
    , app_{app}
    , isApplyingLayout_{false} {

    app_->getProcessorNetwork()->addObserver(this);

    addProperties(autoLayout, alignSources, alignSinks, separation, layout, print);

    layout.onChange([this]() { doLayoutNetwork(); });
    separation.onChange([this]() { autoLayoutNetwork(); });
    alignSources.onChange([this]() { autoLayoutNetwork(); });
    alignSinks.onChange([this]() { autoLayoutNetwork(); });
    autoLayout.onChange([this]() { autoLayoutNetwork(); });

    print.onChange([this]() {
        StrBuffer buff;
        createDotGraph(buff, app_->getProcessorNetwork(), separation, alignSources, alignSinks);
        LogInfo(buff.view());
        calculateLayout(buff, app_->getProcessorNetwork(), [](Processor* p, const dvec2 pos) {
            dvec2 cpos{-1, -1};
            if (auto* meta =
                    p->getMetaData<ProcessorMetaData>(ProcessorMetaData::classIdentifier)) {
                cpos = meta->getPosition();
            }
            LogInfo(fmt::format("{:30} New: {:20} Snapped: {:20} Current: {:20}",
                                p->getIdentifier(), pos, snap(pos), cpos));
        });
    });

    load();
}

void GraphvizSettings::doLayoutNetwork() {
    util::KeepTrueWhileInScope inProgress{&isApplyingLayout_};
    layoutNetwork(app_->getProcessorNetwork(), separation, alignSources, alignSinks);
}
void GraphvizSettings::autoLayoutNetwork() {
    if (autoLayout && !isApplyingLayout_) {
        doLayoutNetwork();
    }
}

void GraphvizSettings::onProcessorNetworkDidAddProcessor(Processor* processor) {
    autoLayoutNetwork();
    auto meta = processor->getMetaData<ProcessorMetaData>(ProcessorMetaData::classIdentifier);
    meta->addObserver(this);
}
void GraphvizSettings::onProcessorNetworkDidRemoveProcessor(Processor* processor) {
    autoLayoutNetwork();
    auto meta = processor->getMetaData<ProcessorMetaData>(ProcessorMetaData::classIdentifier);
    meta->removeObserver(this);
}
void GraphvizSettings::onProcessorNetworkDidAddConnection(const PortConnection&) {
    autoLayoutNetwork();
}
void GraphvizSettings::onProcessorNetworkDidRemoveConnection(const PortConnection&) {
    autoLayoutNetwork();
}
void GraphvizSettings::onProcessorMetaDataPositionChange() { autoLayoutNetwork(); }

}  // namespace inviwo
