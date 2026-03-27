/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2026 Inviwo Foundation
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

#include <inviwo/c3d/c3dmoduledefine.h>
#include <inviwo/core/io/datareader.h>
#include <inviwo/c3d/datastructures/c3ddata.h>

namespace inviwo {

/**
 * @ingroup dataio
 * @brief Reader for C3D (Coordinate 3D) files using the ezc3d library.
 *
 * Reads biomechanics C3D files containing 3D marker positions and analog data.
 *
 * @see https://www.c3d.org
 * @see https://github.com/pyomeca/ezc3d
 */
class IVW_MODULE_C3D_API C3DReader : public DataReaderType<ezc3d::c3d> {
public:
    C3DReader();
    C3DReader(const C3DReader&) = default;
    C3DReader(C3DReader&&) noexcept = default;
    C3DReader& operator=(const C3DReader&) = default;
    C3DReader& operator=(C3DReader&&) noexcept = default;
    virtual C3DReader* clone() const override;
    virtual ~C3DReader() = default;
    using DataReaderType<ezc3d::c3d>::readData;

    virtual std::shared_ptr<ezc3d::c3d> readData(const std::filesystem::path& filePath) override;
};

}  // namespace inviwo
