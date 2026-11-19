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

constexpr std::string_view includes = util::trim(R"(
#include "orca_inverse.glsl"
#include "orca_util.glsl"
)");

constexpr std::string_view uniformsZoom = util::trim(R"(
uniform float zZoom;
)");

constexpr std::string_view depth = util::trim(R"(
)");

constexpr std::string_view sample = util::trim(R"(
{0}SamplePosition = mapPositionToTexture({0}SamplePosition, zZoom);
)");

constexpr std::string_view continueIfOutside = util::trim(R"(
if (any(lessThan({0}SamplePosition, vec3(0.0))) || any(greaterThan({0}SamplePosition, vec3(1.0)))) {{
    continue;
}})");

}  // namespace
}  // namespace nemo

auto NemoVolumeComponent::getSegments() -> std::vector<Segment> {
    auto segments = TemporalVolumeComponent::getSegments();

    segments.emplace_back(Segment{.snippet = fmt::format(nemo::includes, getName()),
                                  .placeholder = placeholder::include,
                                  .priority = 400});

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
    segments.emplace_back(Segment{.snippet = fmt::format(nemo::continueIfOutside, getName()),
                                  .placeholder = placeholder::loop,
                                  .priority = 402});
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

const vec4 landMaskColor = vec4(0.3, 0.31, 0.3, 1.0);
const vec4 oceanMaskColor = vec4(0.3, 0.30, 0.31, 1.0);

)");

constexpr std::string_view first = util::trim(R"(
float {0}Value = getNormalizedVoxel({0}, {0}Parameters, {1}SamplePosition).x;
float {0}ValuePrev = {0}Value;
if ({0}Value > 0.0) {{
   color = landMaskColor;
}}
)");

constexpr std::string_view loop = util::trim(R"(
{0}Value = getNormalizedVoxel({0}, {0}Parameters, {1}SamplePosition).x;
if ({0}Value > 0.0) {{
   {0}ValuePrev = {0}Value;

   vec4 c = oceanMaskColor;
   result.rgb = result.rgb + (1.0 - result.a) * c.a * c.rgb;
   result.a = result.a + (1.0 - result.a) * c.a;

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

SurfaceComponent::SurfaceComponent() : ShaderComponent() {}

std::string_view SurfaceComponent::getName() const { return "surface"; }

void SurfaceComponent::process(Shader&, TextureUnitContainer&) {}

std::vector<std::tuple<Inport*, std::string>> SurfaceComponent::getInports() { return {}; }

namespace surface {
namespace {

constexpr std::string_view grid = util::trim(R"(
vec3 rThetaPhi = cartesianToSpherical(textureSamplePosToSpherePos(entryPoint));
if (mod(rThetaPhi.y*180 / PI, 15) < 0.2 || mod(rThetaPhi.z*180 / PI, 15) < 0.1) {{
    result = mix(result, vec4(0.4,0.4,0.4,1), 0.5);
}}
if (mod(rThetaPhi.r*100+1, 3) < 0.2) {{
    result = mix(result, vec4(0.4,0.4,0.4,1), 0.5);
}}
)");

}  // namespace
}  // namespace surface

auto SurfaceComponent::getSegments() -> std::vector<Segment> {
    using namespace fmt::literals;
    return {
        {fmt::format(surface::grid, "name"_a = getName()), placeholder::post, 2000},
    };
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
    , camera_{"camera", [this]() -> std::optional<dmat4> { return entryExit_.boundingBox(); }}
    , entryExit_{[](CoordinateSpace , CoordinateSpace ) -> glm::dmat4 {
                     return mat4{1.0};
                 },
                 camera_.getName()}
    , background_{*this}
    , isoTF_{&volume_.volumePort}
    , raycasting_{volume_.getName(), isoTF_.isoTFs[0]}

    , light_{&camera_.camera}
    , positionIndicator_{}
    , sampleTransform_{}
    , mask_{"Mask", volume_.volumePort.getIdentifier()}
    , surface_{}
    , surfaceTF_{fmt::format("{}TF", surface_.getName()), fmt::format("{} TF", surface_.getName()),
                 ""_help, TFData{}}
    , trackball_{&camera_.camera} {

    registerComponents(volume_, entryExit_, background_, raycasting_, isoTF_, camera_, light_,
                       positionIndicator_, sampleTransform_, mask_, surface_, surfaceTF_);

    addProperties(trackball_);

    isoTF_.isoTFs[0].setHistogramSelection(histogramSelectionAll);
    isoTF_.isoTFs[0].setCurrentStateAsDefault();
}

void OceanRaycaster::process() {
    util::checkValidChannel(raycasting_.selectedChannel(), volume_.channelsForVolume().value_or(0));

    VolumeRaycasterBase::process();
}

}  // namespace inviwo
