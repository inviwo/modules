#include "registerfilters.h"
#include <inviwo/core/common/inviwomodule.h>
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
    module->registerProcessor(registerttkArrayEditor());
    module->registerProcessor(registerttkBarycentricSubdivision());
    module->registerProcessor(registerttkBlockAggregator());
    module->registerProcessor(registerttkCinemaImaging());
    module->registerProcessor(registerttkCinemaProductReader());
    module->registerProcessor(registerttkCinemaQuery());
    module->registerProcessor(registerttkCinemaReader());
    module->registerProcessor(registerttkCinemaWriter());
    module->registerProcessor(registerttkClusteringMetrics());
    module->registerProcessor(registerttkComponentSize());
    module->registerProcessor(registerttkConnectedComponents());
    module->registerProcessor(registerttkContinuousScatterPlot());
    module->registerProcessor(registerttkContourAroundPoint());
    module->registerProcessor(registerttkContourTree());
    module->registerProcessor(registerttkDataSetInterpolator());
    module->registerProcessor(registerttkDataSetToTable());
    module->registerProcessor(registerttkDepthImageBasedGeometryApproximation());
    module->registerProcessor(registerttkDimensionReduction());
    module->registerProcessor(registerttkDimensionReductionMetrics());
    module->registerProcessor(registerttkDiscreteGradient());
    module->registerProcessor(registerttkDistanceField());
    module->registerProcessor(registerttkDistanceMatrixDistortion());
    module->registerProcessor(registerttkEigenField());
    module->registerProcessor(registerttkFiber());
    module->registerProcessor(registerttkFiberSurface());
    module->registerProcessor(registerttkFlattenMultiBlock());
    module->registerProcessor(registerttkGaussianPointCloud());
    module->registerProcessor(registerttkGeometrySmoother());
    module->registerProcessor(registerttkGridLayout());
    module->registerProcessor(registerttkHarmonicField());
    module->registerProcessor(registerttkHelloWorld());
    module->registerProcessor(registerttkIcosphere());
    module->registerProcessor(registerttkIcosphereFromObject());
    module->registerProcessor(registerttkIcospheresFromPoints());
    module->registerProcessor(registerttkIdentifierRandomizer());
    module->registerProcessor(registerttkIdentifiers());
    module->registerProcessor(registerttkIdentifyByScalarField());
    module->registerProcessor(registerttkImportEmbeddingFromTable());
    module->registerProcessor(registerttkIntegralLines());
    module->registerProcessor(registerttkJacobiSet());
    module->registerProcessor(registerttkLDistance());
    module->registerProcessor(registerttkLDistanceMatrix());
    module->registerProcessor(registerttkMandatoryCriticalPoints());
    module->registerProcessor(registerttkManifoldCheck());
    module->registerProcessor(registerttkMarchingTetrahedra());
    module->registerProcessor(registerttkMatrixToHeatMap());
    module->registerProcessor(registerttkMergeBlockTables());
    module->registerProcessor(registerttkMergeTree());
    module->registerProcessor(registerttkMergeTreeAutoencoder());
    module->registerProcessor(registerttkMergeTreeAutoencoderDecoding());
    module->registerProcessor(registerttkMergeTreePrincipalGeodesics());
    module->registerProcessor(registerttkMergeTreePrincipalGeodesicsDecoding());
    module->registerProcessor(registerttkMergeTreeTemporalReduction());
    module->registerProcessor(registerttkMergeTreeTemporalReductionDecoding());
    module->registerProcessor(registerttkMeshGraph());
    module->registerProcessor(registerttkMeshSubdivision());
    module->registerProcessor(registerttkMetricDistortion());
    module->registerProcessor(registerttkMorphologicalOperators());
    module->registerProcessor(registerttkMorseSmaleComplex());
    module->registerProcessor(registerttkMorseSmaleQuadrangulation());
    module->registerProcessor(registerttkOBJWriter());
    module->registerProcessor(registerttkOFFReader());
    module->registerProcessor(registerttkOFFWriter());
    module->registerProcessor(registerttkPathCompression());
    module->registerProcessor(registerttkPeriodicGhostsGeneration());
    module->registerProcessor(registerttkPersistenceCurve());
    module->registerProcessor(registerttkPersistenceDiagram());
    module->registerProcessor(registerttkPersistenceDiagramApproximation());
    module->registerProcessor(registerttkPlanarGraphLayout());
    module->registerProcessor(registerttkPointDataConverter());
    module->registerProcessor(registerttkPointDataSelector());
    module->registerProcessor(registerttkPointMerger());
    module->registerProcessor(registerttkPointSetToCurve());
    module->registerProcessor(registerttkPointSetToSurface());
    module->registerProcessor(registerttkProjectionFromField());
    module->registerProcessor(registerttkQuadrangulationSubdivision());
    module->registerProcessor(registerttkRangePolygon());
    module->registerProcessor(registerttkReebGraph());
    module->registerProcessor(registerttkReebSpace());
    module->registerProcessor(registerttkRipsComplex());
    module->registerProcessor(registerttkRipsPersistenceDiagram());
    module->registerProcessor(registerttkRipsPersistenceGenerators());
    module->registerProcessor(registerttkScalarFieldCriticalPoints());
    module->registerProcessor(registerttkScalarFieldNormalizer());
    module->registerProcessor(registerttkScalarFieldSmoother());
    module->registerProcessor(registerttkSignedDistanceField());
    module->registerProcessor(registerttkSphereFromPoint());
    module->registerProcessor(registerttkStableManifoldPersistence());
    module->registerProcessor(registerttkStringArrayConverter());
    module->registerProcessor(registerttkSurfaceGeometrySmoother());
    module->registerProcessor(registerttkTableDataSelector());
    module->registerProcessor(registerttkTableDistanceMatrix());
    module->registerProcessor(registerttkTextureMapFromField());
    module->registerProcessor(registerttkTopologicalCompression());
    module->registerProcessor(registerttkTopologicalCompressionReader());
    module->registerProcessor(registerttkTopologicalCompressionWriter());
    module->registerProcessor(registerttkTopologicalSimplification());
    module->registerProcessor(registerttkTopologicalSimplificationByPersistence());
    module->registerProcessor(registerttkTrackingFromOverlap());
    module->registerProcessor(registerttkTriangulationManager());
    module->registerProcessor(registerttkTriangulationReader());
    module->registerProcessor(registerttkTriangulationRequest());
    module->registerProcessor(registerttkTriangulationWriter());
    module->registerProcessor(registerttkUncertainDataEstimator());
    module->registerProcessor(registerttkWebSocketIO());
}

}  // namespace vtkwrapper
}  // namespace inviwo
