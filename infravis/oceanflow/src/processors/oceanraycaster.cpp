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
#include <modules/basegl/shadercomponents/shadercomponentutil.h>

#include <inviwo/core/util/stringconversion.h>
#include <modules/opengl/volume/volumeutils.h>
#include <modules/opengl/shader/shader.h>
#include <modules/opengl/shader/shaderutils.h>

#include <fmt/core.h>
#include <fmt/format.h>

namespace inviwo {

NemoVolumeComponent::NemoVolumeComponent(std::string_view name, Gradients gradients, Document help)
    : ShaderComponent()
    , volumePort{name, std::move(help)}
    , gradients{gradients}
    , zZoom{"zZoom", "Z Zoom factor", util::ordinalScale(1.0f)} {}

std::string_view NemoVolumeComponent::getName() const { return volumePort.getIdentifier(); }

void NemoVolumeComponent::process(Shader& shader, TextureUnitContainer& cont) {
    utilgl::bindAndSetUniforms(shader, cont, volumePort);
    utilgl::setUniforms(shader, zZoom);
}

std::vector<Property*> NemoVolumeComponent::getProperties() { return {&zZoom}; }

std::vector<std::tuple<Inport*, std::string>> NemoVolumeComponent::getInports() {
    return {{&volumePort, std::string{"volumes"}}};
}

namespace {

constexpr std::string_view uniforms = util::trim(R"(
uniform VolumeParameters {0}Parameters;
uniform sampler3D {0};
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

constexpr std::string_view sampleFirst = util::trim(R"(
vec3 {0}SamplePosition = mapPositionToTexture(samplePosition);
)");

// Initialize the VoxelPrev value to the same as the first voxel value. This value is important
// mainly for the isosurface rendering. Setting it to the same voxel value prevents isosurfaces
// being rendered at the volume boundaries.
constexpr std::string_view voxelFirst = util::trim(R"(
vec4 {0}Voxel = getNormalizedVoxel({0}, {0}Parameters, {0}SamplePosition);
vec4 {0}VoxelPrev = {0}Voxel;
)");

constexpr std::string_view sample = util::trim(R"(
{0}SamplePosition = mapPositionToTexture(samplePosition);
)");

constexpr std::string_view voxel = util::trim(R"(
{0}VoxelPrev = {0}Voxel;
{0}Voxel = getNormalizedVoxel({0}, {0}Parameters, {0}SamplePosition);
)");

constexpr std::string_view gradientFirst = util::trim(R"(
vec3 {0}GradientPrev = vec3(0);
vec3 {0}Gradient = vec3(0);
#if defined(GRADIENTS_ENABLED)
{0}Gradient = useSurfaceNormals ? -texture(surfaceNormal, texCoords).xyz :
    normalize(COMPUTE_GRADIENT_FOR_CHANNEL({0}Voxel, {0}, {0}Parameters,
                                           {0}SamplePosition, channel));
if (!useSurfaceNormals) {{
    {0}Gradient *= sign({0}Voxel[channel] / {0}Parameters.texToNormalized.scale + {0}Parameters.texToNormalized.offset);
}}
#endif
)");

constexpr std::string_view gradient = util::trim(R"(
#if defined(GRADIENTS_ENABLED)
{0}GradientPrev = {0}Gradient;
{0}Gradient = normalize(COMPUTE_GRADIENT_FOR_CHANNEL({0}Voxel, {0}, {0}Parameters,
                                                    {0}SamplePosition, channel));
{0}Gradient *= sign({0}Voxel[channel] / {0}Parameters.texToNormalized.scale + {0}Parameters.texToNormalized.offset);
#endif
)");

}  // namespace

auto NemoVolumeComponent::getSegments() -> std::vector<Segment> {

    std::vector<Segment> segments{{.snippet = fmt::format(uniforms, getName()),
                                   .placeholder = placeholder::uniform,
                                   .priority = 400},
                                  {.snippet = fmt::format(depth, getName()),
                                   .placeholder = placeholder::uniform,
                                   .priority = 410},
                                  {.snippet = fmt::format(sampleFirst, getName()),
                                   .placeholder = placeholder::first,
                                   .priority = 400},
                                  {.snippet = fmt::format(voxelFirst, getName()),
                                   .placeholder = placeholder::first,
                                   .priority = 420},
                                  {.snippet = fmt::format(sample, getName()),
                                   .placeholder = placeholder::loop,
                                   .priority = 400},
                                  {.snippet = fmt::format(voxel, getName()),
                                   .placeholder = placeholder::loop,
                                   .priority = 420}};

    if (gradients != Gradients::None) {
        segments.emplace_back(std::string{R"(#include "utils/gradients.glsl")"},
                              placeholder::include, 400);
    }
    if (gradients == Gradients::Single) {
        segments.emplace_back(fmt::format(gradientFirst, getName()), placeholder::first, 440);
        segments.emplace_back(fmt::format(gradient, getName()), placeholder::loop, 440);
    }

    return segments;
}

std::string NemoVolumeComponent::getGradientString() const {
    switch (gradients) {
        case Gradients::None:
        case Gradients::Single:
            return fmt::format("{0}Gradient", getName());
        default:
            return {};
    }
}

std::optional<size_t> NemoVolumeComponent::channelsForVolume() const {
    if (auto data = volumePort.getData()) {
        return data->getDataFormat()->getComponents();
    }
    return std::nullopt;
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
              "input nemo volume (Only one channel will be rendered)"_help}
    , entryExit_{}
    , background_{*this}
    , isoTF_{volume_.volumePort}
    , raycasting_{volume_.getName(), isoTF_.isoTFs[0]}
    , camera_{"camera", util::boundingBox(volume_.volumePort)}
    , light_{&camera_.camera}
    , positionIndicator_{}
    , sampleTransform_{}
    , mask_{volume_.volumePort} {

    registerComponents(volume_, entryExit_, background_, raycasting_, isoTF_, camera_, light_,
                       positionIndicator_, sampleTransform_, mask_);
}

void OceanRaycaster::process() {
    util::checkValidChannel(raycasting_.selectedChannel(), volume_.channelsForVolume().value_or(0));

    mask_.preprocess();

    VolumeRaycasterBase::process();
}

}  // namespace inviwo
