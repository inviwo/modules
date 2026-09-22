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

#include <inviwo/oceanflow/oceanflowmoduledefine.h>

#include <inviwo/core/datastructures/geometry/mesh.h>
#include <inviwo/core/ports/volumeport.h>
#include <inviwo/core/util/utilities.h>

#include <modules/basegl/algorithm/entryexitpoints.h>
#include <modules/basegl/processors/raycasting/volumeraycasterbase.h>
#include <modules/basegl/shadercomponents/backgroundcomponent.h>
#include <modules/basegl/shadercomponents/cameracomponent.h>
#include <modules/basegl/shadercomponents/entryexitcomponent.h>
#include <modules/basegl/shadercomponents/isotfcomponent.h>
#include <modules/basegl/shadercomponents/lightcomponent.h>
#include <modules/basegl/shadercomponents/positionindicatorcomponent.h>
#include <modules/basegl/shadercomponents/raycastingcomponent.h>
#include <modules/basegl/shadercomponents/sampletransformcomponent.h>
#include <modules/basegl/shadercomponents/volumecomponent.h>
#include <modules/basegl/shadercomponents/maskcomponent.h>
#include <modules/basegl/shadercomponents/temporalvolumecomponent.h>

#include <tuple>

namespace inviwo {

class IVW_MODULE_OCEANFLOW_API AlternativeShaderComponent : public ShaderComponent {
public:
    AlternativeShaderComponent(std::string_view aName, std::vector<ShaderComponent*> someComponents)

        : ShaderComponent{}, name{aName}, components{std::move(someComponents)}, active{0} {

        if (components.empty()) {
            throw Exception(SourceContext{}, "Has to have at least 1 component");
        }

        auto makeAltCallback = [&, updating = std::make_shared<bool>(false)](
                                   BoolCompositeProperty* cp, size_t index) {
            return [updating, cp, index, this]() {
                if (*updating) return;
                util::KeepTrueWhileInScope guard{updating.get()};

                if (cp->isChecked()) {
                    active = index;
                } else {
                    active = (index + 1) % opts.size();
                }
                for (auto&& [i, p] : std::views::zip(std::views::iota(0uz), opts)) {
                    p->setChecked(index == i);
                }

                for (auto&& [i, c] : std::views::zip(std::views::iota(0uz), components)) {
                    for (auto&& [p, s] : c->getInports()) {
                        p->setOptional(i != index);
                    }
                }
            };
        };

        for (auto&& [i, comp] : std::views::zip(std::views::iota(0uz), components)) {
            auto& cp = opts.emplace_back(std::make_unique<BoolCompositeProperty>(
                util::stripIdentifier(comp->getName()), comp->getName(), i == active));
            for (auto* p : comp->getProperties()) {
                cp->addProperty(p, false);
            }
            cp->getBoolProperty()->setInvalidationLevel(InvalidationLevel::InvalidResources);
            cp->onChange(makeAltCallback(cp.get(), i));
        }
    }

    virtual std::string_view getName() const override { return name; }
    virtual void initializeResources(Shader& shader) override {
        components[active]->initializeResources(shader);
    }
    virtual void process(Shader& shader, TextureUnitContainer& container) override {
        components[active]->process(shader, container);
    }
    virtual std::vector<std::tuple<Inport*, std::string>> getInports() override {
        std::vector<std::tuple<Inport*, std::string>> inports;
        for (auto& comp : components) {
            inports.append_range(comp->getInports());
        }
        return inports;
    }
    virtual std::vector<Property*> getProperties() override {
        return opts | std::views::transform([](auto& p) { return p.get(); }) |
               std::ranges::to<std::vector<Property*>>();
    }
    virtual std::vector<Segment> getSegments() override {
        return components[active]->getSegments();
    }

    std::string name;
    std::vector<ShaderComponent*> components;
    std::vector<std::unique_ptr<BoolCompositeProperty>> opts;
    size_t active = 0;
};

class IVW_MODULE_OCEANFLOW_API NemoVolumeComponent : public TemporalVolumeComponent {
public:
    explicit NemoVolumeComponent(std::string_view name, Gradients gradients = Gradients::Single,
                                 Document help = {});
    virtual ~NemoVolumeComponent();

    virtual void process(Shader& shader, TextureUnitContainer& cont) override;
    virtual std::vector<Property*> getProperties() override;
    virtual std::vector<Segment> getSegments() override;

    FloatProperty zZoom;
};

class IVW_MODULE_OCEANFLOW_API VolumeMaskComponent : public ShaderComponent {
public:
    explicit VolumeMaskComponent(std::string_view maskName, std::string_view volumeName);

    virtual std::string_view getName() const override;
    virtual void process(Shader& shader, TextureUnitContainer& cont) override;
    virtual std::vector<Segment> getSegments() override;
    virtual std::vector<std::tuple<Inport*, std::string>> getInports() override {
        return {{&mask_, std::string{"volumes"}}};
    }

private:
    VolumeInport mask_;
    std::string volumeName_;
};

class SphericalEntryExitPoints : public ShaderComponent {
public:
    SphericalEntryExitPoints();

    virtual std::string_view getName() const override;
    virtual void initializeResources(Shader& shader) override;
    virtual void process(Shader& shader, TextureUnitContainer& cont) override;
    virtual std::vector<Segment> getSegments() override;
    virtual std::vector<Property*> getProperties() override;

    void preprocess(Camera& camera, size2_t dim);

    OrdinalProperty<dmat4> basis;
    OrdinalProperty<double> outerRadius;
    OrdinalProperty<double> innerRadius;
    Mesh sphereMesh;
    Image entryPoints;
    Image exitPoints;

    algorithm::EntryExitPointsHelper eepHelper;
};

class IVW_MODULE_OCEANFLOW_API OceanRaycaster : public VolumeRaycasterBase {
public:
    OceanRaycaster(std::string_view identifier = "", std::string_view displayName = "");
    virtual ~OceanRaycaster() = default;

    virtual const ProcessorInfo& getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

    virtual void process() override;

private:
    NemoVolumeComponent volume_;
    EntryExitComponent cubeEntryExit_;
    SphericalEntryExitPoints sphericalEntryExit_;
    AlternativeShaderComponent entryExit_;
    BackgroundComponent background_;
    IsoTFComponent<1> isoTF_;
    RaycastingComponent raycasting_;
    CameraComponent camera_;
    LightComponent light_;
    PositionIndicatorComponent positionIndicator_;
    SampleTransformComponent sampleTransform_;
    VolumeMaskComponent mask_;
};

}  // namespace inviwo
