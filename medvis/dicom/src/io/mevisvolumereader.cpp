/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2014-2026 Inviwo Foundation
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
 *****************************************************************************
 *  This tif/dcm (MevisLab) reader is based on
 *  https://github.com/mstaring/elastix/tree/master/src/Common/MevisDicomTiff/
 *  (the corresponding implementation for itk using gdcm and libtiff)
 *
 *  Please not that they used the following license for their work:
 *****************************************************************************
 *
 * Copyright UMC Utrecht and contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *********************************************************************************/

#include <inviwo/dicom/io/mevisvolumereader.h>
#include <inviwo/dicom/errorlogging.h>
#include <inviwo/dicom/io/gdcmvolumereader.h>

#include <inviwo/core/util/filesystem.h>
#include <inviwo/core/util/formatconversion.h>
#include <inviwo/core/util/stringconversion.h>
#include <inviwo/core/util/raiiutils.h>
#include <inviwo/core/util/assertion.h>
#include <inviwo/core/io/datareaderexception.h>

#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/datastructures/volume/volumeramprecision.h>
#include <inviwo/core/datastructures/volume/volumeram.h>

#include <warn/push>
#include <warn/ignore/all>
#include <gdcmImage.h>
#include <gdcmImageReader.h>
#include <gdcmAttribute.h>
#include <gdcmMediaStorage.h>

#include <tiff.h>
#include <tiffio.h>
#include <warn/pop>

#include <fmt/format.h>
#include <fmt/std.h>

