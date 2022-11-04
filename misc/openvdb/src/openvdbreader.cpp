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

#include <inviwo/openvdb/openvdbreader.h>
#include <inviwo/core/datastructures/volume/volumeramprecision.h>

#include <openvdb/openvdb.h>
#include <openvdb/tools/Dense.h>

namespace inviwo {
    
OpenVDBReader::OpenVDBReader() : DataReaderType<Volume>() {
    addExtension(FileExtension("vdb", "OpenVDB"));
}

std::shared_ptr<Volume> OpenVDBReader::readData(const std::string& filePath) {
    openvdb::io::File file(filePath);
    // Open the file.  This reads the file header, but not any grids.
    file.open();
    std::shared_ptr<Volume> volume(nullptr);
    // We actually just support one grid here, lets pick the first one.
    auto baseGrid = file.readGrid(file.beginName().gridName());
    
    using AllowedGridTypes = openvdb::TypeList<
    openvdb::Int32Grid, openvdb::Int64Grid,
    openvdb::FloatGrid, openvdb::DoubleGrid>;
    // Fill the grid if it is one of the allowed types.
    baseGrid->apply<AllowedGridTypes>(
       [&volume](auto& grid) { // C++14
           using GridType = typename std::decay<decltype(grid)>::type;

           auto bbox = grid.evalActiveVoxelBoundingBox();
           auto dim = bbox.dim();
           
           auto vol = std::make_shared<VolumeRAMPrecision<typename GridType::ValueType>>(size3_t(dim[0], dim[1], dim[2]));
           // Copy data to Inviwo Volume
           openvdb::tools::Dense<typename GridType::ValueType, openvdb::tools::MemoryLayout::LayoutXYZ> dense(bbox, vol->getDataTyped());
           openvdb::tools::copyToDense(grid, dense);
           // Setup necessary Volume parameters
           volume = std::make_shared<Volume>(vol);
           typename GridType::ValueType minVal, maxVal;
           grid.evalMinMax(minVal, maxVal);
           volume->dataMap_.dataRange = dvec2(minVal, maxVal);
           volume->dataMap_.valueRange = volume->dataMap_.dataRange;
           auto tr = grid.transformPtr();
           auto spacing = tr->voxelSize();
           mat3 basis(1.0f);
           for (unsigned int i = 0; i < 3; ++i) {
               basis[i][i] = volume->getDimensions()[i] * spacing[i];
           }
           
           volume->setBasis(basis);
           vec3 offset(0.0f);
           for (unsigned int i = 0; i < 3; ++i) {
               offset += basis[i];
           }
           volume->setOffset(-0.5f * offset);
       }
       );
    file.close();
    return volume;
}

}  // namespace inviwo
