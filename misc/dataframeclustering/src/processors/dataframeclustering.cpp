/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020-2024 Inviwo Foundation
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

#include <inviwo/dataframeclustering/processors/dataframeclustering.h>
#include <inviwo/dataframeclustering/dataframeclusteringmodule.h>
#include <modules/python3/pybindutils.h>

#include <inviwo/core/common/inviwoapplication.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming
// scheme
const ProcessorInfo DataFrameClustering::processorInfo_{
    "org.inviwo.DataFrameClustering",  // Class identifier
    "Data Frame Clustering",           // Display name
    "Clustering",                      // Category
    CodeState::Stable,                 // Code state
    Tags::CPU | Tags::PY,              // Tags
};
const ProcessorInfo DataFrameClustering::getProcessorInfo() const { return processorInfo_; }

DataFrameClustering::DataFrameClustering(InviwoApplication* app)
    : Processor()
    , dataFrame_("dataFrame")
    , newDataFrame_("newDataFrame")

    , method_("method", "Method",
              {{"kmeans", "KMeans"},
               {"dbscan", "DBSCAN"},
               {"agglo", "Agglomerative"},
               {"spectral", "Spectral"}})

    , numberOfClusters_("numberOfClusters", "Number Of Clusters", 3,
                        {1, ConstraintBehavior::Immutable}, {10, ConstraintBehavior::Ignore})

    , kmeans_("kmeans_", "K-Means")
    , dbscan_("dbscan_", "DBSCAN")
    , agglomerative_("agglomerative_", "Agglomerative")
    , spectral_("spectral", "Spectral")

    , columnName_("columnName", "Column Name", "Cluster")

    , eps_("eps", "Epsilon", 0.3f, {0.001f, ConstraintBehavior::Ignore},
           {1.f, ConstraintBehavior::Ignore}, 0.001f)
    , N_("N", "Min Points", 10, {1, ConstraintBehavior::Immutable},
         {100, ConstraintBehavior::Ignore})

    , numberOfFoundClusters_("numberOfFoundClusters", "NumberOfFoundClusters", 0,
                             {0, ConstraintBehavior::Immutable}, {1000, ConstraintBehavior::Ignore},
                             1, InvalidationLevel::Valid, PropertySemantics::Text)

    , linkage_("linkage", "Linkage",
               {{"single", "Single"},
                {"complete", "complete"},
                {"average", "average"},
                {"weighted", "weighted"},
                {"centroid", "centroid"},
                {"median", "median"},
                {"ward", "ward"}})

    , columns_("columns", "Columns")
    , script_(app->getModuleByType<DataFrameClusteringModule>()->getPath(ModulePath::Scripts) /
              "dataframeclustering.py") {

    numberOfClusters_.setSerializationMode(PropertySerializationMode::All);

    addPort(dataFrame_);
    addPort(newDataFrame_);
    kmeans_.addProperty(numberOfClusters_);
    dbscan_.addProperties(eps_, N_);
    agglomerative_.addProperties(numberOfClusters_, linkage_);
    spectral_.addProperty(numberOfClusters_);
    addProperties(method_, kmeans_, dbscan_, agglomerative_, spectral_, columnName_, columns_,
                  numberOfFoundClusters_);

    kmeans_.visibilityDependsOn(
        method_, [](const auto& p) { return p.getSelectedIdentifier() == "kmeans"; });
    dbscan_.visibilityDependsOn(
        method_, [](const auto& p) { return p.getSelectedIdentifier() == "dbscan"; });
    agglomerative_.visibilityDependsOn(
        method_, [](const auto& p) { return p.getSelectedIdentifier() == "agglo"; });
    spectral_.visibilityDependsOn(
        method_, [](const auto& p) { return p.getSelectedIdentifier() == "spectral"; });

    method_.set("agglo");

    script_.onChange([&]() { this->invalidate(InvalidationLevel::InvalidOutput); });

    dataFrame_.onChange([&]() { onDataFrameChange(); });
}

void DataFrameClustering::process() {
    pybind11::list cols;
    for (auto& p : columns_.getPropertiesByType<BoolProperty>()) {
        if (p->getVisible() && p->get()) {
            cols.append(p->getDisplayName());
        }
    }

    std::unordered_map<std::string, pybind11::object> vars = {
        {"numClusters", pybind11::cast(numberOfClusters_.get())},
        {"method", pybind11::cast(method_.getSelectedIdentifier())},
        {"linkage", pybind11::cast(linkage_.getSelectedIdentifier())},
        {"N", pybind11::cast(N_.get())},
        {"e", pybind11::cast(eps_.get())},
        {"activeColumnHeaders", cols},
        {"dataframe", pybind11::cast(dataFrame_.getData())}};

    script_.run(vars, [&](pybind11::dict locals) {
        auto newDF = std::make_shared<DataFrame>(*dataFrame_.getData());
        if (locals.contains("labels")) {
            auto pyArr = locals["labels"].cast<pybind11::array>();
            std::shared_ptr<BufferBase> buf = pyutil::createBuffer(pyArr);
            newDF->addColumnFromBuffer(columnName_.get(), buf);
            if (locals.contains("numberOfFoundClusters")) {
                numberOfFoundClusters_.set(locals["numberOfFoundClusters"].cast<int>());
            }
        }
        newDataFrame_.setData(newDF);
    });
}

void DataFrameClustering::onDataFrameChange() {
    if (auto df = dataFrame_.getData()) {
        std::unordered_set<Property*> oldProperties{columns_.getProperties().begin(),
                                                    columns_.getProperties().end()};

        for (const auto& col : *df) {
            auto prop = [header = col->getHeader(), this]() {
                auto id = util::stripIdentifier(toLower(header));
                if (auto p = columns_.getPropertyByIdentifier(id)) {
                    if (auto pp = dynamic_cast<BoolProperty*>(p)) {
                        return pp;
                    }
                    throw inviwo::Exception("Property not a BoolProperty", IVW_CONTEXT);
                } else {
                    auto cp = std::make_unique<BoolProperty>(id, header, columns_.size() < 2);
                    cp->setSerializationMode(PropertySerializationMode::All);
                    auto cpNonPtr = cp.get();
                    columns_.addProperty(cp.release());
                    return cpNonPtr;
                }
            }();
            prop->setSerializationMode(PropertySerializationMode::All);
            prop->setVisible(true);
            oldProperties.erase(prop);
        }

        for (Property* prop : oldProperties) {
            columns_.removeProperty(prop);
        }
    }
}

}  // namespace inviwo
