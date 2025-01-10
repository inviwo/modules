/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2025 Inviwo Foundation
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

#include <inviwo/tensorvisbase/processors/tensorfieldgenerator.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorFieldGenerator::processorInfo_{
    "org.inviwo.TensorFieldGenerator",  // Class identifier
    "Tensor Field Generator",           // Display name
    "Tensor Field IO",                  // Category
    CodeState::Experimental,            // Code state
    Tags::CPU,                          // Tags
};

const ProcessorInfo& TensorFieldGenerator::getProcessorInfo() const { return processorInfo_; }

TensorFieldGenerator::TensorFieldGenerator()
    : Processor()
    , singularityPresets_("singularityPresets", "Singularity presets",
                          {{"trisector", "Trisector", Singularities::Trisector},
                           {"wedgePoint", "Wedge point", Singularities::WedgePoint},
                           {"collection", "Collection", Singularities::Collection},
                           {"custom", "Custom", Singularities::Custom}})
    , tensors_("tensors", "Tensors")
    , T12D_("T12D", "Tensor 1")
    , T22D_("T22D", "Tensor 2")
    , T32D_("T32D", "Tensor 3")
    , T42D_("T42D", "Tensor 4")
    , collectionField_("collectionField_", "collectionField_")
    , collectionRow1_("collectionRow1_", "Row1")
    , collectionRow2_("collectionRow2_", "Row2")
    , collectionRow3_("collectionRow3_", "Row3")
    , collectionRow4_("collectionRow4_", "Row4")
    , collectionRow5_("collectionRow5_", "Row5")
    , collectionRow1T1_("collectionRow1T1_", "T1", dmat2(dvec2(15., 6.4), dvec2(6.4, 14.)), min,
                        max, inc)
    , collectionRow1T2_("collectionRow1T2_", "T2", dmat2(dvec2(7.5, 3.), dvec2(3., 21.)), min, max,
                        inc)
    , collectionRow1T3_("collectionRow1T3_", "T3", dmat2(dvec2(6.6, 0.3), dvec2(0.4, 22)), min, max,
                        inc)
    , collectionRow1T4_("collectionRow1T4_", "T4", dmat2(dvec2(5.1, 0.4), dvec2(0.2, 23)), min, max,
                        inc)
    , collectionRow1T5_("collectionRow1T5_", "T5", dmat2(dvec2(14, -10.2), dvec2(-10.4, 14)), min,
                        max, inc)

    , collectionRow2T1_("collectionRow2T1_", "T1", dmat2(dvec2(23.5, -2.), dvec2(-2., 5.)), min,
                        max, inc)
    , collectionRow2T2_("collectionRow2T2_", "T2", dmat2(dvec2(14, -6.4), dvec2(-6.3, 14)), min,
                        max, inc)
    , collectionRow2T3_("collectionRow2T3_", "T3", dmat2(dvec2(7.3, 0.4), dvec2(0.2, 21)), min, max,
                        inc)
    , collectionRow2T4_("collectionRow2T4_", "T4", dmat2(dvec2(14, 4), dvec2(4, 14)), min, max, inc)
    , collectionRow2T5_("collectionRow2T5_", "T5", dmat2(dvec2(23.5, -2), dvec2(-2, 5)), min, max,
                        inc)

    , collectionRow3T1_("collectionRow3T1_", "T1", dmat2(dvec2(18.1, 3), dvec2(3, 10)), min, max,
                        inc)
    , collectionRow3T2_("collectionRow3T2_", "T2", dmat2(dvec2(18.9, 02), dvec2(0.2, 9)), min, max,
                        inc)
    , collectionRow3T3_("collectionRow3T3_", "T3", dmat2(dvec2(14, -6.3), dvec2(-6.3, 14)), min,
                        max, inc)
    , collectionRow3T4_("collectionRow3T4_", "T4", dmat2(dvec2(21.3, 3), dvec2(3, 7)), min, max,
                        inc)
    , collectionRow3T5_("collectionRow3T5_", "T5", dmat2(dvec2(22.1, 3), dvec2(3, 6)), min, max,
                        inc)

    , collectionRow4T1_("collectionRow4T1_", "T1", dmat2(dvec2(17, -0.2), dvec2(-0.2, 11)), min,
                        max, inc)
    , collectionRow4T2_("collectionRow4T2_", "T2", dmat2(dvec2(14, 4.2), dvec2(4.2, 14)), min, max,
                        inc)
    , collectionRow4T3_("collectionRow4T3_", "T3", dmat2(dvec2(9, 3), dvec2(3, 19)), min, max, inc)
    , collectionRow4T4_("collectionRow4T4_", "T4", dmat2(dvec2(14, 8), dvec2(8, 6)), min, max, inc)
    , collectionRow4T5_("collectionRow4T5_", "T5", dmat2(dvec2(21.2, 3), dvec2(3, 7)), min, max,
                        inc)

    , collectionRow5T1_("collectionRow5T1_", "T1", dmat2(dvec2(14, -1.1), dvec2(-1.1, 14)), min,
                        max, inc)
    , collectionRow5T2_("collectionRow5T2_", "T2", dmat2(dvec2(11.5, 0.1), dvec2(0.1, 17)), min,
                        max, inc)
    , collectionRow5T3_("collectionRow5T3_", "T3", dmat2(dvec2(10, 0.1), dvec2(0.1, 18)), min, max,
                        inc)
    , collectionRow5T4_("collectionRow5T4_", "T4", dmat2(dvec2(9.3, 0.1), dvec2(0.1, 19)), min, max,
                        inc)
    , collectionRow5T5_("collectionRow5T5_", "T5", dmat2(dvec2(14, 6.4), dvec2(6.4, 14)), min, max,
                        inc)
    , outport2D_("outport2d") {

    tensors_.addProperties(T12D_, T22D_, T32D_, T42D_);

    collectionRow1_.addProperties(collectionRow1T1_, collectionRow1T2_, collectionRow1T3_,
                                  collectionRow1T4_, collectionRow1T5_);
    collectionRow2_.addProperties(collectionRow2T1_, collectionRow2T2_, collectionRow2T3_,
                                  collectionRow2T4_, collectionRow2T5_);
    collectionRow3_.addProperties(collectionRow3T1_, collectionRow3T2_, collectionRow3T3_,
                                  collectionRow3T4_, collectionRow3T5_);
    collectionRow4_.addProperties(collectionRow4T1_, collectionRow4T2_, collectionRow4T3_,
                                  collectionRow4T4_, collectionRow4T5_);
    collectionRow5_.addProperties(collectionRow5T1_, collectionRow5T2_, collectionRow5T3_,
                                  collectionRow5T4_, collectionRow5T5_);

    collectionField_.addProperties(collectionRow1_, collectionRow2_, collectionRow3_,
                                   collectionRow4_, collectionRow5_);

    addProperties(singularityPresets_, tensors_, collectionField_);

    addPort(outport2D_);
}

