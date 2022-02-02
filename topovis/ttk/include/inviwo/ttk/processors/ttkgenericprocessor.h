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
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/property.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/compositeproperty.h>
#include <inviwo/core/ports/datainport.h>
#include <inviwo/core/ports/dataoutport.h>
#include <inviwo/core/util/foreacharg.h>
#include <inviwo/core/util/zip.h>
#include <inviwo/core/util/utilities.h>

#include <tuple>
#include <optional>
#include <type_traits>

#include <fmt/format.h>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkNew.h>
#include <vtkInformation.h>
#include <vtkInformationStringVectorKey.h>
#include <vtkDataObject.h>
#include <vtkDataSet.h>
#include <vtkFieldData.h>

#include <vtkSmartPointer.h>
#include <vtkDataObjectTypes.h>
#include <vtkExecutive.h>

#include <ttkAlgorithm.h>


// debug
#include <ttkMorseSmaleComplex.h>

#include <warn/pop>

namespace inviwo {

namespace vtk {

IVW_MODULE_TTK_API void addTable(Document::DocumentHandle& h, std::string_view str);

IVW_MODULE_TTK_API bool TypeIdIsA(int typeId, int targetTypeId);

class IVW_MODULE_TTK_API VtkOutport : public Outport {
public:
    VtkOutport(std::string_view identifier, std::string_view vtkDataClassName)
        : Outport(std::string{identifier}), vtkDataClassName_{vtkDataClassName} {

        isReady_.setUpdate([this]() {
            return invalidationLevel_ == InvalidationLevel::Valid && data_ != nullptr;
        });
    }

    const std::string& getTypeName() const { return vtkDataClassName_; }

    vtkDataObject* getData() const { return data_; }

    void setData(vtkDataObject* data) {
        if (!data) {
            data_ = data;
            isReady_.update();
            return;
        }

        if (data->IsA(vtkDataClassName_.c_str())) {
            data_ = data;
            isReady_.update();
        } else {
            throw Exception(fmt::format("Invalid VTK data type for port {} expected {} got {}",
                                        getPath(), vtkDataClassName_, data->GetClassName()));
        }
    }

    virtual bool hasData() const override { return data_ != nullptr; }

    virtual void clear() override {
        data_ = nullptr;
        isReady_.update();
    }

    virtual std::string getClassIdentifier() const override;

    virtual glm::uvec3 getColorCode() const override {
        // Todo use a table here or something...
        auto offset = 5 * vtkDataObjectTypes::GetTypeIdFromClassName(getTypeName().c_str());
        return uvec3{102, 102, 153 + offset};
    };
    virtual Document getInfo() const override {
        Document doc;
        using P = Document::PathComponent;
        using H = utildoc::TableBuilder::Header;
        auto b = doc.append("html").append("body");
        auto p = b.append("p");
        p.append("b", vtkDataClassName_ + " Outport", {{"style", "color:white;"}});
        utildoc::TableBuilder tb(p, P::end());
        tb(H("Identifier"), getIdentifier());
        tb(H("Class"), getClassIdentifier());
        tb(H("Ready"), isReady());
        tb(H("Connected"), isConnected());
        tb(H("Connections"), connectedInports_.size());

        if (hasData()) {
            std::stringstream ss;
            data_->Print(ss);
            addTable(b, ss.str());
        } else {
            b.append("p", "Port has no data");
        }
        return doc;
    }

private:
    std::string vtkDataClassName_;
    vtkDataObject* data_ = nullptr;
};

class IVW_MODULE_TTK_API VtkInport : public Inport {
public:
    VtkInport(std::string_view identifier, std::string_view vtkDataClassName)
        : Inport(std::string{identifier}), vtkDataClassName_{vtkDataClassName} {}

    virtual bool canConnectTo(const Port* port) const override {

        if (auto base = static_cast<const VtkOutport*>(port)) {
            auto out = vtkDataObjectTypes::GetTypeIdFromClassName(base->getTypeName().c_str());
            auto in = vtkDataObjectTypes::GetTypeIdFromClassName(getTypeName().c_str());
            return TypeIdIsA(out, in);

            // TODO use with vtk 9.0.2
            // return vtkDataObjectTypes::TypeIdIsA(out, in);
        }
        return false;
    }
    virtual size_t getMaxNumberOfConnections() const override { return 1; }

    vtkDataObject* getData() const {
        if (isConnected()) {
            return static_cast<VtkOutport*>(connectedOutports_[0])->getData();
        }
        return nullptr;
    }

    bool hasData() const {
        if (isConnected()) {
            return static_cast<VtkOutport*>(connectedOutports_[0])->hasData();
        }
        return false;
    }

    virtual std::string getClassIdentifier() const override;

    virtual glm::uvec3 getColorCode() const override {
        // Todo use a table here or something...
        auto offset = 5 * vtkDataObjectTypes::GetTypeIdFromClassName(getTypeName().c_str());
        return uvec3{102, 102, 153 + offset};
    }

    virtual Document getInfo() const override {
        Document doc;
        using P = Document::PathComponent;
        using H = utildoc::TableBuilder::Header;
        auto b = doc.append("html").append("body");
        auto p = b.append("p");
        p.append("b", vtkDataClassName_ + " Inport", {{"style", "color:white;"}});
        utildoc::TableBuilder tb(p, P::end());
        tb(H("Identifier"), getIdentifier());
        tb(H("Class"), getClassIdentifier());
        tb(H("Ready"), isReady());
        tb(H("Connected"), isConnected());

        tb(H("Connections"),
           fmt::format("{} ({})", getNumberOfConnections(), getMaxNumberOfConnections()));
        tb(H("Optional"), isOptional());

        if (hasData()) {
            std::stringstream ss;
            getData()->Print(ss);
            addTable(b, ss.str());
        } else {
            b.append("p", "Port has no data");
        }
        return doc;
    }

