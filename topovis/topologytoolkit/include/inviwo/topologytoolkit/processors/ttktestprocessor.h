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

#ifndef IVW_TTKTESTPROCESSOR_H
#define IVW_TTKTESTPROCESSOR_H

#include <inviwo/topologytoolkit/topologytoolkitmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/ports/meshport.h>

namespace inviwo {

/** \docpage{org.inviwo.ttk.TTKTestProcessor, TTKTest Processor}
 * ![](org.inviwo.ttk.TTKTestProcessor.png?classIdentifier=org.inviwo.ttk.TTKTestProcessor)
 * Example processor for using the Topology ToolKit (TTK). It is based on the C++ example
 * shipped with TTK. Will modify the input height field and write the resulting triangle mesh
 * as OFF file to disk.
 *
 * ### Inports
 *   * __mesh__     input geometry
 *
 * ### Outports
 *   * __outport__  processed input geometry
 *
 * ### Properties
 *   * __Filename__   filename of input height field (in OFF format)
 */

/**
 * \class TTKTestProcessor
 * \brief VERY_BRIEFLY_DESCRIBE_THE_PROCESSOR
 * DESCRIBE_THE_PROCESSOR_FROM_A_DEVELOPER_PERSPECTIVE
 */
class IVW_MODULE_TOPOLOGYTOOLKIT_API TTKTestProcessor : public Processor {
public:
    TTKTestProcessor();
    virtual ~TTKTestProcessor() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    void processDataFromFile();

    int load(const std::string &inputPath, std::vector<float> &pointSet,
             std::vector<long long int> &triangleSet);
    int save(const std::vector<float> &pointSet, const std::vector<long long int> &triangleSet,
             const std::string &outputPath);

    MeshInport inport_;
    MeshOutport outport_;

    FloatVec4Property color_;
    FloatProperty threshold_;
};

}  // namespace inviwo

#endif  // IVW_TTKTESTPROCESSOR_H
