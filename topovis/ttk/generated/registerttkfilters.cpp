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
#include "ivwwrapmeshgraph.h"
#include "ivwwrapmeshsubdivision.h"
#include "ivwwrapmorsesmalecomplex.h"
#include "ivwwrapmorsesmalequadrangulation.h"
#include "ivwwrapobjwriter.h"
#include "ivwwrapoffreader.h"
#include "ivwwrapoffwriter.h"
#include "ivwwrapperiodicgrid.h"
#include "ivwwrappersistencecurve.h"
#include "ivwwrappersistencediagram.h"
#include "ivwwrapplanargraphlayout.h"
#include "ivwwrappointdataconverter.h"
#include "ivwwrappointdataselector.h"
#include "ivwwrappointmerger.h"
#include "ivwwrappointsettocurve.h"
#include "ivwwrapprojectionfromfield.h"
#include "ivwwrapquadrangulationsubdivision.h"
#include "ivwwraprangepolygon.h"
#include "ivwwrapreebspace.h"
#include "ivwwrapscalarfieldcriticalpoints.h"
#include "ivwwrapscalarfieldnormalizer.h"
#include "ivwwrapscalarfieldsmoother.h"
#include "ivwwrapspherefrompoint.h"
#include "ivwwrapstringarrayconverter.h"
#include "ivwwraptabledataselector.h"
#include "ivwwraptexturemapfromfield.h"
#include "ivwwraptopologicalcompression.h"
#include "ivwwraptopologicalcompressionreader.h"
#include "ivwwraptopologicalcompressionwriter.h"
#include "ivwwraptopologicalsimplification.h"
#include "ivwwraptrackingfromoverlap.h"
#include "ivwwraptriangulationrequest.h"
#include "ivwwrapuncertaindataestimator.h"

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
    registerttkMeshGraph(module);
    registerttkMeshSubdivision(module);
    registerttkMorseSmaleComplex(module);
    registerttkMorseSmaleQuadrangulation(module);
    registerttkOBJWriter(module);
    registerttkOFFReader(module);
    registerttkOFFWriter(module);
    registerttkPeriodicGrid(module);
    registerttkPersistenceCurve(module);
    registerttkPersistenceDiagram(module);
    registerttkPlanarGraphLayout(module);
    registerttkPointDataConverter(module);
    registerttkPointDataSelector(module);
    registerttkPointMerger(module);
    registerttkPointSetToCurve(module);
    registerttkProjectionFromField(module);
    registerttkQuadrangulationSubdivision(module);
    registerttkRangePolygon(module);
    registerttkReebSpace(module);
    registerttkScalarFieldCriticalPoints(module);
    registerttkScalarFieldNormalizer(module);
    registerttkScalarFieldSmoother(module);
    registerttkSphereFromPoint(module);
    registerttkStringArrayConverter(module);
    registerttkTableDataSelector(module);
    registerttkTextureMapFromField(module);
    registerttkTopologicalCompression(module);
    registerttkTopologicalCompressionReader(module);
    registerttkTopologicalCompressionWriter(module);
    registerttkTopologicalSimplification(module);
    registerttkTrackingFromOverlap(module);
    registerttkTriangulationRequest(module);
    registerttkUncertainDataEstimator(module);
}

}  // namespace ttkwrapper
}  // namespace inviwo
