#include "registerfilters.h"
#include <inviwo/core/common/inviwomodule.h>
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
#include "ivw_vtkxmlhypertreegridreader.h"
#include "ivw_vtkxmlimagedatareader.h"
#include "ivw_vtkxmlimagedatawriter.h"
#include "ivw_vtkxmlphypertreegridreader.h"
#include "ivw_vtkxmlpimagedatareader.h"
#include "ivw_vtkxmlppolydatareader.h"
#include "ivw_vtkxmlprectilineargridreader.h"
#include "ivw_vtkxmlpstructuredgridreader.h"
#include "ivw_vtkxmlptablereader.h"
#include "ivw_vtkxmlpunstructuredgridreader.h"
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
    module->registerProcessor(registervtkAggregateDataSetFilter());
    module->registerProcessor(registervtkAngularPeriodicFilter());
    module->registerProcessor(registervtkAppendArcLength());
    module->registerProcessor(registervtkAppendDataSets());
    module->registerProcessor(registervtkAppendPolyData());
    module->registerProcessor(registervtkBlockIdScalars());
    module->registerProcessor(registervtkBrownianPoints());
    module->registerProcessor(registervtkCellCenters());
    module->registerProcessor(registervtkCellDerivatives());
    module->registerProcessor(registervtkCellValidator());
    module->registerProcessor(registervtkCleanPolyData());
    module->registerProcessor(registervtkCompositeCutter());
    module->registerProcessor(registervtkComputeQuartiles());
    module->registerProcessor(registervtkConnectivityFilter());
    module->registerProcessor(registervtkContourFilter());
    module->registerProcessor(registervtkCountFaces());
    module->registerProcessor(registervtkCountVertices());
    module->registerProcessor(registervtkCurvatures());
    module->registerProcessor(registervtkDataSetRegionSurfaceFilter());
    module->registerProcessor(registervtkDataSetSurfaceFilter());
    module->registerProcessor(registervtkDataSetTriangleFilter());
    module->registerProcessor(registervtkDecimatePolylineFilter());
    module->registerProcessor(registervtkDecimatePro());
    module->registerProcessor(registervtkDelaunay2D());
    module->registerProcessor(registervtkDelaunay3D());
    module->registerProcessor(registervtkDistributedDataFilter());
    module->registerProcessor(registervtkElevationFilter());
    module->registerProcessor(registervtkEvenlySpacedStreamlines2D());
    module->registerProcessor(registervtkExtractBlock());
    module->registerProcessor(registervtkExtractCTHPart());
    module->registerProcessor(registervtkExtractDataSets());
    module->registerProcessor(registervtkExtractEdges());
    module->registerProcessor(registervtkExtractEnclosedPoints());
    module->registerProcessor(registervtkExtractGeometry());
    module->registerProcessor(registervtkExtractLevel());
    module->registerProcessor(registervtkExtractTimeSteps());
    module->registerProcessor(registervtkFeatureEdges());
    module->registerProcessor(registervtkForceTime());
    module->registerProcessor(registervtkGaussianSplatter());
    module->registerProcessor(registervtkGenericClip());
    module->registerProcessor(registervtkGenericContourFilter());
    module->registerProcessor(registervtkGenericCutter());
    module->registerProcessor(registervtkGenericDataSetTessellator());
    module->registerProcessor(registervtkGenericGeometryFilter());
    module->registerProcessor(registervtkGenericOutlineFilter());
    module->registerProcessor(registervtkGenericStreamTracer());
    module->registerProcessor(registervtkGradientFilter());
    module->registerProcessor(registervtkHyperTreeGridAxisClip());
    module->registerProcessor(registervtkHyperTreeGridAxisCut());
    module->registerProcessor(registervtkHyperTreeGridAxisReflection());
    module->registerProcessor(registervtkHyperTreeGridCellCenters());
    module->registerProcessor(registervtkHyperTreeGridContour());
    module->registerProcessor(registervtkHyperTreeGridDepthLimiter());
    module->registerProcessor(registervtkHyperTreeGridGeometry());
    module->registerProcessor(registervtkHyperTreeGridPlaneCutter());
    module->registerProcessor(registervtkHyperTreeGridThreshold());
    module->registerProcessor(registervtkHyperTreeGridToDualGrid());
    module->registerProcessor(registervtkHyperTreeGridToUnstructuredGrid());
    module->registerProcessor(registervtkIdFilter());
    module->registerProcessor(registervtkImageClip());
    module->registerProcessor(registervtkImageDataToPointSet());
    module->registerProcessor(registervtkImageDataToUniformGrid());
    module->registerProcessor(registervtkImageGradient());
    module->registerProcessor(registervtkImageGradientMagnitude());
    module->registerProcessor(registervtkImageMapToColors());
    module->registerProcessor(registervtkImageMedian3D());
    module->registerProcessor(registervtkImageShrink3D());
    module->registerProcessor(registervtkImageToAMR());
    module->registerProcessor(registervtkIntegrateAttributes());
    module->registerProcessor(registervtkLinearSubdivisionFilter());
    module->registerProcessor(registervtkLoopSubdivisionFilter());
    module->registerProcessor(registervtkMeshQuality());
    module->registerProcessor(registervtkMoleculeAppend());
    module->registerProcessor(registervtkMoleculeToLinesFilter());
    module->registerProcessor(registervtkMultiBlockDataGroupFilter());
    module->registerProcessor(registervtkOverlappingAMRLevelIdScalars());
    module->registerProcessor(registervtkPCellDataToPointData());
    module->registerProcessor(registervtkPExtractDataArraysOverTime());
    module->registerProcessor(registervtkPExtractExodusGlobalTemporalVariables());
    module->registerProcessor(registervtkPExtractSelectedArraysOverTime());
    module->registerProcessor(registervtkPMaskPoints());
    module->registerProcessor(registervtkPOutlineCornerFilter());
    module->registerProcessor(registervtkPOutlineFilter());
    module->registerProcessor(registervtkPPolyDataNormals());
    module->registerProcessor(registervtkPProbeFilter());
    module->registerProcessor(registervtkPReflectionFilter());
    module->registerProcessor(registervtkPResampleFilter());
    module->registerProcessor(registervtkParticlePathFilter());
    module->registerProcessor(registervtkParticleTracer());
    module->registerProcessor(registervtkPassArrays());
    module->registerProcessor(registervtkPlaneCutter());
    module->registerProcessor(registervtkPointDataToCellData());
    module->registerProcessor(registervtkPointSetToMoleculeFilter());
    module->registerProcessor(registervtkProcessIdScalars());
    module->registerProcessor(registervtkQuadraturePointInterpolator());
    module->registerProcessor(registervtkQuadraturePointsGenerator());
    module->registerProcessor(registervtkQuadratureSchemeDictionaryGenerator());
    module->registerProcessor(registervtkQuadricClustering());
    module->registerProcessor(registervtkRTXMLPolyDataReader());
    module->registerProcessor(registervtkRandomAttributeGenerator());
    module->registerProcessor(registervtkRectilinearGridGeometryFilter());
    module->registerProcessor(registervtkRectilinearGridToPointSet());
    module->registerProcessor(registervtkRemoveGhosts());
    module->registerProcessor(registervtkResampleToImage());
    module->registerProcessor(registervtkResampleWithDataSet());
    module->registerProcessor(registervtkRibbonFilter());
    module->registerProcessor(registervtkRotationalExtrusionFilter());
    module->registerProcessor(registervtkShrinkFilter());
    module->registerProcessor(registervtkSimpleBondPerceiver());
    module->registerProcessor(registervtkSmoothPolyDataFilter());
    module->registerProcessor(registervtkStreaklineFilter());
    module->registerProcessor(registervtkStreamTracer());
    module->registerProcessor(registervtkStripper());
    module->registerProcessor(registervtkStructuredGridOutlineFilter());
    module->registerProcessor(registervtkSynchronizeTimeFilter());
    module->registerProcessor(registervtkTableFFT());
    module->registerProcessor(registervtkTableToPolyData());
    module->registerProcessor(registervtkTableToStructuredGrid());
    module->registerProcessor(registervtkTemporalArrayOperatorFilter());
    module->registerProcessor(registervtkTemporalDataSetCache());
    module->registerProcessor(registervtkTemporalInterpolator());
    module->registerProcessor(registervtkTemporalPathLineFilter());
    module->registerProcessor(registervtkTemporalShiftScale());
    module->registerProcessor(registervtkTemporalSnapToTimeStep());
    module->registerProcessor(registervtkTemporalStatistics());
    module->registerProcessor(registervtkTensorGlyph());
    module->registerProcessor(registervtkTessellatorFilter());
    module->registerProcessor(registervtkTextureMapToCylinder());
    module->registerProcessor(registervtkTextureMapToPlane());
    module->registerProcessor(registervtkTextureMapToSphere());
    module->registerProcessor(registervtkThreshold());
    module->registerProcessor(registervtkTransformFilter());
    module->registerProcessor(registervtkTriangleFilter());
    module->registerProcessor(registervtkTubeFilter());
    module->registerProcessor(registervtkWarpScalar());
    module->registerProcessor(registervtkWarpVector());
    module->registerProcessor(registervtkXMLDataObjectWriter());
    module->registerProcessor(registervtkXMLHyperTreeGridReader());
    module->registerProcessor(registervtkXMLImageDataReader());
    module->registerProcessor(registervtkXMLImageDataWriter());
    module->registerProcessor(registervtkXMLPHyperTreeGridReader());
    module->registerProcessor(registervtkXMLPImageDataReader());
    module->registerProcessor(registervtkXMLPPolyDataReader());
    module->registerProcessor(registervtkXMLPRectilinearGridReader());
    module->registerProcessor(registervtkXMLPStructuredGridReader());
    module->registerProcessor(registervtkXMLPTableReader());
    module->registerProcessor(registervtkXMLPUnstructuredGridReader());
    module->registerProcessor(registervtkXMLPartitionedDataSetReader());
    module->registerProcessor(registervtkXMLPolyDataReader());
    module->registerProcessor(registervtkXMLRectilinearGridReader());
    module->registerProcessor(registervtkXMLStructuredGridReader());
    module->registerProcessor(registervtkXMLTableReader());
    module->registerProcessor(registervtkXMLUniformGridAMRReader());
    module->registerProcessor(registervtkXMLUnstructuredGridReader());
    module->registerProcessor(registervtkYoungsMaterialInterface());
}

}  // namespace vtkwrapper
}  // namespace inviwo
