#include "registerfilters.h"
#include "ivw_ttkarrayeditor.h"
#include "ivw_ttkbarycentricsubdivision.h"
#include "ivw_ttkblockaggregator.h"
#include "ivw_ttkcinemaimaging.h"
#include "ivw_ttkcinemaproductreader.h"
#include "ivw_ttkcinemaquery.h"
#include "ivw_ttkcinemareader.h"
#include "ivw_ttkcinemawriter.h"
#include "ivw_ttkclusteringmetrics.h"
#include "ivw_ttkcomponentsize.h"
#include "ivw_ttkconnectedcomponents.h"
#include "ivw_ttkcontinuousscatterplot.h"
#include "ivw_ttkcontouraroundpoint.h"
#include "ivw_ttkcontourtree.h"
#include "ivw_ttkdatasetinterpolator.h"
#include "ivw_ttkdatasettotable.h"
#include "ivw_ttkdepthimagebasedgeometryapproximation.h"
#include "ivw_ttkdimensionreduction.h"
#include "ivw_ttkdimensionreductionmetrics.h"
#include "ivw_ttkdiscretegradient.h"
#include "ivw_ttkdistancefield.h"
#include "ivw_ttkdistancematrixdistortion.h"
#include "ivw_ttkeigenfield.h"
#include "ivw_ttkfiber.h"
#include "ivw_ttkfibersurface.h"
#include "ivw_ttkflattenmultiblock.h"
#include "ivw_ttkgaussianpointcloud.h"
#include "ivw_ttkgeometrysmoother.h"
#include "ivw_ttkgridlayout.h"
#include "ivw_ttkharmonicfield.h"
#include "ivw_ttkhelloworld.h"
#include "ivw_ttkicosphere.h"
#include "ivw_ttkicospherefromobject.h"
#include "ivw_ttkicospheresfrompoints.h"
#include "ivw_ttkidentifierrandomizer.h"
#include "ivw_ttkidentifiers.h"
#include "ivw_ttkidentifybyscalarfield.h"
#include "ivw_ttkimportembeddingfromtable.h"
#include "ivw_ttkintegrallines.h"
#include "ivw_ttkjacobiset.h"
#include "ivw_ttkldistance.h"
#include "ivw_ttkldistancematrix.h"
#include "ivw_ttkmandatorycriticalpoints.h"
#include "ivw_ttkmanifoldcheck.h"
#include "ivw_ttkmarchingtetrahedra.h"
#include "ivw_ttkmatrixtoheatmap.h"
#include "ivw_ttkmergeblocktables.h"
#include "ivw_ttkmergetree.h"
#include "ivw_ttkmergetreeautoencoder.h"
#include "ivw_ttkmergetreeautoencoderdecoding.h"
#include "ivw_ttkmergetreeprincipalgeodesics.h"
#include "ivw_ttkmergetreeprincipalgeodesicsdecoding.h"
#include "ivw_ttkmergetreetemporalreduction.h"
#include "ivw_ttkmergetreetemporalreductiondecoding.h"
#include "ivw_ttkmeshgraph.h"
#include "ivw_ttkmeshsubdivision.h"
#include "ivw_ttkmetricdistortion.h"
#include "ivw_ttkmorphologicaloperators.h"
#include "ivw_ttkmorsesmalecomplex.h"
#include "ivw_ttkmorsesmalequadrangulation.h"
#include "ivw_ttkobjwriter.h"
#include "ivw_ttkoffreader.h"
#include "ivw_ttkoffwriter.h"
#include "ivw_ttkpathcompression.h"
#include "ivw_ttkperiodicghostsgeneration.h"
#include "ivw_ttkpersistencecurve.h"
#include "ivw_ttkpersistencediagram.h"
#include "ivw_ttkpersistencediagramapproximation.h"
#include "ivw_ttkplanargraphlayout.h"
#include "ivw_ttkpointdataconverter.h"
#include "ivw_ttkpointdataselector.h"
#include "ivw_ttkpointmerger.h"
#include "ivw_ttkpointsettocurve.h"
#include "ivw_ttkpointsettosurface.h"
#include "ivw_ttkprojectionfromfield.h"
#include "ivw_ttkquadrangulationsubdivision.h"
#include "ivw_ttkrangepolygon.h"
#include "ivw_ttkreebgraph.h"
#include "ivw_ttkreebspace.h"
#include "ivw_ttkripscomplex.h"
#include "ivw_ttkripspersistencediagram.h"
#include "ivw_ttkripspersistencegenerators.h"
#include "ivw_ttkscalarfieldcriticalpoints.h"
#include "ivw_ttkscalarfieldnormalizer.h"
#include "ivw_ttkscalarfieldsmoother.h"
#include "ivw_ttksigneddistancefield.h"
#include "ivw_ttkspherefrompoint.h"
#include "ivw_ttkstablemanifoldpersistence.h"
#include "ivw_ttkstringarrayconverter.h"
#include "ivw_ttksurfacegeometrysmoother.h"
#include "ivw_ttktabledataselector.h"
#include "ivw_ttktabledistancematrix.h"
#include "ivw_ttktexturemapfromfield.h"
#include "ivw_ttktopologicalcompression.h"
#include "ivw_ttktopologicalcompressionreader.h"
#include "ivw_ttktopologicalcompressionwriter.h"
#include "ivw_ttktopologicalsimplification.h"
#include "ivw_ttktopologicalsimplificationbypersistence.h"
#include "ivw_ttktrackingfromoverlap.h"
#include "ivw_ttktriangulationmanager.h"
#include "ivw_ttktriangulationreader.h"
#include "ivw_ttktriangulationrequest.h"
#include "ivw_ttktriangulationwriter.h"
#include "ivw_ttkuncertaindataestimator.h"
#include "ivw_ttkwebsocketio.h"

