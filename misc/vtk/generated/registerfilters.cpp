#include "registerfilters.h"
#include "ivw_vtkaggregatedatasetfilter.h"
#include "ivw_vtkangularperiodicfilter.h"
#include "ivw_vtkappendarclength.h"
#include "ivw_vtkappenddatasets.h"
#include "ivw_vtkappendpolydata.h"
#include "ivw_vtkblockidscalars.h"
#include "ivw_vtkbrownianpoints.h"
#include "ivw_vtkcellcenters.h"
#include "ivw_vtkcellderivatives.h"
#include "ivw_vtkcellvalidator.h"
#include "ivw_vtkcleanpolydata.h"
#include "ivw_vtkcompositecutter.h"
#include "ivw_vtkcomputequartiles.h"
#include "ivw_vtkconnectivityfilter.h"
#include "ivw_vtkcontourfilter.h"
#include "ivw_vtkcountfaces.h"
#include "ivw_vtkcountvertices.h"
#include "ivw_vtkcurvatures.h"
#include "ivw_vtkdatasetregionsurfacefilter.h"
#include "ivw_vtkdatasetsurfacefilter.h"
#include "ivw_vtkdatasettrianglefilter.h"
#include "ivw_vtkdecimatepolylinefilter.h"
#include "ivw_vtkdecimatepro.h"
#include "ivw_vtkdelaunay2d.h"
#include "ivw_vtkdelaunay3d.h"
#include "ivw_vtkdistributeddatafilter.h"
#include "ivw_vtkelevationfilter.h"
#include "ivw_vtkevenlyspacedstreamlines2d.h"
#include "ivw_vtkextractblock.h"
#include "ivw_vtkextractcthpart.h"
#include "ivw_vtkextractdatasets.h"
#include "ivw_vtkextractedges.h"
#include "ivw_vtkextractenclosedpoints.h"
#include "ivw_vtkextractgeometry.h"
#include "ivw_vtkextractlevel.h"
#include "ivw_vtkextracttimesteps.h"
#include "ivw_vtkfeatureedges.h"
#include "ivw_vtkforcetime.h"
#include "ivw_vtkgaussiansplatter.h"
#include "ivw_vtkgenericclip.h"
#include "ivw_vtkgenericcontourfilter.h"
#include "ivw_vtkgenericcutter.h"
#include "ivw_vtkgenericdatasettessellator.h"
#include "ivw_vtkgenericgeometryfilter.h"
#include "ivw_vtkgenericoutlinefilter.h"
#include "ivw_vtkgenericstreamtracer.h"
#include "ivw_vtkgradientfilter.h"
#include "ivw_vtkhypertreegridaxisclip.h"
#include "ivw_vtkhypertreegridaxiscut.h"
#include "ivw_vtkhypertreegridaxisreflection.h"
#include "ivw_vtkhypertreegridcellcenters.h"
#include "ivw_vtkhypertreegridcontour.h"
#include "ivw_vtkhypertreegriddepthlimiter.h"
#include "ivw_vtkhypertreegridgeometry.h"
#include "ivw_vtkhypertreegridplanecutter.h"
#include "ivw_vtkhypertreegridthreshold.h"
#include "ivw_vtkhypertreegridtodualgrid.h"
#include "ivw_vtkhypertreegridtounstructuredgrid.h"
#include "ivw_vtkidfilter.h"
#include "ivw_vtkimageclip.h"
#include "ivw_vtkimagedatatopointset.h"
#include "ivw_vtkimagedatatouniformgrid.h"
#include "ivw_vtkimagegradient.h"
#include "ivw_vtkimagegradientmagnitude.h"
#include "ivw_vtkimagemaptocolors.h"
#include "ivw_vtkimagemedian3d.h"
#include "ivw_vtkimageshrink3d.h"
#include "ivw_vtkimagetoamr.h"
#include "ivw_vtkintegrateattributes.h"
#include "ivw_vtklinearsubdivisionfilter.h"
#include "ivw_vtkloopsubdivisionfilter.h"
#include "ivw_vtkmergearrays.h"
#include "ivw_vtkmeshquality.h"
#include "ivw_vtkmoleculeappend.h"
#include "ivw_vtkmoleculetolinesfilter.h"
#include "ivw_vtkmultiblockdatagroupfilter.h"
#include "ivw_vtkoverlappingamrlevelidscalars.h"
#include "ivw_vtkpcelldatatopointdata.h"
#include "ivw_vtkpextractdataarraysovertime.h"
#include "ivw_vtkpextractexodusglobaltemporalvariables.h"
#include "ivw_vtkpextractselectedarraysovertime.h"
#include "ivw_vtkpmaskpoints.h"
#include "ivw_vtkpoutlinecornerfilter.h"
#include "ivw_vtkpoutlinefilter.h"
#include "ivw_vtkppolydatanormals.h"
#include "ivw_vtkpprobefilter.h"
#include "ivw_vtkpreflectionfilter.h"
#include "ivw_vtkpresamplefilter.h"
#include "ivw_vtkparticlepathfilter.h"
#include "ivw_vtkparticletracer.h"
#include "ivw_vtkpassarrays.h"
#include "ivw_vtkpassthrough.h"
#include "ivw_vtkplanecutter.h"
#include "ivw_vtkpointdatatocelldata.h"
#include "ivw_vtkpointsettomoleculefilter.h"
#include "ivw_vtkprocessidscalars.h"
#include "ivw_vtkquadraturepointinterpolator.h"
#include "ivw_vtkquadraturepointsgenerator.h"
#include "ivw_vtkquadratureschemedictionarygenerator.h"
#include "ivw_vtkquadricclustering.h"
#include "ivw_vtkrtxmlpolydatareader.h"
#include "ivw_vtkrandomattributegenerator.h"
#include "ivw_vtkrectilineargridgeometryfilter.h"
#include "ivw_vtkrectilineargridtopointset.h"
#include "ivw_vtkremoveghosts.h"
#include "ivw_vtkresampletoimage.h"
#include "ivw_vtkresamplewithdataset.h"
#include "ivw_vtkribbonfilter.h"
#include "ivw_vtkrotationalextrusionfilter.h"
#include "ivw_vtkshrinkfilter.h"
#include "ivw_vtksimplebondperceiver.h"
#include "ivw_vtksmoothpolydatafilter.h"
#include "ivw_vtkstreaklinefilter.h"
#include "ivw_vtkstreamtracer.h"
#include "ivw_vtkstripper.h"
#include "ivw_vtkstructuredgridoutlinefilter.h"
#include "ivw_vtksynchronizetimefilter.h"
#include "ivw_vtktablefft.h"
#include "ivw_vtktabletopolydata.h"
#include "ivw_vtktabletostructuredgrid.h"
#include "ivw_vtktemporalarrayoperatorfilter.h"
#include "ivw_vtktemporaldatasetcache.h"
#include "ivw_vtktemporalinterpolator.h"
#include "ivw_vtktemporalpathlinefilter.h"
#include "ivw_vtktemporalshiftscale.h"
#include "ivw_vtktemporalsnaptotimestep.h"
#include "ivw_vtktemporalstatistics.h"
#include "ivw_vtktensorglyph.h"
#include "ivw_vtktessellatorfilter.h"
#include "ivw_vtktexturemaptocylinder.h"
#include "ivw_vtktexturemaptoplane.h"
#include "ivw_vtktexturemaptosphere.h"
#include "ivw_vtkthreshold.h"
#include "ivw_vtktransformfilter.h"
#include "ivw_vtktrianglefilter.h"
#include "ivw_vtktubefilter.h"
#include "ivw_vtkwarpscalar.h"
#include "ivw_vtkwarpvector.h"
#include "ivw_vtkxmldataobjectwriter.h"
#include "ivw_vtkxmlhierarchicalboxdatareader.h"
#include "ivw_vtkxmlhypertreegridreader.h"
#include "ivw_vtkxmlimagedatareader.h"
#include "ivw_vtkxmlimagedatawriter.h"
#include "ivw_vtkxmlmultiblockdatareader.h"
#include "ivw_vtkxmlphypertreegridreader.h"
#include "ivw_vtkxmlpimagedatareader.h"
#include "ivw_vtkxmlppolydatareader.h"
#include "ivw_vtkxmlprectilineargridreader.h"
#include "ivw_vtkxmlpstructuredgridreader.h"
#include "ivw_vtkxmlptablereader.h"
#include "ivw_vtkxmlpunstructuredgridreader.h"
#include "ivw_vtkxmlpartitioneddatasetcollectionreader.h"
#include "ivw_vtkxmlpartitioneddatasetreader.h"
#include "ivw_vtkxmlpolydatareader.h"
#include "ivw_vtkxmlrectilineargridreader.h"
#include "ivw_vtkxmlstructuredgridreader.h"
#include "ivw_vtkxmltablereader.h"
#include "ivw_vtkxmluniformgridamrreader.h"
#include "ivw_vtkxmlunstructuredgridreader.h"
#include "ivw_vtkyoungsmaterialinterface.h"

