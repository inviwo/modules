#include "registerttkfilters.h"

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
#include "ivw_ttkdatasetinterpolator.h"
#include "ivw_ttkdatasettotable.h"
#include "ivw_ttkdepthimagebasedgeometryapproximation.h"
#include "ivw_ttkdimensionreduction.h"
#include "ivw_ttkdiscretegradient.h"
#include "ivw_ttkdistancefield.h"
#include "ivw_ttkdistancematrixdistorsion.h"
#include "ivw_ttkeigenfield.h"
#include "ivw_ttkendfor.h"
#include "ivw_ttkfiber.h"
#include "ivw_ttkfibersurface.h"
#include "ivw_ttkflattenmultiblock.h"
#include "ivw_ttkforeach.h"
#include "ivw_ttkftmtree.h"
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
#include "ivw_ttkmergetreeprincipalgeodesics.h"
#include "ivw_ttkmergetreeprincipalgeodesicsdecoding.h"
#include "ivw_ttkmergetreetemporalreductiondecoding.h"
#include "ivw_ttkmergetreetemporalreductionencoding.h"
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
#include "ivw_ttkreebspace.h"
#include "ivw_ttkripscomplex.h"
#include "ivw_ttkscalarfieldcriticalpoints.h"
#include "ivw_ttkscalarfieldnormalizer.h"
#include "ivw_ttkscalarfieldsmoother.h"
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
#include "ivw_vtkxmlpolydatareader.h"
#include "ivw_vtkxmltablereader.h"
#include "ivw_vtkxmlunstructuredgridreader.h"
#include "ivw_vtkxmlimagedatareader.h"
#include "ivw_vtkxmlstructuredgridreader.h"
#include "ivw_vtkxmlrectilineargridreader.h"
#include "ivw_vtkxmlppolydatareader.h"
#include "ivw_vtkxmlpunstructuredgridreader.h"
#include "ivw_vtkxmlptablereader.h"
#include "ivw_vtkxmlpimagedatareader.h"
#include "ivw_vtkxmlpstructuredgridreader.h"
#include "ivw_vtkxmlprectilineargridreader.h"
#include "ivw_vtkxmlhierarchicalboxdatareader.h"
#include "ivw_vtkxmluniformgridamrreader.h"
#include "ivw_vtkxmlhypertreegridreader.h"
#include "ivw_vtkxmlphypertreegridreader.h"
#include "ivw_vtkxmlmultiblockdatareader.h"
#include "ivw_vtkxmlpartitioneddatasetreader.h"
#include "ivw_vtkxmlpartitioneddatasetcollectionreader.h"
#include "ivw_vtkfileseriesreader.h"
#include "ivw_vtkfileseriesreader.h"
#include "ivw_vtkfileseriesreader.h"
#include "ivw_vtkfileseriesreader.h"
#include "ivw_vtkfileseriesreader.h"
#include "ivw_vtkfileseriesreader.h"
#include "ivw_vtkfileseriesreader.h"
#include "ivw_vtkfileseriesreader.h"
#include "ivw_vtkfileseriesreader.h"
#include "ivw_vtkfileseriesreader.h"
#include "ivw_vtkfileseriesreader.h"
#include "ivw_vtkfileseriesreader.h"
#include "ivw_vtkrtxmlpolydatareader.h"
#include "ivw_vtkfileseriesreader.h"
#include "ivw_vtkfileseriesreader.h"
#include "ivw_vtkfileseriesreader.h"
#include "ivw_vtkfileseriesreader.h"
#include "ivw_vtkfileseriesreader.h"
#include "ivw_vtkfileseriesreader.h"
#include "ivw_vtkfileseriesreader.h"

namespace inviwo {
namespace ttkwrapper {

void registerTTKFilters(InviwoModule* module) {
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
    registerttkDataSetInterpolator(module);
    registerttkDataSetToTable(module);
    registerttkDepthImageBasedGeometryApproximation(module);
    registerttkDimensionReduction(module);
    registerttkDiscreteGradient(module);
    registerttkDistanceField(module);
    registerttkDistanceMatrixDistorsion(module);
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
    registerttkMarchingTetrahedra(module);
    registerttkMatrixToHeatMap(module);
    registerttkMergeBlockTables(module);
    registerttkMergeTreePrincipalGeodesics(module);
    registerttkMergeTreePrincipalGeodesicsDecoding(module);
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
    registerttkPathCompression(module);
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
    registervtkXMLPolyDataReader(module);
    registervtkXMLTableReader(module);
    registervtkXMLUnstructuredGridReader(module);
    registervtkXMLImageDataReader(module);
    registervtkXMLStructuredGridReader(module);
    registervtkXMLRectilinearGridReader(module);
    registervtkXMLPPolyDataReader(module);
    registervtkXMLPUnstructuredGridReader(module);
    registervtkXMLPTableReader(module);
    registervtkXMLPImageDataReader(module);
    registervtkXMLPStructuredGridReader(module);
    registervtkXMLPRectilinearGridReader(module);
    registervtkXMLHierarchicalBoxDataReader(module);
    registervtkXMLUniformGridAMRReader(module);
    registervtkXMLHyperTreeGridReader(module);
    registervtkXMLPHyperTreeGridReader(module);
    registervtkXMLMultiBlockDataReader(module);
    registervtkXMLPartitionedDataSetReader(module);
    registervtkXMLPartitionedDataSetCollectionReader(module);
    registervtkFileSeriesReader(module);
    registervtkFileSeriesReader(module);
    registervtkFileSeriesReader(module);
    registervtkFileSeriesReader(module);
    registervtkFileSeriesReader(module);
    registervtkFileSeriesReader(module);
    registervtkFileSeriesReader(module);
    registervtkFileSeriesReader(module);
    registervtkFileSeriesReader(module);
    registervtkFileSeriesReader(module);
    registervtkFileSeriesReader(module);
    registervtkFileSeriesReader(module);
    registervtkRTXMLPolyDataReader(module);
    registervtkFileSeriesReader(module);
    registervtkFileSeriesReader(module);
    registervtkFileSeriesReader(module);
    registervtkFileSeriesReader(module);
    registervtkFileSeriesReader(module);
    registervtkFileSeriesReader(module);
    registervtkFileSeriesReader(module);
}

}  // namespace ttkwrapper
}  // namespace inviwo
