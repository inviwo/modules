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

#include <inviwo/topologytoolkit/topologytoolkitmoduledefine.h>
#include <inviwo/topologytoolkit/ports/triangulationdataport.h>

#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/poolprocessor.h>
#include <inviwo/core/ports/meshport.h>
#include <inviwo/core/ports/volumeport.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/compositeproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/stringproperty.h>

#include <warn/push>
#include <warn/ignore/all>
#include <ttk/core/base/trackingFromFields/TrackingFromFields.h>
#include <ttk/core/base/trackingFromPersistenceDiagrams/TrackingFromPersistenceDiagrams.h>
#include <ttk/core/base/persistenceDiagram/PersistenceDiagram.h>
#include <warn/pop>

namespace inviwo {

/** \docpage{org.inviwo.TrackingFromFields, Tracking From Fields}
 * ![](org.inviwo.TrackingFromFields.png?classIdentifier=org.inviwo.TrackingFromFields)
 * Computes a tracking based on a TTK triangulation and multiple scalar fields.
 * The tracking is based on persistance diagrams. Matching are created with the Munkres algorithm
 * and the Bottleneck distance with the Wasserstein metric.
 *
 * Related publication:
 *      'Lifted Wasserstein Matcher for Fast and Robust Topology Tracking'
 *      Maxime Soler, Melanie Plainchault, Bruno Conche, Julien Tierny
 *      Proc. of IEEE Symposium on Large Data Analysis and Visualization 2018.
 *
 * ### Inports
 *   * __scalars__ multiple scalar fields all of same dimension as triangulation
 *
 * ### Outports
 *   * __tracking__ output tracks
 *
 * ### Properties
 *	 * __diagrams__
 *      + __<Prop1>__ Time sampling
 *      + __<Prop2>__ Persistance threshold (% of function span)
 *	 * __matching__
 *      + __<Prop1>__ p parameter (inf, 2, ...) for Wasserstein metric
 *      + __<Prop1>__ alpha parameter (0 -> geometry, 1 -> persistance diagram distance)
 *      + __<Prop2>__ extremum weight
 *      + __<Prop1>__ saddle weight
 *      + __<Prop2>__ x weight
 *      + __<Prop2>__ y weight
 *      + __<Prop2>__ z weight
 *	 * __output__
 *      + __<Prop2>__ Force z translation
 *      + __<Prop2>__ Overwrite Transformation
 */
class IVW_MODULE_TOPOLOGYTOOLKIT_API TrackingFromFields : public PoolProcessor {
public:
    TrackingFromFields();
    virtual ~TrackingFromFields() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

    template <typename dataType>
    static void buildMesh(std::vector<trackingTuple> &trackings,
                   std::vector<std::vector<matchingTuple>> &outputMatchings,
                   std::vector<std::vector<diagramTuple>> &inputPersistenceDiagrams,
                          bool useGeometricSpacing, double spacing, bool DoPostProc,
                          std::vector<std::set<int>> &trackingTupleToMerged, Mesh &outputMesh);

private:
    VolumeSequenceInport scalarsInport_;

    MeshOutport outport_;

    CompositeProperty diagrams_;
    /* Temporal sampling (take every N timestep). */
    IntSizeTProperty timeSampling_;
    /* Discard pairs below this threshold (percentage of the function span). */
    FloatProperty persistenceThreshold_;

    CompositeProperty matching_;
    /* Value of the parameter p for the Wp (p-th Wasserstein) distance computation (type "inf" for
     * the Bottleneck distance). */
    StringProperty pParameter_;

    /* Blending coefficient for the cost evaluation of each critical point matching.
    By default (1), only distances in the persistence diagrams are considered between matched
    critical points.
    When set to 0, only distances in the original 3D domain are considered between matched critical
    points. */
    FloatProperty alpha_;
    /* Importance weight for extrema */
    FloatProperty extremumWeight_;
    /* Importance weight for saddles */
    FloatProperty saddleWeight_;
    /*Importance weight for the individual coordinate components of the extremum */
    FloatProperty xWeight_;
    FloatProperty yWeight_;
    FloatProperty zWeight_;

    CompositeProperty output_;
    /* Translation of the second set of critical points if the persistence diagrams are embedded
    in the original domain.
    This is useful to visualize the matching between the diagrams of two 2D scalar fields. */
    BoolProperty forceZTranslation_;
    FloatProperty zTranslation_;
    // Additional properties on the output could be possible
    // BoolProperty filterByLength_;
    BoolProperty overwriteTransform_;

    ttk::TrackingFromFields trackingF_;
    ttk::TrackingFromPersistenceDiagrams tracking_;
};

}  // namespace inviwo
