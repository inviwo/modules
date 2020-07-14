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

#include <inviwo/topologytoolkit/processors/trackingfromfields.h>
#include <inviwo/topologytoolkit/utils/ttkutils.h>
#include <inviwo/core/datastructures/buffer/bufferram.h>
#include <inviwo/core/util/filesystem.h>
#include <inviwo/core/util/formatdispatching.h>

#include <warn/push>
#include <warn/ignore/all>
#include <ttk/core/base/trackingFromFields/TrackingFromFields.h>
#include <ttk/core/base/persistenceDiagram/PersistenceDiagram.h>
#include <warn/pop>

namespace inviwo {

namespace {

struct Track {
    template <typename Result, typename Format>
    Result operator()(pool::Progress progress, ttk::TrackingFromFields &trackingF,
                      ttk::TrackingFromPersistenceDiagrams &tracking, const VolumeSequence &scalars,
                      bool useGeometricSpacing, float spacing, std::string algorithm, float alpha,
                      float tolerance, std::string wasserstein, int threadCount, float PE, float PS,
                      float PX, float PY, float PZ, float mergeSplitThresh,  // Todo: Make Property)
                      bool overwriteTransform) {

        using dataType = Format::type;

        progress(0.01f);

        // Compute triangulation from the first volume
        auto triangulation = std::make_shared<topology::TriangulationData>(
            topology::volumeToTTKTriangulation(*scalars.at(0).get(), static_cast<size_t>(0)));
        // Todo: Expose different channels : channels.get()

        // 0. get data
        trackingF.setThreadNumber(threadCount);
        trackingF.setTriangulation(
            const_cast<ttk::Triangulation *>(&triangulation->getTriangulation()));

        size_t numberOfFields = scalars.size();
        std::vector<void *> inputFields(numberOfFields);

        for (size_t i = 0; i < numberOfFields; ++i) {
            const Volume &volume = *scalars.at(i).get();
            inputFields[i] = const_cast<void *>(volume.getRepresentation<VolumeRAM>()->getData());
        }

        trackingF.setInputScalars(inputFields);

        // 0'. get offsets
        trackingF.setInputOffsets(triangulation->getOffsets().data());

        // 1. get persistence diagrams.
        std::vector<std::vector<diagramTuple>> persistenceDiagrams(numberOfFields,
                                                                   std::vector<diagramTuple>());

        // Last argument is a TTK wrapper that is not used anywhere
        int retVal = trackingF.performDiagramComputation<dataType>((int)numberOfFields,
                                                                   persistenceDiagrams, nullptr);

        // Todo: Add other exceptions
        if (retVal < 0) {
            throw TTKException("Error computing ttk::TrackingFromFields",
                               IVW_CONTEXT_CUSTOM("TopologicalSimplification"));
        }

        progress(0.25f);

        // 2. call feature tracking with threshold.
        std::vector<std::vector<matchingTuple>> outputMatchings(numberOfFields - 1,
                                                                std::vector<matchingTuple>());
        bool is3D = true;
        tracking.setThreadNumber(threadCount);
        tracking.performMatchings<dataType>(
            (int)numberOfFields, persistenceDiagrams, outputMatchings,
            algorithm,  // Not from paraview, from enclosing tracking plugin
            wasserstein, tolerance, is3D,
            alpha,               // Blending
            PX, PY, PZ, PS, PE,  // Coefficients
            nullptr              // Wrapper for accessing threadNumber
        );
        progress(0.5f);

        // (+ vertex id)
        std::vector<trackingTuple> trackingsBase;
        tracking.setThreadNumber(threadCount);
        tracking.performTracking<dataType>(persistenceDiagrams, outputMatchings, trackingsBase);

        std::vector<std::set<int>> trackingTupleToMerged(trackingsBase.size(), std::set<int>());

        tracking.performPostProcess<dataType>(persistenceDiagrams, trackingsBase,
                                              trackingTupleToMerged, mergeSplitThresh);

        progress(0.75f);
        auto mesh = std::make_shared<Mesh>();

        TrackingFromFields::buildMesh<dataType>(trackingsBase, outputMatchings, persistenceDiagrams,
                                                useGeometricSpacing, spacing, true,
                                                trackingTupleToMerged, *mesh.get());

        if (overwriteTransform) {
            // If offset is the same for both first and last volume, volumes are 3D
            vec3 offsetFirst = triangulation->getOffset();
            vec3 offsetLast = scalars.at(scalars.size() - 1)->getOffset();
            if (offsetFirst == offsetLast) {
                mesh->setBasis(triangulation->getBasis());
                // Otherwise volumes are 2D slices in 2D
            } else {
                mat3 newBasis = triangulation->getBasis();
                size3_t dims = scalars.at(0)->getDimensions();
                for (size_t d = 0; d < 3; d++) {
                    if (dims[d] == 1) dims[d] = scalars.size();
                }

                vec3 dimRatio =
                    (static_cast<vec3>(scalars.at(0)->getDimensions()) / static_cast<vec3>(dims));
                newBasis[0] /= dimRatio[0];
                newBasis[1] /= dimRatio[1];
                newBasis[2] /= dimRatio[2];
                mesh->setBasis(newBasis);
            }
            mesh->setOffset(offsetFirst);
            mesh->setWorldMatrix(triangulation->getWorldMatrix());
        }

        // Add meta data based on the first triangulation
        mesh->copyMetaDataFrom(*triangulation.get());

        progress(1.0f);

        return mesh;
    }
};
}  // namespace

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TrackingFromFields::processorInfo_{
    "org.inviwo.TrackingFromFields",  // Class identifier
    "Tracking From Fields",           // Display name
    "Topology",                       // Category
    CodeState::Experimental,          // Code state
    "CPU, TTK, Testing",              // Tags
};
const ProcessorInfo TrackingFromFields::getProcessorInfo() const { return processorInfo_; }

TrackingFromFields::TrackingFromFields()
    : PoolProcessor()
    , scalarsInport_("scalars")
    , outport_("outport")
    , diagrams_("diagrams", "Persistence Diagrams")
    , timeSampling_("timeSampling", "Time Sampling", 1, 1)
    , persistenceThreshold_("persistenceThreshold", "Persistence Threshold", 1, 0, 100)
    , matching_("matching", "Matching")
    , pParameter_("pParameter", "p Parameter (inf, 0, 1..)", "inf")
    , alpha_("alpha", "Alpha", 1, 0, 1)
    , extremumWeight_("extremumWeight", "Extremum Weight", 1, 0, 1)
    , saddleWeight_("saddleWeight", "Saddle Weight", 1, 0, 1)
    , xWeight_("xWeight", "X Weight", 0, 0, 1)
    , yWeight_("yWeight", "Y Weight", 0, 0, 1)
    , zWeight_("zWeight", "Z Weight", 0, 0, 1)
    , output_("output", "Output")
    , forceZTranslation_("forceZTranslation", "Force Z-Translation")
    , zTranslation_("zTranslation", "Z-Translation", 1, 0)
    , overwriteTransform_("overwriteTransform", "Overwrite Transform", false)
    , trackingF_{}
    , tracking_{} {
    addPort(scalarsInport_);
    addPort(outport_);
    addProperties(diagrams_, matching_, output_);
    diagrams_.addProperties(timeSampling_, persistenceThreshold_);
    matching_.addProperties(pParameter_, alpha_, extremumWeight_, saddleWeight_, xWeight_, yWeight_,
                            zWeight_);
    output_.addProperties(forceZTranslation_, zTranslation_, overwriteTransform_);
    zTranslation_.visibilityDependsOn(forceZTranslation_, [](const auto &p) { return p.get(); });
}

void TrackingFromFields::process() {
    auto compute = [this](pool::Stop stop, pool::Progress progress) -> std::shared_ptr<Mesh> {
        const auto scalars = scalarsInport_.getData();

        if (scalars->size() < 2) {
            LogProcessorInfo("Less then 2 volumes given, nothing to track!");
            return nullptr;
        }

        // Todo: Check if all volumes have the same meta data (dimension, transformation, data type
        // of
        // selected channel)

        // Save parameters
        bool useGeometricSpacing = forceZTranslation_.get();
        float spacing = zTranslation_.get();
        std::string algorithm = "ttk";
        //(0: ttk, 1: legacy, 2: geometric, 3:parallel, 4: greedy)
        float alpha = alpha_.get();
        float tolerance = persistenceThreshold_.get();
        // bool is3D = true;  // Somehow always set to true, unclear why;
        std::string wasserstein = pParameter_.get();
        int threadCount = 2;  // Todo: Expose thread count threadCount_.get();
        float PE = extremumWeight_.get();
        float PS = saddleWeight_.get();
        float PX = xWeight_.get();
        float PY = yWeight_.get();
        float PZ = zWeight_.get();
        float mergeSplitThresh = 0;  // Todo: Make Property
        bool overwriteTransform = overwriteTransform_.get();

        // size_t numberOfFields = scalars->size();

        auto mesh = dispatching::dispatch<std::shared_ptr<Mesh>, dispatching::filter::Float1s>(
            (*scalars->at(0)).getDataFormat()->getId(), Track{}, progress, trackingF_, tracking_,
            *scalars.get(), useGeometricSpacing, spacing, algorithm, alpha, tolerance, wasserstein,
            threadCount, PE, PS, PX, PY, PZ, mergeSplitThresh, overwriteTransform);

        return mesh;
    };

    outport_.setData(nullptr);
    dispatchOne(compute, [this](std::shared_ptr<Mesh> result) {
        outport_.setData(result);
        newResults();
    });
}

// Basically a copy of ttkTrackingFromPersistenceDiagrams::buildMesh
template <typename dataType>
void TrackingFromFields::buildMesh(std::vector<trackingTuple> &trackings,
                                   std::vector<std::vector<matchingTuple>> &outputMatchings,
                                   std::vector<std::vector<diagramTuple>> &inputPersistenceDiagrams,
                                   bool useGeometricSpacing, double spacing, bool DoPostProc,
                                   std::vector<std::set<int>> &trackingTupleToMerged,
                                   Mesh &outputMesh) {
    // Containerrs for vertices and lines
    std::vector<vec3> vertices;
    std::vector<uint32_t> indicesLines;

    /*vtkSmartPointer<vtkDoubleArray> persistenceScalars = vtkSmartPointer<vtkDoubleArray>::New();
    vtkSmartPointer<vtkDoubleArray> valueScalars = vtkSmartPointer<vtkDoubleArray>::New();
    vtkSmartPointer<vtkIntArray> matchingIdScalars = vtkSmartPointer<vtkIntArray>::New();
    vtkSmartPointer<vtkIntArray> lengthScalars = vtkSmartPointer<vtkIntArray>::New();
    vtkSmartPointer<vtkIntArray> timeScalars = vtkSmartPointer<vtkIntArray>::New();
    vtkSmartPointer<vtkIntArray> componentIds = vtkSmartPointer<vtkIntArray>::New();
    vtkSmartPointer<vtkIntArray> pointTypeScalars = vtkSmartPointer<vtkIntArray>::New();
    persistenceScalars->SetName("Cost");
    valueScalars->SetName("Scalar");
    matchingIdScalars->SetName("MatchingIdentifier");
    lengthScalars->SetName("ComponentLength");
    timeScalars->SetName("TimeStep");
    componentIds->SetName("ConnectedComponentId");
    pointTypeScalars->SetName("CriticalType");*/

    //};

    int currentMatching = 0;
    for (unsigned int k = 0; k < trackings.size(); ++k) {
        trackingTuple tt = trackings.at((unsigned long)k);

        int numStart = std::get<0>(tt);
        //     int numEnd = std::get<1>(tt);
        std::vector<BIdVertex> chain = std::get<2>(tt);
        int chainLength = chain.size();

        if (chain.size() <= 1) {
            std::cout << "Got an unexpected 0-size chain." << std::endl;
            return;
        }

        // Go through one track
        for (int c = 0; c < (int)chain.size() - 1; ++c) {
            std::vector<matchingTuple> &matchings1 = outputMatchings[numStart + c];
            int d1id = numStart + c;
            int d2id = d1id + 1;  // c % 2 == 0 ? d1id + 1 : d1id;
            std::vector<diagramTuple> &diagram1 = inputPersistenceDiagrams[d1id];
            std::vector<diagramTuple> &diagram2 = inputPersistenceDiagrams[d2id];

            // Insert segments
            int ids[2];
            auto n1 = (int)chain.at((unsigned long)c);
            auto n2 = (int)chain.at((unsigned long)c + 1);

            // Search for right matching.
            double cost = 0.0;
            for (matchingTuple tup : matchings1) {
                auto d1id1 = (int)std::get<0>(tup);
                if (d1id1 == n1) {
                    cost = std::get<2>(tup);
                    break;
                }
            }

            diagramTuple tuple1 = diagram1[n1];
            diagramTuple tuple2 = diagram2[n2];

            double x1, y1, z1, x2, y2, z2;

            BNodeType point1Type1 = std::get<1>(tuple1);
            BNodeType point1Type2 = std::get<3>(tuple1);
            BNodeType point1Type = point1Type1 == BLocalMax || point1Type2 == BLocalMax
                                       ? BLocalMax
                                       : point1Type1 == BLocalMin || point1Type2 == BLocalMin
                                             ? BLocalMin
                                             : point1Type1 == BSaddle2 || point1Type2 == BSaddle2
                                                   ? BSaddle2
                                                   : BSaddle1;
            bool t11Min = point1Type1 == BLocalMin;
            bool t11Max = point1Type1 == BLocalMax;
            bool t12Min = point1Type2 == BLocalMin;
            bool t12Max = point1Type2 == BLocalMax;
            bool bothEx1 = (t11Min && t12Max) || (t11Max && t12Min);
            if (bothEx1) {
                x1 = t12Max ? std::get<11>(tuple1) : std::get<7>(tuple1);
                y1 = t12Max ? std::get<12>(tuple1) : std::get<8>(tuple1);
                z1 = t12Max ? std::get<13>(tuple1) : std::get<9>(tuple1);
                if (useGeometricSpacing) z1 += spacing * (numStart + c);
            } else {
                x1 = t12Max ? std::get<11>(tuple1)
                            : t11Min ? std::get<7>(tuple1)
                                     : (std::get<7>(tuple1) + std::get<11>(tuple1)) / 2;
                y1 = t12Max ? std::get<12>(tuple1)
                            : t11Min ? std::get<8>(tuple1)
                                     : (std::get<8>(tuple1) + std::get<12>(tuple1)) / 2;
                z1 = t12Max ? std::get<13>(tuple1)
                            : t11Min ? std::get<9>(tuple1)
                                     : (std::get<9>(tuple1) + std::get<13>(tuple1)) / 2;
                if (useGeometricSpacing) z1 += spacing * (numStart + c);
            }

            // Postproc component ids.
            int cid = k;
            bool hasMergedFirst = false;
            if (DoPostProc) {
                std::set<int> &connected = trackingTupleToMerged[k];
                if (!connected.empty()) {
                    int min = *(connected.begin());
                    trackingTuple ttt = trackings.at((unsigned long)min);
                    // int numStart2 = std::get<0>(ttt);
                    int numEnd2 = std::get<1>(ttt);
                    if ((numEnd2 > 0 && numStart + c > numEnd2 + 1) && min < (int)k) {
                        // std::cout << "[ttkTrackingFromPersistenceDiagrams] Switched " <<
                        // k << " for " << min << std::endl;
                        cid = min;
                        hasMergedFirst = numStart + c <= numEnd2 + 3;
                    }

                    if (hasMergedFirst) {
                        // std::cout << "Has merged first " << std::endl;

                        // Replace former first end of the segment with previous ending
                        // segment.
                        std::vector<BIdVertex> chain3 = std::get<2>(ttt);
                        auto nn = (int)chain3.at(chain3.size() - 1);
                        std::vector<diagramTuple> &diagramRematch =
                            inputPersistenceDiagrams[numEnd2];
                        diagramTuple tupleN = diagramRematch.at((unsigned long)nn);

                        point1Type1 = std::get<1>(tupleN);
                        point1Type2 = std::get<3>(tupleN);
                        point1Type = point1Type1 == BLocalMax || point1Type2 == BLocalMax
                                         ? BLocalMax
                                         : point1Type1 == BLocalMin || point1Type2 == BLocalMin
                                               ? BLocalMin
                                               : point1Type1 == BSaddle2 || point1Type2 == BSaddle2
                                                     ? BSaddle2
                                                     : BSaddle1;
                        t11Min = point1Type1 == BLocalMin;
                        t11Max = point1Type1 == BLocalMax;
                        t12Min = point1Type2 == BLocalMin;
                        t12Max = point1Type2 == BLocalMax;
                        bothEx1 = (t11Min && t12Max) || (t11Max && t12Min);
                        // std::cout << "xyz " << x1 << ", " << y1 << ", " << z1 <<
                        // std::endl;
                        if (bothEx1) {
                            x1 = t12Max ? std::get<11>(tupleN) : std::get<7>(tupleN);
                            y1 = t12Max ? std::get<12>(tupleN) : std::get<8>(tupleN);
                            z1 = t12Max ? std::get<13>(tupleN) : std::get<9>(tupleN);
                            if (useGeometricSpacing) z1 += spacing * (numStart + c);
                        } else {
                            x1 = t12Max ? std::get<11>(tupleN)
                                        : t11Min ? std::get<7>(tupleN)
                                                 : (std::get<7>(tupleN) + std::get<11>(tupleN)) / 2;
                            y1 = t12Max ? std::get<12>(tupleN)
                                        : t11Min ? std::get<8>(tupleN)
                                                 : (std::get<8>(tupleN) + std::get<12>(tupleN)) / 2;
                            z1 = t12Max ? std::get<13>(tupleN)
                                        : t11Min ? std::get<9>(tupleN)
                                                 : (std::get<9>(tupleN) + std::get<13>(tupleN)) / 2;
                            if (useGeometricSpacing) z1 += spacing * (numStart + c);
                        }
                        // std::cout << "xyz " << x1 << ", " << y1 << ", " << z1 <<
                        // std::endl;
                    }
                }
            }

            vertices.push_back(vec3(x1, y1, z1));

            ids[0] = 2 * currentMatching;
            // pointTypeScalars->InsertTuple1(ids[0], (double)(int)point1Type);
            // timeScalars->InsertTuple1(ids[0], (double)numStart + c);
            // componentIds->InsertTuple1(ids[0], cid);

            BNodeType point2Type1 = std::get<1>(tuple2);
            BNodeType point2Type2 = std::get<3>(tuple2);
            BNodeType point2Type = point2Type1 == BLocalMax || point2Type2 == BLocalMax
                                       ? BLocalMax
                                       : point2Type1 == BLocalMin || point2Type2 == BLocalMin
                                             ? BLocalMin
                                             : point2Type1 == BSaddle2 || point2Type2 == BSaddle2
                                                   ? BSaddle2
                                                   : BSaddle1;
            bool t21Ex = point2Type1 == BLocalMin || point2Type1 == BLocalMax;
            bool t22Ex = point2Type2 == BLocalMin || point2Type2 == BLocalMax;
            bool bothEx2 = t21Ex && t22Ex;
            if (bothEx2) {
                x2 = point2Type2 == BLocalMax ? std::get<11>(tuple2) : std::get<7>(tuple2);
                y2 = point2Type2 == BLocalMax ? std::get<12>(tuple2) : std::get<8>(tuple2);
                z2 = point2Type2 == BLocalMax ? std::get<13>(tuple2) : std::get<9>(tuple2);
                if (useGeometricSpacing) z2 += spacing * (numStart + c + 1);
            } else {
                x2 = t22Ex ? std::get<11>(tuple2)
                           : t21Ex ? std::get<7>(tuple2)
                                   : (std::get<7>(tuple2) + std::get<11>(tuple2)) / 2;
                y2 = t22Ex ? std::get<12>(tuple2)
                           : t21Ex ? std::get<8>(tuple2)
                                   : (std::get<8>(tuple2) + std::get<12>(tuple2)) / 2;
                z2 = t22Ex ? std::get<13>(tuple2)
                           : t21Ex ? std::get<9>(tuple2)
                                   : (std::get<9>(tuple2) + std::get<13>(tuple2)) / 2;
                if (useGeometricSpacing) z2 += spacing * (numStart + c + 1);
            }
            // points->InsertNextPoint(x2, y2, z2);
            vertices.push_back(vec3(x2, y2, z2));
            ids[1] = 2 * currentMatching + 1;
            // pointTypeScalars->InsertTuple1(ids[1], (double)(int)point2Type);
            // timeScalars->InsertTuple1(ids[1], (double)numStart + c);

            // Postproc component ids.
            // componentIds->InsertTuple1(ids[1], cid);

            // Eqivalent for: persistenceDiagram->InsertNextCell(VTK_LINE, 2, ids);
            indicesLines.push_back(static_cast<uint32_t>(ids[0]));
            indicesLines.push_back(static_cast<uint32_t>(ids[1]));

            // persistenceScalars->InsertTuple1(currentMatching, cost);
            // valueScalars->InsertTuple1(currentMatching,
            //                           (std::get<10>(tuple1) + std::get<10>(tuple2)) / 2);
            // matchingIdScalars->InsertTuple1(currentMatching, currentMatching);
            // lengthScalars->InsertTuple1(currentMatching, chainLength);

            currentMatching++;
        }
    }

    /*persistenceDiagram->SetPoints(points);
    persistenceDiagram->GetCellData()->AddArray(persistenceScalars);
    persistenceDiagram->GetCellData()->AddArray(valueScalars);
    persistenceDiagram->GetCellData()->AddArray(matchingIdScalars);
    persistenceDiagram->GetCellData()->AddArray(lengthScalars);
    persistenceDiagram->GetPointData()->AddArray(timeScalars);
    persistenceDiagram->GetPointData()->AddArray(componentIds);
    persistenceDiagram->GetPointData()->AddArray(pointTypeScalars);*/

    // Add Buffers to the mesh
    auto vertexRAM = std::make_shared<BufferRAMPrecision<vec3>>(std::move(vertices));
    outputMesh.addBuffer(Mesh::BufferInfo(BufferType::PositionAttrib),
                         std::make_shared<Buffer<vec3>>(vertexRAM));

    if (!indicesLines.empty()) {
        outputMesh.addIndices(Mesh::MeshInfo(DrawType::Lines, ConnectivityType::None),
                              util::makeIndexBuffer(std::move(indicesLines)));
    }
}

}  // namespace inviwo