namespace inviwo {
namespace vtkwrapper {

void registerVTKFilters(InviwoModule* module) {
    registervtkAggregateDataSetFilter(module);
    registervtkAngularPeriodicFilter(module);
    registervtkAppendArcLength(module);
    registervtkAppendDataSets(module);
    registervtkAppendPolyData(module);
    registervtkBlockIdScalars(module);
    registervtkBrownianPoints(module);
    registervtkCellCenters(module);
    registervtkCellDerivatives(module);
    registervtkCellValidator(module);
    registervtkCleanPolyData(module);
    registervtkCompositeCutter(module);
    registervtkComputeQuartiles(module);
    registervtkConnectivityFilter(module);
    registervtkContourFilter(module);
    registervtkCountFaces(module);
    registervtkCountVertices(module);
    registervtkCurvatures(module);
    registervtkDataSetRegionSurfaceFilter(module);
    registervtkDataSetSurfaceFilter(module);
    registervtkDataSetTriangleFilter(module);
    registervtkDecimatePolylineFilter(module);
    registervtkDecimatePro(module);
    registervtkDelaunay2D(module);
    registervtkDelaunay3D(module);
    registervtkDistributedDataFilter(module);
    registervtkElevationFilter(module);
    registervtkEvenlySpacedStreamlines2D(module);
    registervtkExtractBlock(module);
    registervtkExtractCTHPart(module);
    registervtkExtractDataSets(module);
    registervtkExtractEdges(module);
    registervtkExtractEnclosedPoints(module);
    registervtkExtractGeometry(module);
    registervtkExtractLevel(module);
    registervtkExtractTimeSteps(module);
    registervtkFeatureEdges(module);
    registervtkForceTime(module);
    registervtkGaussianSplatter(module);
    registervtkGenericClip(module);
    registervtkGenericContourFilter(module);
    registervtkGenericCutter(module);
    registervtkGenericDataSetTessellator(module);
    registervtkGenericGeometryFilter(module);
    registervtkGenericOutlineFilter(module);
    registervtkGenericStreamTracer(module);
    registervtkGradientFilter(module);
    registervtkHyperTreeGridAxisClip(module);
    registervtkHyperTreeGridAxisCut(module);
    registervtkHyperTreeGridAxisReflection(module);
    registervtkHyperTreeGridCellCenters(module);
    registervtkHyperTreeGridContour(module);
    registervtkHyperTreeGridDepthLimiter(module);
    registervtkHyperTreeGridGeometry(module);
    registervtkHyperTreeGridPlaneCutter(module);
    registervtkHyperTreeGridThreshold(module);
    registervtkHyperTreeGridToDualGrid(module);
    registervtkHyperTreeGridToUnstructuredGrid(module);
    registervtkIdFilter(module);
    registervtkImageClip(module);
    registervtkImageDataToPointSet(module);
    registervtkImageDataToUniformGrid(module);
    registervtkImageGradient(module);
    registervtkImageGradientMagnitude(module);
    registervtkImageMapToColors(module);
    registervtkImageMedian3D(module);
    registervtkImageShrink3D(module);
    registervtkImageToAMR(module);
    registervtkIntegrateAttributes(module);
    registervtkLinearSubdivisionFilter(module);
    registervtkLoopSubdivisionFilter(module);
    registervtkMergeArrays(module);
    registervtkMeshQuality(module);
    registervtkMoleculeAppend(module);
    registervtkMoleculeToLinesFilter(module);
    registervtkMultiBlockDataGroupFilter(module);
    registervtkOverlappingAMRLevelIdScalars(module);
    registervtkPCellDataToPointData(module);
    registervtkPExtractDataArraysOverTime(module);
    registervtkPExtractExodusGlobalTemporalVariables(module);
    registervtkPExtractSelectedArraysOverTime(module);
    registervtkPMaskPoints(module);
    registervtkPOutlineCornerFilter(module);
    registervtkPOutlineFilter(module);
    registervtkPPolyDataNormals(module);
    registervtkPProbeFilter(module);
    registervtkPReflectionFilter(module);
    registervtkPResampleFilter(module);
    registervtkParticlePathFilter(module);
    registervtkParticleTracer(module);
    registervtkPassArrays(module);
    registervtkPassThrough(module);
    registervtkPlaneCutter(module);
    registervtkPointDataToCellData(module);
    registervtkPointSetToMoleculeFilter(module);
    registervtkProcessIdScalars(module);
    registervtkQuadraturePointInterpolator(module);
    registervtkQuadraturePointsGenerator(module);
    registervtkQuadratureSchemeDictionaryGenerator(module);
    registervtkQuadricClustering(module);
    registervtkRTXMLPolyDataReader(module);
    registervtkRandomAttributeGenerator(module);
    registervtkRectilinearGridGeometryFilter(module);
    registervtkRectilinearGridToPointSet(module);
    registervtkRemoveGhosts(module);
    registervtkResampleToImage(module);
    registervtkResampleWithDataSet(module);
    registervtkRibbonFilter(module);
    registervtkRotationalExtrusionFilter(module);
    registervtkShrinkFilter(module);
    registervtkSimpleBondPerceiver(module);
    registervtkSmoothPolyDataFilter(module);
    registervtkStreaklineFilter(module);
    registervtkStreamTracer(module);
    registervtkStripper(module);
    registervtkStructuredGridOutlineFilter(module);
    registervtkSynchronizeTimeFilter(module);
    registervtkTableFFT(module);
    registervtkTableToPolyData(module);
    registervtkTableToStructuredGrid(module);
    registervtkTemporalArrayOperatorFilter(module);
    registervtkTemporalDataSetCache(module);
    registervtkTemporalInterpolator(module);
    registervtkTemporalPathLineFilter(module);
    registervtkTemporalShiftScale(module);
    registervtkTemporalSnapToTimeStep(module);
    registervtkTemporalStatistics(module);
    registervtkTensorGlyph(module);
    registervtkTessellatorFilter(module);
    registervtkTextureMapToCylinder(module);
    registervtkTextureMapToPlane(module);
    registervtkTextureMapToSphere(module);
    registervtkThreshold(module);
    registervtkTransformFilter(module);
    registervtkTriangleFilter(module);
    registervtkTubeFilter(module);
    registervtkWarpScalar(module);
    registervtkWarpVector(module);
    registervtkXMLDataObjectWriter(module);
    registervtkXMLHierarchicalBoxDataReader(module);
    registervtkXMLHyperTreeGridReader(module);
    registervtkXMLImageDataReader(module);
    registervtkXMLImageDataWriter(module);
    registervtkXMLMultiBlockDataReader(module);
    registervtkXMLPHyperTreeGridReader(module);
    registervtkXMLPImageDataReader(module);
    registervtkXMLPPolyDataReader(module);
    registervtkXMLPRectilinearGridReader(module);
    registervtkXMLPStructuredGridReader(module);
    registervtkXMLPTableReader(module);
    registervtkXMLPUnstructuredGridReader(module);
    registervtkXMLPartitionedDataSetCollectionReader(module);
    registervtkXMLPartitionedDataSetReader(module);
    registervtkXMLPolyDataReader(module);
    registervtkXMLRectilinearGridReader(module);
    registervtkXMLStructuredGridReader(module);
    registervtkXMLTableReader(module);
    registervtkXMLUniformGridAMRReader(module);
    registervtkXMLUnstructuredGridReader(module);
    registervtkYoungsMaterialInterface(module);
}

}  // namespace vtkwrapper
}  // namespace inviwo
