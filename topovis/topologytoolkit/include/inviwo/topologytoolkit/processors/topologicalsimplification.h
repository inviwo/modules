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

#ifndef IVW_TOPOLOGICALSIMPLIFICATION_H
#define IVW_TOPOLOGICALSIMPLIFICATION_H

#include <inviwo/topologytoolkit/topologytoolkitmoduledefine.h>
#include <inviwo/topologytoolkit/ports/persistencediagramport.h>
#include <inviwo/topologytoolkit/ports/triangulationdataport.h>

#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/processors/activityindicator.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/boolproperty.h>

namespace inviwo {

/** \docpage{org.inviwo.ttk.TopologicalSimplification, Topological Simplification}
 * ![](org.inviwo.ttk.TopologicalSimplification.png?classIdentifier=org.inviwo.ttk.TopologicalSimplification)
 * Removes critical points that have a persistence below the given threshold.
 * Used in conjunction with PersistenceDiagram.
 *
 * ### Inports
 *   * __triangulation__   input triangulation
 *   * __persistance__     matching persistence diagram
 *
 * ### Outports
 *   * __outport__   output triangulation with critical points below/above threshold removed
 *
 * ### Properties
 *   * __Threshold__   persistence threshold
 *   * __Invert__      if checked, critical points above the threshold are removed
 */

/**
 * \class TopologicalSimplification
 * \brief removes critical points with a persistence below the given threshold
 */
class IVW_MODULE_TOPOLOGYTOOLKIT_API TopologicalSimplification : public Processor,
                                                                 public ActivityIndicatorOwner {
public:
    TopologicalSimplification();
    virtual ~TopologicalSimplification() = default;

    virtual void process() override;

    virtual void invalidate(InvalidationLevel invalidationLevel,
                            Property* source = nullptr) override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    void updateOutport();

    topology::TriangulationInport inport_;
    topology::PersistenceDiagramInport persistenceInport_;
    topology::TriangulationOutport outport_;

    FloatProperty threshold_;
    BoolProperty invert_;

    std::future<std::shared_ptr<const topology::TriangulationData>> newTriangulation_;

    bool simplificationDirty_ = true;
    bool hasNewData_ = false;
};

}  // namespace inviwo

#endif  // IVW_TOPOLOGICALSIMPLIFICATION_H
