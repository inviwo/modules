# Name: FitsDepthCorrection

import inviwopy as ivw
from inviwopy import glm
import astroviscommon
from astroviscommon import LatLong
import ivwastrovis

import time


class FitsDepthCorrection(ivw.Processor):
    """
    Reconstructs the depth information given by a FITS image stack and a set of FitsParameters.

    The corresponding spatial extent in au is also made available via the extent and
    offset properties.
    """

    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)

        self.addInport(ivw.PythonInport("fitsdata"))
        self.addOutport(ivw.data.MeshOutport("depthmesh"))

        self.fitsParameters = ivwastrovis.createFitsCompositeProperty(
            "fitsParameters", "Fits Parameters")
        self.tf = ivw.properties.TransferFunctionProperty(
            "colormap", "Mesh Colormap",
            ivw.doc.Document("Colormap used for coloring the frequency slices according to depth"))

        self.addProperties([self.fitsParameters, self.tf])

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.FitsDepthCorrection",
            displayName="FitsDepthCorrection",
            category="Python",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags("PY, FITS, Astrophysics"),
            help=ivw.unindentMd2doc(FitsDepthCorrection.__doc__)
        )

    def getProcessorInfo(self):
        return FitsDepthCorrection.processorInfo()

    def initializeResources(self):
        pass

    def process(self):
        fits_data = self.inports.fitsdata.getData()
        if not isinstance(fits_data, astroviscommon.FitsData):
            raise ValueError(f"Invalid input data of type '{type(fits_data)}', "
                             f"expected '{astroviscommon.FitsData}'.")

        t = time.perf_counter_ns()
        depth_mesh, min_max = ivwastrovis.createDepthMesh(
            fits_data.params, self.tf.value,
            LatLong.RelativeArcsec,
            assumed_distance=self.fitsParameters.assumedDistance.value,
            velocity_inf=self.fitsParameters.velocityInf.value,
            velocity_star=self.fitsParameters.velocityStar.value)
        elapsed = time.perf_counter_ns() - t
        print(f'{elapsed/1000_000.0} ms')

        extent, offset = astroviscommon.getLatLongBasis(fits_data.params, LatLong.RelativeArcsec)
        extent *= self.fitsParameters.assumedDistance.value
        offset *= self.fitsParameters.assumedDistance.value
        self.fitsParameters.dataExtent.value = glm.vec3(
            extent[0], extent[1], min_max[1] - min_max[0])
        self.fitsParameters.dataOffset.value = glm.vec3(offset[0], offset[1], min_max[0])

        self.outports.depthmesh.setData(depth_mesh)
