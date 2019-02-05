/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2014-2018 Inviwo Foundation
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

#ifndef IVW_VTKXMLRECTILINEARGRIDREADER_H
#define IVW_VTKXMLRECTILINEARGRIDREADER_H

#include <modules/tensorvisio/tensorvisiomoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/fileproperty.h>
#include <inviwo/core/ports/dataoutport.h>
#include <inviwo/core/properties/buttonproperty.h>
#include <modules/tensorvisbase/ports/tensorfieldport.h>
#include <inviwo/core/properties/buttonproperty.h>

namespace inviwo {

/** \docpage{org.inviwo.VTKXMLRectilinearGridReader, VTK Rectilinear Grid Reader}
 * ![](org.inviwo.VTKXMLRectilinearGridReader.png?classIdentifier=org.inviwo.VTKXMLRectilinearGridReader)
 *
 * ...
 *
 * ### Properties
 *   * __Case file__ ...
 *
 */
class IVW_MODULE_TENSORVISIO_API VTKXMLRectilinearGridReader : public Processor {
public:
    VTKXMLRectilinearGridReader();
    virtual ~VTKXMLRectilinearGridReader();

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

protected:
    virtual void initializeResources() override;
    virtual void process() override;

	private:
    FileProperty file_;
    TensorField3DOutport outport_;
    ButtonProperty loadArrays_;
    ButtonProperty loadData_;
    OptionPropertyInt arrays_;

};

}  // namespace

#endif  // IVW_VTKXMLRECTILINEARGRIDREADER_H
