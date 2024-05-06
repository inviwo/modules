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
#include <inviwo/core/interaction/events/pickingevent.h>
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
#include <array>
#include <string_view>

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
augmentation occupancies   1  33
  0.9171881E+00 -0.4694991E+00 -0.4258742E-01 -0.7362697E-01  0.3280824E-05
  0.1388477E-01  0.4189455E-01 -0.1198004E-05  0.6993045E-01  0.1220388E-01
  0.6665500E-01 -0.1082870E-05 -0.3460060E-02 -0.1718538E-01  0.5094644E-06
  0.2403475E+01  0.1298231E-05  0.1608450E-01 -0.2671053E+00 -0.5273834E-06
  0.8953507E-01 -0.1260738E+01 -0.4249313E-06  0.7984281E-04  0.2247795E+00
  0.1107842E-05 -0.5135760E-01  0.1763209E+00  0.6364891E-07 -0.5156355E-03
 -0.4001328E-01 -0.2421517E-06  0.8458568E-02
augmentation occupancies   2 138
  0.5628734E+00 -0.2339283E-01 -0.1037611E-04  0.1179873E-04  0.2025907E-04
 -0.2705897E-04  0.2961381E-04  0.3133694E-04 -0.5720380E-03  0.1444370E-03
 -0.7370506E-02 -0.5848981E-05  0.1186330E-02  0.4859838E-03 -0.1277535E-03
  0.6321534E-02  0.2100453E-04 -0.1038597E-02  0.3362825E+00 -0.2280994E-03
...
...
  216  216  300
 -.14445251069E-03 -.14420052582E-03 -.14352027883E-03 -.14254002376E-03 -.14144594269E-03
 -.14044872189E-03 -.13974044584E-03 -.13944650061E-03 -.13958317550E-03 -.14003806953E-03
 ...
 ...
augmentation occupancies   1  33
  0.9171881E+00 -0.4694991E+00 -0.4258742E-01 -0.7362697E-01  0.3280824E-05
  0.1388477E-01  0.4189455E-01 -0.1198004E-05  0.6993045E-01  0.1220388E-01
  0.6665500E-01 -0.1082870E-05 -0.3460060E-02 -0.1718538E-01  0.5094644E-06
  0.2403475E+01  0.1298231E-05  0.1608450E-01 -0.2671053E+00 -0.5273834E-06
  0.8953507E-01 -0.1260738E+01 -0.4249313E-06  0.7984281E-04  0.2247795E+00
  0.1107842E-05 -0.5135760E-01  0.1763209E+00  0.6364891E-07 -0.5156355E-03
 -0.4001328E-01 -0.2421517E-06  0.8458568E-02
augmentation occupancies   2 138
  0.5628734E+00 -0.2339283E-01 -0.1037611E-04  0.1179873E-04  0.2025907E-04
 -0.2705897E-04  0.2961381E-04  0.3133694E-04 -0.5720380E-03  0.1444370E-03
 -0.7370506E-02 -0.5848981E-05  0.1186330E-02  0.4859838E-03 -0.1277535E-03
  0.6321534E-02  0.2100453E-04 -0.1038597E-02  0.3362825E+00 -0.2280994E-03
...
...
*/

namespace inviwo {

struct Chgcar {
    std::string desc = "";
    double scale = 1.0;
    glm::dvec3 a1{1.0, 0.0, 0.0};
    glm::dvec3 a2{0.0, 1.0, 0.0};
    glm::dvec3 a3{0.0, 0.0, 1.0};
    glm::mat4 model{1.0};

