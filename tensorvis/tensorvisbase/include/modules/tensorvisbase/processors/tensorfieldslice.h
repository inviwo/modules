/*********************************************************************************
*
* Inviwo - Interactive Visualization Workshop
*
* Copyright (c) 2016-2018 Inviwo Foundation
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

#ifndef IVW_TENSORFIELDSLICE_H
#define IVW_TENSORFIELDSLICE_H

#include <modules/tensorvisbase/util/tensorutil.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <modules/tensorvisbase/tensorvisbasemoduledefine.h>
#include <modules/tensorvisbase/util/tensorfieldutil.h>
#include <inviwo/core/ports/meshport.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/ports/dataoutport.h>
#include <inviwo/core/datastructures/geometry/geometrytype.h>
#include <modules/tensorvisbase/ports/tensorfieldport.h>
#include <modules/tensorvisbase/ports/tensorfieldport.h>

namespace inviwo {

    /** \docpage{org.inviwo.TensorFieldSource, TensorFieldSource}
    * ![](org.inviwo.<name>.png?classIdentifier=org.inviwo.TensorFieldSource)
    * Explanation of how to use the processor.
    *
    * ### Inports
    *   * __<Inport1>__ <description>.
    *
    * ### Outports
    *   * __<Outport1>__ <description>.
    *
    * ### Properties
    *   * __<Prop1>__ <description>.
    *   * __<Prop2>__ <description>
    */

    /**
    * \class TensorFieldSlice
    * \brief VERY_BRIEFLY_DESCRIBE_THE_CLASS
    * DESCRIBE_THE_CLASS_FROM_A_DEVELOPER_PERSPECTIVE
    */
    class IVW_MODULE_TENSORVISBASE_API TensorFieldSlice : public Processor {
    public:
        TensorFieldSlice();
        virtual ~TensorFieldSlice() = default;

        virtual void process() override;

        virtual const ProcessorInfo getProcessorInfo() const override;
        static const ProcessorInfo processorInfo_;

    private:
        TensorField3DInport inport_;
        TensorField2DOutport outport2D_;
        TensorField3DOutport outport3D_;

        MeshOutport meshOutport_;
        MeshOutport planeOutport_;
        DataOutport<size_t> offsetOutport_;

        TemplateOptionProperty<CartesianCoordinateAxis> sliceAlongAxis_;
        OrdinalProperty<size_t> sliceNr_;
        FloatVec4Property color_;
    };

}  // namespace

#endif  // IVW_TENSORFIELDSLICE_H
