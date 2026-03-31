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

#include <infravis/astrophysics/shadercomponents/fitsnonlineardepthcomponent.h>

#include <modules/opengl/shader/shader.h>
#include <modules/opengl/shader/shaderutils.h>
#include <modules/opengl/volume/volumeutils.h>

#include <glm/gtx/transform.hpp>
#include <fmt/core.h>
#include <fmt/format.h>

namespace inviwo {

FitsNonlinearDepthComponent::FitsNonlinearDepthComponent(std::string_view name, Document help)
    : ShaderComponent{}
    , volumePort_{name, std::move(help)}
    , fitsParameters_{"fitsParameters", "FitsParameters",
                      "Various parameters required for coordinate transformations and other calculations."_help}
    , objectName_{"objectName", "Object",
                  "Contents of the 'objects' field in the FITS Header."_help, "",
                  InvalidationLevel::Valid}
    , assumedDistance_{"assumedDistance", "Assumed Distance [pc]",
                       util::ordinalSymmetricVector(0.0f)
                           .setInc(0.001f)
                           .set(PropertySemantics::Text)
                           .set("Assumed distance of the star in parsec [pc]."_help)}
    , velocityInf_{"velocityInf", "v_infinity [km/s]",
                   util::ordinalSymmetricVector(0.0f)
                       .setInc(0.001f)
                       .set(PropertySemantics::Text)
                       .set("The hyperbolic excess speed v_inf [km/s]."_help)}
    , velocityStar_{"velocityStar", "v_star [km/s]",
                    util::ordinalSymmetricVector(0.0f)
                        .setInc(0.001f)
                        .set(PropertySemantics::Text)
                        .set("v_star [km/s]"_help)}
    , crpix3_{"crpix3", "crpix3",
              util::ordinalSymmetricVector(0.0f)
                  .setInc(0.001f)
                  .set(PropertySemantics::Text)
                  .set("Reference pixel in axis3"_help)}
    , crval3_{"crval3", "crval3",
              util::ordinalSymmetricVector(0.0f)
                  .setInc(0.001f)
                  .set(PropertySemantics::Text)
                  .set("Physical value of the reference pixel"_help)}
    , cdelt3_{"cdelt3", "cdelt3",
              util::ordinalSymmetricVector(0.0f)
                  .setInc(0.001f)
                  .set(PropertySemantics::Text)
                  .set(""_help)}
    , restFrequency_{"restFrequency", "Rest Frequency [Hz]",
                     util::ordinalSymmetricVector(0.0f)
                         .setInc(0.001f)
                         .set(PropertySemantics::Text)
                         .set("restfreq [Hz]"_help)}
    , dataExtent_{"dataExtent", "Extent [au]",
                  util::ordinalSymmetricVector(vec3{0.0f})
                      .setInc(vec3{0.001f})
                      .set(PropertySemantics::Text)
                      .set("Extent of the dataset in au."_help)}
    , dataOffset_{"dataOffset", "Offset [au]",
                  util::ordinalSymmetricVector(vec3{0.0f})
                      .setInc(vec3{0.001f})
                      .set(PropertySemantics::Text)
                      .set("Coordinate of the lower left corner of the dataset in au"_help)}
    , validChannelRange_{"validChannelRange", "Valid Channel Range",
                         OrdinalPropertyState<ivec2>{
                             .value = ivec2{0},
                             .min = ivec2{0},
                             .minConstraint = ConstraintBehavior::Ignore,
                             .max = ivec2{100},
                             .maxConstraint = ConstraintBehavior::Ignore,
                             .semantics = PropertySemantics::Text,
                             .help =
                                 "Range of valid channels based on the line-of-sight velocity"_help,
                         }}