    std::vector<std::string> elem;
    std::vector<size_t> nelem;
    size_t ntot;
    bool direct = false;
    std::vector<glm::dvec3> pos;
    glm::size3_t dims;
};

namespace {

template <size_t n = std::numeric_limits<size_t>::max(), typename Func>
constexpr size_t forEachPart(std::string_view str, Func&& func) {
    if (str.empty()) return 0;
    size_t i{0};
    for (size_t first = 0; first < str.size();) {
        const auto second = str.find(' ', first);
        if constexpr (n != std::numeric_limits<size_t>::max()) {
            if (i >= n) {
                throw Exception(IVW_CONTEXT_CUSTOM("forEachPart"),
                                "Expected {} elements, in str '{}'", n, str);
            }
        }
        std::invoke(func, str.substr(first, second - first), i);
        ++i;
        if (second == std::string_view::npos) break;
        first = str.find_first_not_of(' ', second + 1);
    }
    if constexpr (n != std::numeric_limits<size_t>::max()) {
        if (i != n) {
            throw Exception(IVW_CONTEXT_CUSTOM("forEachPart"),
                            "Expected {} elements, found {}, in str '{}'", n, i, str);
        }
    }
    return i;
}

template <size_t N>
constexpr std::array<std::string_view, N> split(std::string_view str) {
    std::array<std::string_view, N> res;
    forEachPart<N>(str, [&](auto elem, size_t i) { res[i] = elem; });
    return res;
}

struct File {
    File(std::filesystem::path file)
        : stream{file.generic_string(), std::ios_base::in | std::ios_base::binary}
        , buffer{}
        , currentLine{0} {
        if (!stream) {
            throw Exception(IVW_CONTEXT, "Error opening file at {}", file);
        }
    }

    auto line(auto&& func) {
        ++currentLine;
        if (std::getline(stream, buffer)) {
            return func(util::trim(buffer));
        } else {
            throw Exception(IVW_CONTEXT, "Invalid format at line {}", currentLine);
        }
    }

    bool peekLine(auto&& func) {
        const auto pos = stream.tellg();
        if (std::getline(stream, buffer)) {
            stream.seekg(pos, std::ios_base::beg);
            return func(util::trim(buffer));
        } else {
            stream.seekg(pos, std::ios_base::beg);
            return false;
        }
    }

    template <size_t n = std::numeric_limits<size_t>::max()>
    size_t lineParts(auto&& func) {
        try {
            return line([&](std::string_view line) { return forEachPart<n>(line, func); });
        } catch (const Exception& e) {
            throw Exception(e.getContext(), "Error on line {}: {}", currentLine, e.getMessage());
        }
    }

