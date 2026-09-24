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

#include <inviwo/oceanflow/processors/oceanraycaster.h>

#include <inviwo/core/algorithm/boundingbox.h>
#include <inviwo/core/util/stringconversion.h>

#include <modules/opengl/shader/shader.h>
#include <modules/opengl/shader/shaderutils.h>
#include <modules/basegl/shadercomponents/shadercomponentutil.h>
#include <modules/opengl/texture/textureutils.h>
#include <modules/opengl/volume/volumeutils.h>

#include <fmt/base.h>
#include <fmt/format.h>

#include <numbers>

namespace inviwo {

NemoVolumeComponent::NemoVolumeComponent(std::string_view name, Gradients gradients, Document help)
    : TemporalVolumeComponent(name, gradients, std::move(help))
    , zZoom{"zZoom", "Z Zoom factor", util::ordinalScale(1.0f)} {}

NemoVolumeComponent::~NemoVolumeComponent() = default;

void NemoVolumeComponent::process(Shader& shader, TextureUnitContainer& cont) {
    TemporalVolumeComponent::process(shader, cont);
    utilgl::setUniforms(shader, zZoom);
}

std::vector<Property*> NemoVolumeComponent::getProperties() {
    auto props = TemporalVolumeComponent::getProperties();
    props.emplace_back(&zZoom);
    return props;
}

namespace nemo {
namespace {

constexpr std::string_view uniformsZoom = util::trim(R"(
uniform float zZoom;
)");

constexpr std::string_view depth = util::trim(R"(
int coeffIndex(float d) {{
    return int(floor((200*log((6 + d)/6.))/173.));
}}

vec4 nemoCoeffs[8] = vec4[](
 vec4(-0.5020485620454235, 1.0064965390207923, -0.026022656770173513, 0.00011339152867423767),
 vec4(-0.040511845501916374, 0.9188060606908754, -0.02326253117084194, 0.0002693122897196264),
 vec4(4.947890418847749, 0.38650206094726314, -0.0035014310967430335, 0.000015357339276672554),
 vec4(9.435867702868881, 0.19363090075034473, -0.0006519616483171304, 9.953310338937843e-7),
 vec4(15.290389876996707, 0.10380091532715348, -0.000176565679655483, 1.2741608192830535e-7),
 vec4(24.71897593205658, 0.03985744167034901, -0.000027506296541509556, 8.466529994835795e-9),
 vec4(33.024019406608886, 0.015991187545708305, -3.9489148091425246e-6, 5.183556638826113e-10),
 vec4(40.021437807532486, 0.007584806553897775, -4.797526641657579e-7, 2.882092803741341e-11));

float mapDepthToTexture(float depth) {{
    int index = clamp(coeffIndex(depth), 0, 7);
    vec4 c = nemoCoeffs[index];
    return c.x + depth * c.y + depth*depth*c.z + depth*depth*depth*c.w;
}}

vec3 mapPositionToTexture(vec3 position) {{
    float z = mapDepthToTexture(position.z * 6000.0 / zZoom) / {0}Parameters.dimensions.z;
    return vec3(position.x, position.y, z);
}}

)");

constexpr std::string_view sample = util::trim(R"(
{0}SamplePosition = mapPositionToTexture({0}SamplePosition);
)");
}  // namespace
}  // namespace nemo

auto NemoVolumeComponent::getSegments() -> std::vector<Segment> {
    auto segments = TemporalVolumeComponent::getSegments();

    segments.emplace_back(Segment{.snippet = fmt::format(nemo::uniformsZoom, getName()),
                                  .placeholder = placeholder::uniform,
                                  .priority = 400});
    segments.emplace_back(Segment{.snippet = fmt::format(nemo::depth, getName()),
                                  .placeholder = placeholder::uniform,
                                  .priority = 410});

    segments.emplace_back(Segment{.snippet = fmt::format(nemo::sample, getName()),
                                  .placeholder = placeholder::first,
                                  .priority = 401});
    segments.emplace_back(Segment{.snippet = fmt::format(nemo::sample, getName()),
                                  .placeholder = placeholder::loop,
                                  .priority = 401});
    return segments;
}

VolumeMaskComponent::VolumeMaskComponent(std::string_view maskName, std::string_view volumeName)
    : mask_{maskName}, volumeName_{volumeName} {}

std::string_view VolumeMaskComponent::getName() const { return mask_.getIdentifier(); }

void VolumeMaskComponent::process(Shader& shader, TextureUnitContainer& cont) {
    utilgl::bindAndSetUniforms(shader, cont, *mask_.getData(), getName());
}

namespace mask {
namespace {

constexpr std::string_view uniforms = util::trim(R"(
uniform VolumeParameters {0}Parameters;
uniform sampler3D {0};
)");

constexpr std::string_view first = util::trim(R"(
float {0}Value = getNormalizedVoxel({0}, {0}Parameters, {1}SamplePosition).x;
float {0}ValuePrev = {0}Value;
if ({0}Value > 0.0) {{
   color = vec4(0);
}}
)");

constexpr std::string_view loop = util::trim(R"(
{0}Value = getNormalizedVoxel({0}, {0}Parameters, {1}SamplePosition).x;
if ({0}Value > 0.0) {{
   {0}ValuePrev = {0}Value;
   continue;
}}
)");

constexpr std::string_view loop2 = util::trim(R"(
if ({0}ValuePrev > 0.0) {{
    // We just left a masked region.
    // Reset the prev values to the current value.
    {1}VoxelPrev = {1}Voxel;
    {1}GradientPrev = {1}Gradient;
}}
{0}ValuePrev = {0}Value;
)");
}  // namespace
}  // namespace mask

std::vector<ShaderComponent::Segment> VolumeMaskComponent::getSegments() {
    return {{.snippet = fmt::format(mask::uniforms, getName()),
             .placeholder = placeholder::uniform,
             .priority = 410},
            {.snippet = fmt::format(mask::first, getName(), volumeName_),
             .placeholder = placeholder::first,
             .priority = 950},
            {.snippet = fmt::format(mask::loop, getName(), volumeName_),
             .placeholder = placeholder::loop,
             .priority = 410},
            {.snippet = fmt::format(mask::loop2, getName(), volumeName_),
             .placeholder = placeholder::loop,
             .priority = 550}};
}

namespace eep {
namespace {
Mesh sphere(double outerRadius, double innerRadius, unsigned int numLoops,
            unsigned int segmentsPerLoop) {

    numLoops = std::max(4u, numLoops);
    segmentsPerLoop = std::max(8u, segmentsPerLoop);

    // Create Vertices
    const auto shellSize = (numLoops + 1) * (segmentsPerLoop + 1);
    const auto points = (innerRadius == 0.0 ? 1 : 2) * shellSize;

    std::vector<vec3> vertices;
    vertices.reserve(points);
    std::vector<vec3> normals;
    normals.reserve(points);
    std::vector<vec4> colors;
    colors.reserve(points);

    const unsigned int pointsPerLine = segmentsPerLoop + 1;

    std::array<double, 2> radii{outerRadius, innerRadius};

    for (auto r : radii | std::views::take(innerRadius == 0 ? 1 : 2)) {
        for (unsigned int i : std::views::iota(0u, numLoops + 1u)) {
            for (unsigned int j : std::views::iota(0u, segmentsPerLoop + 1u)) {
                const auto theta = i == numLoops ? std::numbers::pi
                                                 : std::numbers::pi * static_cast<double>(i) /
                                                       static_cast<double>(numLoops);

                const double phi = std::numbers::pi * static_cast<double>(j * 2) /
                                   static_cast<double>(segmentsPerLoop);

                const double sinTheta = std::sin(theta);
                const double sinPhi = std::sin(phi);
                const double cosTheta = std::cos(theta);
                const double cosPhi = std::cos(phi);

                const dvec3 normal{cosPhi * sinTheta, sinPhi * sinTheta, cosTheta};
                const dvec3 vertex{normal * r};
                const dvec3 color{dvec3{1.0 / (2.0 * outerRadius)} * (vertex + dvec3{outerRadius})};

                vertices.emplace_back(vec3{vertex});
                normals.emplace_back(vec3{normal});
                colors.emplace_back(vec4{vec3{color}, 1.0f});
            }
        }
    }

    // compute indices
    std::vector<std::uint32_t> indices;
    indices.reserve(points * 6);

    const auto shellOffsets = std::array<unsigned int, 2>{0, shellSize};

    const auto trig1 = std::array{0u, pointsPerLine, 1u, 1u, pointsPerLine, pointsPerLine + 1u};
    const auto trig2 = std::array{0u, 1u, pointsPerLine, 1u, pointsPerLine + 1u, pointsPerLine};

    for (auto&& [trig, o] : std::views::zip(std::array{trig1, trig2}, shellOffsets) |
                                std::views::take(innerRadius == 0 ? 1 : 2)) {
        for (unsigned int y = 0; y < numLoops; ++y) {
            for (unsigned int x = 0; x < pointsPerLine; ++x) {
                for (auto i : trig) {
                    indices.emplace_back(x + i + y * pointsPerLine + o);
                }
            }
        }
    }

    const auto pos = std::make_shared<Buffer<vec3>>(
        std::make_shared<BufferRAMPrecision<vec3>>(std::move(vertices)));
    const auto norm = std::make_shared<Buffer<vec3>>(
        std::make_shared<BufferRAMPrecision<vec3>>(std::move(normals)));
    const auto col = std::make_shared<Buffer<vec4>>(
        std::make_shared<BufferRAMPrecision<vec4>>(std::move(colors)));

    const auto inds =
        std::make_shared<IndexBuffer>(std::make_shared<IndexBufferRAM>(std::move(indices)));

    return {Mesh::BufferVector{{BufferType::PositionAttrib, pos},
                               {BufferType::NormalAttrib, norm},
                               {BufferType::ColorAttrib, col}},
            Mesh::IndexVector{{Mesh::MeshInfo{DrawType::Triangles, ConnectivityType::None}, inds}}};
}

constexpr std::string_view uniforms = util::trim(R"(
uniform ImageParameters {0}Parameters;
uniform sampler2D {0}Color;
uniform sampler2D {0}Depth;
)");

constexpr std::string_view surfaceNormalUniforms = util::trim(R"(
uniform sampler2D surfaceNormal;
uniform bool useSurfaceNormals;
)");

constexpr std::string_view setup = util::trim(R"(
vec3 entryPoint = texture(entryColor, texCoords).rgb;
vec3 exitPoint = texture(exitColor, texCoords).rgb;
float entryPointDepth = texture(entryDepth, texCoords).x;
float exitPointDepth = texture(exitDepth, texCoords).x;

// The length of the ray in texture space
float rayLength = length(exitPoint - entryPoint);

// The normalized direction of the ray
vec3 rayDirection = normalize(exitPoint - entryPoint);
)");

}  // namespace
}  // namespace eep
SphericalEntryExitPoints::SphericalEntryExitPoints(CameraProperty* camera)
    : ShaderComponent{}
    , basis{"basis", "Basis", util::ordinalMatrix(dmat4{1.0})}
    , outerRadius{"outerRadius", "outerRadius", util::ordinalScale(1.0, 5.0).setInc(0.01)}
    , innerRadius{"innerRadius", "innerRadius", util::ordinalScale(0.9, 5.0).setInc(0.01)}
    , sphereMesh{eep::sphere(outerRadius.get(), innerRadius.get(), 16, 32)}
    , entryPoints{LayerConfig::defaultDimensions, DataVec4UInt16::get()}
    , exitPoints{LayerConfig::defaultDimensions, DataVec4UInt16::get()}
    , eepHelper{}
    , trackball{camera} {}

std::string_view SphericalEntryExitPoints::getName() const { return "Spherical EEP"; }
void SphericalEntryExitPoints::initializeResources(Shader&) {}
void SphericalEntryExitPoints::process(Shader& shader, TextureUnitContainer& cont) {

    utilgl::bindAndSetUniforms(shader, cont, entryPoints, "entry", ImageType::ColorDepth);
    utilgl::bindAndSetUniforms(shader, cont, exitPoints, "exit", ImageType::ColorDepth);
    shader.setUniform("useSurfaceNormals", true);
    shader.setUniform("sphereBasis", basis.get());
    shader.setUniform("outerRadius", outerRadius.get());
    shader.setUniform("innerRadius", innerRadius.get());
}

auto SphericalEntryExitPoints::getSegments() -> std::vector<Segment> {
    using namespace fmt::literals;
    return {{fmt::format(eep::uniforms, "entry"), placeholder::uniform, 100},
            {fmt::format(eep::uniforms, "exit"), placeholder::uniform, 101},
            {"uniform mat4 sphereBasis;", placeholder::uniform, 102},
            {"uniform float outerRadius;", placeholder::uniform, 103},
            {"uniform float innerRadius;", placeholder::uniform, 104},
            {std::string{eep::surfaceNormalUniforms}, placeholder::uniform, 102},
            {std::string{eep::setup}, placeholder::setup, 100}};
}

std::vector<Property*> SphericalEntryExitPoints::getProperties() {
    return {&basis, &outerRadius, &innerRadius, &trackball};
}

void SphericalEntryExitPoints::preprocess(Camera& camera, size2_t dim) {
    entryPoints.setDimensions(dim);
    exitPoints.setDimensions(dim);

    sphereMesh.setModelMatrix(basis.get());

    eepHelper(entryPoints, exitPoints, camera, sphereMesh, algorithm::CapNearClip::Yes,
              algorithm::IncludeNormals::Yes);
}

SurfaceComponent::SurfaceComponent()
    : ShaderComponent(), surface{"surface", "Surface / depth texture"_help} {}

std::string_view SurfaceComponent::getName() const { return surface.getIdentifier(); }

void SurfaceComponent::process(Shader& shader, TextureUnitContainer& cont) {
    utilgl::bindAndSetUniforms(shader, cont, surface);
}

std::vector<std::tuple<Inport*, std::string>> SurfaceComponent::getInports() {
    return {{&surface, std::string{"surface"}}};
}

namespace surface {
namespace {

constexpr std::string_view uniforms = util::trim(R"(
uniform ImageParameters {name}Parameters;
uniform sampler2D {name};

const float PI = 3.14159265358979323846;
vec3 cartesianToSpherical(vec3 p) {{
    float r = length(p);
    float theta = atan(p.y, p.x);           // azimuth, [-π, π]
    float phi = atan(length(p.xy), p.z);    // polar angle, [0, π]

    return vec3(r, theta, phi);
}}

vec3 cartesianToLatLongNormalized(vec3 p) {{
    vec3 rThetaPhi = cartesianToSpherical(p);
    return vec3(rThetaPhi.x, 1.0 - (rThetaPhi.y / PI / 2.0 + 0.5), rThetaPhi.z / PI);
}}

// Take a texture pos [0,1] return a pos [-1,1]
vec3 textureSamplePosToSpherePos(vec3 p) {{
    return 2.0 * p - vec3(1.0);
}}

vec3 radiiAtTexturePos(vec3 p) {{
    vec3 rLatLong = cartesianToLatLongNormalized(textureSamplePosToSpherePos(p));
    float depth = texture({name}, rLatLong.yz).x;
    const float maxDepth = 10000.0;
    const float depthFrac = (outerRadius - innerRadius) / outerRadius;
    float normDepth = depth / maxDepth;
    float surfaceNormRadii = 1.0 + depthFrac * normDepth;
    return vec3(rLatLong.x, surfaceNormRadii, 1.0 + normDepth);
}}

)");

constexpr std::string_view setup = util::trim(R"()");

constexpr std::string_view loop = util::trim(R"(
vec3 rs = radiiAtTexturePos(samplePosition);
if (rs.x < rs.y) {{
    vec4 c = vec4(1,rs.z,0,1);
    shadingParams.colors = defaultMaterialColors(c.rgb);
    shadingParams.normal = normalize(textureSamplePosToSpherePos(samplePosition));
    shadingParams.worldPosition = (mat4(1.0) * vec4(samplePosition, 1.0)).xyz;
    c.rgb = APPLY_LIGHTING_FUNC(lighting, shadingParams, cameraDir);
    
    result.rgb = result.rgb + (1.0 - result.a) * c.a * c.rgb;
    result.a = result.a + (1.0 - result.a) * c.a;
}}
)");

}  // namespace
}  // namespace surface

auto SurfaceComponent::getSegments() -> std::vector<Segment> {
    using namespace fmt::literals;
    if (surface.isConnected()) {
        return {{fmt::format(surface::uniforms, "name"_a = getName()), placeholder::uniform, 900},
                {fmt::format(surface::setup, "name"_a = getName()), placeholder::setup, 900},
                {fmt::format(surface::loop, "name"_a = getName()), placeholder::loop, 1400}};
    } else {
        return {};
    }
}

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo OceanRaycaster::processorInfo_{
    "org.inviwo.OceanRaycaster",                  // Class identifier
    "Ocean Raycaster",                            // Display name
    "Volume Rendering",                           // Category
    CodeState::Stable,                            // Code state
    Tags::GL | Tag{"Volume"} | Tag{"Raycaster"},  // Tags
    R"(<Explanation of how to use the processor.>)"_unindentHelp,
};

const ProcessorInfo& OceanRaycaster::getProcessorInfo() const { return processorInfo_; }

OceanRaycaster::OceanRaycaster(std::string_view identifier, std::string_view displayName)
    : VolumeRaycasterBase(identifier, displayName)
    , volume_{"volume", NemoVolumeComponent::Gradients::Single,
              "input Nemo volume (Only one channel will be rendered)"_help}
    , camera_{"camera",
              [this]() -> std::optional<dmat4> {
                  if (entryExit_.active == 0) {
                      if (auto data = volume_.volumePort.getData()) {
                          return data->prototype().world.value_or(VolumeConfig::defaultWorld) *
                                 data->prototype().model.value_or(VolumeConfig::defaultModel);
                      } else {
                          return std::nullopt;
                      }
                  } else {
                      return dmat4{1.0};
                  }
              }}

