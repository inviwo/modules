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

        cb = ivw.properties.ConstraintBehavior
        semantics = ivw.properties.PropertySemantics

        self.addInport(ivw.PythonInport("fitsdata"))
        self.addOutport(ivw.data.MeshOutport("depthmesh"))

        self.fitsParameters = ivwastrovis.createFitsCompositeProperty(
            "fitsParameters", "Fits Parameters")
        self.validChannelRange = ivw.properties.IntVec2Property(
            "validChannelRange", "Valid Channel Range",
            ivw.doc.Document("Range of valid channels based on the line-of-sight velocity"),
            glm.ivec2(0, 0),
            min=(glm.ivec2(0), cb.Ignore), max=(glm.ivec2(100), cb.Ignore),
            semantics=semantics.Text)
        self.axisScaling = ivw.properties.FloatProperty(
            "axisScaling", "Z Scaling",
            ivw.md2doc("Scaling factor applied to the z axis"),
            1.0, increment=0.001, min=(1.0e-8, cb.Immutable), max=(10.0, cb.Ignore))
        self.tf = ivw.properties.TransferFunctionProperty(
            "colormap", "Mesh Colormap",
            ivw.doc.Document("Colormap used for coloring the frequency slices according to depth"))

        self.addProperties([self.fitsParameters, self.validChannelRange, self.axisScaling, self.tf])

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
        depth_mesh, min_max, valid_channel_range = ivwastrovis.createDepthMesh(
            fits_data.params, self.tf.value,
            LatLong.RelativeArcsec,
            assumed_distance=self.fitsParameters.assumedDistance.value,
            velocity_inf=self.fitsParameters.velocityInf.value,
            velocity_star=self.fitsParameters.velocityStar.value)
        elapsed = time.perf_counter_ns() - t
        print(f'{elapsed/1000_000.0} ms')

        extent, offset, unit = astroviscommon.getLatLongBasis(
            fits_data.params, LatLong.RelativeArcsec)
        axes = [("x", unit), ("y", unit), ("z", "au")]
        for i, (label, unit) in enumerate(axes):
            depth_mesh.axes[i].name = label
            depth_mesh.axes[i].unit = ivw.data.Unit(unit)

        # m = ivw.glm.mat4(1.0)
        w = ivw.glm.mat4(1.0)
        w[0][0] = 1.0 / extent[0]
        w[1][1] = 1.0 / extent[1]
        w[2][2] = self.axisScaling.value / (min_max[1] - min_max[0])
        w[3] = [-0.5 + offset[0] * w[0][0], -0.5 + offset[1] * w[1][1],
                -0.5 + (-min_max[0]) * w[2][2], 1.0]

        # depth_mesh.modelMatrix = m
        depth_mesh.worldMatrix = w

        extent, offset, _ = astroviscommon.getLatLongBasis(fits_data.params, LatLong.RelativeArcsec)
        extent *= self.fitsParameters.assumedDistance.value
        offset *= self.fitsParameters.assumedDistance.value
        self.fitsParameters.dataExtent.value = glm.vec3(
            extent[0], extent[1], min_max[1] - min_max[0])
        self.fitsParameters.dataOffset.value = glm.vec3(offset[0], offset[1], min_max[0])
        self.validChannelRange.value = glm.ivec2(valid_channel_range)

        self.outports.depthmesh.setData(depth_mesh)
