#include "registerttkfilters.h"

#include "ivwwraparrayeditor.h"
#include "ivwwraparraypreconditioning.h"
#include "ivwwrapbarycentricsubdivision.h"
#include "ivwwrapblockaggregator.h"
#include "ivwwrapcinemaimaging.h"
#include "ivwwrapcinemaproductreader.h"
#include "ivwwrapcinemaquery.h"
#include "ivwwrapcinemareader.h"
#include "ivwwrapcinemawriter.h"
#include "ivwwrapcomponentsize.h"
#include "ivwwrapconnectedcomponents.h"
#include "ivwwrapcontinuousscatterplot.h"
#include "ivwwrapcontouraroundpoint.h"
#include "ivwwrapdatasetinterpolator.h"
#include "ivwwrapdatasettotable.h"
#include "ivwwrapdepthimagebasedgeometryapproximation.h"
#include "ivwwrapdimensionreduction.h"
#include "ivwwrapdiscretegradient.h"
#include "ivwwrapdistancefield.h"
#include "ivwwrapeigenfield.h"
#include "ivwwrapendfor.h"
#include "ivwwrapfiber.h"
#include "ivwwrapfibersurface.h"
#include "ivwwrapflattenmultiblock.h"
#include "ivwwrapforeach.h"
#include "ivwwrapftmtree.h"
#include "ivwwrapgaussianpointcloud.h"
#include "ivwwrapgeometrysmoother.h"
#include "ivwwrapgridlayout.h"
#include "ivwwrapharmonicfield.h"
#include "ivwwraphelloworld.h"
#include "ivwwrapicosphere.h"
#include "ivwwrapicospherefromobject.h"
#include "ivwwrapicospheresfrompoints.h"
#include "ivwwrapidentifierrandomizer.h"
#include "ivwwrapidentifiers.h"
#include "ivwwrapidentifybyscalarfield.h"
#include "ivwwrapimportembeddingfromtable.h"
#include "ivwwrapintegrallines.h"
#include "ivwwrapjacobiset.h"
#include "ivwwrapldistance.h"
#include "ivwwrapldistancematrix.h"
#include "ivwwrapmandatorycriticalpoints.h"
#include "ivwwrapmanifoldcheck.h"
#include "ivwwrapmatrixtoheatmap.h"
#include "ivwwrapmergeblocktables.h"
#include "ivwwrapmergetreeclustering.h"
#include "ivwwrapmergetreedistancematrix.h"
#include "ivwwrapmergetreetemporalreductiondecoding.h"
#include "ivwwrapmergetreetemporalreductionencoding.h"
#include "ivwwrapmeshgraph.h"
#include "ivwwrapmeshsubdivision.h"
#include "ivwwrapmetricdistortion.h"
#include "ivwwrapmorphologicaloperators.h"
#include "ivwwrapmorsesmalecomplex.h"
#include "ivwwrapmorsesmalequadrangulation.h"
#include "ivwwrapobjwriter.h"
#include "ivwwrapoffreader.h"
#include "ivwwrapoffwriter.h"
#include "ivwwrappersistencecurve.h"
#include "ivwwrappersistencediagram.h"
#include "ivwwrappersistencediagramapproximation.h"
#include "ivwwrapplanargraphlayout.h"
#include "ivwwrappointdataconverter.h"
#include "ivwwrappointdataselector.h"
#include "ivwwrappointmerger.h"
#include "ivwwrappointsettocurve.h"
#include "ivwwrappointsettosurface.h"
#include "ivwwrapprojectionfromfield.h"
#include "ivwwrapquadrangulationsubdivision.h"
#include "ivwwraprangepolygon.h"
#include "ivwwrapreebspace.h"
#include "ivwwrapripscomplex.h"
#include "ivwwrapscalarfieldcriticalpoints.h"
#include "ivwwrapscalarfieldnormalizer.h"
#include "ivwwrapscalarfieldsmoother.h"
#include "ivwwrapspherefrompoint.h"
#include "ivwwrapstablemanifoldpersistence.h"
#include "ivwwrapstringarrayconverter.h"
#include "ivwwrapsurfacegeometrysmoother.h"
#include "ivwwraptabledataselector.h"
#include "ivwwraptabledistancematrix.h"
#include "ivwwraptexturemapfromfield.h"
#include "ivwwraptopologicalcompression.h"
#include "ivwwraptopologicalcompressionreader.h"
#include "ivwwraptopologicalcompressionwriter.h"
#include "ivwwraptopologicalsimplification.h"
#include "ivwwraptopologicalsimplificationbypersistence.h"
#include "ivwwraptrackingfromoverlap.h"
#include "ivwwraptriangulationmanager.h"
#include "ivwwraptriangulationreader.h"
#include "ivwwraptriangulationrequest.h"
#include "ivwwraptriangulationwriter.h"
#include "ivwwrapuncertaindataestimator.h"
#include "ivwwrapwebsocketio.h"