    , invalidColor_{"invalidColor", "Invalid Value Color",
                    util::ordinalColor(vec3{0.2f, 0.2f, 0.2f})}
    , invalidAlpha_{"invalidAlpha",
                    "Invalid Value Alpha",
                    0.005f,
                    {0.0f, ConstraintBehavior::Immutable},
                    {1.0f, ConstraintBehavior::Immutable},
                    0.001f} {

    fitsParameters_.addProperties(objectName_, assumedDistance_, velocityInf_, velocityStar_,
                                  crpix3_, crval3_, cdelt3_, restFrequency_, dataExtent_,
                                  dataOffset_, validChannelRange_);
    fitsParameters_.setCollapsed(true);
}

std::string_view FitsNonlinearDepthComponent::getName() const {
    return volumePort_.getIdentifier();
}

void FitsNonlinearDepthComponent::process(Shader& shader, TextureUnitContainer& cont) {
    utilgl::bindAndSetUniforms(shader, cont, volumePort_);

    const auto data = volumePort_.getData();
    mat4 m = glm::scale(dataExtent_.get());
    m[3] = vec4{dataOffset_.get(), 1.0f};

    StrBuffer buf{"{}FitsParameters", getName()};
    std::string prefix{buf};
    shader.setUniform(buf.replace("{}.textureToModel", prefix), m);
    shader.setUniform(buf.replace("{}.assumedDistance", prefix), assumedDistance_);
    shader.setUniform(buf.replace("{}.velocityInf", prefix), velocityInf_);
    shader.setUniform(buf.replace("{}.velocityStar", prefix), velocityStar_);
    shader.setUniform(buf.replace("{}.crpix3", prefix), crpix3_);
    shader.setUniform(buf.replace("{}.crval3", prefix), crval3_);
    shader.setUniform(buf.replace("{}.cdelt3", prefix), cdelt3_);
    shader.setUniform(buf.replace("{}.restFrequency", prefix), restFrequency_);
    shader.setUniform(buf.replace("{}.validChannelRange", prefix), validChannelRange_);
    shader.setUniform("invalidColor", vec4{invalidColor_.get(), invalidAlpha_.get()});
}

std::vector<std::tuple<Inport*, std::string>> FitsNonlinearDepthComponent::getInports() {
    return {{&volumePort_, std::string{"volumes"}}};
}

void FitsNonlinearDepthComponent::initializeResources(Shader&) {
    // shader definitions...
}

std::vector<Property*> FitsNonlinearDepthComponent::getProperties() {
    return {&fitsParameters_, &invalidColor_, &invalidAlpha_};
}

namespace {

constexpr std::string_view includes = util::trim(R"(
struct FitsParameters {
    mat4 textureToModel; // transform from [0 1] to relative distances [au]

    float assumedDistance;  // [parsec]
    float velocityInf;      // [km/s]
    float velocityStar;     // [km/s]

    float crpix3;
    float crval3;
    float cdelt3;
    float restFrequency;  // [Hz]

    ivec2 validChannelRange;  // range of valid channels based on line-of-sight velocity
};

const float invalidValue = -1.e20;
)");

constexpr std::string_view uniforms = util::trim(R"(
uniform VolumeParameters {0}Parameters;
uniform sampler3D {0};

uniform FitsParameters {0}FitsParameters;
uniform vec4 invalidColor = vec4(0.2, 0.2, 0.2, 0.005);
)");

constexpr std::string_view utilityFuncs = util::trim(R"(
// Convert a velocity [km/s] to frequency channel [-].
float velocityToChannel(in FitsParameters params, in float velocity) {{
    float vc = 299792458;  // speed of light [m/s]
    return (((params.restFrequency *
              (1.0 - velocity * 1000 / vc) - params.crval3) / params.cdelt3 + params.crpix3 - 1));
}}

// convert a frequency channel index to velocity [km/s]
float channelToVelocity(in FitsParameters params, in int channel) {{
    float vc = 299792458;  // speed of light [m/s]
    float freq = (params.restFrequency -
                  ((channel + 1 - params.crpix3) * params.cdelt3 + params.crval3));
    return vc * freq / params.restFrequency / 1000.0;
}}

// Convert a relative spatial coordinate [au] to velocity [km/s]
float spatialCoordinateToVelocity(in FitsParameters params, in vec3 coord) {{
    // convert xy coord from arcseconds to au
    //coord.xy *= params.assumedDistance;
    return params.velocityInf * coord.z / length(coord) + params.velocityStar;
}}

// Sample the volume texture by transforming normalized texture coordinates @p texCoord into 
// the non-linear frequency domain using velocityToChannel(spatialCoordinateToVelocity(...)).
//
// @param texCoord   normalized texture coordinate [0, 1]
// @return volume sample at re-normalized texture coordinate, if v_los > v_star. 
//         Otherwise vec4(invalidValue);
vec4 getNormalizedVoxel(in FitsParameters params, in vec3 texCoord, in vec2 minMaxVelocity) {{
    vec3 coord = (params.textureToModel * vec4(texCoord, 1.0)).xyz;
    float velocity = spatialCoordinateToVelocity(params, coord);

    float u = velocityToChannel(params, velocity) / textureSize(volume, 0).z;
    float v_normalized = (velocity - minMaxVelocity.x) / (minMaxVelocity.y - minMaxVelocity.x);

    float v_los = velocity - params.velocityStar;
    if (abs(v_los) > params.velocityInf || u < 0.0 || u > 1.0
        || v_normalized < 0.0 || v_normalized > 1.0) {{
        return vec4(invalidValue);
    }}

    texCoord = vec3(texCoord.st, u);
    return getNormalizedVoxel({0}, {0}Parameters, texCoord);
}}
)");

// Initialize the VoxelPrev value to the same as the first voxel value. This value is important
// mainly for the isosurface rendering. Setting it to the same voxel value prevents isosurfaces
// being rendered at the volume boundaries.
constexpr std::string_view voxelFirst = util::trim(R"(
vec2 minMaxVelocity = vec2(channelToVelocity({0}FitsParameters, {0}FitsParameters.validChannelRange.x), 
                           channelToVelocity({0}FitsParameters, {0}FitsParameters.validChannelRange.y));
if (minMaxVelocity.x > minMaxVelocity.y) {{
    minMaxVelocity.xy = minMaxVelocity.yx;
}}

vec4 {0}Voxel = getNormalizedVoxel({0}FitsParameters, samplePosition, minMaxVelocity);
vec4 {0}VoxelPrev = {0}Voxel;
)");

constexpr std::string_view voxel = util::trim(R"(
{0}VoxelPrev = {0}Voxel;
{0}Voxel = getNormalizedVoxel({0}FitsParameters, samplePosition, minMaxVelocity);
)");

constexpr std::string_view unsetColor = util::trim(R"(
color = vec4(0.0);
)");

constexpr std::string_view invalidValue = util::trim(R"(
if (volumeVoxel[channel] <= invalidValue) {{
    color = invalidColor;
}}
)");

}  // namespace

auto FitsNonlinearDepthComponent::getSegments() -> std::vector<Segment> {
    using fmt::literals::operator""_a;

    std::vector<Segment> segments{
        {.snippet = std::string{includes}, .placeholder = placeholder::include, .priority = 400},
        {.snippet = fmt::format(uniforms, getName()),
         .placeholder = placeholder::uniform,
         .priority = 400},
        {.snippet = fmt::format(utilityFuncs, getName()),
         .placeholder = placeholder::uniform,
         .priority = 800},
        {.snippet = fmt::format(voxelFirst, getName()),
         .placeholder = placeholder::first,
         .priority = 400},
        {.snippet = fmt::format(unsetColor, getName()),
         .placeholder = placeholder::first,
         .priority = 701},
        {.snippet = fmt::format(voxel, getName()),
         .placeholder = placeholder::loop,
         .priority = 400},
        {.snippet = fmt::format(invalidValue, getName()),
         .placeholder = placeholder::loop,
         .priority = 701},
    };

    return segments;
}

std::optional<size_t> FitsNonlinearDepthComponent::channelsForVolume() const {
    if (auto data = volumePort_.getData()) {
        return data->getDataFormat()->getComponents();
    }
    return std::nullopt;
}

}  // namespace inviwo
