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
#include <inviwo/core/util/stringconversion.h>
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
                        const auto newPos = pos + glm::dvec3{x, y, z};
                        if (glm::all(glm::greaterThan(newPos, -glm::dvec3{borderMargin})) &&
                            glm::all(glm::lessThan(newPos, glm::dvec3{1.0 + borderMargin}))) {
                            std::invoke(func, elem, newPos, atomIndex, runningIndex);
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
                                   buffertraits::RadiiBuffer, buffertraits::PickingBuffer,
                                   buffertraits::IndexBuffer>>();

    auto& ib = mesh->addIndexBuffer(DrawType::Points, ConnectivityType::None)->getDataContainer();

    mesh->reserveSizeInVertexBuffer(chg.pos.size());

    forEachAtom(
        chg, borderMargin,
        [&](molvis::Element elem, const glm::dvec3& pos, size_t atomIndex, size_t runningIndex) {
            const auto color = molvis::element::color(elem, colormap);
            const auto radius = molvis::element::vdwRadius(elem) * radiusScale;
            const auto pi = startPickId + atomIndex;
            mesh->addVertex(pos, color, radius, pi, atomIndex);
            ib.push_back(static_cast<uint32_t>(runningIndex));
        });

    mesh->setModelMatrix(chg.model);

    return mesh;
}

std::string supToUnicode(std::string_view str) {
    static const std::map<char, std::string> map{{
        {'.', "\u22C5"},
        {'0', "\u2070"},
        {'1', "\u00B9"},
        {'2', "\u00B2"},
        {'3', "\u00B3"},
        {'4', "\u2074"},
        {'5', "\u2075"},
        {'6', "\u2076"},
        {'7', "\u2077"},
        {'8', "\u2078"},
        {'9', "\u2079"},
    }};

    std::string res;
    for (auto n = str.find("<sup>"); n != std::string_view::npos; n = str.find("<sup>")) {
        res.append(str, 0, n);
        const auto m = str.find("</sup>");
        if (m == std::string_view::npos) {
            throw Exception(IVW_CONTEXT_CUSTOM("supToUnicode"), "Invalid html in str: {}", str);
        }

        const auto sup = str.substr(n + 5, m - (n + 5));
        for (char c : sup) {
            auto it = map.find(c);
            if (it != map.end()) {
                res += it->second;
            } else {
                res += c;
            }
        }

        str = str.substr(m + 6);
    }
    res.append(str);
    return res;
}

