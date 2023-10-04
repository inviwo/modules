/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2021 Inviwo Foundation
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

#include <inviwo/ttk/ttkmoduledefine.h>
#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/network/processornetwork.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/processors/progressbarowner.h>
#include <inviwo/core/properties/property.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/compositeproperty.h>
#include <inviwo/core/util/foreacharg.h>
#include <inviwo/core/util/zip.h>
#include <inviwo/core/util/utilities.h>
#include <inviwo/core/util/rendercontext.h>
#include <inviwo/core/algorithm/markdown.h>

#include <inviwo/ttk/ports/vtkinport.h>
#include <inviwo/ttk/ports/vtkoutport.h>

#include <tuple>
#include <optional>
#include <type_traits>

#include <fmt/format.h>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkNew.h>
#include <vtkInformation.h>
#include <vtkInformationStringVectorKey.h>
#include <vtkDataSet.h>
#include <vtkDataObject.h>
#include <vtkDataObjectTypes.h>
#include <vtkFieldData.h>

#include <vtkSmartPointer.h>
#include <vtkExecutive.h>
#include <ttkAlgorithm.h>
#include "vtkCommand.h"

// debug
#include <ttkMorseSmaleComplex.h>

#include <warn/pop>

namespace inviwo {

namespace ttkwrapper {

struct IVW_MODULE_TTK_API InputData {
    std::string identifier;
    std::string dataType;
    int numComp;
    std::string doc;
};

struct IVW_MODULE_TTK_API OutputData {
    std::string identifier;
    std::string displayName;
    int index;
};

struct IVW_MODULE_TTK_API Group {
    std::string displayName;
    std::vector<std::string> members;
};

template <typename TTKFilter>
struct TTKTraits;

struct IVW_MODULE_TTK_API FieldSelection {};

class IVW_MODULE_TTK_API Command : public vtkCommand {
public:
    vtkTypeMacro(Command, vtkCommand)

        static Command* New() {
        return new Command;
    }
    void Execute(vtkObject* caller, unsigned long eid, void* callData) override {
        if (callback) {
            callback(caller, eid, callData);
        }
    }

