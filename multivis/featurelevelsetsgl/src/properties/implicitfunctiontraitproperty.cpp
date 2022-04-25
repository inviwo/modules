#include <inviwo/featurelevelsetsgl/properties/implicitfunctiontraitproperty.h>
#include <inviwo/core/util/stringconversion.h>

namespace inviwo {
const std::string ImplicitFunctionTraitProperty::classIdentifier =
    "org.inviwo.ImplicitFunctionTraitProperty";
std::string ImplicitFunctionTraitProperty::getClassIdentifier() const { return classIdentifier; }

void ImplicitFunctionTraitProperty::addAttribute(const std::string&, std::shared_ptr<const Volume>,
                                                 bool) {}

void ImplicitFunctionTraitProperty::inject(Shader& shader) {
    const auto identifier = getIdentifier();
    const auto itIdentifier = std::find_if(identifier.rbegin(), identifier.rend(),
                                           [](char c) { return !std::isdigit(c); });
    std::string number(itIdentifier.base(), identifier.end());
    if (number.empty()) number = "1";

    if (originalShader_.empty()) {
        originalShader_ = shader.getComputeShaderObject()->getResource()->source();
    }

    auto content = originalShader_;

    replaceInString(content, "// #define ENABLE_IMPLICIT_FUNCTION_" + number,
                    "#define ENABLE_IMPLICIT_FUNCTION_" + number);

    auto functionCode = shaderInjection_.get();
    replaceInString(functionCode, "\n", "\n    ");

    replaceInString(content, "// #IMPLICIT_FUNCTION_" + number, functionCode);

    auto shaderResource =
        std::make_shared<StringShaderResource>("featurelevelsets" + number + ".comp", content);

    Shader newShader{{{
                         ShaderType::Compute,
                         shaderResource,
                     }},
                     Shader::Build::No};
    newShader.getComputeShaderObject()->setShaderDefines(
        shader.getComputeShaderObject()->getShaderDefines());

    shader = newShader;

    shader.build();
}

}  // namespace inviwo
