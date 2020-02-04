/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2017 Inviwo Foundation
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

#include <inviwo/heightfieldrendering/heightfieldrenderingmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/ports/meshport.h>
#include <inviwo/core/ports/imageport.h>
#include <inviwo/core/properties/transferfunctionproperty.h>
#include <inviwo/core/properties/ordinalproperty.h>

namespace inviwo {

    /** \docpage{org.inviwo.ImageToPoints, Image To Points}
     * ![](org.inviwo.ImageToPoints.png?classIdentifier=org.inviwo.ImageToPoints)
     * Gives out one point in 3D space per input pixel where the pixel coordinates
     * correspond to the point's x and y coordinates and the point's z coordinate is given by the pixel
     * value * heightScale.
     *
     * ### Inports
     *   * __Image__ Single channel input image defining the heightfield.
     *
     * ### Outports
     *   * __Points__ Mesh containing one point per pixel.
     *
     * ### Properties
     *   * __Transfer function__ Assigns colors to the points according to the heightfield.
     *   * __Height scale__ Sets the scaling parameter for the heightfield.
     */

     /**
      * \class ImageToPoints
      * \brief Creates a point in 3D space for every input pixel
      * Gives out one point in 3D space per input pixel where the pixel coordinates
      * correspond to the point's x and y coordinates and the point's z coordinate is given by the pixel
      * value * heightScale.
      */
    class IVW_MODULE_HEIGHTFIELDRENDERING_API ImageToPoints : public Processor {
    public:
        ImageToPoints();
        virtual ~ImageToPoints() = default;

        virtual void process() override;

        virtual const ProcessorInfo getProcessorInfo() const override;
        static const ProcessorInfo processorInfo_;

    private:
        ImageInport inport_;
        MeshOutport outport_;

        TransferFunctionProperty transferFunctionProperty_;

        FloatProperty heightScale_;
    };

}  // namespace inviwo
