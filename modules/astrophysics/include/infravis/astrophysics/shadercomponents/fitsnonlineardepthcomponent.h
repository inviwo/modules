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
#pragma once

#include <infravis/astrophysics/astrophysicsmoduledefine.h>

#include <inviwo/core/ports/volumeport.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/compositeproperty.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <modules/basegl/shadercomponents/shadercomponent.h>

namespace inviwo {

class Inport;
class Property;
class Shader;
class TextureUnitContainer;

class IVW_MODULE_ASTROPHYSICS_API FitsNonlinearDepthComponent : public ShaderComponent {
public:
    explicit FitsNonlinearDepthComponent(std::string_view volume, Document help = {});

    virtual std::string_view getName() const override;
    virtual void process(Shader& shader, TextureUnitContainer&) override;
    virtual void initializeResources(Shader& shader) override;
    virtual std::vector<std::tuple<Inport*, std::string>> getInports() override;
    virtual std::vector<Property*> getProperties() override;

    virtual std::vector<Segment> getSegments() override;

    std::optional<size_t> channelsForVolume() const;

    VolumeInport volumePort_;

private:
    CompositeProperty fitsParameters_;
    FloatProperty assumedDistance_;
    FloatProperty velocityInf_;
    FloatProperty velocityStar_;
    FloatProperty crpix3_;
    FloatProperty crval3_;
    FloatProperty cdelt3_;
    FloatProperty restFrequency_;
    FloatVec3Property dataExtent_;
    FloatVec3Property dataOffset_;
};

}  // namespace inviwo