    const std::string& getTypeName() const { return vtkDataClassName_; }

private:
    std::string vtkDataClassName_;
};

}  // namespace vtk

template <>
struct PortTraits<vtk::VtkOutport> {
    static const std::string& classIdentifier() {
        static std::string id{"org.inviwo.vtk.outport"};
        return id;
    }
};

template <>
struct PortTraits<vtk::VtkInport> {
    static const std::string& classIdentifier() {
        static std::string id{"org.inviwo.vtk.inport"};
        return id;
    }
};

inline std::string vtk::VtkOutport::getClassIdentifier() const {
    return PortTraits<vtk::VtkOutport>::classIdentifier();
}

inline std::string vtk::VtkInport::getClassIdentifier() const {
    return PortTraits<vtk::VtkInport>::classIdentifier();
}

namespace ttkwrapper {

struct InputData {
    std::string identifier;
    std::string dataType;
    int numComp;
};

struct OutputData {
    std::string identifier;
    std::string displayName;
    int index;
};

struct Group {
    std::string displayName;
    std::vector<std::string> members;
};

template <typename TTKFilter>
struct TTKTraits;

struct FieldSelection {};

/**
 * @brief A Wrapper for all TTK filters
 */
template <typename TTKFilter>
class TTKGenericProcessor : public Processor {
public:
    TTKGenericProcessor() : Processor() {
        const auto nInputs = filter_->GetNumberOfInputPorts();
        const auto nOutputs = filter_->GetNumberOfOutputPorts();

        for (int i = 0; i < nInputs; ++i) {
            vtkInformation* info = filter_->GetInputPortInformation(i);

            std::optional<std::string> dataType;
            if (info->Has(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE())) {
                dataType = info->Get(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE());
            }

            if (dataType) {
                auto id = i < TTKTraits<TTKFilter>::inports.size()
                              ? TTKTraits<TTKFilter>::inports[i].identifier
                              : fmt::format("inport{}", i);

                addPort(std::make_unique<vtk::VtkInport>(id, *dataType));
            } else {
                throw Exception(fmt::format("missing inport for {}", getIdentifier()));
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
                }
            } else if (info->Has(vtkDataObject::DATA_TYPE_NAME())) {
                dataType = info->Get(vtkDataObject::DATA_TYPE_NAME());
            }

            if (dataType) {
                auto id = i < TTKTraits<TTKFilter>::outports.size()
                              ? TTKTraits<TTKFilter>::outports[i].identifier
                              : fmt::format("output{}", i);

                addPort(std::make_unique<vtk::VtkOutport>(id, *dataType));
            } else {
                throw Exception(fmt::format("missing outport for {}", getIdentifier()));
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

                        vtkinport->onChange([this, port = vtkinport, w = &wrapper]() {
                            if (port->isReady()) {
                                vtkDataObject* data = port->getData();
                                vtkFieldData* fd =
                                    data->GetAttributesAsFieldData(vtkDataObject::POINT);
                                std::vector<std::string> options;
                                for (int i = 0; i < fd->GetNumberOfArrays(); ++i) {
                                    options.emplace_back(fd->GetArrayName(i));
                                }

                                fd = data->GetAttributesAsFieldData(vtkDataObject::CELL);
                                for (int i = 0; i < fd->GetNumberOfArrays(); ++i) {
                                    options.emplace_back(fd->GetArrayName(i));
                                }
                                w->property.replaceOptions(std::move(options));
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

        //vtkNew<ttkMorseSmaleComplex> filter_;

        const auto nInputs = filter_->GetNumberOfInputPorts();
        const auto nOutputs = filter_->GetNumberOfOutputPorts();

        for (int i = 0; i < nInputs; ++i) {
            auto data = static_cast<vtk::VtkInport*>(getInports()[i])->getData();
            filter_->SetInputDataObject(i, data);
        }

        vtkTypeBool error = filter_->GetExecutive()->Update();

        if(error != 1) {
            LogError("process error");
        }

        // vtkGlobalWarning display off

        for (int i = 0; i < nOutputs; ++i) {
            if (auto result = filter_->GetOutputDataObject(i)) {
                static_cast<vtk::VtkOutport*>(getOutports()[i])->setData(result);
            } else {
                static_cast<vtk::VtkOutport*>(getOutports()[i])->setData(nullptr);
            }
        }
    }

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    vtkNew<TTKFilter> filter_;
    TTKTraits<TTKFilter> traits_;
};

}  // namespace ttkwrapper

template <typename TTKFilter>
struct ProcessorTraits<ttkwrapper::TTKGenericProcessor<TTKFilter>> {
    static ProcessorInfo getProcessorInfo() {
        return ProcessorInfo{
            fmt::format("org.inviwo.tkk.{}",
                        ttkwrapper::TTKTraits<TTKFilter>::identifier),   // Class identifier
            std::string{ttkwrapper::TTKTraits<TTKFilter>::displayName},  // Display name
            "Topology",                                                  // Category
            CodeState::Experimental,                                     // Code state
            Tag{"TTK"}                                                   // Tags
        };
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