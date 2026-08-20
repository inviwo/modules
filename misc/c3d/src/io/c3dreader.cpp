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

#include <inviwo/c3d/io/c3dreader.h>

#include <inviwo/core/util/filesystem.h>
#include <inviwo/core/util/fileextension.h>

#include <ezc3d/ezc3d.h>

#include <fmt/std.h>

namespace inviwo {

C3DReader::C3DReader() { addExtension(FileExtension("c3d", "C3D motion capture files (c3d)")); }

C3DReader* C3DReader::clone() const { return new C3DReader(*this); }

std::shared_ptr<C3D> C3DReader::readData(const std::filesystem::path& filePath) {
    const auto localPath = downloadAndCacheIfUrl(filePath);
    checkExists(localPath);
    return std::make_shared<C3D>(localPath, opts);
}

bool C3DReader::setOption(std::string_view key, std::any value) {
    if (auto* readAnalogs = std::any_cast<bool>(&value); readAnalogs && key == "ReadAnalogs") {
        opts.readAnalogs = *readAnalogs;
        return true;
    } else if (auto* readRotations = std::any_cast<bool>(&value);
               readRotations && key == "ReadRotations") {
        opts.readRotations = *readRotations;
        return true;
    }
    return false;
}
std::any C3DReader::getOption(std::string_view key) {
    if (key == "ReadAnalogs") {
        return opts.readAnalogs;
    } else if (key == "ReadRotations") {
        return opts.readAnalogs;
    }
    return {};
}

}  // namespace inviwo
