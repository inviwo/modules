/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2025 Inviwo Foundation
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

#include <inviwo/tensorvisbase/tensorvisbasemoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/tensorvisbase/datastructures/invariantspace.h>
#include <inviwo/tensorvisbase/ports/tensorfieldport.h>

namespace inviwo {

/** \docpage{org.inviwo.InvariantSpaceFilter, Invariant Space Filter}
 * ![](org.inviwo.InvariantSpaceFilter.png?classIdentifier=org.inviwo.InvariantSpaceFilter)
 */
class IVW_MODULE_TENSORVISBASE_API InvariantSpaceFilter : public Processor {
public:
    InvariantSpaceFilter();
    virtual ~InvariantSpaceFilter() = default;

    virtual void process() override;

    virtual const ProcessorInfo& getProcessorInfo() const override;

    static const ProcessorInfo processorInfo_;

private:
    InvariantSpaceInport invariantSpaceInport_;
    TensorField3DInport tensorField3DInport_;

    InvariantSpaceOutport invariantSpaceOutport_;
};

}  // namespace inviwo
