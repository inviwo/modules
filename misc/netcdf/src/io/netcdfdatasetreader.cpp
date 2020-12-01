/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2013-2020 Inviwo Foundation
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

#include <inviwo/netcdf/io/netcdfdatasetreader.h>
// #include <modules/discretedata/connectivity
#include <modules/discretedata/connectivity/structuredgrid.h>
#include <inviwo/core/util/filesystem.h>
// #include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/io/datareaderexception.h>
#include <modules/python3/pythonscript.h>

#include <pybind11/pybind11.h>
namespace py = pybind11;
using namespace pybind11::literals;

#include <fstream>

namespace inviwo {
using namespace discretedata;

NetCDFDataSetReader::NetCDFDataSetReader() : DataReaderType<DataSetInitializer>() {
    addExtension(FileExtension("nc", "NetCDF scientific data format"));
}

NetCDFDataSetReader* NetCDFDataSetReader::clone() const { return new NetCDFDataSetReader(*this); }

std::shared_ptr<DataSetInitializer> NetCDFDataSetReader::readData(const std::string& filePath) {
    if (!filesystem::fileExists(filePath)) {
        throw DataReaderException("Error, could not find input file: " + filePath, IVW_CONTEXT);
    }

    std::ifstream stream("modules/misc/netcdf/python/scripts/netcdfdatasetreader.py");
    std::string source;

    stream.seekg(0, std::ios::end);
    source.reserve(stream.tellg());
    stream.seekg(0, std::ios::beg);

    source.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

    PythonScript script_;
    // std::string sciptSource = "modules/misc/netcdf/python/scripts/netcdfdatasetreader.py"
    script_.setSource(source);

    std::unordered_map<std::string, pybind11::object> vars = {
        {"filePath", pybind11::cast(filePath)}};

    auto data = std::make_shared<DataSetInitializer>();
    std::array<ind, 3> size = {128, 256, 64};
    auto grid = std::make_shared<StructuredGrid<3>>(size);
    data->grid_ = grid;  // std::make_shared<StructuredGrid<2>>(3, 3);

    script_.run(vars, [this, &data](pybind11::dict results) {
        LogWarn("Hullo!");
        for (auto item : results) {
            // LogWarn("key: " << item.first << ", value=" << item.second);
            py::print("key: {}, value={}"_s.format(item.first, item.second));
        };
    });

    return data;
}

}  // namespace inviwo
