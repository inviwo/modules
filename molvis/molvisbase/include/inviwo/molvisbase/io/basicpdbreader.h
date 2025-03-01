/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020-2025 Inviwo Foundation
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
#include <inviwo/core/io/datareader.h>
#include <inviwo/molvisbase/datastructures/molecularstructure.h>

#include <string>

namespace inviwo {

/**
 * \ingroup dataio
 *
 * Basic reader for Protein Databank (PDB) files. Will only interpret ATOM and HETATM data.
 * To support large PDB files, atom serial numbers > 99,999 and residue IDs > 9,999 are interpreted
 * as hexadecimal numbers. This enables support for files generated for example by VMD, but is
 * non-standard behavior.
 *
 * \see https://www.wwpdb.org/documentation/file-format-content/format33/sect9.html#ATOM
 */
class IVW_MODULE_MOLVISBASE_API BasicPDBReader : public DataReaderType<molvis::MolecularStructure> {
public:
    BasicPDBReader();
    BasicPDBReader(const BasicPDBReader&) = default;
    BasicPDBReader(BasicPDBReader&&) noexcept = default;
    BasicPDBReader& operator=(const BasicPDBReader&) = default;
    BasicPDBReader& operator=(BasicPDBReader&&) noexcept = default;
    virtual BasicPDBReader* clone() const override;
    virtual ~BasicPDBReader() = default;
    using DataReaderType<molvis::MolecularStructure>::readData;

    virtual std::shared_ptr<molvis::MolecularStructure> readData(
        const std::filesystem::path& fileName) override;
};

}  // namespace inviwo
