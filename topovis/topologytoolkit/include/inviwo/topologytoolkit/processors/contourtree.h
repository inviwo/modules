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

#ifndef IVW_CONTOURTREE_H
#define IVW_CONTOURTREE_H

#include <inviwo/topologytoolkit/topologytoolkitmoduledefine.h>
#include <inviwo/topologytoolkit/ports/triangulationdataport.h>

#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/ports/meshport.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/compositeproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/minmaxproperty.h>
#include <inviwo/core/interaction/pickingmapper.h>
#include <inviwo/dataframe/datastructures/dataframe.h>
#include <inviwo/core/properties/buttonproperty.h>

#include <inviwo/topologytoolkit/ports/contourtreeport.h>

#include <warn/push>
#include <warn/ignore/all>
#include <ttk/core/base/ftmTree/FTMTree.h>
#include <warn/pop>

namespace inviwo {

/** \docpage{org.inviwo.ContourTree, Contour Tree}
 * ![](org.inviwo.ContourTree.png?classIdentifier=org.inviwo.ContourTree)
 * Computes the contour tree for a given TTK triangulation.
 *
 * \see ttk::PersistenceCurve
 *
 * ### Inports
 *   * __triangulation__   input triangulation
 *
 * ### Outports
 *   * __outport__     DataFrame with critical points
 *   * __meshOutport__ Mesh containing the critical points and associated scalar values
 *                     as spheres and the connecting arcs as lines
 *
 * ### Properties
 *	 * __Contour Tree__
 *		+ __Tree Type__ Defines which tree type to calculate
 *		+ __Number of Threads__ Defines how many threads to use when calculating the tree
 *
 */

/**
 * \class ContourTree
 * \brief computes the contour tree for a given TTK triangulation
 */
class IVW_MODULE_TOPOLOGYTOOLKIT_API ContourTree : public Processor {
public:
    ContourTree();
    virtual ~ContourTree() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    topology::TriangulationInport inport_;
    topology::ContourTreeOutport outport_;

    TemplateOptionProperty<topology::TreeType> treeType_;
    IntProperty threadCount_;
    BoolProperty segmentation_;
    BoolProperty normalization_;

    std::shared_ptr<topology::ContourTreeData> treeData_;
    bool treeIsFinished_ = true;
    bool inportChanged_ = false;

    bool dirty_ = true;
};

}  // namespace inviwo

#endif  // IVW_CONTOURTREE_H
