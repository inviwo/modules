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

#include <modules/openmesh/openmeshreader.h>

#include <modules/openmesh/utils/openmeshconverters.h>

#define _USE_MATH_DEFINES
#include <warn/push>
#include <warn/ignore/all>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/IO/IOManager.hh>
#include <warn/pop>

namespace inviwo {

OpenMeshReader::OpenMeshReader() : DataReaderType<Mesh>() {
    auto fileTypes = splitString(OpenMesh::IO::IOManager().qt_read_filters(), ';');
    for (auto fileType : fileTypes) {
        if (fileType == "") continue;
        fileType = trim(fileType);

        auto p = fileType.find("(");
        auto name = trim(fileType.substr(0, p));
        if (name == "All files") continue;
        auto exts = splitString(fileType.substr(p + 1, fileType.find(")") - p - 1));
        for (auto ext : exts) {
            if (ext == "") continue;
            replaceInString(ext, "*.", "");
            addExtension(FileExtension(ext, name));
        }
    }
}

std::shared_ptr<inviwo::Mesh> OpenMeshReader::readData(const std::string& filePath) {
    TriMesh mesh;
    if (!OpenMesh::IO::read_mesh(mesh, filePath)) {
        throw inviwo::Exception("Failed reading mesh from disk " + filePath, IvwContext);
    }
    mesh.request_vertex_normals();
    mesh.request_face_normals();
    mesh.update_face_normals();
    mesh.update_vertex_normals();
    return openmeshutil::toInviwo(mesh);
}

}  // namespace inviwo
