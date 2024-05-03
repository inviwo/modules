/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2024 Inviwo Foundation
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

#include <inviwo/vasp/processors/chgcarsource.h>

#include <inviwo/core/util/stringconversion.h>
#include <inviwo/core/util/glm.h>
#include <inviwo/core/util/zip.h>
#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/core/datastructures/geometry/typedmesh.h>
#include <inviwo/core/interaction/pickingmapper.h>
#include <inviwo/molvisbase/datastructures/molecularstructure.h>
#include <inviwo/molvisbase/util/molvisutils.h>

#include <bxzstr/bxzstr.hpp>
#include <fast_float/fast_float.h>

#include <fmt/std.h>

#include <ranges>
#include <numeric>
#include <algorithm>

/*
0: unknown system
1:    1.0000000000000000
2:     12.736266    0.000000    0.000000
3:     -6.368133   11.029930    0.000000
4:      0.000000    0.000000   18.127113
5:    Mo   N    S
6:     16     1    32
7: Direct
8:  -0.005676 -0.011353  0.500000
9:   0.750144  0.748796  0.500000
...
...
...
56:   0.418234  0.083922  0.586255
57:
58:   216  216  300
59:  -.31671618672E-03 -.31666464702E-03 -.31682936753E-03 -.31759945887E-03 -.31905215440E-03
60:  -.32067707343E-03 -.32168462724E-03 -.32169411543E-03 -.32108324240E-03 -.32047497439E-03
...
...
*/

namespace inviwo {
namespace {

struct Chgcar {
    std::string desc = "";
    double scale = 1.0;
    glm::dvec3 a1{1.0, 0.0, 0.0};
    glm::dvec3 a2{0.0, 1.0, 0.0};
    glm::dvec3 a3{0.0, 0.0, 1.0};
    glm::mat4 model{1.0};

    std::vector<std::string> elem;
    std::vector<size_t> nelem;
    bool direct = false;
    std::vector<glm::dvec3> pos;
    glm::size3_t dims;
};

template <typename Func>
constexpr void forEachPart(std::string_view str, Func&& func) {
    if (str.empty()) return;
    for (size_t first = 0; first < str.size();) {
        const auto second = str.find(' ', first);
        std::invoke(func, str.substr(first, second - first));
        if (second == std::string_view::npos) break;
        first = str.find_first_not_of(' ', second + 1);
    }
}

struct File {
    File(std::filesystem::path file) : stream{file.generic_string()}, buffer{}, currentLine{0} {
        if (!stream) {
            throw Exception(IVW_CONTEXT, "Error opening file at {}", file);
        }
    }

    void line(auto&& func) {
        ++currentLine;
        if (std::getline(stream, buffer)) {
            func(util::trim(buffer));
        } else {
            throw Exception(IVW_CONTEXT, "Invalid format at line {}", currentLine);
        }
    }