namespace inviwo {

MevisVolumeReader::MevisVolumeReader()
    : DataReaderType<Volume>()
    , dcm_file_(std::string())
    , tif_file_(std::string())
    , format(nullptr)
    , dimension_(uvec3(0, 0, 0)) {
    // in case one opens the dcm file the dicom reader will automatically point to readMetaData
    // only a (unnecessary) warning about missing pixel data will show up from the gdcm library
    addExtension(FileExtension("tif", "Mevislab DICOM/TIF file format"));
    addExtension(FileExtension("tiff", "Mevislab DICOM/TIF file format"));

    enableGdcmLogging(LogVerbosity::Error);
    enableTiffLogging(LogVerbosity::Error);
}

MevisVolumeReader* MevisVolumeReader::clone() const { return new MevisVolumeReader(*this); }

std::shared_ptr<Volume> MevisVolumeReader::readData(const std::filesystem::path& filePath) {
    auto formatErrorMsg = [](const auto& filename, const auto& msg) {
        return fmt::format("{} ('{}')", msg, filename);
    };

    if (!setFilenames(filePath)) {
        throw DataReaderException(formatErrorMsg(
            filePath, "could not locate associated tif and dcm files to read Mevis DICOM format"));
    }

    TIFF* tiffimage = TIFFOpen(tif_file_.string().c_str(), "rc");
    if (!tiffimage) {
        throw DataReaderException(formatErrorMsg(tif_file_, "invalid tif file"));
    }

    util::OnScopeExit closeTiffImg([&]() {
        if (tiffimage) {
            TIFFClose(tiffimage);
            tiffimage = nullptr;
        }
    });

    unsigned int samplecount = 1;
    unsigned int bitspersample = 16;
    {  // get some info out of the tif file
        if (!TIFFGetField(tiffimage, TIFFTAG_IMAGEWIDTH, &(dimension_).x)) {
            throw DataReaderException(
                formatErrorMsg(tif_file_, "failed to get TIFFTAG_IMAGEWIDTH"));
        }
        if (!TIFFGetField(tiffimage, TIFFTAG_IMAGELENGTH, &(dimension_).y)) {
            throw DataReaderException(
                formatErrorMsg(tif_file_, "failed to get TIFFTAG_IMAGELENGTH"));
        }
        if (!TIFFGetField(tiffimage, TIFFTAG_IMAGEDEPTH, &(dimension_).z)) {
            dimension_.z = 1;
        }

        if (!TIFFGetField(tiffimage, TIFFTAG_SAMPLESPERPIXEL, &samplecount)) {
            throw DataReaderException(
                formatErrorMsg(tif_file_, "failed to get TIFFTAG_SAMPLESPERPIXEL"));
        }

        if (!TIFFGetField(tiffimage, TIFFTAG_BITSPERSAMPLE, &bitspersample)) {
            throw DataReaderException(
                formatErrorMsg(tif_file_, "failed to get TIFFTAG_BITSPERSAMPLE"));
        }
    }

    // the gdcm image reader *will* fail - saying no pixel data found
    gdcm::ImageReader reader;
    reader.SetFileName(dcm_file_.string().c_str());
    if (reader.Read()) {
        LogWarn("dcm file " << dcm_file_ << " contains an image/volume - maybe the data in "
                            << tif_file_ << " does not belong to this dcm file?")
    } else {
        LogInfo("You can savely ignore the last \"no pixel data found!\" warning from gdcm.");
        // as a alternative call gdcm::Trace::WarningOff(); before reader.Read()
    }

    // due to conflicting information from tif/dcm and problems like the one mentioned below
    // we trust the gdcm library and do only minimal comparative checking between tif and dcm.
    // :: voxel spacing problem:
    //  -
    //  http://sourceforge.net/p/gdcm/mailman/gdcm-developers/thread/31ddba770805020102l17919e28h5f7f9b6f79ef46e6@mail.gmail.com/
    //  - http://gdcm.sourceforge.net/wiki/index.php/Using_GDCM_API
    //  - http://itk-users.7.n7.nabble.com/How-to-calculate-volume-of-an-image-td17195.html

    // to prevent code duplication call our gdcm reader
    GdcmVolumeReader gdcmreader;
    gdcm::Image& image = reader.GetImage();
    gdcm::File& file = reader.GetFile();
    std::shared_ptr<Volume> volume(gdcmreader.generateVolume(image, file));

    // TODO: check that everything else is also freed (reader instance that got cloned in
    // generateVolume...)

    format = (gdcmreader.getFormat());

    {  // compare gdcm's info to that stored in the tif file
        if ((dimension_) != gdcmreader.getDimension()) {
            throw DataReaderException(formatErrorMsg(filePath, "tif and dcm info differ (size)"));
        }

        if (samplecount != format->getComponents()) {
            throw DataReaderException(
                formatErrorMsg(filePath, "tif and dcm info differ (components)"));
        }

        if (bitspersample != 8 * (format->getSizeInBytes())) {
            throw DataReaderException(
                formatErrorMsg(filePath, "tif and dcm info differ (bitspersample)"));
        }
    }

    auto vd = std::make_shared<VolumeDisk>(tif_file_, dimension_, format);
    vd->setLoader(new MevisVolumeRAMLoader(tif_file_, dimension_, format));
    volume->addRepresentation(vd);

    return volume;
}

MevisVolumeRAMLoader::MevisVolumeRAMLoader(const std::filesystem::path& file, size3_t dimension,
                                           const DataFormatBase* format)
    : tif_file_(file), dimension_(dimension), format_(format) {}

MevisVolumeRAMLoader* MevisVolumeRAMLoader::clone() const {
    return new MevisVolumeRAMLoader(*this);
}

std::shared_ptr<VolumeRepresentation> MevisVolumeRAMLoader::createRepresentation(
    const VolumeRepresentation&) const {
    return dispatching::singleDispatch<std::shared_ptr<VolumeRepresentation>,
                                       dispatching::filter::All>(
        format_->getId(), [this]<typename T>() {
            const std::size_t size = glm::compMul(dimension_);
            auto data = std::make_unique<T[]>(size);
            if (!data) {
                throw DataReaderException(
                    IVW_CONTEXT,
                    "Error: Could not allocate memory for loading mevis volume data: ", tif_file_);
            }

            readDataInto(reinterpret_cast<char*>(data.get()));
            auto repr = std::make_shared<VolumeRAMPrecision<T>>(data.get(), dimension_);
            data.release();
            return repr;
        });
}

void MevisVolumeRAMLoader::updateRepresentation(std::shared_ptr<VolumeRepresentation> dest,
                                                const VolumeRepresentation&) const {
    auto volumeDst = std::static_pointer_cast<VolumeRAM>(dest);
    auto data = volumeDst->getData();

    readDataInto(reinterpret_cast<void*>(data));
}

void MevisVolumeRAMLoader::readDataInto(void* destination) const {
    // currently only packed, tiled volume data is supported

    auto formatErrorMsg = [filename = tif_file_](const auto& msg) {
        return fmt::format("{} ({})", msg, filename);
    };

    TIFF* tiffimage = TIFFOpen(tif_file_.string().c_str(), "rc");
    if (!tiffimage) {
        throw DataReaderException(fmt::format("cannot open TIFF: {}", tif_file_));
    }

    util::OnScopeExit closeTiffImg([&]() {
        if (tiffimage) {
            TIFFClose(tiffimage);
            tiffimage = nullptr;
        }
    });

    {  // tif checks
        short int planarconfig;
        if (!TIFFGetField(tiffimage, TIFFTAG_PLANARCONFIG, &planarconfig)) {
            throw DataReaderException(formatErrorMsg("failed to read TIFFTAG_PLANARCONFIG"));
        }

        if (planarconfig != PLANARCONFIG_CONTIG) {
            throw DataReaderException(formatErrorMsg("pixel values are not stored contiguously"));
        }

        int tiled = TIFFIsTiled(tiffimage);
        if (tiled != 1) {
            throw DataReaderException(formatErrorMsg("only tiled tiffs are supported"));
        }
    }

    size3_t tilesize(0);
    {  // get tif tile size
        if (!TIFFGetField(tiffimage, TIFFTAG_TILEWIDTH, &tilesize.x)) {
            throw DataReaderException(formatErrorMsg("failed to read TIFFTAG_TILEWIDTH"));
        }
        if (!TIFFGetField(tiffimage, TIFFTAG_TILELENGTH, &tilesize.y)) {
            throw DataReaderException(formatErrorMsg("failed to read TIFFTAG_TILELENGTH"));
        }
        if (!TIFFGetField(tiffimage, TIFFTAG_TILEDEPTH, &tilesize.z)) {
            tilesize.z = 1;
            if (dimension_[2] != 1) {
                throw DataReaderException(formatErrorMsg("failed to read TIFFTAG_TILEDEPTH"));
            }
        }
    }

    unsigned char* vol = reinterpret_cast<unsigned char*>(destination);
    const size_t tilesz = TIFFTileSize(tiffimage);
    const size_t tilerowbytes = TIFFTileRowSize(tiffimage);
    const size_t bytespersample = format_->getSizeInBytes();

#if defined(IVW_DEBUG)
    unsigned int number_of_tiles = TIFFNumberOfTiles(tiffimage);
    LogInfo("number of tiles: " << number_of_tiles);
    LogInfo(fmt::format("tilesize: {} ({}x{}x{})", tilesz, tilesize.x, tilesize.y, tilesize.z));
    LogInfo(fmt::format("rowbyte: {}, bytespersample: {}", tilerowbytes, bytespersample));
#endif

    IVW_ASSERT(tilerowbytes == tilesize.x * bytespersample,
               "tilerowbytes and tilesize.x * bytespersample differ");

    unsigned char* tilebuf = static_cast<unsigned char*>(_TIFFmalloc(tilesz));
    if (!tilebuf) {
        throw DataReaderException(formatErrorMsg("could not allocate tile buffer"));
    }

    util::OnScopeExit FreeTileBuf([&]() {
        if (tilebuf) {
            _TIFFfree(tilebuf);
            tilebuf = nullptr;
        }
    });

    for (std::size_t z0 = 0; z0 < dimension_[2]; z0 += tilesize.z) {
        for (std::size_t y0 = 0; y0 < dimension_[1]; y0 += tilesize.y) {
            for (std::size_t x0 = 0; x0 < dimension_[0]; x0 += tilesize.x) {
                const auto tiffreadtile_ret = TIFFReadTile(
                    tiffimage, tilebuf, static_cast<uint32_t>(x0), static_cast<uint32_t>(y0),
                    static_cast<uint32_t>(z0), static_cast<uint16_t>(tilesz));
                if (0 > tiffreadtile_ret) {
                    throw DataReaderException(formatErrorMsg("error reading tile"));
                }

                // pointer & offset calculations
                const size_t pv_off = z0 * dimension_.y * dimension_.x + y0 * dimension_.x + x0;
                unsigned char* pv = vol + pv_off * bytespersample;

                unsigned char* pb = tilebuf;
                unsigned char* pv_slice_ptr = pv;

                std::size_t zmin = std::min(tilesize.z, dimension_.z - z0);
                std::size_t ymin = std::min(tilesize.y, dimension_.y - y0);
                std::size_t xmin = std::min(tilesize.x, dimension_.x - x0) * bytespersample;

                std::size_t pv_step = dimension_.x * bytespersample;
                std::size_t pb_step = tilesize.x * bytespersample;

                std::size_t pv_w_step = dimension_.y * dimension_.x * bytespersample;

                // copy the tile
                for (std::size_t w = 0; w < zmin; ++w) {
                    for (std::size_t v = 0; v < ymin; ++v) {
                        for (std::size_t u = 0; u < xmin; ++u) {
                            pv[u] = pb[u];
                        }

                        pv += pv_step;
                        pb += pb_step;
                    }

                    pv = pv_slice_ptr;
                    pv_slice_ptr += pv_w_step;
                }
            }
        }
    }
}

bool MevisVolumeReader::setFilenames(const std::filesystem::path& filePath) {
    dcm_file_.clear();
    tif_file_.clear();

    const std::filesystem::path basepath = filePath.stem();

    const auto ext = toLower(filePath.extension().string());

    auto checkForFile = [basepath](const auto& exts) -> std::filesystem::path {
        for (auto& e : exts) {
            std::filesystem::path path{basepath};
            path += e;
            if (std::filesystem::is_regular_file(path)) {
                return path;
            }
        }
        return {};
    };

    if ((ext == ".tif") || (ext == ".tiff")) {
        tif_file_ = filePath;
    } else {
        tif_file_ = checkForFile(std::vector<std::string_view>{".tif", ".TIF", ".tiff", ".TIFF"});
    }

    if (ext == ".dcm") {
        dcm_file_ = filePath;
    } else {
        dcm_file_ = checkForFile(std::vector<std::string_view>{".dmc", ".DCM"});
    }
    return !tif_file_.empty() && !dcm_file_.empty();
}

}  // namespace inviwo