namespace inviwo {
namespace vtkwrapper {

void registerVTKFilters(InviwoModule* module) {
    registerttkArrayEditor(module);
    registerttkBarycentricSubdivision(module);
    registerttkBlockAggregator(module);
    registerttkCinemaImaging(module);
    registerttkCinemaProductReader(module);
    registerttkCinemaQuery(module);
    registerttkCinemaReader(module);
    registerttkCinemaWriter(module);
    registerttkClusteringMetrics(module);
    registerttkComponentSize(module);
    registerttkConnectedComponents(module);
    registerttkContinuousScatterPlot(module);
    registerttkContourAroundPoint(module);
    registerttkContourTree(module);
    registerttkDataSetInterpolator(module);
    registerttkDataSetToTable(module);
    registerttkDepthImageBasedGeometryApproximation(module);
    registerttkDimensionReduction(module);
    registerttkDimensionReductionMetrics(module);
    registerttkDiscreteGradient(module);
    registerttkDistanceField(module);
    registerttkDistanceMatrixDistortion(module);
    registerttkEigenField(module);
    registerttkFiber(module);
    registerttkFiberSurface(module);
    registerttkFlattenMultiBlock(module);
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
    registerttkMarchingTetrahedra(module);
    registerttkMatrixToHeatMap(module);
    registerttkMergeBlockTables(module);
    registerttkMergeTree(module);
    registerttkMergeTreeAutoencoder(module);
    registerttkMergeTreeAutoencoderDecoding(module);
    registerttkMergeTreePrincipalGeodesics(module);
    registerttkMergeTreePrincipalGeodesicsDecoding(module);
    registerttkMergeTreeTemporalReduction(module);
    registerttkMergeTreeTemporalReductionDecoding(module);
    registerttkMeshGraph(module);
    registerttkMeshSubdivision(module);
    registerttkMetricDistortion(module);
    registerttkMorphologicalOperators(module);
    registerttkMorseSmaleComplex(module);
    registerttkMorseSmaleQuadrangulation(module);
    registerttkOBJWriter(module);
    registerttkOFFReader(module);
    registerttkOFFWriter(module);
    registerttkPathCompression(module);
    registerttkPeriodicGhostsGeneration(module);
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
    registerttkReebGraph(module);
    registerttkReebSpace(module);
    registerttkRipsComplex(module);
    registerttkRipsPersistenceDiagram(module);
    registerttkRipsPersistenceGenerators(module);
    registerttkScalarFieldCriticalPoints(module);
    registerttkScalarFieldNormalizer(module);
    registerttkScalarFieldSmoother(module);
    registerttkSignedDistanceField(module);
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

}  // namespace vtkwrapper
}  // namespace inviwo
