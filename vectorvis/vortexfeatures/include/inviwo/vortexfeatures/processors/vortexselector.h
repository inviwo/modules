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

#pragma once

#include <inviwo/vortexfeatures/vortexfeaturesmoduledefine.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/boolcompositeproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/listproperty.h>
#include <modules/brushingandlinking/ports/brushingandlinkingports.h>

#include <inviwo/dataframe/datastructures/dataframe.h>
#include <inviwo/dataframe/properties/dataframeproperty.h>

namespace inviwo {

/** \docpage{org.inviwo.VortexSelector, Vortex Selector}
 * ![](org.inviwo.VortexSelector.png?classIdentifier=org.inviwo.VortexSelector)
 * Filter vortices from a DataFrame based on the selection/filtering in the "ensemble" DataFrame
 *
 * ### Inports
 *   * __ensemble__   DataFrame with the active selection/filtering
 *   * __vortices__  Vortex DataFrame which is filtered
 *   * __brushing__   inport for brushing & linking on the ensemble DataFrame
 *
 * ### Outports
 *   * __outport__  DataFrame with selected vortices
 *
 */
class IVW_MODULE_VORTEXFEATURES_API VortexSelector : public Processor {
public:
    enum class FilterAction { Drop, ZeroOut };
    enum class FilterPredicate { Equal, Less, Greater, NotEqual };

    VortexSelector();
    virtual ~VortexSelector() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    DataFrameInport inportEnsemble_;
    DataFrameInport inportVortices_;
    BrushingAndLinkingInport brushingPort_;
    DataFrameOutport outport_;

    DataFrameColumnProperty key_;
    DataFrameColumnProperty keyEnsemble_;
    IntProperty ensembleId_;
    BoolCompositeProperty dataFilter_;
    DataFrameColumnProperty filterKey_;
    TemplateOptionProperty<FilterAction> filterAction_;
    TemplateOptionProperty<FilterPredicate> predicate_;
    FloatProperty predicateValue_;
};

}  // namespace inviwo
