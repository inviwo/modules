/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020 Inviwo Foundation
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

#include <inviwo/tensorvisbase/processors/tensorfield2dmetadata.h>
#include <inviwo/tensorvisbase/util/tensorutil.h>
#include <inviwo/core/util/stringconversion.h>
#include <inviwo/core/network/networklock.h>
#include <inviwo/tensorvisbase/tensorvisbasemodule.h>
#include <inviwo/core/util/constexprhash.h>
#include <inviwo/tensorvisbase/util/attributeutil.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorField2DMetaData::processorInfo_{
    "org.inviwo.TensorField2DMetaData",  // Class identifier
    "Tensor Field 2D Meta Data",         // Display name
    "Data generation",                   // Category
    CodeState::Experimental,             // Code state
    tag::OpenTensorVis | Tag::CPU        // Tags
};
const ProcessorInfo TensorField2DMetaData::getProcessorInfo() const { return processorInfo_; }

TensorField2DMetaData::TensorField2DMetaData()
    : Processor()
    , inport_("inport")
    , outport_("outport")
    , metaDataPropertyContainer_("metaDataPropertyContainer", "Meta data")
    , selectAll_("selectAll", "Select all")
    , deselectAll_("deselectAll", "Deselect all") {

    addPort(inport_);
    addPort(outport_);

    addProperty(metaDataPropertyContainer_);
    addProperty(selectAll_);
    addProperty(deselectAll_);

    inport_.onChange([this]() { invalidate(InvalidationLevel::InvalidResources); });

    selectAll_.onChange([this]() {
        auto comp = getPropertiesByType<CompositeProperty>().front()->getProperties();

        for (auto prop : comp) {
            static_cast<BoolProperty*>(prop)->set(true);
        }
    });

    deselectAll_.onChange([this]() {
        auto comp = getPropertiesByType<CompositeProperty>().front()->getProperties();

        for (auto prop : comp) {
            static_cast<BoolProperty*>(prop)->set(false);
        }
    });
}

void TensorField2DMetaData::initializeResources() {
    if (!inport_.hasData()) return;

    auto comp =
        static_cast<CompositeProperty*>(getPropertyByIdentifier("metaDataPropertyContainer"));
    auto props = comp->getProperties();

    NetworkLock l;

    for (auto prop : props) {
        comp->removeProperty(prop);
    }

    util::for_each_type<attributes::types2D>{}(
        util::AddMetaDataProperties<2>{this, inport_.getData()});

    /*
    This is a bit of a hack but it'll do. What happens is that adding/removing of the default meta
    data such as eigen values and eigen vectors is disabled by setting the respective properties to
    readonly.
    */
    props = comp->getProperties();
    for (size_t i{0}; i < TensorField2D::dimensionality * 2; ++i) {
        props[i]->setReadOnly(true);
    }
}

void TensorField2DMetaData::addRemoveMetaData(std::shared_ptr<TensorField2D> tensorField) {
    auto comp = getPropertiesByType<CompositeProperty>().front();
    auto props = comp->getProperties();

    for (auto prop : props) {
        const auto id = util::constexpr_hash(std::string_view(prop->getDisplayName()));
        const auto add = static_cast<BoolProperty*>(prop)->get();

        util::for_each_type<attributes::types2D>{}(util::AddRemoveMetaData<2>{tensorField}, id,
                                                   add);
    }
}

void TensorField2DMetaData::process() {
    auto newTensorField = std::make_shared<TensorField2D>(*inport_.getData());

    addRemoveMetaData(newTensorField);

    outport_.setData(newTensorField);
}

}  // namespace inviwo