void TensorFieldGenerator::process() { generate2DField(); }

void TensorFieldGenerator::generate2DField() {
    std::vector<dmat2> rawData;
    size2_t dimensions{0};

    switch (singularityPresets_.get()) {
        case Singularities::Trisector:
            tensors_.setVisible(true);
            collectionField_.setVisible(false);

            dimensions = size2_t(2, 2);

            rawData.push_back(T1);
            rawData.push_back(T2);

            rawData.push_back(T4);
            rawData.push_back(T3);

            T12D_.set(T1);
            T22D_.set(T2);

            T32D_.set(T4);
            T42D_.set(T3);

            break;
        case Singularities::WedgePoint:
            tensors_.setVisible(true);
            collectionField_.setVisible(false);

            dimensions = size2_t(2, 2);

            rawData.push_back(T1);
            rawData.push_back(T4);

            rawData.push_back(T2);
            rawData.push_back(T3);

            T12D_.set(T1);
            T22D_.set(T4);

            T42D_.set(T2);
            T32D_.set(T3);

            break;
        case Singularities::Collection:
            tensors_.setVisible(false);
            collectionField_.setVisible(true);

            dimensions = size2_t(5, 5);

            // ROW 1
            rawData.push_back(collectionRow1T1_.get());
            rawData.push_back(collectionRow1T2_.get());
            rawData.push_back(collectionRow1T3_.get());
            rawData.push_back(collectionRow1T4_.get());
            rawData.push_back(collectionRow1T5_.get());

            // ROW 2
            rawData.push_back(collectionRow2T1_.get());
            rawData.push_back(collectionRow2T2_.get());
            rawData.push_back(collectionRow2T3_.get());
            rawData.push_back(collectionRow2T4_.get());
            rawData.push_back(collectionRow2T5_.get());

            // ROW 3
            rawData.push_back(collectionRow3T1_.get());
            rawData.push_back(collectionRow3T2_.get());
            rawData.push_back(collectionRow3T3_.get());
            rawData.push_back(collectionRow3T4_.get());
            rawData.push_back(collectionRow3T5_.get());

            // ROW 4
            rawData.push_back(collectionRow4T1_.get());
            rawData.push_back(collectionRow4T2_.get());
            rawData.push_back(collectionRow4T3_.get());
            rawData.push_back(collectionRow4T4_.get());
            rawData.push_back(collectionRow4T5_.get());

            // ROW 5
            rawData.push_back(collectionRow5T1_.get());
            rawData.push_back(collectionRow5T2_.get());
            rawData.push_back(collectionRow5T3_.get());
            rawData.push_back(collectionRow5T4_.get());
            rawData.push_back(collectionRow5T5_.get());

            break;
        case Singularities::Custom:
            tensors_.setVisible(true);
            collectionField_.setVisible(false);

            dimensions = size2_t(2, 2);

            rawData.push_back(T12D_);
            rawData.push_back(T22D_);

            rawData.push_back(T32D_);
            rawData.push_back(T42D_);
            break;
        default:
            LogError(
                "In function generateSelectedSingularity the default case in switch statement was "
                "reached. This should not happen!");
            break;
    }

    outport2D_.setData(std::make_shared<TensorField2D>(dimensions, rawData));
}

}  // namespace inviwo