namespace inviwo {
namespace ttkwrapper {

void registerTTKFilters(InviwoModule* module) {
    registerttkArrayEditor(module);
    registerttkArrayPreconditioning(module);
    registerttkBarycentricSubdivision(module);
    registerttkBlockAggregator(module);
    registerttkCinemaImaging(module);
    registerttkCinemaProductReader(module);
    registerttkCinemaQuery(module);
    registerttkCinemaReader(module);
    registerttkCinemaWriter(module);
    registerttkComponentSize(module);
    registerttkConnectedComponents(module);
    registerttkContinuousScatterPlot(module);
    registerttkContourAroundPoint(module);
    registerttkDataSetInterpolator(module);
    registerttkDataSetToTable(module);
    registerttkDepthImageBasedGeometryApproximation(module);
    registerttkDimensionReduction(module);
    registerttkDiscreteGradient(module);
    registerttkDistanceField(module);
    registerttkEigenField(module);
    registerttkEndFor(module);
    registerttkFiber(module);
    registerttkFiberSurface(module);
    registerttkFlattenMultiBlock(module);
    registerttkForEach(module);
    registerttkFTMTree(module);
    registerttkGaussianPointCloud(module);
    registerttkGeometrySmoother(module);
    registerttkGridLayout(module);
    registerttkHarmonicField(module);
    registerttkHelloWorld(module);
    registerttkIcosphere(module);
    registerttkIcosphereFromObject(module);
    registerttkIcospheresFromPoints(module);
    registerttkIdentifierRandomizer(module);
    registerttkIdentifiers(module);
    registerttkIdentifyByScalarField(module);
    registerttkImportEmbeddingFromTable(module);
    registerttkIntegralLines(module);
    registerttkJacobiSet(module);
    registerttkLDistance(module);
    registerttkLDistanceMatrix(module);
    registerttkMandatoryCriticalPoints(module);
    registerttkManifoldCheck(module);
    registerttkMatrixToHeatMap(module);
    registerttkMergeBlockTables(module);
    registerttkMergeTreeClustering(module);
    registerttkMergeTreeDistanceMatrix(module);
    registerttkMergeTreeTemporalReductionDecoding(module);
    registerttkMergeTreeTemporalReductionEncoding(module);
    registerttkMeshGraph(module);
    registerttkMeshSubdivision(module);
    registerttkMetricDistortion(module);
    registerttkMorphologicalOperators(module);
    registerttkMorseSmaleComplex(module);
    registerttkMorseSmaleQuadrangulation(module);
    registerttkOBJWriter(module);
    registerttkOFFReader(module);
    registerttkOFFWriter(module);
    registerttkPersistenceCurve(module);
    registerttkPersistenceDiagram(module);
    registerttkPersistenceDiagramApproximation(module);
    registerttkPlanarGraphLayout(module);
    registerttkPointDataConverter(module);
    registerttkPointDataSelector(module);
    registerttkPointMerger(module);
    registerttkPointSetToCurve(module);
    registerttkPointSetToSurface(module);
    registerttkProjectionFromField(module);
    registerttkQuadrangulationSubdivision(module);
    registerttkRangePolygon(module);
    registerttkReebSpace(module);
    registerttkRipsComplex(module);
    registerttkScalarFieldCriticalPoints(module);
    registerttkScalarFieldNormalizer(module);
    registerttkScalarFieldSmoother(module);
    registerttkSphereFromPoint(module);
    registerttkStableManifoldPersistence(module);
    registerttkStringArrayConverter(module);
    registerttkSurfaceGeometrySmoother(module);
    registerttkTableDataSelector(module);
    registerttkTableDistanceMatrix(module);
    registerttkTextureMapFromField(module);
    registerttkTopologicalCompression(module);
    registerttkTopologicalCompressionReader(module);
    registerttkTopologicalCompressionWriter(module);
    registerttkTopologicalSimplification(module);
    registerttkTopologicalSimplificationByPersistence(module);
    registerttkTrackingFromOverlap(module);
    registerttkTriangulationManager(module);
    registerttkTriangulationReader(module);
    registerttkTriangulationRequest(module);
    registerttkTriangulationWriter(module);
    registerttkUncertainDataEstimator(module);
    registerttkWebSocketIO(module);
}

}  // namespace ttkwrapper
}  // namespace inviwo
