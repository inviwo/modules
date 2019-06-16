/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018-2019 Inviwo Foundation
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

#ifndef IVW_PERSISTENCEDIAGRAM_H
#define IVW_PERSISTENCEDIAGRAM_H

#include <inviwo/topologytoolkit/topologytoolkitmoduledefine.h>
#include <inviwo/topologytoolkit/ports/persistencediagramport.h>
#include <inviwo/topologytoolkit/ports/triangulationdataport.h>

#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>

#include <inviwo/dataframe/datastructures/dataframe.h>

namespace inviwo {

/** \docpage{org.inviwo.ttk.PersistenceDiagram, Persistence Diagram}
 * ![](org.inviwo.ttk.PersistenceDiagram.png?classIdentifier=org.inviwo.ttk.PersistenceDiagram)
 * Computes the persistence diagram for a given TTK triangulation.
 *
 * \see ttk::PersistenceDiagram
 *
 * ### Inports
 *   * __triangulation__   input triangulation
 *
 * ### Outports
 *   * __outport__   resulting persistence diagram
 *   * __dataframe__ DataFrame with birth and death of the extremum-saddle pairs. The
 *                   persistence diagram can be created of these by setting X to birth and
 *                   drawing vertical lines from birth to death
 */

/**
 * \class PersistenceDiagram
 * \brief computes the persistence diagram for a given TTK triangulation
 */
class IVW_MODULE_TOPOLOGYTOOLKIT_API PersistenceDiagram : public Processor {
public:
    PersistenceDiagram();
    virtual ~PersistenceDiagram() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    topology::TriangulationInport inport_;
    topology::PersistenceDiagramOutport outport_;

    DataFrameOutport dataFrameOutport_;
};

}  // namespace inviwo

#endif  // IVW_PERSISTENCEDIAGRAM_H
