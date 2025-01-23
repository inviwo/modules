/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2015-2025 Inviwo Foundation
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

#include <inviwo/tensorvisbase/properties/eigenvalueproperty.h>
#include <inviwo/tensorvisbase/util/tensorutil.h>
#include <inviwo/tensorvisbase/properties/eigenvalueproperty.h>

namespace inviwo {

std::string_view EigenValueProperty::getClassIdentifier() const { return classIdentifier; }

EigenValueProperty::EigenValueProperty(std::string identifier, std::string displayName)
    : CompositeProperty(identifier, displayName)
    , lambda1_("lambda1", tensorutil::lamda1_str, 3.0, -100.0, 100.0, 0.1)
    , lambda2_("lambda2", tensorutil::lamda2_str, 2.0, -100.0, 100.0, 0.1)
    , lambda3_("lambda3", tensorutil::lamda3_str, 1.0, -100.0, 100.0, 0.1) {
    addProperty(lambda1_);
    addProperty(lambda2_);
    addProperty(lambda3_);
}

EigenValueProperty::EigenValueProperty(const EigenValueProperty& rhs)
    : CompositeProperty(rhs)
    , lambda1_(rhs.lambda1_)
    , lambda2_(rhs.lambda2_)
    , lambda3_(rhs.lambda3_) {
    util::for_each_in_tuple([&](auto& e) { this->addProperty(e); }, props());
}

EigenValueProperty* EigenValueProperty::clone() const { return new EigenValueProperty(*this); }

EigenValueProperty::~EigenValueProperty() = default;

void EigenValueProperty::set(const double lambda1, const double lambda2, const double lambda3) {
    lambda1_.set(lambda1);
    lambda2_.set(lambda2);
    lambda3_.set(lambda3);
}

void EigenValueProperty::set(const double lambda, const glm::uint8 index) {
    DoubleProperty* lambdas[3] = {&lambda1_, &lambda2_, &lambda3_};
    lambdas[index]->set(lambda);
}

}  // namespace inviwo