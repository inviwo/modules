/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2020 Inviwo Foundation
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

#ifndef IVW_TENSORFIELDGENERATOR_H
#define IVW_TENSORFIELDGENERATOR_H

#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/ports/imageport.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/tensorvisbase/tensorvisbasemoduledefine.h>
#include <inviwo/tensorvisbase/ports/tensorfieldport.h>

namespace inviwo {

/** \docpage{org.inviwo.TensorFieldSource, TensorFieldSource}
 * ![](org.inviwo.<name>.png?classIdentifier=org.inviwo.TensorFieldSource)
 * Explanation of how to use the processor.
 *
 * ### Inports
 *   * __<Inport1>__ <description>.
 *
 * ### Outports
 *   * __<Outport1>__ <description>.
 *
 * ### Properties
 *   * __<Prop1>__ <description>.
 *   * __<Prop2>__ <description>
 */

/**
 * \class TensorFieldSource
 * \brief VERY_BRIEFLY_DESCRIBE_THE_CLASS
 * DESCRIBE_THE_CLASS_FROM_A_DEVELOPER_PERSPECTIVE
 */
class IVW_MODULE_TENSORVISBASE_API TensorFieldGenerator : public Processor {
public:
    TensorFieldGenerator();
    virtual ~TensorFieldGenerator() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    const dmat2 T1 = dmat2(dvec2(6, 0), dvec2(0, 2));
    const dmat2 T2 = dmat2(dvec2(4, -2), dvec2(-2, 4));
    const dmat2 T3 = dmat2(dvec2(2, 0), dvec2(0, 6));
    const dmat2 T4 = dmat2(dvec2(4, 2), dvec2(2, 4));

    const dmat2 min = dmat2(-100.0, -100.0, -100.0, -100.0);
    const dmat2 max = dmat2(100.0, 100.0, 100.0, 100.0);
    const dmat2 inc = dmat2(0.0001, 0.0001, 0.0001, 0.0001);

    OptionPropertyInt singularityPresets_;

    CompositeProperty tensors_;

    DoubleMat2Property T12D_;
    DoubleMat2Property T22D_;
    DoubleMat2Property T32D_;
    DoubleMat2Property T42D_;

    CompositeProperty collectionField_;
    CompositeProperty collectionRow1_;
    CompositeProperty collectionRow2_;
    CompositeProperty collectionRow3_;
    CompositeProperty collectionRow4_;
    CompositeProperty collectionRow5_;

    DoubleMat2Property collectionRow1T1_;
    DoubleMat2Property collectionRow1T2_;
    DoubleMat2Property collectionRow1T3_;
    DoubleMat2Property collectionRow1T4_;
    DoubleMat2Property collectionRow1T5_;

    DoubleMat2Property collectionRow2T1_;
    DoubleMat2Property collectionRow2T2_;
    DoubleMat2Property collectionRow2T3_;
    DoubleMat2Property collectionRow2T4_;
    DoubleMat2Property collectionRow2T5_;

    DoubleMat2Property collectionRow3T1_;
    DoubleMat2Property collectionRow3T2_;
    DoubleMat2Property collectionRow3T3_;
    DoubleMat2Property collectionRow3T4_;
    DoubleMat2Property collectionRow3T5_;

    DoubleMat2Property collectionRow4T1_;
    DoubleMat2Property collectionRow4T2_;
    DoubleMat2Property collectionRow4T3_;
    DoubleMat2Property collectionRow4T4_;
    DoubleMat2Property collectionRow4T5_;

    DoubleMat2Property collectionRow5T1_;
    DoubleMat2Property collectionRow5T2_;
    DoubleMat2Property collectionRow5T3_;
    DoubleMat2Property collectionRow5T4_;
    DoubleMat2Property collectionRow5T5_;

    TensorField2DOutport outport2D_;

    void generate2DField();

    enum Singularities { Trisector, WedgePoint, Collection, Custom };
};

}  // namespace inviwo

#endif  // IVW_TENSORFIELDGENERATOR_H
