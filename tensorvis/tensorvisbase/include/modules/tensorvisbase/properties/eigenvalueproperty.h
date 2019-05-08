/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2015-2018 Inviwo Foundation
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

#ifndef IVW_EIGENVALUEPROPERTY_H
#define IVW_EIGENVALUEPROPERTY_H

#include <inviwo/core/common/inviwo.h>
#include <modules/tensorvisbase/tensorvisbasemoduledefine.h>
#include <inviwo/core/properties/compositeproperty.h>
#include <inviwo/core/properties/ordinalproperty.h>

namespace inviwo {

/**
 * \class EigenValueProperty
 * \brief VERY_BRIEFLY_DESCRIBE_THE_CLASS
 * DESCRIBE_THE_CLASS
 */
class IVW_MODULE_TENSORVISBASE_API EigenValueProperty : public CompositeProperty {
public:
    virtual std::string getClassIdentifier() const override;
    static const std::string classIdentifier;

    EigenValueProperty(std::string identifier = std::string("eigenValueProperty"),
                       std::string displayName = std::string("Eigenvalues"));
    EigenValueProperty(const EigenValueProperty& rhs);
    EigenValueProperty& operator=(const EigenValueProperty& that);
    virtual EigenValueProperty* clone() const override;
    virtual ~EigenValueProperty();

    double lambda1() const { return lambda1_.get(); }
    double lambda2() const { return lambda2_.get(); }
    double lambda3() const { return lambda3_.get(); }

    void set(double lambda1, double lambda2, double lambda3);
    void set(double lambda, glm::uint8 index);

    std::array<double, 3> get() const { return {{lambda1_.get(), lambda2_.get(), lambda3_.get()}}; }

private:
    DoubleProperty lambda1_;
    DoubleProperty lambda2_;
    DoubleProperty lambda3_;
};

}  // namespace inviwo

#endif  // IVW_EIGENVALUEPROPERTY_H
