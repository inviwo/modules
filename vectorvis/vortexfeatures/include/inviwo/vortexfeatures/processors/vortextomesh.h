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
#include <inviwo/vortexfeatures/ports/vortexport.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/boolcompositeproperty.h>
#include <inviwo/core/ports/meshport.h>
#include <inviwo/core/interaction/pickingmapper.h>
// #include <modules/brushingandlinking/brushingandlinkingmanager.h>
#include <modules/brushingandlinking/ports/brushingandlinkingports.h>
#include <inviwo/core/datastructures/geometry/basicmesh.h>
#include <inviwo/dataframe/datastructures/dataframe.h>

namespace inviwo {

/** \docpage{org.inviwo.VortexToMesh, Vortex To Mesh}
 * ![](org.inviwo.VortexToMesh.png?classIdentifier=org.inviwo.VortexToMesh)
 * Explanation of how to use the processor.
 */
class IVW_MODULE_VORTEXFEATURES_API VortexToMesh : public Processor {
public:
    VortexToMesh();
    virtual ~VortexToMesh() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

    static vec4 getTurningColor(Vortex::Turning rotation);

    enum class ShowSlices3D : char { None, Time, Height };
    enum class BoundaryRepresentation : char { Lines, Meshes };
    enum class CenterRepresentation : char { Points, Lines };
    enum class GroupDisplay : char { All, HighlightFirst, OnlyFirst, Score };
    enum class ColorSource : char { Group, Rotation };
    enum class SelectionHighlight : char { Ignore, FilterSelect };

private:
    using PickingMesh = TypedMesh<buffertraits::PositionsBuffer, buffertraits::ColorsBuffer,
                                  buffertraits::PickingBuffer>;
    // using SurfaceMesh = TypedMesh<buffertraits::PositionsBuffer, buffertraits::PickingBuffer>;

    bool keepVortex(const Vortex& vort) const;
    size_t getPos3D(const Vortex& vort) const;
    size_t getSelectionID(size_t vortexID) const;
    vec4 getColor(bool firstVort, const vec4& groupColor, const Vortex& vort,
                  const dvec2& scoreRange);
    void createLineBoundaryMesh(const std::vector<vec4>& colors, const mat4& modelMat,
                                const mat4& worldMat);
    void createSurfaceBoundaryMesh(const std::vector<vec4>& colors, const mat4& modelMat,
                                   const mat4& worldMat);
    void createPointCenterMesh(const std::vector<vec4>& colors, const mat4& modelMat,
                               const mat4& worldMat);
    void createLineCenterMesh(const std::vector<vec4>& colors, const mat4& modelMat,
                              const mat4& worldMat);
    void objectPicked(PickingEvent* p);

    VortexSetInport vorticesIn_;
    BrushingAndLinkingInport selectionIn_, selectionLocalIdIn_;
    DataInport<DataFrame> dataFrameIn_;
    MeshOutport boundaryOut_, centerOut_;

    TemplateOptionProperty<BoundaryRepresentation> boundaryRep_;
    TemplateOptionProperty<CenterRepresentation> centerRep_;
    TemplateOptionProperty<ShowSlices3D> showSlices3D_;
    TemplateOptionProperty<GroupDisplay> groupDisplay_;
    TemplateOptionProperty<ColorSource> colorSource_;
    TemplateOptionProperty<SelectionHighlight> selectionHighlight_;
    IntSizeTProperty colorSeed_;

    IntSizeTProperty ensembleMember_;
    IntSizeTProperty timeSlice_;
    IntSizeTProperty heightSlice_;
    struct SingleGroupProperty : public BoolCompositeProperty {
        SingleGroupProperty(const std::string& identifier, const std::string& displayName);
        IntSizeTProperty selectedGroup_;
    } singleGroupProperty_;

    BoolProperty overrideScaleZ_;
    DoubleProperty scaleZ_;

    BoolProperty skipLastGroup_;

    PickingMapper picking_;
};

}  // namespace inviwo