    std::function<void(vtkObject*, unsigned long, void*)> callback;

protected:
    Command() = default;
    ~Command() override = default;
};

inline std::string toString(vtkInformation* info) {
    vtkIndent indent;
    std::stringstream ss;
    info->PrintSelf(ss, indent);
    return std::move(ss.str());
}

/**
 * @brief A Wrapper for all TTK filters
 */
template <typename TTKFilter>
class TTKGenericProcessor : public Processor, public ProgressBarOwner {
public:
    TTKGenericProcessor() : Processor() {
        observer_ = vtkSmartPointer<Command>::Take(Command::New());

        observer_->callback = [this](vtkObject*, unsigned long eid, void* data) {
            if (eid == vtkCommand::ErrorEvent) {
                LogError(static_cast<const char*>(data));
            } else if (eid == vtkCommand::WarningEvent) {
                LogWarn(static_cast<const char*>(data));
            } else if (eid == vtkCommand::MessageEvent) {
                LogInfo(static_cast<const char*>(data));
            } else if (eid == vtkCommand::TextEvent) {
                LogInfo(static_cast<const char*>(data));
            } else if (eid == vtkCommand::ProgressEvent) {
                updateProgress(static_cast<float>(*static_cast<double*>(data)));
                getNetwork()->getApplication()->processEvents();
                rendercontext::activateDefault();
            }
        };

        filter_->AddObserver(vtkCommand::ErrorEvent, observer_);
        filter_->AddObserver(vtkCommand::WarningEvent, observer_);
        filter_->AddObserver(vtkCommand::MessageEvent, observer_);
        filter_->AddObserver(vtkCommand::TextEvent, observer_);
        filter_->AddObserver(vtkCommand::ProgressEvent, observer_);
        filter_->DebugOn();

        const auto nInputs = filter_->GetNumberOfInputPorts();
        const auto nOutputs = filter_->GetNumberOfOutputPorts();

        for (int i = 0; i < nInputs; ++i) {
            vtkInformation* info = filter_->GetInputPortInformation(i);

            std::optional<std::string> dataType;
            if (info->Has(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE())) {
                dataType = info->Get(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE());
            } else if (info->Has(vtkDataObject::DATA_TYPE_NAME())) {
                dataType = info->Get(vtkDataObject::DATA_TYPE_NAME());
            }

            if (dataType) {
                auto id = i < static_cast<int>(TTKTraits<TTKFilter>::inports.size())
                              ? util::stripIdentifier(TTKTraits<TTKFilter>::inports[i].identifier)
                              : fmt::format("inport{}", i);

                auto doc = i < static_cast<int>(TTKTraits<TTKFilter>::outports.size())
                               ? util::unindentMd2doc(TTKTraits<TTKFilter>::inports[i].doc)
                               : Document{};
                addPort(std::make_unique<vtk::VtkInport>(id, *dataType, std::move(doc)));
            } else {
                throw Exception(
                    fmt::format("missing inport for {}, unknown VTK dataType. Port info: {}",
                                TTKTraits<TTKFilter>::identifier, toString(info)));
            }
        }

        for (int i = 0; i < nOutputs; ++i) {
            vtkInformation* info = filter_->GetOutputPortInformation(i);
            std::optional<std::string> dataType;
            if (info->Has(ttkAlgorithm::SAME_DATA_TYPE_AS_INPUT_PORT())) {
                int inportNum = info->Get(ttkAlgorithm::SAME_DATA_TYPE_AS_INPUT_PORT());
                vtkInformation* inportInfo = filter_->GetInputPortInformation(inportNum);
                if (inportInfo->Has(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE())) {
                    dataType = inportInfo->Get(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE());
                } else if (info->Has(vtkDataObject::DATA_TYPE_NAME())) {
                    dataType = info->Get(vtkDataObject::DATA_TYPE_NAME());
                }
            } else if (info->Has(vtkDataObject::DATA_TYPE_NAME())) {
                dataType = info->Get(vtkDataObject::DATA_TYPE_NAME());
            }

            if (dataType) {
                auto id = i < static_cast<int>(TTKTraits<TTKFilter>::outports.size())
                              ? util::stripIdentifier(TTKTraits<TTKFilter>::outports[i].identifier)
                              : fmt::format("output{}", i);

                auto doc = i < static_cast<int>(TTKTraits<TTKFilter>::outports.size())
                               ? util::unindentMd2doc(TTKTraits<TTKFilter>::outports[i].displayName)
                               : Document{};
                addPort(std::make_unique<vtk::VtkOutport>(id, *dataType, std::move(doc)));
            } else {
                throw Exception(
                    fmt::format("missing outport for {}, dataType not known, port info: {}",
                                TTKTraits<TTKFilter>::identifier, toString(info)));
            }
        }

        std::unordered_map<std::string_view, CompositeProperty*> groupMap;
        for (auto&& group : traits_.groups) {
            auto comp = std::make_unique<CompositeProperty>(
                util::stripIdentifier(group.displayName), group.displayName);
            for (auto&& member : group.members) {
                groupMap[member] = comp.get();
            }
            addProperty(comp.release(), true);
        }

        util::for_each_in_tuple(
            [&](auto& wrapper) {
                if (auto it = groupMap.find(wrapper.property.getIdentifier());
                    it != groupMap.end()) {
                    it->second->addProperty(wrapper.property);
                } else {
                    addProperty(wrapper.property);
                }

                if constexpr (std::is_base_of_v<FieldSelection, std::decay_t<decltype(wrapper)>>) {
                    if (auto inport = getInport(wrapper.inport)) {
                        auto vtkinport = static_cast<vtk::VtkInport*>(inport);

                        auto update = [](vtkDataObject::FieldAssociations assoc,
                                         vtkDataObject* data, OptionPropertyString& opts) {
                            std::vector<OptionPropertyStringOption> options;
                            if (assoc == vtkDataObject::FIELD_ASSOCIATION_POINTS ||
                                assoc == vtkDataObject::FIELD_ASSOCIATION_POINTS_THEN_CELLS) {

                                vtkFieldData* fd =
                                    data->GetAttributesAsFieldData(vtkDataObject::POINT);
                                for (int i = 0; i < fd->GetNumberOfArrays(); ++i) {
                                    options.emplace_back(fd->GetArrayName(i));
                                }
                            }

                            if (assoc == vtkDataObject::FIELD_ASSOCIATION_CELLS ||
                                assoc == vtkDataObject::FIELD_ASSOCIATION_POINTS_THEN_CELLS) {

                                vtkFieldData* fd =
                                    data->GetAttributesAsFieldData(vtkDataObject::CELL);
                                for (int i = 0; i < fd->GetNumberOfArrays(); ++i) {
                                    options.emplace_back(fd->GetArrayName(i));
                                }
                            }

                            opts.replaceOptions(std::move(options));
                        };

                        vtkinport->onChange([update, port = vtkinport, w = &wrapper]() {
                            if (port->isReady()) {
                                vtkDataObject* data = port->getData();
                                update(w->fieldAssociation.get(), data, w->name);
                            }
                        });
                        wrapper.fieldAssociation.onChange(
                            [update, port = vtkinport, w = &wrapper]() {
                                if (port->isReady()) {
                                    vtkDataObject* data = port->getData();
                                    update(w->fieldAssociation.get(), data, w->name);
                                }
                            });
                    }
                }
            },
            traits_.properties);
    }
    virtual ~TTKGenericProcessor() = default;

