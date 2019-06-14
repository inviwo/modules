/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019 Inviwo Foundation
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

#include <inviwo/topologytoolkit/topologytoolkitmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/dataframe/datastructures/dataframe.h>
#include <inviwo/core/properties/stringproperty.h>

#include <inviwo/topologytoolkit/ports/contourtreeport.h>

namespace inviwo {

/** \docpage{org.inviwo.ContourTreeToDataFrame, Contour Tree To DataFrame}
 * ![](org.inviwo.ContourTreeToDataFrame.png?classIdentifier=org.inviwo.ContourTreeToDataFrame)
 * Converts a TTK contour tree to a DataFrame with the following columns:
 * vertex ID (int), up (uchar), down (uchar), valence up (int), valence down (int), scalar value
 *
 * ### Inports
 *   * __contourTree__  Contour tree input
 *
 * ### Outports
 *   * __outport__   dataframe created extracted from contour tree
 *
 */

/**
 * \brief convert a TTK contour tree to a DataFrame
 */
class IVW_MODULE_TOPOLOGYTOOLKIT_API ContourTreeToDataFrame : public Processor {
public:
    ContourTreeToDataFrame();
    virtual ~ContourTreeToDataFrame() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    topology::ContourTreeInport inport_{"contourTree"};
    DataFrameOutport outport_{"outport"};

    StringProperty treeType_;
};

}  // namespace inviwo
