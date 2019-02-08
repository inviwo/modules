/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018 Inviwo Foundation
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

#ifndef IVW_OPENMESHWRITER_H
#define IVW_OPENMESHWRITER_H

#include <modules/openmesh/openmeshmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/io/datawriter.h>
#include <inviwo/core/datastructures/geometry/mesh.h>
#include <inviwo/core/util/logcentral.h>

namespace inviwo {

/**
 * \class OpenMeshWriter
 * \ingroup dataio
 * \brief Writer for various mesh types using the OpenMesh library
 */
class IVW_MODULE_OPENMESH_API OpenMeshWriter : public DataWriterType<Mesh> {
public:
    OpenMeshWriter();
    OpenMeshWriter(const OpenMeshWriter& rhs) = default;
    OpenMeshWriter& operator=(const OpenMeshWriter& that) = default;
    virtual OpenMeshWriter* clone() const override;
    virtual ~OpenMeshWriter() = default;

    virtual void writeData(const Mesh* data, const std::string filePath) const override;
/*
    virtual std::unique_ptr<std::vector<unsigned char>> writeDataToBuffer(
        const Mesh*, const std::string&) const override;

    virtual bool writeDataToRepresentation(const repr*, repr*) const override;*/
};

}  // namespace inviwo

#endif  // IVW_OPENMESHWRITER_H
