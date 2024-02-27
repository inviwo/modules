# -*- coding: utf-8 -*-
# ********************************************************************************
#
# Inviwo - Interactive Visualization Workshop
#
# Copyright (c) 2024 Inviwo Foundation
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# ********************************************************************************

ttk_filters = [
    "Algorithm"       # Empty?
    "Compatibility",  # Very odd one
    "Extract",        # Has double prop of dim 6...

    "ContourTreeAlignment",              # BinaryTree missing ContourTreeAlignment.h:40
    "CinemaDarkroom",                    # Seems strange
    "PersistenceDiagramClustering",      # KDTree.h:423 missing include buildWeights.h
    "PersistenceDiagramDistanceMatrix",  # KDTree.h:423 missing include buildWeights.h
    "ContourForests",                    # MergeTree.h compile problem
    "BottleneckDistance",                # Munkres.h:200 missing include MunkresImpl.h
    "TrackingFromFields",                # Munkres.h:200 missing include MunkresImpl.h
    "TrackingFromPersistenceDiagrams",   # Munkres.h:200 missing include MunkresImpl.h
    "FTRGraph",                          # Missing include
    "ArrayPreconditioning",              # Could not find SetBurstSize (member/function)
    "GhostCellPreconditioning",          # Could not find import ttkGhostCellPreconditioning.h
    "MergeTreeDistanceMatrix",     # Could not find SetUseFieldDataParameters (member/funcion)
    "PersistentGenerators",        # Could not find import ttkPersistentGenerators.h
    "ProjectionFromTable",         # Could not find import ttkProjectionFromTable.h
    # Could not find SetBarycenterSizeLimitPercent (member/function)
    "MergeTreeClustering"
]

# exclude the following ParaView filters due to missing header files
vtk_paraview_filters = [
    "vtkAttributeDataToTableFilter",
    "vtkAdditionalFieldReader",
    "vtkCleanUnstructuredGrid",
    "vtkCleanUnstructuredGridCells",
    "vtkClientServerMoveData",
    "vtkCTHSimpleSurface",
    "vtkDataObjectTreeToPointSetFilter",
    "vtkDistributedPointCloudFilter",
    "vtkEnvironmentAnnotationFilter",
    "vtkExtractSelectionRange",
    "vtkGridConnectivity",
    "vtkHybridProbeFilter",
    "vtkIntegrateFlowThroughSurface",
    "vtkInSituPParticlePathFilter",
    "vtkIsoVolume",
    "vtkIntersectFragments",
    "vtkMaterialInterfaceFilter",
    "vtkMinMax",
    "vtkMergeCompositeDataSet",
    "vtkMPIMoveData",
    "vtkNonOverlappingAMRLevelIdScalars",
    "vtkMultiBlockFromTimeSeriesFilter",
    "vtkOrderedCompositeDistributor",
    "vtkPCellSizeFilter",
    "vtkPConvertSelection",
    "vtkPExtractHistogram",
    "vtkPlotEdges",
    "vtkPolyLineToRectilinearGridFilter",
    "vtkPSciVizContingencyStats",
    "vtkPSciVizKMeans",
    "vtkPSciVizDescriptiveStats",
    "vtkPSciVizMultiCorrelativeStats",
    "vtkPSciVizPCAStats",
    "vtkRectilinearGridConnectivity",
    "vtkReductionFilter",
    "vtkRulerLineForInput",
    "vtkSliceAlongPolyPlane",
    "vtkSurfaceVectors",
    "vtkTimeStepProgressFilter",
    "vtkTimeToTextConvertor"
]
