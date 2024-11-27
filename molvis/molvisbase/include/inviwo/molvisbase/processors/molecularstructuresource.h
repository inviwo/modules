/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020-2024 Inviwo Foundation
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

#include <inviwo/molvisbase/molvisbasemoduledefine.h>
#include <inviwo/core/processors/processor.h>
#include <modules/base/processors/datasource.h>

#include <inviwo/molvisbase/ports/molecularstructureport.h>

namespace inviwo {

/** \docpage{org.inviwo.MolecularStructureSource, MolecularStructureSource}
 * ![](org.inviwo.MolecularStructureSource.png?classIdentifier=org.inviwo.MolecularStructureSource)
 * Loads a molecular structures/atoms and creates a MolecularStructure object.
 *
 * ### Outports
 *   * __outport__  molecular structure
 *
 * ### Properties
 *   * __File name__ File to load.
 */
class IVW_MODULE_MOLVISBASE_API MolecularStructureSource
    : public DataSource<molvis::MolecularStructure, molvis::MolecularStructureOutport> {
public:
    MolecularStructureSource(InviwoApplication* app, const std::filesystem::path& file = {});
    virtual ~MolecularStructureSource() = default;

    virtual const ProcessorInfo& getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;
};

}  // namespace inviwo
