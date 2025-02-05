/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020-2025 Inviwo Foundation
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

#include <inviwo/dataframeclustering/dataframeclusteringmoduledefine.h>
#include <inviwo/dataframe/datastructures/dataframe.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/boolcompositeproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/ports/imageport.h>

#include <modules/python3/pythonscript.h>

namespace inviwo {

/** \docpage{org.inviwo.DataFrameClustering, Data Frame Clustering}
 * ![](org.inviwo.DataFrameClustering.png?classIdentifier=org.inviwo.DataFrameClustering)
 */
class IVW_MODULE_DATAFRAMECLUSTERING_API DataFrameClustering : public Processor {
public:
    DataFrameClustering();
    virtual ~DataFrameClustering() = default;

    virtual void process() override;

    virtual const ProcessorInfo& getProcessorInfo() const override;

    static const ProcessorInfo processorInfo_;

private:
    DataFrameInport dataFrame_;
    DataFrameOutport newDataFrame_;

    OptionPropertyString method_;

    IntProperty numberOfClusters_;

    CompositeProperty kmeans_;
    CompositeProperty dbscan_;
    CompositeProperty agglomerative_;
    CompositeProperty spectral_;

    StringProperty columnName_;

    FloatProperty eps_;
    IntProperty N_;

    IntProperty numberOfFoundClusters_;

    OptionPropertyString linkage_;

    CompositeProperty columns_;

    PythonScript script_;

    void onDataFrameChange();
};

}  // namespace inviwo
