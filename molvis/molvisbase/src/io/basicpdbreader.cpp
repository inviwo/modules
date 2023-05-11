/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020-2023 Inviwo Foundation
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

#include <inviwo/molvisbase/io/basicpdbreader.h>

#include <inviwo/core/util/fileextension.h>
#include <inviwo/core/util/filesystem.h>
#include <inviwo/core/util/stringconversion.h>
#include <inviwo/core/io/datareaderexception.h>

#include <inviwo/molvisbase/util/molvisutils.h>
#include <inviwo/molvisbase/util/chain.h>
#include <inviwo/molvisbase/util/aminoacid.h>

#include <string_view>
#include <sstream>
#include <charconv>
#include <fmt/format.h>

namespace inviwo {

BasicPDBReader::BasicPDBReader() {
    addExtension(FileExtension("pdb", "Protein Databank files (pdb)"));
}

BasicPDBReader* BasicPDBReader::clone() const { return new BasicPDBReader(*this); }

namespace detail {

namespace config {
#if defined(__cpp_lib_to_chars) && __cpp_lib_to_chars >= 201611L
constexpr bool charconv = true;
#else
constexpr bool charconv = false;
#endif
}  // namespace config

template <class T>
bool fromStr(std::string_view value, T& dest, bool parseHexadecimal = false) {
    std::string_view trimmed = util::trim(value);
    if constexpr (std::is_same_v<std::string, T>) {
        dest = std::string(trimmed);
    } else if constexpr (config::charconv &&
                         (std::is_same_v<double, T> || std::is_same_v<float, T>)) {
        const auto end = trimmed.data() + trimmed.size();
        if (auto [p, ec] = std::from_chars(trimmed.data(), end, dest);
            ec != std::errc() || p != end) {
            return false;
        }
    } else if constexpr (config::charconv && (std::is_integral_v<T> && !std::is_same_v<bool, T>)) {
        const auto end = trimmed.data() + trimmed.size();
        const int base = parseHexadecimal ? 16 : 10;
        if (auto [p, ec] = std::from_chars(trimmed.data(), end, dest, base);
            ec != std::errc() || p != end) {
            return false;
        }
    } else {
        std::istringstream stream{std::string{trimmed}};
        if (std::is_integral_v<T> && parseHexadecimal) {
            stream >> std::hex;
        }
        stream >> dest;
    }
    return true;
}

}  // namespace detail

template <typename T>
T parseSection(std::string_view line, size_t begin, size_t size, std::string_view tag,
               std::string_view desc, int lineNumber, bool parseHexadecimal = false) {
    T res = T{};
    if (!detail::fromStr(line.substr(begin, size), res, parseHexadecimal)) {
        throw DataReaderException(IVW_CONTEXT_CUSTOM("BasicPDBReader"),
                                  "BasicPDBReader: invalid {} entry detected{} ({})\n'{}'", tag,
                                  desc.empty() ? "" : fmt::format(", {}", desc), lineNumber, line);
    }
    return res;
}

std::shared_ptr<molvis::MolecularStructure> BasicPDBReader::readData(
    const std::filesystem::path& path) {
    auto file = open(path);

    std::string contents;
    file.seekg(0, std::ios::end);
    contents.reserve(file.tellg());
    file.seekg(0, std::ios::beg);
    contents.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

    molvis::MolecularData data;
    data.source = path.filename().string();

    int lineNumber = 0;
    int currentModel = 0;
    bool serialNoHexBase = false;
    bool resIdHexBase = false;
    auto parseline = [&](std::string_view line) {
        ++lineNumber;

        if (line.substr(0, 5) == "MODEL") {
            currentModel =
                parseSection<int>(line, 10, 4, "MODEL", "invalid model serial number", lineNumber);
        } else if ((line.substr(0, 4) == "ATOM") || (line.substr(0, 6) == "HETATM")) {
            const std::string_view tag = util::trim(line.substr(0, 6));

            const int serialNumber = parseSection<int>(line, 6, 5, tag, "invalid serial number",
                                                       lineNumber, serialNoHexBase);
            if (serialNumber == 99999) {
                serialNoHexBase = true;
            }

            std::string_view fullName(util::trim(line.substr(12, 4)));
            std::string_view element(util::trim(line.substr(76, 2)));
            std::string_view residueName(util::trim(line.substr(17, 4)));
            std::string_view chainName(util::trim(line.substr(21, 1)));

            const int residueId = parseSection<int>(
                line, 22, 4, tag, "invalid residue sequence number", lineNumber, resIdHexBase);
            if (residueId == 9999) {
                resIdHexBase = true;
            }

            const dvec3 pos{parseSection<double>(line, 30, 8, tag, "invalid position", lineNumber),
                            parseSection<double>(line, 38, 8, tag, "invalid position", lineNumber),
                            parseSection<double>(line, 46, 8, tag, "invalid position", lineNumber)};

            const double bFactor =
                parseSection<double>(line, 60, 6, tag, "invalid temperature factor", lineNumber);
            [[maybe_unused]] const double occupancy =
                parseSection<double>(line, 54, 6, tag, "invalid occupancy", lineNumber);
            // charge is optional and might not exist in PDB file
            [[maybe_unused]] double charge = 0.0;
            detail::fromStr(line.substr(78, 2), charge);

            molvis::ChainId chain = molvis::chain::fromName(chainName);
            int chainId = molvis::chain::id(chain);
            if (!molvis::findChain(data, chainId)) {
                data.chains.push_back({chainId, std::string(molvis::chain::name(chain))});
            }
            if (!molvis::findResidue(data, residueId, chainId)) {
                data.residues.push_back({residueId, molvis::aminoacid::fromFullName(residueName),
                                         std::string(residueName), chainId});
            }
            data.atoms.positions.push_back(pos);
            data.atoms.serialNumbers.push_back(serialNumber);
            data.atoms.bFactors.push_back(bFactor);
            data.atoms.modelIds.push_back(currentModel);
            data.atoms.chainIds.push_back(chainId);
            data.atoms.residueIds.push_back(residueId);
            data.atoms.atomicNumbers.push_back(molvis::element::fromAbbr(element));
            data.atoms.fullNames.emplace_back(fullName);
        }
    };

    util::forEachStringPart(contents, "\n", parseline);

    data.bonds = molvis::computeCovalentBonds(data.atoms);

    return std::make_shared<molvis::MolecularStructure>(data);
}

}  // namespace inviwo