    virtual void process() override {
        bool ready = true;
        util::for_each_in_tuple([&](auto& wrapper) { ready &= wrapper.set(*filter_); },
                                traits_.properties);

        if (!ready) return;

        getProgressBar().setActive(true);

        // vtkNew<ttkMorseSmaleComplex> filter_;

        const auto nInputs = filter_->GetNumberOfInputPorts();
        const auto nOutputs = filter_->GetNumberOfOutputPorts();

        for (int i = 0; i < nInputs; ++i) {
            auto data = static_cast<vtk::VtkInport*>(getInports()[i])->getData();
            filter_->SetInputDataObject(i, data);
        }

        if (filter_->GetExecutive()->Update() != 1) {
            for (int i = 0; i < nOutputs; ++i) {
                static_cast<vtk::VtkOutport*>(getOutports()[i])->setData(nullptr);
            }
            throw Exception(IVW_CONTEXT, "Error running vtk filter");
        }

        for (int i = 0; i < nOutputs; ++i) {
            if (auto result = filter_->GetOutputDataObject(i)) {
                static_cast<vtk::VtkOutport*>(getOutports()[i])->setData(result);
            } else {
                static_cast<vtk::VtkOutport*>(getOutports()[i])->setData(nullptr);
            }
        }

        getProgressBar().setActive(false);
    }

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    vtkNew<TTKFilter> filter_;
    TTKTraits<TTKFilter> traits_;
    vtkSmartPointer<Command> observer_;
};

}  // namespace ttkwrapper

template <typename TTKFilter>
struct ProcessorTraits<ttkwrapper::TTKGenericProcessor<TTKFilter>> {
    static ProcessorInfo getProcessorInfo() {
        return ProcessorInfo{
            fmt::format("org.inviwo.tkk.{}",
                        ttkwrapper::TTKTraits<TTKFilter>::className),      // Class identifier
            std::string{ttkwrapper::TTKTraits<TTKFilter>::displayName},    // Display name
            std::string{ttkwrapper::TTKTraits<TTKFilter>::category},       // Category
            CodeState::Experimental,                                       // Code state
            Tags{ttkwrapper::TTKTraits<TTKFilter>::tags},                  // Tags
            util::unindentMd2doc(ttkwrapper::TTKTraits<TTKFilter>::doc)};  // Documentation
    }
};

template <typename TTKFilter>
inline const ProcessorInfo ttkwrapper::TTKGenericProcessor<TTKFilter>::getProcessorInfo() const {
    return ProcessorTraits<TTKGenericProcessor<TTKFilter>>::getProcessorInfo();
}

}  // namespace inviwo

/*

#include <vtkType.h>

int GetVTKType(std::size_t hash_code)
{
    static std::map<std::size_t, long> typeMap;
    if(typeMap.empty())
    {
        typeMap[typeid(void).hash_code()]               = VTK_VOID;
        typeMap[typeid(char).hash_code()]               = VTK_CHAR;
        typeMap[typeid(signed char).hash_code()]        = VTK_SIGNED_CHAR;
        typeMap[typeid(unsigned char).hash_code()]      = VTK_UNSIGNED_CHAR;
        typeMap[typeid(short).hash_code()]              = VTK_SHORT;
        typeMap[typeid(unsigned short).hash_code()]     = VTK_UNSIGNED_SHORT;
        typeMap[typeid(int).hash_code()]                = VTK_INT;
        typeMap[typeid(unsigned int).hash_code()]       = VTK_UNSIGNED_INT;
        typeMap[typeid(long).hash_code()]               = VTK_LONG;
        typeMap[typeid(unsigned long).hash_code()]      = VTK_UNSIGNED_LONG;
        typeMap[typeid(float).hash_code()]              = VTK_FLOAT;
        typeMap[typeid(double).hash_code()]             = VTK_DOUBLE;
        typeMap[typeid(std::string).hash_code()]        = VTK_STRING;
        typeMap[typeid(long long).hash_code()]          = VTK_LONG_LONG;
        typeMap[typeid(unsigned long long).hash_code()] = VTK_UNSIGNED_LONG_LONG;
        typeMap[typeid(int64_t).hash_code()]            = VTK___INT64;
        typeMap[typeid(uint64_t).hash_code()]           = VTK_UNSIGNED___INT64;
    }
    return typeMap[hash_code];
}


*/