    template <size_t n = std::numeric_limits<size_t>::max()>
    void lineParts(auto&& func) {
        line([&](std::string_view line) {
            size_t i{0};
            forEachPart(line, [&](std::string_view elem) {
                if (i >= n) {
                    throw Exception(IVW_CONTEXT, "Expected {} elements in line {}, found {}", n,
                                    currentLine, line);
                }
                func(util::trim(elem), i);
                ++i;
            });
            if constexpr (n != std::numeric_limits<size_t>::max()) {
                if (i != n) {
                    throw Exception(IVW_CONTEXT, "Expected {} elements in line {}, found {}", n,
                                    currentLine, line);
                }
            }
        });
    }

private:
    bxz::ifstream stream;
    std::string buffer;
    size_t currentLine;
};

auto toNum(std::string_view elem, auto& dest) {
    const auto answer = fast_float::from_chars(elem.data(), elem.data() + elem.size(), dest);
    if (answer.ec != std::errc()) {
        throw Exception(IVW_CONTEXT_CUSTOM("ChgcarSource"), "Invalid number: {}", elem);
    }
}

void forEachAtom(const Chgcar& chg, double borderMargin, auto&& func) {
    std::vector<molvis::Element> elements;
    for (auto&& [num, type] : util::zip(chg.nelem, chg.elem)) {
        const auto elem = molvis::element::fromAbbr(type);
        for ([[maybe_unused]] auto&& s : std::views::iota(size_t{0}, num)) {
            elements.push_back(elem);
        }
    }

    size_t atomIndex{0};
    int runningIndex{0};
    for (auto&& [pos, elem] : util::zip(chg.pos, elements)) {
        if (borderMargin > 0.0) {
            for (auto x : {-1.0, 0.0, 1.0}) {
                for (auto y : {-1.0, 0.0, 1.0}) {
                    for (auto z : {-1.0, 0.0, 1.0}) {
                        const auto newpos = pos + glm::dvec3{x, y, z};
                        if (glm::all(glm::greaterThan(newpos, -glm::dvec3{borderMargin})) &&
                            glm::all(glm::lessThan(newpos, glm::dvec3{1.0 + borderMargin}))) {
                            std::invoke(func, elem, newpos, atomIndex, runningIndex);
                            ++runningIndex;
                        }
                    }
                }
            }
        } else {
            std::invoke(func, elem, pos, atomIndex, runningIndex);
            ++runningIndex;
        }
        ++atomIndex;
    }
}

std::shared_ptr<Mesh> createMesh(const Chgcar& chg, size_t startPickId,
                                 molvis::element::Colormap colormap, double radiusScale,
                                 double borderMargin) {

    auto mesh =
        std::make_shared<TypedMesh<buffertraits::PositionsBuffer, buffertraits::ColorsBuffer,
                                   buffertraits::RadiiBuffer, buffertraits::PickingBuffer>>();

    auto& ib = mesh->addIndexBuffer(DrawType::Points, ConnectivityType::None)->getDataContainer();

    mesh->reserveSizeInVertexBuffer(chg.pos.size());

    forEachAtom(
        chg, borderMargin,
        [&](molvis::Element elem, const glm::dvec3& pos, size_t atomIndex, size_t runningIndex) {
            const auto color = molvis::element::color(elem, colormap);
            const auto radius = molvis::element::vdwRadius(elem) * radiusScale;
            const auto pi = startPickId + atomIndex;
            mesh->addVertex(pos, color, radius, pi);
            ib.push_back(static_cast<uint32_t>(runningIndex));
        });

    mesh->setModelMatrix(chg.model);

    return mesh;
}

std::shared_ptr<DataFrame> createDataFrame(const Chgcar& chg) {
    auto df = std::make_shared<DataFrame>();

    auto ct = df->addCategoricalColumn("type");
    auto cx = df->addColumn<float>("x", 0, units::unit_from_string("Angstrom"));
    auto cy = df->addColumn<float>("y", 0, units::unit_from_string("Angstrom"));
    auto cz = df->addColumn<float>("z", 0, units::unit_from_string("Angstrom"));
    auto cr = df->addColumn<float>("r", 0, units::unit_from_string("Angstrom"));

    forEachAtom(chg, 0.0, [&](molvis::Element elem, const glm::dvec3& pos, size_t, size_t) {
        const auto mp = chg.model * vec4(pos, 1.0f);
        ct->add(molvis::element::symbol(elem));
        cx->add(mp.x);
        cy->add(mp.y);
        cz->add(mp.z);
        cr->add(static_cast<float>(molvis::element::vdwRadius(elem)));
    });

    df->updateIndexBuffer();

    return df;
}

std::shared_ptr<molvis::MolecularStructure> createMolecularStructure(
    const Chgcar& chg, double borderMargin, std::optional<std::string> source = std::nullopt) {

    molvis::Atoms atoms;

    forEachAtom(
        chg, borderMargin,
        [&](molvis::Element elem, const glm::dvec3& pos, size_t atomIndex, size_t runningIndex) {
            atoms.positions.push_back(pos);
            atoms.serialNumbers.push_back(static_cast<int>(atomIndex));
            atoms.atomicNumbers.push_back(elem);
        });

    auto bonds = molvis::computeCovalentBonds(atoms);

    auto ms = std::make_shared<molvis::MolecularStructure>(molvis::MolecularData{
        .source = source, .atoms = std::move(atoms), .bonds = std::move(bonds)});

    ms->setBasis(glm::mat3{chg.model});
    ms->setOffset(glm::vec3{chg.model[3]});

    return ms;
}

}  // namespace

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo ChgcarSource::processorInfo_{"org.inviwo.ChgcarSource.CPu",  // Class identifier
                                                 "Chgcar Source",                // Display name
                                                 "Source",                       // Category
                                                 CodeState::Experimental,        // Code state
                                                 Tags::CPU,                      // Tags
                                                 R"(Chgcar data reader, see
    https://www.vasp.at/wiki/index.php/CHGCAR
    )"_unindentHelp};

const ProcessorInfo ChgcarSource::getProcessorInfo() const { return processorInfo_; }

ChgcarSource::ChgcarSource()
    : PoolProcessor{}
    , chargeOutport_{"chargedensity", "Charge density volume"_help}
    , atomsOutport_{"atoms", "SphereMesh (positions, radii, and colors) of the atoms"_help}
    , atomInformationOutport_{"atomInformation",
                              "DataFrame holding atom positions and atomic type"_help}
    , moleculeOutport_{"molecule", "MolecularStructure representing all atoms"_help}
    , bnlInport_{"bnl"}
    , file_{"chgcar", "CHGCAR", "", "chgcarfile"}
    , information_{"information", "information"}
    , basis_{"basis", "basis"}
    , colormap_{"colormap",
                "colormap",
                {{"cpk", "Rasmol CPK", molvis::element::Colormap::RasmolCPK},
                 {"cpknew", "Rasmol CPK new", molvis::element::Colormap::RasmolCPKnew},
                 {"jmol", "Jmol", molvis::element::Colormap::Jmol}},
                0}
    , radiusScaling_{"radiusScaling", "radiusScaling", 0.25, 0.0, 2.0, 0.01}
    , borderMargin_{"borderMargin", "borderMargin", 0.05, 0.0, 0.5}
    , pm_{this, 1, [](PickingEvent*) {}} {

    addPorts(chargeOutport_, atomsOutport_, atomInformationOutport_, moleculeOutport_, bnlInport_);
    addProperties(file_, information_, basis_, colormap_, radiusScaling_, borderMargin_);
}

void ChgcarSource::process() {
    if (file_->empty()) {
        return;
    }

    if constexpr (true) {
        bxz::ifstream stream(file_.get().string());
        std::string line;
        uint32_t i = 0;
        while (std::getline(stream, line) && i < 100) {
            util::logInfo(IVW_CONTEXT, "{}: {}", i, line);
            ++i;
        }
    }


    using Result =
        std::tuple<std::shared_ptr<Volume>, std::shared_ptr<Mesh>, std::shared_ptr<DataFrame>,
                   std::shared_ptr<molvis::MolecularStructure>>;

    auto calc = [path = file_.get(), colormap = colormap_.get(),
                 radiusScaling = radiusScaling_.get(), borderMargin = borderMargin_.get(),
                 this](pool::Stop stop, pool::Progress progress) -> Result {
        File file{path};

        Chgcar chg;

        file.line([&](std::string_view line) { chg.desc = line; });
        file.line([&](std::string_view line) { toNum(line, chg.scale); });

        file.lineParts<3>([&](std::string_view elem, size_t i) { toNum(elem, chg.a1[i]); });
        file.lineParts<3>([&](std::string_view elem, size_t i) { toNum(elem, chg.a2[i]); });
        file.lineParts<3>([&](std::string_view elem, size_t i) { toNum(elem, chg.a3[i]); });
        file.lineParts([&](std::string_view elem, size_t) { chg.elem.emplace_back(elem); });
        file.lineParts(
            [&](std::string_view elem, size_t) { toNum(elem, chg.nelem.emplace_back()); });

        file.line([&](std::string_view line) {
            if (line.empty()) return;
            chg.direct = !(line[0] == 'K' || line[0] == 'k' || line[0] == 'C' || line[0] == 'c');
        });

        const auto factor =
            chg.scale > 0.0
                ? chg.scale
                : std::pow(-chg.scale / glm::dot(chg.a1, glm::cross(chg.a2, chg.a1)), 1.0 / 3.0);
        const auto basis = factor * dmat3{chg.a1, chg.a2, chg.a3};
        chg.model = glm::mat4{basis};
        chg.model[3][3] = 1.0;

        const auto ntot = std::reduce(chg.nelem.begin(), chg.nelem.end(), size_t{0}, std::plus<>{});

        for ([[maybe_unused]] auto _ : std::views::iota(size_t{0}, ntot)) {
            auto& pos = chg.pos.emplace_back();
            file.lineParts<3>([&](std::string_view elem, size_t i) { toNum(elem, pos[i]); });
        }

        if (!chg.direct) {
            const auto invBases = glm::inverse(basis);
            std::transform(chg.pos.begin(), chg.pos.end(), chg.pos.begin(),
                           [&](glm::dvec3 pos) { return invBases * pos; });
        }

        file.line([&](std::string_view) {});  // empty line

        file.lineParts<3>([&](std::string_view elem, size_t i) { toNum(elem, chg.dims[i]); });

        const auto voxels = glm::compMul(chg.dims);

        auto volumeRep = std::make_shared<VolumeRAMPrecision<float>>(
            VolumeReprConfig{.dimensions = chg.dims, .wrapping = wrapping3d::repeatAll});

        auto ram = volumeRep->getView();

        for (size_t i = 0; i < voxels;) {
            file.lineParts([&](std::string_view elem, size_t) { toNum(elem, ram[i++]); });

            if (stop) return {};
            if (i % 1'000'000 == 0) {
                progress(static_cast<float>(i) / voxels);
            }
        }

        auto [minIt, maxIt] = std::minmax_element(ram.begin(), ram.end());

        auto volume = std::make_shared<Volume>(
            VolumeConfig{.dimensions = chg.dims,
                         .format = DataFormat<float>::get(),
                         .wrapping = wrapping3d::repeatAll,
                         .xAxis = Axis{"x", units::unit_from_string("Angstrom")},
                         .yAxis = Axis{"y", units::unit_from_string("Angstrom")},
                         .zAxis = Axis{"z", units::unit_from_string("Angstrom")},
                         .valueAxis = Axis{"Charge Density", units::unit_from_string("e")},
                         .dataRange = glm::dvec2{*minIt, *maxIt},
                         .valueRange = glm::dvec2{*minIt, *maxIt},
                         .model = chg.model});
        volume->addRepresentation(volumeRep);

        pm_.resize(chg.pos.size());

        auto mesh = createMesh(chg, pm_.getPickingId(0), colormap, radiusScaling, borderMargin);
        auto df = createDataFrame(chg);
        auto ms = createMolecularStructure(chg, borderMargin, path.generic_string());

        return {volume, mesh, df, ms};
    };

    chargeOutport_.setData(nullptr);
    dispatchOne(calc, [this](Result result) {
        chargeOutport_.setData(std::get<0>(result));
        atomsOutport_.setData(std::get<1>(result));
        atomInformationOutport_.setData(std::get<2>(result));
        moleculeOutport_.setData(std::get<3>(result));
        newResults();
    });
}

}  // namespace inviwo