std::shared_ptr<DataFrame> createDataFrame(const Chgcar& chg, vasp::PotentialType potential,
                                           std::string_view potcarNames) {

    std::map<std::string_view, std::string_view> potcars;
    util::forEachStringPart(potcarNames, "\n", [&](std::string_view line) {
        auto [elem, potcar] = util::splitByFirst(line, "->");
        potcars[util::trim(elem)] = util::trim(potcar);
    });

    std::map<molvis::Element, vasp::Potential> potmap;
    for (const auto& name : chg.elem) {
        const auto elem = molvis::element::fromAbbr(name);
        const auto potname = [&]() -> std::string_view {
            if (auto it = potcars.find(name); it != potcars.end()) {
                return it->second;
            } else {
                return name;
            }
        }();
        const auto maybePot = vasp::findPotential(potential, potname);
        potmap[elem] = maybePot.value_or(vasp::Potential{});
    }

    auto df = std::make_shared<DataFrame>();

    auto ct = df->addCategoricalColumn("type");
    auto cx = df->addColumn<double>("x", 0, units::unit_from_string("Angstrom"));
    auto cy = df->addColumn<double>("y", 0, units::unit_from_string("Angstrom"));
    auto cz = df->addColumn<double>("z", 0, units::unit_from_string("Angstrom"));
    auto cr = df->addColumn<double>("r", 0, units::unit_from_string("Angstrom"));

    auto ce = df->addColumn<double>("Valence Electrons", 0, units::unit_from_string("e"));
    auto cc = df->addCategoricalColumn("Valence Config");

    forEachAtom(chg, 0.0, [&](molvis::Element elem, const glm::dvec3& pos, size_t, size_t) {
        const auto mp = chg.model * vec4(pos, 1.0f);
        ct->add(molvis::element::symbol(elem));
        cx->add(mp.x);
        cy->add(mp.y);
        cz->add(mp.z);
        cr->add(static_cast<double>(molvis::element::vdwRadius(elem)));

        const auto& pot = potmap[elem];
        ce->add(pot.valenceElectrons);
        cc->add(supToUnicode(pot.valenceConfiguration));
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

    ms->setModelMatrix(chg.model);

    return ms;
}

std::shared_ptr<VolumeRAMPrecision<float>> readChg(const Chgcar& chg, File& file, pool::Stop stop,
                                                   pool::Progress progress) {
    const auto voxels = glm::compMul(chg.dims);

    auto volumeRep = std::make_shared<VolumeRAMPrecision<float>>(
        VolumeReprConfig{.dimensions = chg.dims, .wrapping = wrapping3d::repeatAll});

    const double volume = glm::abs(glm::dot(chg.a1, glm::cross(chg.a2, chg.a3)));
    const float scale = static_cast<float>(1.0 / volume);

    const auto ram = volumeRep->getView();
    for (size_t i = 0; i < voxels;) {
        file.lineParts([&](std::string_view elem, size_t) {
            toNum(elem, ram[i]);
            ram[i] *= scale;
            ++i;
        });

        if (stop) return {};
        if (i % 1'000'000 == 0) {
            progress(static_cast<float>(i) / voxels);
        }
    }

    return volumeRep;
}

dvec2 calcDataRange(const std::shared_ptr<VolumeRAMPrecision<float>>& rep) {
    const auto [minIt, maxIt] = std::minmax_element(rep->getView().begin(), rep->getView().end());
    return dvec2{*minIt, *maxIt};
}

std::pair<std::shared_ptr<VolumeRAMPrecision<float>>, dvec2> readChgAndRange(
    const Chgcar& chg, File& file, pool::Stop stop, pool::Progress progress) {
    const auto data = readChg(chg, file, stop, progress);
    if (!data) {
        return {nullptr, {}};
    }
    const auto dataRange = calcDataRange(data);
    return {data, dataRange};
}

std::shared_ptr<Volume> createVolume(std::string_view name, dvec2 dataRange, const mat4& model,
                                     std::shared_ptr<VolumeRAMPrecision<float>> rep) {
    auto volume = std::make_shared<Volume>(
        VolumeConfig{.dimensions = rep->getDimensions(),
                     .format = DataFormat<float>::get(),
                     .wrapping = wrapping3d::repeatAll,
                     .xAxis = Axis{"x", units::unit_from_string("Angstrom")},
                     .yAxis = Axis{"y", units::unit_from_string("Angstrom")},
                     .zAxis = Axis{"z", units::unit_from_string("Angstrom")},
                     .valueAxis = Axis{std::string{name}, units::unit_from_string("e/Angstrom^3")},
                     .dataRange = dataRange,
                     .valueRange = dataRange,
                     .model = model});
    volume->addRepresentation(rep);
    return volume;
}
std::shared_ptr<Volume> createChg(dvec2 dataRange, const mat4& model,
                                  std::shared_ptr<VolumeRAMPrecision<float>> rep) {
    return createVolume("Charge Density", dataRange, model, rep);
}
std::shared_ptr<Volume> createMag(dvec2 dataRange, const mat4& model,
                                  std::shared_ptr<VolumeRAMPrecision<float>> rep) {
    return createVolume("Magnetization Density", dataRange, model, rep);
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
    , reload_("reload", "Reload data")
    , readChg_{"readChg", "Read charge density", true, InvalidationLevel::Valid}
    , readMag_{"readMag", "Read magnetisation density", true, InvalidationLevel::Valid}
    , chgInfo_{"chgInfo", "Charge information"}
    , magInfo_{"magInfo", "Magnetization information"}
    , basis_{"basis", "Basis and offset"}
    , potential_{"potential",
                 "Potential",
                 "Select a potential type, "
                 "See https://www.vasp.at/wiki/index.php/Available_pseudopotentials"
                 " for documentation. This refer to the potpaw.64 potentials"_help,
                 {{"standard_lda", "Standard LDA", vasp::PotentialType::Standard_LDA},
                  {"standard_pbe", "Standard PBE", vasp::PotentialType::Standard_PBE},
                  {"gw_lda", "GW LDA", vasp::PotentialType::GW_LDA},
                  {"gw_pbe", "GW PBE", vasp::PotentialType::GW_PBE}},
                 0}
    , potcars_{"potcars",
               "Potcar Names",
               "Add POTCAR name mappings here in the format [elem] -> [potcar name], one per line."
               " For example:<br> Mo->Mo_sv<br>Ti->Ti_sv<br>If noting is given the element name is"
               " used directly. See "
               "https://www.vasp.at/wiki/index.php/Available_pseudopotentials"
               " for documentation."_help,
               "",
               InvalidationLevel::InvalidOutput,
               PropertySemantics::Multiline}
    , colormap_{"colormap",
                "Colormap",
                {{"cpk", "Rasmol CPK", molvis::element::Colormap::RasmolCPK},
                 {"cpknew", "Rasmol CPK new", molvis::element::Colormap::RasmolCPKnew},
                 {"jmol", "Jmol", molvis::element::Colormap::Jmol}},
                0}
    , radiusScaling_{"radiusScaling", "Radius Scaling", 0.25, 0.0, 2.0, 0.01}
    , borderMargin_{"borderMargin", "Border Repetition Margin", 0.05, 0.0, 0.5}
    , pm_{this, 1, [this](PickingEvent* event) { picking(event); }}
    , data_{}
    , chg_{}
    , chgDataRange_{}
    , mag_{}
    , magDataRange_{} {

    isReady_.setUpdate([this]() -> ProcessorStatus {
        if (const auto& err = error()) {
            return {ProcessorStatus::Error, err.value()};
        } else if (file_.get().empty()) {
            static constexpr std::string_view reason{"File not set"};
            return {ProcessorStatus::NotReady, reason};
        } else if (!std::filesystem::is_regular_file(file_.get())) {
            static constexpr std::string_view reason{"Invalid or missing file"};
            return {ProcessorStatus::Error, reason};
        } else {
            return ProcessorStatus::Ready;
        }
    });

    addPorts(chargeOutport_, magnetizationOutport_, atomsOutport_, atomInformationOutport_,
             moleculeOutport_, bnlInport_);
    addProperties(file_, reload_, readChg_, readMag_, chgInfo_, magInfo_, basis_, potential_,
                  potcars_, colormap_, radiusScaling_, borderMargin_);
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

    if (data_ && !file_.isModified() && !reload_.isModified()) {
        if (chg_) {
            auto chg = createChg(chgDataRange_, data_->model, chg_);
            chgInfo_.updateVolume(*chg);
            basis_.updateEntity(*chg);
            chargeOutport_.setData(chg);
        }

        if (mag_) {
            auto mag = createMag(magDataRange_, data_->model, mag_);
            magInfo_.updateVolume(*mag);
            basis_.updateEntity(*mag);
            magnetizationOutport_.setData(mag);
        }

        pm_.resize(data_->pos.size());
        auto mesh =
            createMesh(*data_, pm_.getPickingId(0), colormap_, radiusScaling_, borderMargin_);
        auto df = createDataFrame(*data_, potential_.getSelectedValue(), potcars_.get());
        auto ms = createMolecularStructure(*data_, borderMargin_, file_.get().generic_string());

        basis_.updateEntity(*mesh);
        basis_.updateEntity(*ms);

        atomsOutport_.setData(mesh);
        atomInformationOutport_.setData(df);
        moleculeOutport_.setData(ms);

        return;
    }

    using Result = std::tuple<Chgcar, std::pair<std::shared_ptr<VolumeRAMPrecision<float>>, dvec2>,
                              std::pair<std::shared_ptr<VolumeRAMPrecision<float>>, dvec2>>;
    auto calc = [path = file_.get(), readChg = readChg_.get(), readMag = readMag_.get()](
                    pool::Stop stop, pool::Progress progress) -> Result {
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
                : std::pow(-chg.scale / glm::abs(glm::dot(chg.a1, glm::cross(chg.a2, chg.a1))),
                           1.0 / 3.0);
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

        if (!readChg && !readMag) {
            return {std::move(chg), {nullptr, {}}, {nullptr, {}}};
        }

        auto charge = readChgAndRange(chg, file, stop, progress);
        if (!charge.first) {
            return {std::move(chg), {nullptr, {}}, {nullptr, {}}};
        }

        if (!readMag) {
            return {std::move(chg), std::move(charge), {nullptr, {}}};
        }

        // Optionally read augmentation occupancies, might not be there in a chg file
        discardAugmentationOccupancies(chg, file);

        const auto mag = [&]() -> std::pair<std::shared_ptr<VolumeRAMPrecision<float>>, dvec2> {
            // Check if we have magnetization data?
            if (file.peekLine([&](std::string_view line) {
                    try {
                        size3_t dims;
                        forEachPart<3>(
                            line, [&](std::string_view elem, size_t i) { toNum(elem, dims[i]); });
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

                return readChgAndRange(chg, file, stop, progress);
            } else {
                return {nullptr, {}};
            }
        }();

        return {std::move(chg), std::move(charge), std::move(mag)};
    };

    chargeOutport_.clear();
    magnetizationOutport_.clear();
    atomsOutport_.clear();
    atomInformationOutport_.clear();
    moleculeOutport_.clear();

    dispatchOne(calc, [this](Result result) {
        data_ = std::make_unique<Chgcar>(std::move(std::get<0>(result)));
        std::tie(chg_, chgDataRange_) = std::get<1>(result);
        std::tie(mag_, magDataRange_) = std::get<2>(result);

        if (!data_) {
            chargeOutport_.clear();
            magnetizationOutport_.clear();
            atomsOutport_.clear();
            atomInformationOutport_.clear();
            moleculeOutport_.clear();
            newResults();
            return;
        }

        if (chg_) {
            auto chg = createChg(chgDataRange_, data_->model, chg_);
            chgInfo_.updateForNewVolume(
                *chg, deserialized_ ? util::OverwriteState::Yes : util::OverwriteState::No);
            chgInfo_.updateVolume(*chg);
            basis_.updateForNewEntity(*chg, deserialized_);
            basis_.updateEntity(*chg);
            chargeOutport_.setData(chg);
        } else {
            chargeOutport_.clear();
        }

        if (mag_) {
            auto mag = createMag(magDataRange_, data_->model, mag_);
            magInfo_.updateForNewVolume(
                *mag, deserialized_ ? util::OverwriteState::Yes : util::OverwriteState::No);
            magInfo_.updateVolume(*mag);
            basis_.updateEntity(*mag);
            magnetizationOutport_.setData(mag);
        } else {
            magnetizationOutport_.clear();
        }

        pm_.resize(data_->pos.size());
        auto mesh =
            createMesh(*data_, pm_.getPickingId(0), colormap_, radiusScaling_, borderMargin_);
        auto df = createDataFrame(*data_, potential_.getSelectedValue(), potcars_.get());
        auto ms = createMolecularStructure(*data_, borderMargin_, file_.get().generic_string());
        basis_.updateEntity(*mesh);
        basis_.updateEntity(*ms);

        atomsOutport_.setData(mesh);
        atomInformationOutport_.setData(df);
        moleculeOutport_.setData(ms);

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
            const auto i = event->getPickedId();
            bnlInport_.highlight(BitSet{static_cast<uint32_t>(i)});
            if (data_) {

                const auto pos = glm::dmat3{data_->model} * data_->pos.at(i);
                const auto elem = [&]() {
                    size_t count = 0;
                    for (auto&& [num, type] : util::zip(data_->nelem, data_->elem)) {
                        count += num;
                        if (i < count) {
                            return type;
                        }
                    }
                    return std::string{""};
                }();
                event->setToolTip(fmt::format("Atom id: {}\nType: {}\nPosition: {}\nFractional:{}",
                                              i, elem, pos, data_->pos.at(i)));

            } else {
                event->setToolTip(fmt::format("Atom id: {}", i));
            }
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

namespace vasp {

std::optional<Potential> findPotential(PotentialType type, std::string_view name) {
    static constexpr std::array<Potential, 170> standard_LDA{
        {{"H", 1, "1s<sup>1</sup>"},
         {"H.25", 0.25, "1s<sup>0.25</sup>"},
         {"H.33", 0.33, "1s<sup>0.33</sup>"},
         {"H.42", 0.42, "1s<sup>0.42</sup>"},
         {"H.5", 0.5, "1s<sup>0.5</sup>"},
         {"H.58", 0.58, "1s<sup>0.58</sup>"},
         {"H.66", 0.66, "1s<sup>0.66</sup>"},
         {"H.75", 0.75, "1s<sup>0.75</sup>"},
         {"H1.25", 1.25, "1s<sup>1.25</sup>"},
         {"H1.33", 1.33, "1s<sup>1.33</sup>"},
         {"H1.5", 1.5, "1s<sup>1.5</sup>"},
         {"H1.66", 1.66, "1s<sup>1.66</sup>"},
         {"H1.75", 1.75, "1s<sup>1.75</sup>"},
         {"H_AE", 1, ""},
         {"H_h", 1, "1s<sup>1</sup>"},
         {"H_s", 1, "1s<sup>1</sup>"},
         {"He", 2, "1s<sup>2</sup>"},
         {"Li", 1, "2s<sup>1</sup>"},
         {"Li_sv", 3, "1s<sup>2</sup> 2s<sup>1</sup>"},
         {"Be", 2, "2s<sup>1.9999</sup> 2p<sup>0.001</sup>"},
         {"Be_sv", 4, "1s<sup>2</sup> 2s<sup>1.9999</sup> 2p<sup>0.001</sup>"},
         {"B", 3, "2s<sup>2</sup> 2p<sup>1</sup>"},
         {"B_h", 3, "2s<sup>2</sup> 2p<sup>1</sup>"},
         {"B_s", 3, "2s<sup>2</sup> 2p<sup>1</sup>"},
         {"C", 4, "2s<sup>2</sup> 2p<sup>2</sup>"},
         {"C_h", 4, "2s<sup>2</sup> 2p<sup>2</sup>"},
         {"C_s", 4, "2s<sup>2</sup> 2p<sup>2</sup>"},
         {"N", 5, "2s<sup>2</sup> 2p<sup>3</sup>"},
         {"N_h", 5, "2s<sup>2</sup> 2p<sup>3</sup>"},
         {"N_s", 5, "2s<sup>2</sup> 2p<sup>3</sup>"},
         {"O", 6, "2s<sup>2</sup> 2p<sup>4</sup>"},
         {"O_h", 6, "2s<sup>2</sup> 2p<sup>4</sup>"},
         {"O_s", 6, "2s<sup>2</sup> 2p<sup>4</sup>"},
         {"F", 7, "2s<sup>2</sup> 2p<sup>5</sup>"},
         {"F_h", 7, "2s<sup>2</sup> 2p<sup>5</sup>"},
         {"F_s", 7, "2s<sup>2</sup> 2p<sup>5</sup>"},
         {"Ne", 8, "2s<sup>2</sup> 2p<sup>6</sup>"},
         {"Na", 1, "3s<sup>1</sup>"},
         {"Na_pv", 7, "2p<sup>6</sup> 3s<sup>1</sup>"},
         {"Na_sv", 9, "2s<sup>2</sup> 2p<sup>6</sup> 3s<sup>1</sup>"},
         {"Mg", 2, "3s<sup>1.999</sup> 3p<sup>0.001</sup>"},
         {"Mg_pv", 8, "2p<sup>6</sup> 3s<sup>2</sup>"},
         {"Mg_sv", 10, "2s<sup>2</sup> 2p<sup>6</sup> 3s<sup>2</sup>"},
         {"Al", 3, "3s<sup>2</sup> 3p<sup>1</sup>"},
         {"Si", 4, "3s<sup>2</sup> 3p<sup>2</sup>"},
         {"P", 5, "3s<sup>2</sup> 3p<sup>3</sup>"},
         {"P_h", 5, "3s<sup>2</sup> 3p<sup>3</sup>"},
         {"S", 6, "3s<sup>2</sup> 3p<sup>4</sup>"},
         {"S_h", 6, "3s<sup>2</sup> 3p<sup>4</sup>"},
         {"Cl", 7, "3s<sup>2</sup> 3p<sup>5</sup>"},
         {"Cl_h", 7, "3s<sup>2</sup> 3p<sup>5</sup>"},
         {"Ar", 8, "3s<sup>2</sup> 3p<sup>6</sup>"},
         {"K_pv", 7, "3p<sup>6</sup> 4s<sup>1</sup>"},
         {"K_sv", 9, "3s<sup>2</sup> 3p<sup>6</sup> 4s<sup>1</sup>"},
         {"Ca_pv", 8, "3p<sup>6</sup> 4s<sup>2</sup>"},
         {"Ca_sv", 10, "3s<sup>2</sup> 3p<sup>6</sup> 4s<sup>2</sup>"},
         {"Sc", 3, "3d<sup>2</sup> 4s<sup>1</sup>"},
         {"Sc_sv", 11, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>2</sup> 4s<sup>1</sup>"},
         {"Ti", 4, "3d<sup>3</sup> 4s<sup>1</sup>"},
         {"Ti_pv", 10, "3p<sup>6</sup> 3d<sup>3</sup> 4s<sup>1</sup>"},
         {"Ti_sv", 12, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>3</sup> 4s<sup>1</sup>"},
         {"V", 5, "3d<sup>4</sup> 4s<sup>1</sup>"},
         {"V_pv", 11, "3p<sup>6</sup> 3d<sup>4</sup> 4s<sup>1</sup>"},
         {"V_sv", 13, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>4</sup> 4s<sup>1</sup>"},
         {"Cr", 6, "3d<sup>5</sup> 4s<sup>1</sup>"},
         {"Cr_pv", 12, "3p<sup>6</sup> 3d<sup>5</sup> 4s<sup>1</sup>"},
         {"Cr_sv", 14, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>5</sup> 4s<sup>1</sup>"},
         {"Mn", 7, "3d<sup>6</sup> 4s<sup>1</sup>"},
         {"Mn_pv", 13, "3p<sup>6</sup> 3d<sup>6</sup> 4s<sup>1</sup>"},
         {"Mn_sv", 15, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>6</sup> 4s<sup>1</sup>"},
         {"Fe", 8, "3d<sup>7</sup> 4s<sup>1</sup>"},
         {"Fe_pv", 14, "3p<sup>6</sup> 3d<sup>7</sup> 4s<sup>1</sup>"},
         {"Fe_sv", 16, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>7</sup> 4s<sup>1</sup>"},
         {"Co", 9, "3d<sup>8</sup> 4s<sup>1</sup>"},
         {"Co_pv", 15, "3p<sup>6</sup> 3d<sup>8</sup> 4s<sup>1</sup>"},
         {"Co_sv", 17, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>8</sup> 4s<sup>1</sup>"},
         {"Ni", 10, "3d<sup>9</sup> 4s<sup>1</sup>"},
         {"Ni_pv", 16, "3p<sup>6</sup> 3d<sup>9</sup> 4s<sup>1</sup>"},
         {"Cu", 11, "3d<sup>10</sup> 4s<sup>1</sup>"},
         {"Cu_pv", 17, "3p<sup>6</sup> 3d<sup>10</sup> 4s<sup>1</sup>"},
         {"Zn", 12, "3d<sup>10</sup> 4s<sup>2</sup>"},
         {"Ga", 3, "4s<sup>2</sup> 4p<sup>1</sup>"},
         {"Ga_d", 13, "3d<sup>10</sup> 4s<sup>2</sup> 4p<sup>1</sup>"},
         {"Ga_h", 13, "3d<sup>10</sup> 4s<sup>2</sup> 4p<sup>1</sup>"},
         {"Ge", 4, "4s<sup>2</sup> 4p<sup>2</sup>"},
         {"Ge_d", 14, "3d<sup>10</sup> 4s<sup>2</sup> 4p<sup>2</sup>"},
         {"Ge_h", 14, "3d<sup>10</sup> 4s<sup>2</sup> 4p<sup>2</sup>"},
         {"As", 5, "4s<sup>2</sup> 4p<sup>3</sup>"},
         {"As_d", 15, "3d<sup>10</sup> 4s<sup>2</sup> 4p<sup>3</sup>"},
         {"Se", 6, "4s<sup>2</sup> 4p<sup>4</sup>"},
         {"Br", 7, "4s<sup>2</sup> 4p<sup>5</sup>"},
         {"Kr", 8, "4s<sup>2</sup> 4p<sup>6</sup>"},
         {"Rb_pv", 7, "4p<sup>6</sup> 4d<sup>0.001</sup> 5s<sup>0.999</sup>"},
         {"Rb_sv", 9, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>0.001</sup> 5s<sup>0.999</sup>"},
         {"Sr_sv", 10, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>0.001</sup> 5s<sup>1.999</sup>"},
         {"Y_sv", 11, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>2</sup> 5s<sup>1</sup>"},
         {"Zr_sv", 12, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>3</sup> 5s<sup>1</sup>"},
         {"Nb_pv", 11, "4p<sup>6</sup> 4d<sup>4</sup> 5s<sup>1</sup>"},
         {"Nb_sv", 13, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>4</sup> 5s<sup>1</sup>"},
         {"Mo", 6, "4d<sup>5</sup> 5s<sup>1</sup>"},
         {"Mo_pv", 12, "4p<sup>6</sup> 4d<sup>5</sup> 5s<sup>1</sup>"},
         {"Mo_sv", 14, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>5</sup> 5s<sup>1</sup>"},
         {"Tc", 7, "4d<sup>6</sup> 5s<sup>1</sup>"},
         {"Tc_pv", 13, "4p<sup>6</sup> 4d<sup>6</sup> 5s<sup>1</sup>"},
         {"Tc_sv", 15, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>6</sup> 5s<sup>1</sup>"},
         {"Ru", 8, "4d<sup>7</sup> 5s<sup>1</sup>"},
         {"Ru_pv", 14, "4p<sup>6</sup> 4d<sup>7</sup> 5s<sup>1</sup>"},
         {"Ru_sv", 16, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>7</sup> 5s<sup>1</sup>"},
         {"Rh", 9, "4d<sup>8</sup> 5s<sup>1</sup>"},
         {"Rh_pv", 15, "4p<sup>6</sup> 4d<sup>8</sup> 5s<sup>1</sup>"},
         {"Pd", 10, "4d<sup>9</sup> 5s<sup>1</sup>"},
         {"Pd_pv", 16, "4p<sup>6</sup> 4d<sup>9</sup> 5s<sup>1</sup>"},
         {"Ag", 11, "4d<sup>10</sup> 5s<sup>1</sup>"},
         {"Ag_pv", 17, "4p<sup>6</sup> 4d<sup>10</sup> 5s<sup>1</sup>"},
         {"Cd", 12, "4d<sup>10</sup> 5s<sup>2</sup>"},
         {"In", 3, "5s<sup>2</sup> 5p<sup>1</sup>"},
         {"In_d", 13, "4d<sup>10</sup> 5s<sup>2</sup> 5p<sup>1</sup>"},
         {"Sn", 4, "5s<sup>2</sup> 5p<sup>2</sup>"},
         {"Sn_d", 14, "4d<sup>10</sup> 5s<sup>2</sup> 5p<sup>2</sup>"},
         {"Sb", 5, "5s<sup>2</sup> 5p<sup>3</sup>"},
         {"Te", 6, "5s<sup>2</sup> 5p<sup>4</sup>"},
         {"I", 7, "5s<sup>2</sup> 5p<sup>5</sup>"},
         {"Xe", 8, "5s<sup>2</sup> 5p<sup>6</sup>"},
         {"Cs_sv", 9, "5s<sup>2</sup> 5p<sup>6</sup> 6s<sup>1</sup>"},
         {"Ba_sv", 10, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.001</sup> 6s<sup>1.999</sup>"},
         {"La", 11,
          "4f<sup>0.0001</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.9999</sup> 6s<sup>2</sup>"},
         {"La_s", 9, "4f<sup>0.0001</sup> 5p<sup>6</sup> 5d<sup>0.9999</sup> 6s<sup>2</sup>"},
         {"Ce", 12, "4f<sup>1</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>1</sup> 6s<sup>2</sup>"},
         {"Ce_h", 12, "4f<sup>1</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>1</sup> 6s<sup>2</sup>"},
         {"Hf", 4, "5d<sup>3</sup> 6s<sup>1</sup>"},
         {"Hf_pv", 10, "5p<sup>6</sup> 5d<sup>3</sup> 6s<sup>1</sup>"},
         {"Hf_sv", 12, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>4</sup>"},
         {"Ta", 5, "5d<sup>4</sup> 6s<sup>1</sup>"},
         {"Ta_pv", 11, "5p<sup>6</sup> 5d<sup>4</sup> 6s<sup>1</sup>"},
         {"W", 6, "5d<sup>5</sup> 6s<sup>1</sup>"},
         {"W_sv", 14, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>5</sup> 6s<sup>1</sup>"},
         {"Re", 7, "5d<sup>6</sup> 6s<sup>1</sup>"},
         {"Re_pv", 13, "5p<sup>6</sup> 5d<sup>6</sup> 6s<sup>1</sup>"},
         {"Os", 8, "5d<sup>7</sup> 6s<sup>1</sup>"},
         {"Os_pv", 14, "5p<sup>6</sup> 5d<sup>7</sup> 6s<sup>1</sup>"},
         {"Ir", 9, "5d<sup>8</sup> 6s<sup>1</sup>"},
         {"Pt", 10, "5d<sup>9</sup> 6s<sup>1</sup>"},
         {"Pt_pv", 16, "5p<sup>6</sup> 5d<sup>9</sup> 6s<sup>1</sup>"},
         {"Au", 11, "5d<sup>10</sup> 6s<sup>1</sup>"},
         {"Hg", 12, "5d<sup>10</sup> 6s<sup>2</sup>"},
         {"Tl", 3, "6s<sup>2</sup> 6p<sup>1</sup>"},
         {"Tl_d", 13, "5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>1</sup>"},
         {"Pb", 4, "6s<sup>2</sup> 6p<sup>2</sup>"},
         {"Pb_d", 14, "5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>2</sup>"},
         {"Bi", 5, "6s<sup>2</sup> 6p<sup>3</sup>"},
         {"Bi_d", 15, "5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>3</sup>"},
         {"Po", 6, "6s<sup>2</sup> 6p<sup>4</sup>"},
         {"Po_d", 16, "5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>4</sup>"},
         {"At", 7, "6s<sup>2</sup> 6p<sup>5</sup>"},
         {"Rn", 8, "6s<sup>2</sup> 6p<sup>6</sup>"},
         {"Fr_sv", 9, "6s<sup>2</sup> 6p<sup>6</sup> 7s<sup>1</sup>"},
         {"Ra_sv", 10, "6s<sup>2</sup> 6p<sup>6</sup> 7s<sup>2</sup>"},
         {"Ac", 11,
          "5f<sup>0.0001</sup> 6s<sup>2</sup> 6p<sup>6</sup> 6d<sup>0.9999</sup> 7s<sup>2</sup>"},
         {"Th", 12, "5f<sup>1</sup> 6s<sup>2</sup> 6p<sup>6</sup> 6d<sup>1</sup> 7s<sup>2</sup>"},
         {"Th_s", 10, "5f<sup>1</sup> 6p<sup>6</sup> 6d<sup>1</sup> 7s<sup>2</sup>"},
         {"Pa", 13, "5f<sup>1</sup> 6s<sup>2</sup> 6p<sup>6</sup> 6d<sup>2</sup> 7s<sup>2</sup>"},
         {"Pa_s", 11, "5f<sup>1</sup> 6p<sup>6</sup> 6d<sup>2</sup> 7s<sup>2</sup>"},
         {"U", 14, "5f<sup>2</sup> 6s<sup>2</sup> 6p<sup>6</sup> 6d<sup>2</sup> 7s<sup>2</sup>"},
         {"U_s", 14, "5f<sup>2</sup> 6s<sup>2</sup> 6p<sup>6</sup> 6d<sup>2</sup> 7s<sup>2</sup>"},
         {"Np", 15, "5f<sup>3</sup> 6s<sup>2</sup> 6p<sup>6</sup> 6d<sup>2</sup> 7s<sup>2</sup>"},
         {"Np_s", 15, "5f<sup>3</sup> 6s<sup>2</sup> 6p<sup>6</sup> 6d<sup>2</sup> 7s<sup>2</sup>"},
         {"Pu", 16, "5f<sup>4</sup> 6s<sup>2</sup> 6p<sup>6</sup> 6d<sup>2</sup> 7s<sup>2</sup>"},
         {"Pu_s", 16, "5f<sup>4</sup> 6s<sup>2</sup> 6p<sup>6</sup> 6d<sup>2</sup> 7s<sup>2</sup>"},
         {"Am", 17, "5f<sup>5</sup> 6s<sup>2</sup> 6p<sup>6</sup> 6d<sup>2</sup> 7s<sup>2</sup>"},
         {"Cm", 18, "5f<sup>6</sup> 6s<sup>2</sup> 6p<sup>6</sup> 6d<sup>2</sup> 7s<sup>2</sup>"}}};

    static constexpr std::array<Potential, 214> standard_PBE{
        {{"H", 1, "1s<sup>1</sup>"},
         {"H.25", 0.25, "1s<sup>0.25</sup>"},
         {"H.33", 0.33, "1s<sup>0.33</sup>"},
         {"H.42", 0.42, "1s<sup>0.42</sup>"},
         {"H.5", 0.5, "1s<sup>0.5</sup>"},
         {"H.58", 0.58, "1s<sup>0.58</sup>"},
         {"H.66", 0.66, "1s<sup>0.66</sup>"},
         {"H.75", 0.75, "1s<sup>0.75</sup>"},
         {"H1.25", 1.25, "1s<sup>1.25</sup>"},
         {"H1.33", 1.33, "1s<sup>1.33</sup>"},
         {"H1.5", 1.5, "1s<sup>1.5</sup>"},
         {"H1.66", 1.66, "1s<sup>1.66</sup>"},
         {"H1.75", 1.75, "1s<sup>1.75</sup>"},
         {"H_AE", 1, ""},
         {"H_h", 1, "1s<sup>1</sup>"},
         {"H_s", 1, "1s<sup>1</sup>"},
         {"He", 2, "1s<sup>2</sup>"},
         {"He_AE", 2, "1s<sup>2</sup>"},
         {"Li", 1, "2s<sup>1</sup>"},
         {"Li_sv", 3, "1s<sup>2</sup> 2s<sup>1</sup>"},
         {"Be", 2, "2s<sup>1.99</sup> 2p<sup>0.01</sup>"},
         {"Be_sv", 4, "1s<sup>2</sup> 2s<sup>1.99</sup> 2p<sup>0.01</sup>"},
         {"B", 3, "2s<sup>2</sup> 2p<sup>1</sup>"},
         {"B_h", 3, "2s<sup>2</sup> 2p<sup>1</sup>"},
         {"B_s", 3, "2s<sup>2</sup> 2p<sup>1</sup>"},
         {"C", 4, "2s<sup>2</sup> 2p<sup>2</sup>"},
         {"C_h", 4, "2s<sup>2</sup> 2p<sup>2</sup>"},
         {"C_s", 4, "2s<sup>2</sup> 2p<sup>2</sup>"},
         {"N", 5, "2s<sup>2</sup> 2p<sup>3</sup>"},
         {"N_h", 5, "2s<sup>2</sup> 2p<sup>3</sup>"},
         {"N_s", 5, "2s<sup>2</sup> 2p<sup>3</sup>"},
         {"O", 6, "2s<sup>2</sup> 2p<sup>4</sup>"},
         {"O_h", 6, "2s<sup>2</sup> 2p<sup>4</sup>"},
         {"O_s", 6, "2s<sup>2</sup> 2p<sup>4</sup>"},
         {"F", 7, "2s<sup>2</sup> 2p<sup>5</sup>"},
         {"F_h", 7, "2s<sup>2</sup> 2p<sup>5</sup>"},
         {"F_s", 7, "2s<sup>2</sup> 2p<sup>5</sup>"},
         {"Ne", 8, "2s<sup>2</sup> 2p<sup>6</sup>"},
         {"Na", 1, "3s<sup>1</sup>"},
         {"Na_pv", 7, "2p<sup>6</sup> 3s<sup>1</sup>"},
         {"Na_sv", 9, "2s<sup>2</sup> 2p<sup>6</sup> 3s<sup>1</sup>"},
         {"Mg", 2, "3s<sup>2</sup>"},
         {"Mg_pv", 8, "2p<sup>6</sup> 3s<sup>2</sup>"},
         {"Mg_sv", 10, "2s<sup>2</sup> 2p<sup>6</sup> 3s<sup>2</sup>"},
         {"Al", 3, "3s<sup>2</sup> 3p<sup>1</sup>"},
         {"Si", 4, "3s<sup>2</sup> 3p<sup>2</sup>"},
         {"P", 5, "3s<sup>2</sup> 3p<sup>3</sup>"},
         {"P_h", 5, "3s<sup>2</sup> 3p<sup>3</sup>"},
         {"S", 6, "3s<sup>2</sup> 3p<sup>4</sup>"},
         {"S_h", 6, "3s<sup>2</sup> 3p<sup>4</sup>"},
         {"Cl", 7, "3s<sup>2</sup> 3p<sup>5</sup>"},
         {"Cl_h", 7, "3s<sup>2</sup> 3p<sup>5</sup>"},
         {"Ar", 8, "3s<sup>2</sup> 3p<sup>6</sup>"},
         {"K_pv", 7, "3p<sup>6</sup> 4s<sup>1</sup>"},
         {"K_sv", 9, "3s<sup>2</sup> 3p<sup>6</sup> 4s<sup>1</sup>"},
         {"Ca_pv", 8, "3p<sup>6</sup> 4s<sup>2</sup>"},
         {"Ca_sv", 10, "3s<sup>2</sup> 3p<sup>6</sup> 4s<sup>2</sup>"},
         {"Sc", 3, "3d<sup>2</sup> 4s<sup>1</sup>"},
         {"Sc_sv", 11, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>2</sup> 4s<sup>1</sup>"},
         {"Ti", 4, "3d<sup>3</sup> 4s<sup>1</sup>"},
         {"Ti_pv", 10, "3p<sup>6</sup> 3d<sup>3</sup> 4s<sup>1</sup>"},
         {"Ti_sv", 12, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>3</sup> 4s<sup>1</sup>"},
         {"V", 5, "3d<sup>4</sup> 4s<sup>1</sup>"},
         {"V_pv", 11, "3p<sup>6</sup> 3d<sup>4</sup> 4s<sup>1</sup>"},
         {"V_sv", 13, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>4</sup> 4s<sup>1</sup>"},
         {"Cr", 6, "3d<sup>5</sup> 4s<sup>1</sup>"},
         {"Cr_pv", 12, "3p<sup>6</sup> 3d<sup>5</sup> 4s<sup>1</sup>"},
         {"Cr_sv", 14, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>5</sup> 4s<sup>1</sup>"},
         {"Mn", 7, "3d<sup>6</sup> 4s<sup>1</sup>"},
         {"Mn_pv", 13, "3p<sup>6</sup> 3d<sup>6</sup> 4s<sup>1</sup>"},
         {"Mn_sv", 15, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>6</sup> 4s<sup>1</sup>"},
         {"Fe", 8, "3d<sup>7</sup> 4s<sup>1</sup>"},
         {"Fe_pv", 14, "3p<sup>6</sup> 3d<sup>7</sup> 4s<sup>1</sup>"},
         {"Fe_sv", 16, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>7</sup> 4s<sup>1</sup>"},
         {"Co", 9, "3d<sup>8</sup> 4s<sup>1</sup>"},
         {"Co_pv", 15, "3p<sup>6</sup> 3d<sup>8</sup> 4s<sup>1</sup>"},
         {"Co_sv", 17, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>8</sup> 4s<sup>1</sup>"},
         {"Ni", 10, "3d<sup>9</sup> 4s<sup>1</sup>"},
         {"Ni_pv", 16, "3p<sup>6</sup> 3d<sup>9</sup> 4s<sup>1</sup>"},
         {"Cu", 11, "3d<sup>10</sup> 4s<sup>1</sup>"},
         {"Cu_pv", 17, "3p<sup>6</sup> 3d<sup>10</sup> 4s<sup>1</sup>"},
         {"Zn", 12, "3d<sup>10</sup> 4s<sup>2</sup>"},
         {"Ga", 3, "4s<sup>2</sup> 4p<sup>1</sup>"},
         {"Ga_d", 13, "3d<sup>10</sup> 4s<sup>2</sup> 4p<sup>1</sup>"},
         {"Ga_h", 13, "3d<sup>10</sup> 4s<sup>2</sup> 4p<sup>1</sup>"},
         {"Ge", 4, "4s<sup>2</sup> 4p<sup>2</sup>"},
         {"Ge_d", 14, "3d<sup>10</sup> 4s<sup>2</sup> 4p<sup>2</sup>"},
         {"Ge_h", 14, "3d<sup>10</sup> 4s<sup>2</sup> 4p<sup>2</sup>"},
         {"As", 5, "4s<sup>2</sup> 4p<sup>3</sup>"},
         {"As_d", 15, "3d<sup>10</sup> 4s<sup>2</sup> 4p<sup>3</sup>"},
         {"Se", 6, "4s<sup>2</sup> 4p<sup>4</sup>"},
         {"Br", 7, "4s<sup>2</sup> 4p<sup>5</sup>"},
         {"Kr", 8, "4s<sup>2</sup> 4p<sup>6</sup>"},
         {"Rb_pv", 7, "4p<sup>6</sup> 4d<sup>0.001</sup> 5s<sup>0.999</sup>"},
         {"Rb_sv", 9, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>0.001</sup> 5s<sup>0.999</sup>"},
         {"Sr_sv", 10, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>0.001</sup> 5s<sup>1.999</sup>"},
         {"Y_sv", 11, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>2</sup> 5s<sup>1</sup>"},
         {"Zr_sv", 12, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>3</sup> 5s<sup>1</sup>"},
         {"Nb_pv", 11, "4p<sup>6</sup> 4d<sup>4</sup> 5s<sup>1</sup>"},
         {"Nb_sv", 13, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>4</sup> 5s<sup>1</sup>"},
         {"Mo", 6, "4d<sup>5</sup> 5s<sup>1</sup>"},
         {"Mo_pv", 12, "4p<sup>6</sup> 4d<sup>5</sup> 5s<sup>1</sup>"},
         {"Mo_sv", 14, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>5</sup> 5s<sup>1</sup>"},
         {"Tc", 7, "4d<sup>6</sup> 5s<sup>1</sup>"},
         {"Tc_pv", 13, "4p<sup>6</sup> 4d<sup>6</sup> 5s<sup>1</sup>"},
         {"Tc_sv", 15, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>6</sup> 5s<sup>1</sup>"},
         {"Ru", 8, "4d<sup>7</sup> 5s<sup>1</sup>"},
         {"Ru_pv", 14, "4p<sup>6</sup> 4d<sup>7</sup> 5s<sup>1</sup>"},
         {"Ru_sv", 16, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>7</sup> 5s<sup>1</sup>"},
         {"Rh", 9, "4d<sup>8</sup> 5s<sup>1</sup>"},
         {"Rh_pv", 15, "4p<sup>6</sup> 4d<sup>8</sup> 5s<sup>1</sup>"},
         {"Pd", 10, "4d<sup>9</sup> 5s<sup>1</sup>"},
         {"Pd_pv", 16, "4p<sup>6</sup> 4d<sup>9</sup> 5s<sup>1</sup>"},
         {"Ag", 11, "4d<sup>10</sup> 5s<sup>1</sup>"},
         {"Ag_pv", 17, "4p<sup>6</sup> 4d<sup>10</sup> 5s<sup>1</sup>"},
         {"Cd", 12, "4d<sup>10</sup> 5s<sup>2</sup>"},
         {"In", 3, "5s<sup>2</sup> 5p<sup>1</sup>"},
         {"In_d", 13, "4d<sup>10</sup> 5s<sup>2</sup> 5p<sup>1</sup>"},
         {"Sn", 4, "5s<sup>2</sup> 5p<sup>2</sup>"},
         {"Sn_d", 14, "4d<sup>10</sup> 5s<sup>2</sup> 5p<sup>2</sup>"},
         {"Sb", 5, "5s<sup>2</sup> 5p<sup>3</sup>"},
         {"Te", 6, "5s<sup>2</sup> 5p<sup>4</sup>"},
         {"I", 7, "5s<sup>2</sup> 5p<sup>5</sup>"},
         {"Xe", 8, "5s<sup>2</sup> 5p<sup>6</sup>"},
         {"Cs_sv", 9, "5s<sup>2</sup> 5p<sup>6</sup> 6s<sup>1</sup>"},
         {"Ba_sv", 10, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.01</sup> 6s<sup>1.99</sup>"},
         {"La", 11,
          "4f<sup>0.0001</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.9999</sup> 6s<sup>2</sup>"},
         {"La_s", 9, "5p<sup>6</sup> 5d<sup>1</sup> 6s<sup>2</sup>"},
         {"Ce", 12, "4f<sup>1</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>1</sup> 6s<sup>2</sup>"},
         {"Ce_3", 11, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>1</sup> 6s<sup>2</sup>"},
         {"Ce_h", 12, "4f<sup>1</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>1</sup> 6s<sup>2</sup>"},
         {"Pr", 13,
          "4f<sup>2.5</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.5</sup> 6s<sup>2</sup>"},
         {"Pr_3", 11, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>1</sup> 6s<sup>2</sup>"},
         {"Pr_h", 13,
          "4f<sup>2.5</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.5</sup> 6s<sup>2</sup>"},
         {"Nd", 14,
          "4f<sup>3.5</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.5</sup> 6s<sup>2</sup>"},
         {"Nd_3", 11, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>1</sup> 6s<sup>2</sup>"},
         {"Nd_h", 14,
          "4f<sup>3.5</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.5</sup> 6s<sup>2</sup>"},
         {"Pm", 15,
          "4f<sup>4.5</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.5</sup> 6s<sup>2</sup>"},
         {"Pm_3", 11, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>1</sup> 6s<sup>2</sup>"},
         {"Pm_h", 15,
          "4f<sup>4.5</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.5</sup> 6s<sup>2</sup>"},
         {"Sm", 16,
          "4f<sup>5.5</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.5</sup> 6s<sup>2</sup>"},
         {"Sm_3", 11, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>1</sup> 6s<sup>2</sup>"},
         {"Sm_h", 16,
          "4f<sup>5.5</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.5</sup> 6s<sup>2</sup>"},
         {"Eu", 17,
          "4f<sup>6.5</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.5</sup> 6s<sup>2</sup>"},
         {"Eu_2", 8, "5p<sup>6</sup> 6s<sup>2</sup>"},
         {"Eu_3", 9, "5p<sup>6</sup> 5d<sup>1</sup> 6s<sup>2</sup>"},
         {"Eu_h", 17,
          "4f<sup>6.5</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.5</sup> 6s<sup>2</sup>"},
         {"Gd", 18,
          "4f<sup>7.5</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.5</sup> 6s<sup>2</sup>"},
         {"Gd_3", 9, "5p<sup>6</sup> 5d<sup>1</sup> 6s<sup>2</sup>"},
         {"Gd_h", 18,
          "4f<sup>7.5</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.5</sup> 6s<sup>2</sup>"},
         {"Tb", 19,
          "4f<sup>8.5</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.5</sup> 6s<sup>2</sup>"},
         {"Tb_3", 9, "5p<sup>6</sup> 5d<sup>1</sup> 6s<sup>2</sup>"},
         {"Tb_h", 19,
          "4f<sup>8.5</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.5</sup> 6s<sup>2</sup>"},
         {"Dy", 20,
          "4f<sup>9.5</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.5</sup> 6s<sup>2</sup>"},
         {"Dy_3", 9, "5p<sup>6</sup> 5d<sup>1</sup> 6s<sup>2</sup>"},
         {"Dy_h", 20,
          "4f<sup>9.5</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.5</sup> 6s<sup>2</sup>"},
         {"Ho", 21,
          "4f<sup>10.5</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.5</sup> 6s<sup>2</sup>"},
         {"Ho_3", 9, "5p<sup>6</sup> 5d<sup>1</sup> 6s<sup>2</sup>"},
         {"Ho_h", 21,
          "4f<sup>10.5</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.5</sup> 6s<sup>2</sup>"},
         {"Er", 22,
          "4f<sup>11.5</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.5</sup> 6s<sup>2</sup>"},
         {"Er_2", 8, "5p<sup>6</sup> 6s<sup>2</sup>"},
         {"Er_3", 9, "5p<sup>6</sup> 5d<sup>1</sup> 6s<sup>2</sup>"},
         {"Er_h", 22,
          "4f<sup>11.5</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.5</sup> 6s<sup>2</sup>"},
         {"Tm", 23,
          "4f<sup>12.5</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.5</sup> 6s<sup>2</sup>"},
         {"Tm_3", 9, "5p<sup>6</sup> 5d<sup>1</sup> 6s<sup>2</sup>"},
         {"Tm_h", 23,
          "4f<sup>12.5</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.5</sup> 6s<sup>2</sup>"},
         {"Yb", 24,
          "4f<sup>13.5</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.5</sup> 6s<sup>2</sup>"},
         {"Yb_2", 8, "5p<sup>6</sup> 6s<sup>2</sup>"},
         {"Yb_3", 9, "5p<sup>6</sup> 5d<sup>1</sup> 6s<sup>2</sup>"},
         {"Yb_h", 24,
          "4f<sup>13.5</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.5</sup> 6s<sup>2</sup>"},
         {"Lu", 25, "4f<sup>14</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>1</sup> 6s<sup>2</sup>"},
         {"Lu_3", 9, "5p<sup>6</sup> 5d<sup>1</sup> 6s<sup>2</sup>"},
         {"Hf", 4, "5d<sup>3</sup> 6s<sup>1</sup>"},
         {"Hf_pv", 10, "5p<sup>6</sup> 5d<sup>3</sup> 6s<sup>1</sup>"},
         {"Hf_sv", 12, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>4</sup>"},
         {"Ta", 5, "5d<sup>4</sup> 6s<sup>1</sup>"},
         {"Ta_pv", 11, "5p<sup>6</sup> 5d<sup>4</sup> 6s<sup>1</sup>"},
         {"W", 6, "5d<sup>5</sup> 6s<sup>1</sup>"},
         {"W_sv", 14, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>5</sup> 6s<sup>1</sup>"},
         {"Re", 7, "5d<sup>6</sup> 6s<sup>1</sup>"},
         {"Re_pv", 13, "5p<sup>6</sup> 5d<sup>6</sup> 6s<sup>1</sup>"},
         {"Os", 8, "5d<sup>7</sup> 6s<sup>1</sup>"},
         {"Os_pv", 14, "5p<sup>6</sup> 5d<sup>7</sup> 6s<sup>1</sup>"},
         {"Ir", 9, "5d<sup>8</sup> 6s<sup>1</sup>"},
         {"Pt", 10, "5d<sup>9</sup> 6s<sup>1</sup>"},
         {"Pt_pv", 16, "5p<sup>6</sup> 5d<sup>9</sup> 6s<sup>1</sup>"},
         {"Au", 11, "5d<sup>10</sup> 6s<sup>1</sup>"},
         {"Hg", 12, "5d<sup>10</sup> 6s<sup>2</sup>"},
         {"Tl", 3, "6s<sup>2</sup> 6p<sup>1</sup>"},
         {"Tl_d", 13, "5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>1</sup>"},
         {"Pb", 4, "6s<sup>2</sup> 6p<sup>2</sup>"},
         {"Pb_d", 14, "5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>2</sup>"},
         {"Bi", 5, "6s<sup>2</sup> 6p<sup>3</sup>"},
         {"Bi_d", 15, "5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>3</sup>"},
         {"Po", 6, "6s<sup>2</sup> 6p<sup>4</sup>"},
         {"Po_d", 16, "5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>4</sup>"},
         {"At", 7, "6s<sup>2</sup> 6p<sup>5</sup>"},
         {"Rn", 8, "6s<sup>2</sup> 6p<sup>6</sup>"},
         {"Fr_sv", 9, "6s<sup>2</sup> 6p<sup>6</sup> 7s<sup>1</sup>"},
         {"Ra_sv", 10, "6s<sup>2</sup> 6p<sup>6</sup> 7s<sup>2</sup>"},
         {"Ac", 11, "6s<sup>2</sup> 6p<sup>6</sup> 6d<sup>1</sup> 7s<sup>2</sup>"},
         {"Th", 12, "5f<sup>1</sup> 6s<sup>2</sup> 6p<sup>6</sup> 6d<sup>1</sup> 7s<sup>2</sup>"},
         {"Th_s", 10, "5f<sup>1</sup> 6p<sup>6</sup> 6d<sup>1</sup> 7s<sup>2</sup>"},
         {"Pa", 13, "5f<sup>1</sup> 6s<sup>2</sup> 6p<sup>6</sup> 6d<sup>2</sup> 7s<sup>2</sup>"},
         {"Pa_s", 11, "5f<sup>1</sup> 6p<sup>6</sup> 6d<sup>2</sup> 7s<sup>2</sup>"},
         {"U", 14, "5f<sup>2</sup> 6s<sup>2</sup> 6p<sup>6</sup> 6d<sup>2</sup> 7s<sup>2</sup>"},
         {"U_s", 14, "5f<sup>2</sup> 6s<sup>2</sup> 6p<sup>6</sup> 6d<sup>2</sup> 7s<sup>2</sup>"},
         {"Np", 15, "5f<sup>3</sup> 6s<sup>2</sup> 6p<sup>6</sup> 6d<sup>2</sup> 7s<sup>2</sup>"},
         {"Np_s", 15, "5f<sup>3</sup> 6s<sup>2</sup> 6p<sup>6</sup> 6d<sup>2</sup> 7s<sup>2</sup>"},
         {"Pu", 16, "5f<sup>4</sup> 6s<sup>2</sup> 6p<sup>6</sup> 6d<sup>2</sup> 7s<sup>2</sup>"},
         {"Pu_s", 16, "5f<sup>4</sup> 6s<sup>2</sup> 6p<sup>6</sup> 6d<sup>2</sup> 7s<sup>2</sup>"},
         {"Am", 17, "5f<sup>5</sup> 6s<sup>2</sup> 6p<sup>6</sup> 6d<sup>2</sup> 7s<sup>2</sup>"},
         {"Cm", 18, "5f<sup>6</sup> 6s<sup>2</sup> 6p<sup>6</sup> 6d<sup>2</sup> 7s<sup>2</sup>"},
         {"Cf", 20, "5f<sup>8</sup> 6s<sup>2</sup> 6p<sup>6</sup> 6d<sup>2</sup> 7s<sup>2</sup>"}}};

    static constexpr std::array<Potential, 127> GW_LDA{
        {{"H_GW", 1, "1s<sup>1</sup>"},
         {"H_h_GW", 1, "1s<sup>1</sup>"},
         {"He_GW", 2, "1s<sup>2</sup>"},
         {"Li_AE_GW", 3, "1s<sup>2</sup> 2p<sup>1</sup>"},
         {"Li_GW", 1, "2s<sup>1</sup>"},
         {"Li_sv_GW", 3, "1s<sup>2</sup> 2p<sup>1</sup>"},
         {"Be_GW", 2, "2s<sup>1.9999</sup> 2p<sup>0.001</sup>"},
         {"Be_sv_GW", 4, "1s<sup>2</sup> 2p<sup>2</sup>"},
         {"B_GW", 3, "2s<sup>2</sup> 2p<sup>1</sup>"},
         {"B_GW_new", 3, "2s<sup>2</sup> 2p<sup>1</sup>"},
         {"C_GW", 4, "2s<sup>2</sup> 2p<sup>2</sup>"},
         {"C_GW_new", 4, "2s<sup>2</sup> 2p<sup>2</sup>"},
         {"C_h_GW", 4, "2s<sup>2</sup> 2p<sup>2</sup>"},
         {"C_s_GW", 4, "2s<sup>2</sup> 2p<sup>2</sup>"},
         {"N_GW", 5, "2s<sup>2</sup> 2p<sup>3</sup>"},
         {"N_GW_new", 5, "2s<sup>2</sup> 2p<sup>3</sup>"},
         {"N_h_GW", 5, "2s<sup>2</sup> 2p<sup>3</sup>"},
         {"N_s_GW", 5, "2s<sup>2</sup> 2p<sup>3</sup>"},
         {"O_GW", 6, "2s<sup>2</sup> 2p<sup>4</sup>"},
         {"O_GW_new", 6, "2s<sup>2</sup> 2p<sup>4</sup>"},
         {"O_h_GW", 6, "2s<sup>2</sup> 2p<sup>4</sup>"},
         {"O_s_GW", 6, "2s<sup>2</sup> 2p<sup>4</sup>"},
         {"F_GW", 7, "2s<sup>2</sup> 2p<sup>5</sup>"},
         {"F_GW_new", 7, "2s<sup>2</sup> 2p<sup>5</sup>"},
         {"F_h_GW", 7, "2s<sup>2</sup> 2p<sup>5</sup>"},
         {"Ne_GW", 8, "2s<sup>2</sup> 2p<sup>6</sup>"},
         {"Ne_s_GW", 8, "2s<sup>2</sup> 2p<sup>6</sup>"},
         {"Na_sv_GW", 9, "2s<sup>2</sup> 2p<sup>6</sup> 3p<sup>1</sup>"},
         {"Mg_GW", 2, "3s<sup>2</sup>"},
         {"Mg_pv_GW", 8, "2p<sup>6</sup> 3s<sup>2</sup>"},
         {"Mg_sv_GW", 10, "2s<sup>2</sup> 2p<sup>6</sup> 3d<sup>2</sup>"},
         {"Al_GW", 3, "3s<sup>2</sup> 3p<sup>1</sup>"},
         {"Al_sv_GW", 11, "2s<sup>2</sup> 2p<sup>6</sup> 3s<sup>2</sup> 3p<sup>1</sup>"},
         {"Si_GW", 4, "3s<sup>2</sup> 3p<sup>2</sup>"},
         {"Si_sv_GW", 12, "2s<sup>2</sup> 2p<sup>6</sup> 3s<sup>2</sup> 3p<sup>2</sup>"},
         {"P_GW", 5, "3s<sup>2</sup> 3p<sup>3</sup>"},
         {"S_GW", 6, "3s<sup>2</sup> 3p<sup>4</sup>"},
         {"Cl_GW", 7, "3s<sup>2</sup> 3p<sup>5</sup>"},
         {"Ar_GW", 8, "3s<sup>2</sup> 3p<sup>6</sup>"},
         {"K_sv_GW", 9, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>1</sup>"},
         {"Ca_sv_GW", 10, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>2</sup>"},
         {"Sc_sv_GW", 11, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>3</sup>"},
         {"Ti_sv_GW", 12, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>4</sup>"},
         {"V_sv_GW", 13, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>5</sup>"},
         {"Cr_sv_GW", 14, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>6</sup>"},
         {"Mn_GW", 7, "3d<sup>6</sup> 4s<sup>1</sup>"},
         {"Mn_sv_GW", 15, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>7</sup>"},
         {"Fe_GW", 8, "3d<sup>7</sup> 4s<sup>1</sup>"},
         {"Fe_sv_GW", 16, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>8</sup>"},
         {"Co_GW", 9, "3d<sup>8</sup> 4s<sup>1</sup>"},
         {"Co_sv_GW", 17, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>9</sup>"},
         {"Ni_GW", 10, "3d<sup>9</sup> 4s<sup>1</sup>"},
         {"Ni_sv_GW", 18, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>10</sup>"},
         {"Cu_GW", 11, "3d<sup>10</sup> 4s<sup>1</sup>"},
         {"Cu_sv_GW", 19, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>10</sup> 4s<sup>1</sup>"},
         {"Zn_GW", 12, "3d<sup>10</sup> 4s<sup>2</sup>"},
         {"Zn_sv_GW", 20, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>10</sup> 4s<sup>2</sup>"},
         {"Ga_GW", 3, "4s<sup>2</sup> 4p<sup>1</sup>"},
         {"Ga_d_GW", 13, "3d<sup>10</sup> 4s<sup>2</sup> 4p<sup>1</sup>"},
         {"Ga_sv_GW", 21,
          "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>10</sup> 4s<sup>2</sup> 4p<sup>1</sup>"},
         {"Ge_GW", 4, "4s<sup>2</sup> 4p<sup>2</sup>"},
         {"Ge_d_GW", 14, "3d<sup>10</sup> 4s<sup>2</sup> 4p<sup>2</sup>"},
         {"Ge_sv_GW", 22,
          "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>10</sup> 4s<sup>2</sup> 4p<sup>2</sup>"},
         {"As_GW", 5, "4s<sup>2</sup> 4p<sup>3</sup>"},
         {"As_sv_GW", 23,
          "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>10</sup> 4s<sup>2</sup> 4p<sup>3</sup>"},
         {"Se_GW", 6, "4s<sup>2</sup> 4p<sup>4</sup>"},
         {"Se_sv_GW", 24,
          "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>10</sup> 4s<sup>2</sup> 4p<sup>4</sup>"},
         {"Br_GW", 7, "4s<sup>2</sup> 4p<sup>5</sup>"},
         {"Br_sv_GW", 25,
          "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>10</sup> 4s<sup>2</sup> 4p<sup>5</sup>"},
         {"Kr_GW", 8, "4s<sup>2</sup> 4p<sup>6</sup>"},
         {"Rb_sv_GW", 9, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>1</sup>"},
         {"Sr_sv_GW", 10, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>2</sup>"},
         {"Y_sv_GW", 11, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>3</sup>"},
         {"Zr_sv_GW", 12, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>4</sup>"},
         {"Nb_sv_GW", 13, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>5</sup>"},
         {"Mo_sv_GW", 14, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>6</sup>"},
         {"Tc_sv_GW", 15, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>7</sup>"},
         {"Ru_sv_GW", 16, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>8</sup>"},
         {"Rh_GW", 9, "4d<sup>8</sup> 5s<sup>1</sup>"},
         {"Rh_sv_GW", 17, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>9</sup>"},
         {"Pd_GW", 10, "4d<sup>9</sup> 5s<sup>1</sup>"},
         {"Pd_sv_GW", 18, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>10</sup>"},
         {"Ag_GW", 11, "4d<sup>10</sup> 5s<sup>1</sup>"},
         {"Ag_sv_GW", 19, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>11</sup>"},
         {"Cd_GW", 12, "4d<sup>10</sup> 5s<sup>2</sup>"},
         {"Cd_sv_GW", 20, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>10</sup> 5s<sup>2</sup>"},
         {"In_d_GW", 13, "4d<sup>10</sup> 5s<sup>2</sup> 5p<sup>1</sup>"},
         {"In_sv_GW", 21,
          "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>10</sup> 5s<sup>2</sup> 5p<sup>1</sup>"},
         {"Sn_d_GW", 14, "4d<sup>10</sup> 5s<sup>2</sup> 5p<sup>2</sup>"},
         {"Sn_sv_GW", 22,
          "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>10</sup> 5s<sup>2</sup> 5p<sup>2</sup>"},
         {"Sb_GW", 5, "5s<sup>2</sup> 5p<sup>3</sup>"},
         {"Sb_d_GW", 15, "4d<sup>10</sup> 5s<sup>2</sup> 5p<sup>3</sup>"},
         {"Sb_sv_GW", 23,
          "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>10</sup> 5s<sup>2</sup> 5p<sup>3</sup>"},
         {"Te_GW", 6, "5s<sup>2</sup> 5p<sup>4</sup>"},
         {"Te_sv_GW", 24,
          "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>10</sup> 5s<sup>2</sup> 5p<sup>4</sup>"},
         {"I_GW", 7, "5s<sup>2</sup> 5p<sup>5</sup>"},
         {"I_sv_GW", 25,
          "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>10</sup> 5s<sup>2</sup> 5p<sup>5</sup>"},
         {"Xe_GW", 8, "5s<sup>2</sup> 5p<sup>6</sup>"},
         {"Xe_sv_GW", 26,
          "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>10</sup> 5s<sup>2</sup> 5p<sup>6</sup>"},
         {"Cs_sv_GW", 9, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>1</sup>"},
         {"Ba_sv_GW", 10, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>2</sup>"},
         {"La_GW", 11,
          "4f<sup>0.2</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.8</sup> 6s<sup>2</sup>"},
         {"Ce_GW", 12,
          "4f<sup>1</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>1</sup> 6s<sup>2</sup>"},
         {"Hf_sv_GW", 12, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>4</sup>"},
         {"Ta_sv_GW", 13, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>5</sup>"},
         {"W_sv_GW", 14, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>6</sup>"},
         {"Re_sv_GW", 15, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>7</sup>"},
         {"Os_sv_GW", 16, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>8</sup>"},
         {"Ir_sv_GW", 17, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>9</sup>"},
         {"Pt_GW", 10, "5d<sup>9</sup> 6s<sup>1</sup>"},
         {"Pt_sv_GW", 18, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>10</sup>"},
         {"Au_GW", 11, "5d<sup>10</sup> 6s<sup>1</sup>"},
         {"Au_sv_GW", 19, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>11</sup>"},
         {"Hg_sv_GW", 20, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>10</sup> 6s<sup>2</sup>"},
         {"Tl_d_GW", 15, "5s<sup>2</sup> 5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>1</sup>"},
         {"Tl_sv_GW", 21,
          "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>1</sup>"},
         {"Pb_d_GW", 16, "5s<sup>2</sup> 5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>2</sup>"},
         {"Pb_sv_GW", 22,
          "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>2</sup>"},
         {"Bi_GW", 5, "6s<sup>2</sup> 6p<sup>3</sup>"},
         {"Bi_d_GW", 17, "5s<sup>2</sup> 5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>3</sup>"},
         {"Bi_sv_GW", 23,
          "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>3</sup>"},
         {"Po_d_GW", 18, "5s<sup>2</sup> 5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>4</sup>"},
         {"Po_sv_GW", 24,
          "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>4</sup>"},
         {"At_d_GW", 17, "5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>5</sup>"},
         {"At_sv_GW", 25,
          "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>5</sup>"},
         {"Rn_d_GW", 18, "5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>6</sup>"},
         {"Rn_sv_GW", 26,
          "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>6</sup>"}}};

    static constexpr std::array<Potential, 129> GW_PBE{
        {{"H_GW", 1, "1s<sup>1</sup>"},
         {"H_GW_new", 1, "1s<sup>1</sup>"},
         {"H_h_GW", 1, "1s<sup>1</sup>"},
         {"He_GW", 2, "1s<sup>2</sup>"},
         {"Li_AE_GW", 3, "1s<sup>2</sup> 2p<sup>1</sup>"},
         {"Li_GW", 1, "2s<sup>1</sup>"},
         {"Li_sv_GW", 3, "1s<sup>2</sup> 2p<sup>1</sup>"},
         {"Be_GW", 2, "2s<sup>1.9999</sup> 2p<sup>0.001</sup>"},
         {"Be_sv_GW", 4, "1s<sup>2</sup> 2p<sup>2</sup>"},
         {"B_GW", 3, "2s<sup>2</sup> 2p<sup>1</sup>"},
         {"B_GW_new", 3, "2s<sup>2</sup> 2p<sup>1</sup>"},
         {"B_h_GW", 3, "2s<sup>2</sup> 2p<sup>1</sup>"},
         {"C_GW", 4, "2s<sup>2</sup> 2p<sup>2</sup>"},
         {"C_GW_new", 4, "2s<sup>2</sup> 2p<sup>2</sup>"},
         {"C_h_GW", 4, "2s<sup>2</sup> 2p<sup>2</sup>"},
         {"C_s_GW", 4, "2s<sup>2</sup> 2p<sup>2</sup>"},
         {"N_GW", 5, "2s<sup>2</sup> 2p<sup>3</sup>"},
         {"N_GW_new", 5, "2s<sup>2</sup> 2p<sup>3</sup>"},
         {"N_h_GW", 5, "2s<sup>2</sup> 2p<sup>3</sup>"},
         {"N_s_GW", 5, "2s<sup>2</sup> 2p<sup>3</sup>"},
         {"O_GW", 6, "2s<sup>2</sup> 2p<sup>4</sup>"},
         {"O_GW_new", 6, "2s<sup>2</sup> 2p<sup>4</sup>"},
         {"O_h_GW", 6, "2s<sup>2</sup> 2p<sup>4</sup>"},
         {"O_s_GW", 6, "2s<sup>2</sup> 2p<sup>4</sup>"},
         {"F_GW", 7, "2s<sup>2</sup> 2p<sup>5</sup>"},
         {"F_GW_new", 7, "2s<sup>2</sup> 2p<sup>5</sup>"},
         {"F_h_GW", 7, "2s<sup>2</sup> 2p<sup>5</sup>"},
         {"Ne_GW", 8, "2s<sup>2</sup> 2p<sup>6</sup>"},
         {"Ne_s_GW", 8, "2s<sup>2</sup> 2p<sup>6</sup>"},
         {"Na_sv_GW", 9, "2s<sup>2</sup> 2p<sup>6</sup> 3p<sup>1</sup>"},
         {"Mg_GW", 2, "3s<sup>2</sup>"},
         {"Mg_pv_GW", 8, "2p<sup>6</sup> 3s<sup>2</sup>"},
         {"Mg_sv_GW", 10, "2s<sup>2</sup> 2p<sup>6</sup> 3d<sup>2</sup>"},
         {"Al_GW", 3, "3s<sup>2</sup> 3p<sup>1</sup>"},
         {"Al_sv_GW", 11, "2s<sup>2</sup> 2p<sup>6</sup> 3s<sup>2</sup> 3p<sup>1</sup>"},
         {"Si_GW", 4, "3s<sup>2</sup> 3p<sup>2</sup>"},
         {"Si_sv_GW", 12, "2s<sup>2</sup> 2p<sup>6</sup> 3s<sup>2</sup> 3p<sup>2</sup>"},
         {"P_GW", 5, "3s<sup>2</sup> 3p<sup>3</sup>"},
         {"S_GW", 6, "3s<sup>2</sup> 3p<sup>4</sup>"},
         {"Cl_GW", 7, "3s<sup>2</sup> 3p<sup>5</sup>"},
         {"Ar_GW", 8, "3s<sup>2</sup> 3p<sup>6</sup>"},
         {"K_sv_GW", 9, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>1</sup>"},
         {"Ca_sv_GW", 10, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>2</sup>"},
         {"Sc_sv_GW", 11, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>3</sup>"},
         {"Ti_sv_GW", 12, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>4</sup>"},
         {"V_sv_GW", 13, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>5</sup>"},
         {"Cr_sv_GW", 14, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>6</sup>"},
         {"Mn_GW", 7, "3d<sup>6</sup> 4s<sup>1</sup>"},
         {"Mn_sv_GW", 15, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>7</sup>"},
         {"Fe_GW", 8, "3d<sup>7</sup> 4s<sup>1</sup>"},
         {"Fe_sv_GW", 16, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>8</sup>"},
         {"Co_GW", 9, "3d<sup>8</sup> 4s<sup>1</sup>"},
         {"Co_sv_GW", 17, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>9</sup>"},
         {"Ni_GW", 10, "3d<sup>9</sup> 4s<sup>1</sup>"},
         {"Ni_sv_GW", 18, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>10</sup>"},
         {"Cu_GW", 11, "3d<sup>10</sup> 4s<sup>1</sup>"},
         {"Cu_sv_GW", 19, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>10</sup> 4s<sup>1</sup>"},
         {"Zn_GW", 12, "3d<sup>10</sup> 4s<sup>2</sup>"},
         {"Zn_sv_GW", 20, "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>10</sup> 4s<sup>2</sup>"},
         {"Ga_GW", 3, "4s<sup>2</sup> 4p<sup>1</sup>"},
         {"Ga_d_GW", 13, "3d<sup>10</sup> 4s<sup>2</sup> 4p<sup>1</sup>"},
         {"Ga_sv_GW", 21,
          "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>10</sup> 4s<sup>2</sup> 4p<sup>1</sup>"},
         {"Ge_GW", 4, "4s<sup>2</sup> 4p<sup>2</sup>"},
         {"Ge_d_GW", 14, "3d<sup>10</sup> 4s<sup>2</sup> 4p<sup>2</sup>"},
         {"Ge_sv_GW", 22,
          "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>10</sup> 4s<sup>2</sup> 4p<sup>2</sup>"},
         {"As_GW", 5, "4s<sup>2</sup> 4p<sup>3</sup>"},
         {"As_sv_GW", 23,
          "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>10</sup> 4s<sup>2</sup> 4p<sup>3</sup>"},
         {"Se_GW", 6, "4s<sup>2</sup> 4p<sup>4</sup>"},
         {"Se_sv_GW", 24,
          "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>10</sup> 4s<sup>2</sup> 4p<sup>4</sup>"},
         {"Br_GW", 7, "4s<sup>2</sup> 4p<sup>5</sup>"},
         {"Br_sv_GW", 25,
          "3s<sup>2</sup> 3p<sup>6</sup> 3d<sup>10</sup> 4s<sup>2</sup> 4p<sup>5</sup>"},
         {"Kr_GW", 8, "4s<sup>2</sup> 4p<sup>6</sup>"},
         {"Rb_sv_GW", 9, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>1</sup>"},
         {"Sr_sv_GW", 10, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>2</sup>"},
         {"Y_sv_GW", 11, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>3</sup>"},
         {"Zr_sv_GW", 12, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>4</sup>"},
         {"Nb_sv_GW", 13, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>5</sup>"},
         {"Mo_sv_GW", 14, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>6</sup>"},
         {"Tc_sv_GW", 15, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>7</sup>"},
         {"Ru_sv_GW", 16, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>8</sup>"},
         {"Rh_GW", 9, "4d<sup>8</sup> 5s<sup>1</sup>"},
         {"Rh_sv_GW", 17, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>9</sup>"},
         {"Pd_GW", 10, "4d<sup>9</sup> 5s<sup>1</sup>"},
         {"Pd_sv_GW", 18, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>10</sup>"},
         {"Ag_GW", 11, "4d<sup>10</sup> 5s<sup>1</sup>"},
         {"Ag_sv_GW", 19, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>11</sup>"},
         {"Cd_GW", 12, "4d<sup>10</sup> 5s<sup>2</sup>"},
         {"Cd_sv_GW", 20, "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>10</sup> 5s<sup>2</sup>"},
         {"In_d_GW", 13, "4d<sup>10</sup> 5s<sup>2</sup> 5p<sup>1</sup>"},
         {"In_sv_GW", 21,
          "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>10</sup> 5s<sup>2</sup> 5p<sup>1</sup>"},
         {"Sn_d_GW", 14, "4d<sup>10</sup> 5s<sup>2</sup> 5p<sup>2</sup>"},
         {"Sn_sv_GW", 22,
          "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>10</sup> 5s<sup>2</sup> 5p<sup>2</sup>"},
         {"Sb_GW", 5, "5s<sup>2</sup> 5p<sup>3</sup>"},
         {"Sb_d_GW", 15, "4d<sup>10</sup> 5s<sup>2</sup> 5p<sup>3</sup>"},
         {"Sb_sv_GW", 23,
          "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>10</sup> 5s<sup>2</sup> 5p<sup>3</sup>"},
         {"Te_GW", 6, "5s<sup>2</sup> 5p<sup>4</sup>"},
         {"Te_sv_GW", 24,
          "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>10</sup> 5s<sup>2</sup> 5p<sup>4</sup>"},
         {"I_GW", 7, "5s<sup>2</sup> 5p<sup>5</sup>"},
         {"I_sv_GW", 25,
          "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>10</sup> 5s<sup>2</sup> 5p<sup>5</sup>"},
         {"Xe_GW", 8, "5s<sup>2</sup> 5p<sup>6</sup>"},
         {"Xe_sv_GW", 26,
          "4s<sup>2</sup> 4p<sup>6</sup> 4d<sup>10</sup> 5s<sup>2</sup> 5p<sup>6</sup>"},
         {"Cs_sv_GW", 9, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>1</sup>"},
         {"Ba_sv_GW", 10, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>1</sup> 6s<sup>1</sup>"},
         {"La_GW", 11,
          "4f<sup>0.2</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>0.8</sup> 6s<sup>2</sup>"},
         {"Ce_GW", 12,
          "4f<sup>1</sup> 5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>1</sup> 6s<sup>2</sup>"},
         {"Hf_sv_GW", 12, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>4</sup>"},
         {"Ta_sv_GW", 13, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>5</sup>"},
         {"W_sv_GW", 14, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>6</sup>"},
         {"Re_sv_GW", 15, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>7</sup>"},
         {"Os_sv_GW", 16, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>8</sup>"},
         {"Ir_sv_GW", 17, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>9</sup>"},
         {"Pt_GW", 10, "5d<sup>9</sup> 6s<sup>1</sup>"},
         {"Pt_sv_GW", 18, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>10</sup>"},
         {"Au_GW", 11, "5d<sup>10</sup> 6s<sup>1</sup>"},
         {"Au_sv_GW", 19, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>11</sup>"},
         {"Hg_sv_GW", 20, "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>10</sup> 6s<sup>2</sup>"},
         {"Tl_d_GW", 15, "5s<sup>2</sup> 5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>1</sup>"},
         {"Tl_sv_GW", 21,
          "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>1</sup>"},
         {"Pb_d_GW", 16, "5s<sup>2</sup> 5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>2</sup>"},
         {"Pb_sv_GW", 22,
          "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>2</sup>"},
         {"Bi_GW", 5, "6s<sup>2</sup> 6p<sup>3</sup>"},
         {"Bi_d_GW", 17, "5s<sup>2</sup> 5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>3</sup>"},
         {"Bi_sv_GW", 23,
          "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>3</sup>"},
         {"Po_d_GW", 18, "5s<sup>2</sup> 5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>4</sup>"},
         {"Po_sv_GW", 24,
          "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>4</sup>"},
         {"At_d_GW", 17, "5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>5</sup>"},
         {"At_sv_GW", 25,
          "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>5</sup>"},
         {"Rn_d_GW", 18, "5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>6</sup>"},
         {"Rn_sv_GW", 26,
          "5s<sup>2</sup> 5p<sup>6</sup> 5d<sup>10</sup> 6s<sup>2</sup> 6p<sup>6</sup>"}}};

    const auto pots = [&]() -> std::span<const Potential> {
        switch (type) {
            case PotentialType::Standard_LDA:
                return standard_LDA;
            case PotentialType::Standard_PBE:
                return standard_PBE;
            case PotentialType::GW_LDA:
                return GW_LDA;
            case PotentialType::GW_PBE:
                return GW_PBE;
        }
        return standard_LDA;
    }();

    const auto itFullName = std::ranges::find(pots, name, &Potential::name);
    if (itFullName != pots.end()) {
        return *itFullName;
    }

    const auto itStartsWith = std::ranges::find_if(
        pots, [&](const Potential& pot) { return pot.name.starts_with(name); });

    if (itStartsWith != pots.end()) {
        return *itStartsWith;
    }

    return std::nullopt;
}
}  // namespace vasp
}  // namespace inviwo
