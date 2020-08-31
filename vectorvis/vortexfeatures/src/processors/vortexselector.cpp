/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020 Inviwo Foundation
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

#include <inviwo/vortexfeatures/processors/vortexselector.h>
#include <inviwo/core/util/exception.h>
#include <inviwo/core/util/stdextensions.h>
#include <inviwo/core/util/zip.h>
#include <inviwo/core/datastructures/buffer/buffer.h>
#include <inviwo/core/datastructures/buffer/bufferram.h>

#include <inviwo/dataframe/datastructures/column.h>
#include <inviwo/dataframe/util/dataframeutil.h>

#include <algorithm>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VortexSelector::processorInfo_{
    "org.inviwo.VortexSelector",  // Class identifier
    "Vortex Selector",            // Display name
    "DataFrame",                  // Category
    CodeState::Experimental,      // Code state
    "DataFrame, Vortices",        // Tags
};
const ProcessorInfo VortexSelector::getProcessorInfo() const { return processorInfo_; }

namespace detail {

// TODO: taken from dataframeutil.h
void addColumns(std::shared_ptr<DataFrame> dst, const DataFrame& srcDataFrame,
                const std::vector<size_t>& rows, const std::vector<std::string>& keyColumns,
                bool skipKeyCol) {
    for (auto srcCol : srcDataFrame) {
        if (srcCol == srcDataFrame.getIndexColumn()) continue;
        if (skipKeyCol && util::contains(keyColumns, srcCol->getHeader())) {
            continue;
        }

        if (auto c = dynamic_cast<CategoricalColumn*>(srcCol.get())) {
            auto data = util::transform(rows, [src = c->getValues()](size_t i) { return src[i]; });
            dst->addCategoricalColumn(c->getHeader(), data);
        } else {
            srcCol->getBuffer()->getRepresentation<BufferRAM>()->dispatch<void>(
                [dst, srcCol, header = srcCol->getHeader(), rows](auto typedBuf) {
                    auto dstData = util::transform(
                        rows, [&src = typedBuf->getDataContainer()](size_t i) { return src[i]; });
                    dst->addColumn(header, std::move(dstData));
                });
        }
    }
}

/**
 * \brief for each row in leftCol return a list of matching row indices in rightCol
 */
template <bool firstMatchOnly = false>
std::vector<std::vector<size_t>> getMatchingRows(std::shared_ptr<const Column> leftCol,
                                                 std::shared_ptr<const Column> rightCol) {
    std::vector<std::vector<size_t>> rows(leftCol->getSize());

    if (auto catCol1 = dynamic_cast<const CategoricalColumn*>(leftCol.get())) {
        // need to match values of categorical columns instead of indices stored in buffer
        auto catCol2 = dynamic_cast<const CategoricalColumn*>(rightCol.get());
        IVW_ASSERT(catCol2, "right column is not categorical");

        auto valuesRight = catCol2->getValues();
        for (auto&& [i, key] : util::enumerate(catCol1->getValues())) {
            // find all matching rows in right column
            std::vector<size_t> matches;
            for (auto&& [r, value] : util::enumerate(valuesRight)) {
                if (key == value) {
                    matches.emplace_back(r);
                    if constexpr (firstMatchOnly) break;
                }
            }
            rows[i] = std::move(matches);
        }
    } else {
        leftCol->getBuffer()->getRepresentation<BufferRAM>()->dispatch<void>(
            [rightBuffer = rightCol->getBuffer(), &rows](auto typedBuf) {
                using ValueType = util::PrecisionValueType<decltype(typedBuf)>;

                const auto& left = typedBuf->getDataContainer();
                const auto& right = static_cast<const BufferRAMPrecision<ValueType>*>(
                                        rightBuffer->getRepresentation<BufferRAM>())
                                        ->getDataContainer();

                for (auto&& [i, key] : util::enumerate(left)) {
                    // find all matching rows in right
                    std::vector<size_t> matches;
                    for (auto&& [r, value] : util::enumerate(right)) {
                        if (key == value) {
                            matches.emplace_back(r);
                            if constexpr (firstMatchOnly) break;
                        }
                    }
                    rows[i] = std::move(matches);
                }
            });
    }
    return rows;
}
// -------------------

//
// TODO: move to dataframe/util/dataframeutil.h
//
std::shared_ptr<DataFrame> applySelection(const DataFrame& src, const std::vector<size_t>& rows) {
    auto dataframe = std::make_shared<DataFrame>();
    addColumns(dataframe, src, rows, {}, false);
    dataframe->updateIndexBuffer();
    return dataframe;
}

//
// TODO: move to dataframe/util/dataframeutil.h
//
// TODO: filter categorical columns
//
#include <warn/push>
#include <warn/ignore/conversion>
template <typename T>
std::vector<size_t> filterRows(std::shared_ptr<const Column> col,
                               VortexSelector::FilterPredicate predicate, T predValue) {

    if (auto catCol = dynamic_cast<const CategoricalColumn*>(col.get())) {
        throw Exception("filtering of categorical column not supported",
                        IVW_CONTEXT_CUSTOM("detail::filterRows"));
    } else {
        return col->getBuffer()->getRepresentation<BufferRAM>()->dispatch<std::vector<size_t>>(
            [predicate, predValue](auto typedBuf) {
                std::vector<size_t> rows;
                using ValueType = util::PrecisionValueType<decltype(typedBuf)>;

                auto comp = [predicate, p = static_cast<ValueType>(
                                            predValue)]() -> std::function<bool(ValueType)> {
                    switch (predicate) {
                        case VortexSelector::FilterPredicate::NotEqual:
                            return [p](ValueType v) { return p != v; };
                        case VortexSelector::FilterPredicate::Less:
                            return [p](ValueType v) { return glm::all(glm::lessThan(v, p)); };
                        case VortexSelector::FilterPredicate::Greater:
                            return [p](ValueType v) { return glm::all(glm::greaterThan(v, p)); };
                        case VortexSelector::FilterPredicate::Equal:
                        default:
                            return [p](ValueType v) { return p == v; };
                    }
                }();

                const auto& left = typedBuf->getDataContainer();
                for (auto&& [i, value] : util::enumerate(left)) {
                    // find all rows fulfilling the predicate
                    if (comp(value)) {
                        rows.push_back(i);
                    }
                }
                return rows;
            });
    }
}
#include <warn/pop>

}  // namespace detail