    template <size_t n>
    std::array<std::string_view, n> lineSplit() {
        try {
            return line([&](std::string_view line) { return split<n>(line); });
        } catch (const Exception& e) {
            throw Exception(e.getContext(), "Error on line {}: {}", currentLine, e.getMessage());
        }
    }

private:
    bxz::ifstream stream;
    std::string buffer;
    size_t currentLine;
};

void toNum(std::string_view elem, auto& dest) {
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

std::shared_ptr<Volume> readVolume(const Chgcar& chg, File& file, std::string_view name,
                                   pool::Stop stop, pool::Progress progress) {
    const auto voxels = glm::compMul(chg.dims);

    auto volumeRep = std::make_shared<VolumeRAMPrecision<float>>(
        VolumeReprConfig{.dimensions = chg.dims, .wrapping = wrapping3d::repeatAll});

    const auto ram = volumeRep->getView();
    for (size_t i = 0; i < voxels;) {
        file.lineParts([&](std::string_view elem, size_t) { toNum(elem, ram[i++]); });

        if (stop) return {};
        if (i % 1'000'000 == 0) {
            progress(static_cast<float>(i) / voxels);
        }
    }

    const auto [minIt, maxIt] = std::minmax_element(ram.begin(), ram.end());

    auto volume = std::make_shared<Volume>(
        VolumeConfig{.dimensions = chg.dims,
                     .format = DataFormat<float>::get(),
                     .wrapping = wrapping3d::repeatAll,
                     .xAxis = Axis{"x", units::unit_from_string("Angstrom")},
                     .yAxis = Axis{"y", units::unit_from_string("Angstrom")},
                     .zAxis = Axis{"z", units::unit_from_string("Angstrom")},
                     .valueAxis = Axis{std::string{name}, units::unit_from_string("e")},
                     .dataRange = glm::dvec2{*minIt, *maxIt},
                     .valueRange = glm::dvec2{*minIt, *maxIt},
                     .model = chg.model});
    volume->addRepresentation(volumeRep);

    return volume;
}

void discardAugmentationOccupancies(const Chgcar& chg, File& file) {
    if (!file.peekLine([](std::string_view line) {
            const auto [str1, str2, num, strSize] = split<4>(line);
            return str1 == "augmentation" && str2 == "occupancies";
        })) {
        return;
    }

    for (size_t i = 0; i < chg.ntot; ++i) {
        const auto [str1, str2, num, strSize] = file.lineSplit<4>();
        size_t size{0};
        toNum(strSize, size);
        for (size_t c = 0; c < size;) {
            file.lineParts([&](std::string_view, size_t) { c++; });
        }
    }
    // here comes ntot numbers of something?
    for (size_t c = 0; c < chg.ntot;) {
        file.lineParts([&](std::string_view, size_t) { c++; });
    }
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
    , chargeOutport_{"chargedensity",
                     "Charge density (spin up + spin down, if spin polarized)"_help}
    , magnetizationOutport_{"magnetization",
                            "Charge density (spin up - spin down, "
                            "if spin polarized, otherwise empty"_help}
    , atomsOutport_{"atoms", "SphereMesh (positions, radii, and colors) of the atoms"_help}
    , atomInformationOutport_{"atomInformation",
                              "DataFrame holding atom positions and atomic type"_help}
    , moleculeOutport_{"molecule", "MolecularStructure representing all atoms"_help}
    , bnlInport_{"bnl", ""_help, {}}
    , file_{"chgcar", "CHGCAR", "", "chgcarfile"}
    , chgInfo_{"chgInfo", "Charge information"}
    , magInfo_{"magInfo", "Magnetization information"}
    , basis_{"basis", "basis"}
    , colormap_{"colormap",
                "colormap",
                {{"cpk", "Rasmol CPK", molvis::element::Colormap::RasmolCPK},
                 {"cpknew", "Rasmol CPK new", molvis::element::Colormap::RasmolCPKnew},
                 {"jmol", "Jmol", molvis::element::Colormap::Jmol}},
                0}
    , radiusScaling_{"radiusScaling", "radiusScaling", 0.25, 0.0, 2.0, 0.01}
    , borderMargin_{"borderMargin", "borderMargin", 0.05, 0.0, 0.5}
    , pm_{this, 1, [this](PickingEvent* event) { picking(event); }}
    , data_{}
    , chg_{}
    , mag_{} {

    addPorts(chargeOutport_, magnetizationOutport_, atomsOutport_, atomInformationOutport_,
             moleculeOutport_, bnlInport_);
    addProperties(file_, chgInfo_, magInfo_, basis_, colormap_, radiusScaling_, borderMargin_);
}

ChgcarSource::~ChgcarSource() = default;

void ChgcarSource::process() {
    if (file_->empty()) {
        data_.reset();
        chg_.reset();
        mag_.reset();
        chargeOutport_.clear();
        magnetizationOutport_.clear();
        atomsOutport_.clear();
        atomInformationOutport_.clear();
        moleculeOutport_.clear();
        return;
    }

    if constexpr (false) {
        bxz::ifstream stream(file_.get().string());
        std::string line;
        uint32_t i = 0;
        while (std::getline(stream, line) && i < 100) {
            util::logInfo(IVW_CONTEXT, "{}: {}", i, line);
            ++i;
        }
    }

    if (!file_.isModified()) {
        if (chg_) {
            chgInfo_.updateVolume(*chg_);
            basis_.updateEntity(*chg_);
            chargeOutport_.setData(chg_);
        }

        if (mag_) {
            magInfo_.updateVolume(*mag_);
            basis_.updateEntity(*mag_);
            magnetizationOutport_.setData(mag_);
        }

        if (data_) {
            pm_.resize(data_->pos.size());
            auto mesh =
                createMesh(*data_, pm_.getPickingId(0), colormap_, radiusScaling_, borderMargin_);
            auto df = createDataFrame(*data_);
            auto ms = createMolecularStructure(*data_, borderMargin_, file_.get().generic_string());

            basis_.updateEntity(*mesh);
            basis_.updateEntity(*ms);

            atomsOutport_.setData(mesh);
            atomInformationOutport_.setData(df);
            moleculeOutport_.setData(ms);
        }

        return;
    }

    using Result = std::tuple<Chgcar, std::shared_ptr<Volume>, std::shared_ptr<Volume>>;
    auto calc = [path = file_.get()](pool::Stop stop, pool::Progress progress) -> Result {
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

        chg.ntot = std::reduce(chg.nelem.begin(), chg.nelem.end(), size_t{0}, std::plus<>{});

        for ([[maybe_unused]] auto _ : std::views::iota(size_t{0}, chg.ntot)) {
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

        auto charge = readVolume(chg, file, "Charge Density", stop, progress);
        if (!charge) {
            return {std::move(chg), nullptr, nullptr};
        }

        // Optionally read augmentation occupancies, might not be there in a chg file
        discardAugmentationOccupancies(chg, file);

        // Check if we have magnetization data?
        auto magnetization = std::shared_ptr<Volume>{};
        if (file.peekLine([&](std::string_view line) {
                try {
                    size3_t dims;
                    forEachPart<3>(line,
                                   [&](std::string_view elem, size_t i) { toNum(elem, dims[i]); });
                    return true;
                } catch (...) {
                    return false;
                }
            })) {

            size3_t dims;
            file.lineParts<3>([&](std::string_view elem, size_t i) { toNum(elem, dims[i]); });
            if (chg.dims != dims) {
                throw Exception(IVW_CONTEXT_CUSTOM("ChgcarSource"),
                                "Dimensions for charge density {}, does not match dimensions "
                                "for magnetization density {}",
                                chg.dims, dims);
            }

            magnetization = readVolume(chg, file, "Magnetization Density", stop, progress);
        }

        return {
            std::move(chg),
            charge,
            magnetization,
        };
    };

    chargeOutport_.setData(nullptr);
    dispatchOne(calc, [this](Result result) {
        data_ = std::make_unique<Chgcar>(std::move(std::get<0>(result)));
        chg_ = std::get<1>(result);
        mag_ = std::get<2>(result);

        if (chg_) {
            chgInfo_.updateForNewVolume(
                *chg_, deserialized_ ? util::OverwriteState::Yes : util::OverwriteState::No);
            chgInfo_.updateVolume(*chg_);
            basis_.updateForNewEntity(*chg_, deserialized_);
            basis_.updateEntity(*chg_);
            chargeOutport_.setData(chg_);
        } else {
            chargeOutport_.clear();
        }

        if (mag_) {
            magInfo_.updateForNewVolume(
                *mag_, deserialized_ ? util::OverwriteState::Yes : util::OverwriteState::No);
            magInfo_.updateVolume(*mag_);
            basis_.updateEntity(*mag_);
            magnetizationOutport_.setData(mag_);
        } else {
            magnetizationOutport_.clear();
        }

        if (data_) {
            pm_.resize(data_->pos.size());
            auto mesh =
                createMesh(*data_, pm_.getPickingId(0), colormap_, radiusScaling_, borderMargin_);
            auto df = createDataFrame(*data_);
            auto ms = createMolecularStructure(*data_, borderMargin_, file_.get().generic_string());
            basis_.updateEntity(*mesh);
            basis_.updateEntity(*ms);

            atomsOutport_.setData(mesh);
            atomInformationOutport_.setData(df);
            moleculeOutport_.setData(ms);
        } else {
            atomsOutport_.clear();
            atomInformationOutport_.clear();
            moleculeOutport_.clear();
        }

        deserialized_ = false;
        newResults();
    });
}

void ChgcarSource::deserialize(Deserializer& d) {
    PoolProcessor::deserialize(d);
    deserialized_ = true;
}

void ChgcarSource::picking(const PickingEvent* event) {
    if (event->getPressState() == PickingPressState::None) {
        if (event->getHoverState() == PickingHoverState::Enter) {
            auto i = event->getPickedId();
            bnlInport_.highlight(BitSet{i});
            auto pos = data_ ? glm::dmat3{data_->model} * data_->pos.at(i) : dvec3{};
            // auto elem = molvis::atomicelement::symbol(self.atomTypes[i]);
            // event->setToolTip(fmt::format("Atom id: {}\nType: {}\nPosition: {}\nFractional: {}"),
            // i,
            //                   elem, pos, atomsPos);

            event->setToolTip(fmt::format("Atom id: {}\nType: {}", i, pos));
        } else if (event->getHoverState() == PickingHoverState::Exit) {
            bnlInport_.highlight(BitSet{});
            event->setToolTip("");
        }
    }
    if (event->getPressState() == PickingPressState::Release &&
        event->getPressItem() == PickingPressItem::Primary &&
        std::abs(event->getDeltaPressedPosition().x) < 0.01 &&
        std::abs(event->getDeltaPressedPosition().y) < 0.01) {

        auto selection = bnlInport_.getSelectedIndices();
        selection.flip(static_cast<uint32_t>(event->getPickedId()));
        bnlInport_.select(selection);
    }
}

}  // namespace inviwo
