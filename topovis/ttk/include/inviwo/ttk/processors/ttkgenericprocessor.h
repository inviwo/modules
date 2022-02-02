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
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/ports/datainport.h>
#include <inviwo/core/ports/dataoutport.h>
#include <inviwo/core/util/foreacharg.h>

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

#include <ttkAlgorithm.h>

// debug
#include <ttkMorseSmaleComplex.h>

#include <warn/pop>

namespace inviwo {

namespace vtk {

class VtkInportBase {
public:
    virtual vtkDataObject* getData() const = 0;
};

class VtkOutportBase {
public:
    virtual const char* getTypeName() const = 0;
    virtual vtkDataObject* getData() const = 0;
    virtual void setData(vtkDataObject* data) = 0;
};

template <typename T>
class VtkInport : public Inport, public VtkInportBase {
public:
    VtkInport(std::string_view identifier) : Inport(std::string{identifier}) {
        static_assert(std::is_base_of_v<vtkDataObject, T>);
    }

    virtual bool canConnectTo(const Port* port) const override {
        if (auto base = dynamic_cast<const VtkOutportBase*>(port)) {
            static vtkNew<T> obj;
            if (obj->IsA(base->getTypeName()) == 1) return true;
        }
        return false;
    }
    virtual size_t getMaxNumberOfConnections() const override { return 1; }

    virtual vtkDataObject* getData() const override {
        if (isConnected()) {
            return dynamic_cast<VtkOutportBase*>(connectedOutports_[0])->getData();
        }
        return nullptr;
    }

    virtual std::string getClassIdentifier() const override;

    virtual glm::uvec3 getColorCode() const override { return uvec3{102, 102, 153}; }

    virtual Document getInfo() const override {
        Document doc;
        doc.append("vtkInport");
        return doc;
    }
};

template <typename T>
class VtkOutport : public Outport, public VtkOutportBase {
public:
    VtkOutport(std::string_view identifier) : Outport(std::string{identifier}) {
        static_assert(std::is_base_of_v<vtkDataObject, T>);

        isReady_.setUpdate([this]() {
            return invalidationLevel_ == InvalidationLevel::Valid && data_ != nullptr;
        });
    }

    static const char* typeName() {
        static vtkNew<T> obj;
        return obj->GetClassName();
    }

    virtual const char* getTypeName() const override { return typeName(); }

    virtual T* getData() const override { return data_; }

    void setData(vtkDataObject* data) {
        data_ = T::SafeDownCast(data);
        isReady_.update();
    }

    virtual bool hasData() const override { return data_ != nullptr; }

    virtual void clear() override {
        data_ = nullptr;
        isReady_.update();
    }

    virtual std::string getClassIdentifier() const override;

    virtual glm::uvec3 getColorCode() const override { return uvec3{102, 102, 153}; };
    virtual Document getInfo() const override {
        Document doc;
        doc.append(getTypeName());
        return doc;
    }

private:
    T* data_;
};

}  // namespace vtk

template <typename T>
struct PortTraits<vtk::VtkInport<T>> {
    static const std::string& classIdentifier() {
        static std::string id{
            fmt::format("org.inviwo.vtk.{}.inport", vtk::VtkOutport<T>::typeName())};
        return id;
    }
};
template <typename T>
struct PortTraits<vtk::VtkOutport<T>> {
    static const std::string& classIdentifier() {
        static std::string id{
            fmt::format("org.inviwo.vtk.{}.outport", vtk::VtkOutport<T>::typeName())};
        return id;
    }
};

template <typename T>
inline std::string vtk::VtkOutport<T>::getClassIdentifier() const {
    return PortTraits<vtk::VtkOutport<T>>::classIdentifier();
}

template <typename T>
inline std::string vtk::VtkInport<T>::getClassIdentifier() const {
    return PortTraits<vtk::VtkInport<T>>::classIdentifier();
}

namespace ttkwrapper {

template <typename TTKFilter>
struct TTKTraits;

/**
 * @brief A Wrapper for all TTK filters
 */
template <typename TTKFilter>
class TTKGenericProcessor : public Processor {
public:
    TTKGenericProcessor() : Processor() {
        util::for_each_in_tuple([this](auto& wrapper) { addProperty(wrapper.property); },
                                traits_.properties);

        const auto nInputs = filter_->GetNumberOfInputPorts();
        const auto nOutputs = filter_->GetNumberOfOutputPorts();

        for (int i = 0; i < nInputs; ++i) {
            vtkInformation* info = filter_->GetInputPortInformation(i);

            std::optional<std::string> dataType;
            if (info->Has(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE())) {
                dataType = info->Get(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE());
            }

            if (dataType) {
                addPort(std::make_unique<vtk::VtkInport<vtkDataObject>>(fmt::format("input{}", i)));
            } else {
                LogInfo("missing inport for " << (dataType ? dataType.value() : "?"));
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
                addPort(
                    std::make_unique<vtk::VtkOutport<vtkDataObject>>(fmt::format("output{}", i)));
            } else {
                LogInfo("missing outport for " << (dataType ? dataType.value() : "?"));
            }
        }
    }
    virtual ~TTKGenericProcessor() = default;

    virtual void process() override {
        util::for_each_in_tuple([this](auto& wrapper) { wrapper.set(*filter_); },
                                traits_.properties);

        // vtkNew<ttkMorseSmaleComplex> filter_;

        const auto nInputs = filter_->GetNumberOfInputPorts();
        const auto nOutputs = filter_->GetNumberOfOutputPorts();

        for (int i = 0; i < nInputs; ++i) {
            auto data = dynamic_cast<vtk::VtkInportBase*>(getInports()[i])->getData();
            filter_->SetInputDataObject(i, data);
        }

        // filter_->SetInputArrayToProcess(idx, port, con, cells, d->GetArrayName(0))

        filter_->Update();
        filter_->GetErrorCode();

        for (int i = 0; i < nOutputs; ++i) {
            // vtkSmartPointer<vtkDataSet>::Take()
            if (auto result = filter_->GetOutputDataObject(i)) {

                // auto copy = vtkSmartPointer<vtkDataObject>::New();
                // copy->ShallowCopy(result);

                // auto* d = thing->GetAttributesAsFieldData(0);
                // d->GetNumberOfArrays()
                // d->GetArrayName(0)

                dynamic_cast<vtk::VtkOutportBase*>(getOutports()[i])->setData(result);
            } else {
                dynamic_cast<vtk::VtkOutportBase*>(getOutports()[i])->setData(nullptr);
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