VortexSelector::VortexSelector()
    : Processor()
    , inportEnsemble_("ensemble")
    , inportVortices_("vortices")
    , brushingPort_("brushing")
    , outport_("outport")
    , key_("key", "Key Column", inportEnsemble_, false)
    , keyEnsemble_("keyEnsemble", "Ensemble ID Column", inportEnsemble_, true, 0)
    , ensembleId_("ensembleId", "Ensemble ID", 0, 0, 100)
    , dataFilter_("dataFilter", "Data Filter", false)
    , filterKey_("filterKey", "Filter Column", inportVortices_, false)
    , filterAction_(
          "filterAction", "Filter Action",
          {{"drop", "Drop", FilterAction::Drop}, {"zeroout", "Zero Out", FilterAction::ZeroOut}})
    , predicate_("filterPredicate", "Predicate",
                 {{"equal", "== (equal)", FilterPredicate::Equal},
                  {"less", "< (less)", FilterPredicate::Less},
                  {"greater", "> (greater)", FilterPredicate::Greater},
                  {"notequal", "!= (not equal)", FilterPredicate::NotEqual}})
    , predicateValue_("predicateValue", "Predicate Value", 0.0f, -100.0f, 100.0f, 0.1f) {

    brushingPort_.setOptional(true);
    addPort(inportEnsemble_);
    addPort(inportVortices_);
    addPort(brushingPort_);
    addPort(outport_);

    dataFilter_.addProperties(filterKey_, predicate_, predicateValue_);

    addProperties(key_, keyEnsemble_, ensembleId_, dataFilter_);
}

void VortexSelector::process() {
    std::vector<size_t> selectedRows(brushingPort_.getSelectedIndices().begin(),
                                     brushingPort_.getSelectedIndices().end());
    std::sort(selectedRows.begin(), selectedRows.end());

    // when dropping rows from second DataFrame (vortices
    auto ensembleDataFrame = detail::applySelection(*inportEnsemble_.getData(), selectedRows);
    ensembleDataFrame = detail::applySelection(
        *ensembleDataFrame, detail::filterRows(ensembleDataFrame->getColumn(keyEnsemble_),
                                               FilterPredicate::Equal, ensembleId_.get()));

    const auto key = key_.getColumnHeader();

    std::vector<size_t> rowsRight;
    for (auto&& [i, rowIndices] : util::enumerate(detail::getMatchingRows<false>(
             ensembleDataFrame->getColumn(key), inportVortices_.getData()->getColumn(key)))) {
        std::copy(rowIndices.begin(), rowIndices.end(), std::back_inserter(rowsRight));
    }

    auto dataframe = detail::applySelection(*inportVortices_.getData(), rowsRight);

    /*
    std::vector<std::string> headers;
    for (auto col : *ensembleDataFrame.get()) {
        if (col == ensembleDataFrame->getIndexColumn()) continue;
        if (col->getHeader() == key_.getColumnHeader()) continue;
        if ((keyEnsemble_.get() > -1) && (col->getHeader() == keyEnsemble_.getColumnHeader()))
            continue;
        headers.push_back(col->getHeader());
    }
    for (auto h : headers) {
        ensembleDataFrame->dropColumn(h);
    }

    auto dataframe = dataframe::innerJoin(*ensembleDataFrame, *inportVortices_.getData(),
                                          {key_.getColumnHeader(), keyEnsemble_.getColumnHeader()});
    */
    if (dataFilter_.isChecked()) {
        dataframe = detail::applySelection(
            *dataframe, detail::filterRows(dataframe->getColumn(filterKey_),
                                           predicate_.getSelectedValue(), predicateValue_.get()));
    }

    outport_.setData(dataframe);
}

}  // namespace inviwo
