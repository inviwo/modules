/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2025 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 *FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#ifndef IVW_TENSORFIELD3DIMPORT_H
#define IVW_TENSORFIELD3DIMPORT_H

#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/fileproperty.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/tensorvisbase/ports/tensorfieldport.h>
#include <inviwo/tensorvisio/tensorvisiomoduledefine.h>

namespace inviwo {

/** \docpage{org.inviwo.TensorField3DImport, Tensor Field File Reader}
 * ![](org.inviwo.TensorField3DImport.png?classIdentifier=org.inviwo.TensorField3DImport)
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
 * \class TensorField3DImport
 * \brief <brief description>
 * <Detailed description from a developer prespective>
 */
class IVW_MODULE_TENSORVISIO_API TensorField3DImport : public Processor {
public:
    TensorField3DImport();
    virtual ~TensorField3DImport() = default;

    virtual void initializeResources() override;
    virtual void process() override;

    virtual const ProcessorInfo& getProcessorInfo() const override;

    static const ProcessorInfo processorInfo_;

private:
    FileProperty inFile_;

    TensorField3DOutport outport_;

    BoolProperty normalizeExtents_;

    FloatVec3Property extent_;

    FloatVec3Property offset_;
    IntVec3Property dimensions_;
    std::shared_ptr<TensorField3D> tensorFieldOut_;

    dvec3 dextents_;

    void buildTensors(const std::vector<double>& data, std::vector<dmat3>& tensors) const;
};

}  // namespace inviwo

#endif  // IVW_TENSORFIELD3DIMPORT_H
