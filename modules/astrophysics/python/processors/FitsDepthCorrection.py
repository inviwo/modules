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
        IntOption = ivw.properties.IntOption
        semantics = ivw.properties.PropertySemantics

        self.addInport(ivw.PythonInport("fitsdata"))
        self.addOutport(ivw.data.MeshOutport("depthmesh"))

        self.fitsParameters = ivwastrovis.createFitsCompositeProperty(
            "fitsParameters", "Fits Parameters")

        options = [IntOption("absolute", "Absolute (degree)", LatLong.AbsoluteDeg.value),
                   IntOption("fractionalArcsec", "Fractional (arcsec)",
                             LatLong.FractionalArcsec.value),
                   IntOption("relativeArcsec", "Relative (arcsec)", LatLong.RelativeArcsec.value)]
        self.latLonCoords = ivw.properties.OptionPropertyInt(
            "latLonCoords", "Lat/Long Coordinates",
            help=ivw.unindentMd2doc(r'''Defines how Latitude and Longitude coordinates are shown.

* _Absolute_ full coordinate in decimal degrees
* _Relative (arcsec)_ coordinates in arcseconds relative to the star/center
* _Relative (arcsec)_ coordinates in arcseconds relative to the star/center'''),
            options=options, selectedIndex=2)

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

        self.addProperties([self.fitsParameters, self.validChannelRange,
                            self.latLonCoords, self.axisScaling, self.tf])

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
        latlong_calc: LatLong = LatLong.RelativeArcsec
        depth_mesh, min_max, valid_channel_range = ivwastrovis.createDepthMesh(
            fits_data.params, self.tf.value,
            latlong_calc,
            assumed_distance=self.fitsParameters.assumedDistance.value,
            velocity_inf=self.fitsParameters.velocityInf.value,
            velocity_star=self.fitsParameters.velocityStar.value)
        elapsed = time.perf_counter_ns() - t
        print(f'{elapsed/1000_000.0} ms')

        # extent and offset in same coord system as the depth mesh
        mesh_extent, mesh_offset, _ = astroviscommon.getLatLongBasis(fits_data.params, latlong_calc)
        extent_au = mesh_extent * self.fitsParameters.assumedDistance.value
        offset_au = mesh_offset * self.fitsParameters.assumedDistance.value
        self.fitsParameters.dataExtent.value = glm.vec3(
            extent_au[0], extent_au[1], min_max[1] - min_max[0])
        self.fitsParameters.dataOffset.value = glm.vec3(
            offset_au[0], offset_au[1], min_max[0])
        self.validChannelRange.value = glm.ivec2(valid_channel_range)

        # extent and offset for positioning and axes
        extent, offset, unit = astroviscommon.getLatLongBasis(
            fits_data.params, LatLong(self.latLonCoords.value))
        axes = [("x", unit), ("y", unit), ("z", "au")]
        for i, (label, unit) in enumerate(axes):
            depth_mesh.axes[i].name = label
            depth_mesh.axes[i].unit = ivw.data.Unit(unit)

        ivw.logInfo(f'd: {extent=}\n{offset=}')

        m = ivw.glm.mat4(1.0)
        m[0][0] = extent[0] / mesh_extent[0]
        m[1][1] = extent[1] / mesh_extent[1]
        if LatLong(self.latLonCoords.value) == LatLong.AbsoluteDeg:
            # TODO: why without mesh_offset?
            m[3] = [offset[0], offset[1], 1.0, 1.0]
        else:
            m[3] = [offset[0] - mesh_offset[0], offset[1] - mesh_offset[1], 1.0, 1.0]

        w = ivw.glm.mat4(1.0)
        w[0][0] = 1.0 / extent[0]
        w[1][1] = 1.0 / extent[1]
        w[2][2] = self.axisScaling.value / (min_max[1] - min_max[0])
        w[3] = [- m[3][0] * w[0][0],
                - m[3][1] * w[1][1],
                -0.5 - min_max[0] * w[2][2], 1.0]
        depth_mesh.modelMatrix = m
        depth_mesh.worldMatrix = w

        self.outports.depthmesh.setData(depth_mesh)