    , cubeEntryExit_{}
    , sphericalEntryExit_{&camera_.camera}
    , entryExit_{"Entry Exit", {&cubeEntryExit_, &sphericalEntryExit_}}
    , background_{*this}
    , isoTF_{&volume_.volumePort}
    , raycasting_{volume_.getName(), isoTF_.isoTFs[0]}

    , light_{&camera_.camera}
    , positionIndicator_{}
    , sampleTransform_{}
    , mask_{"Mask", volume_.volumePort.getIdentifier()}
    , surface_{} {

    registerComponents(volume_, entryExit_, background_, raycasting_, isoTF_, camera_, light_,
                       positionIndicator_, sampleTransform_, mask_, surface_);
}

void OceanRaycaster::process() {
    util::checkValidChannel(raycasting_.selectedChannel(), volume_.channelsForVolume().value_or(0));

    if (entryExit_.activeComponent() == &sphericalEntryExit_) {
        if (sphericalEntryExit_.outerRadius.isModified()) {
            sphericalEntryExit_.sphereMesh =
                eep::sphere(sphericalEntryExit_.outerRadius.get(), 0.0, 16, 32);
        }

        if (camera_.camera.isModified() ||
            outport_.getDimensions() != sphericalEntryExit_.entryPoints.getDimensions() ||
            sphericalEntryExit_.outerRadius.isModified()) {
            sphericalEntryExit_.preprocess(camera_.camera.get(), outport_.getDimensions());
        }
    }

    VolumeRaycasterBase::process();
}

}  // namespace inviwo
