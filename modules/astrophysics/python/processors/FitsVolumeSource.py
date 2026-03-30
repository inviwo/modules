# Name: FitsVolumeSource

import inviwopy as ivw
from inviwopy import glm
import ivwbase
import astroviscommon
from astroviscommon import LatLong
import ivwastrovis

import numpy as np
from pathlib import Path
from collections.abc import Callable
from enum import Enum

import time


class AxisType(Enum):
    SliceIndex = 0
    Dataset = 1
    Velocity = 2


class FitsVolumeSource(ivw.Processor):
    """
    Reads a FITS file and outputs the first array as a Volume
    """

    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.deserializing = False

        self.addOutport(ivw.data.VolumeOutport("intensity"))
        self.addOutport(ivw.PythonOutport("fitsdata"))

        self.filename = ivw.properties.FileProperty(
            "filename", "Filename", "", "fits")
        self.filename.addNameFilter(ivw.properties.FileExtension("fits", "FITS file format"))

        cb = ivw.properties.ConstraintBehavior
        IntOption = ivw.properties.IntOption
        inv = ivw.properties.InvalidationLevel
        semantics = ivw.properties.PropertySemantics

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

        options = [IntOption("slice", "Slice Indices", AxisType.SliceIndex.value),
                   IntOption("dataset", "From DataSet", AxisType.Dataset.value),
                   IntOption("velocity", "Velocity [km/s]", AxisType.Velocity.value)]
        self.axisType = ivw.properties.OptionPropertyInt(
            "axisType", "Z Axis",
            help=ivw.unindentMd2doc(r'''Type of the z axis, that is depth.

* _Slice Indices_ zero-based indices of the 2D slice images
* _From Dataset__ use the `ctype3` and `cunit3` fields of the FITS dataset, e.g. frequency [Hz]
* _Velocity_  velocity derived from the frequency channels [km/s]'''),
            options=options, selectedIndex=1)
        self.axisScaling = ivw.properties.FloatProperty(
            "axisScaling", "Z Scaling",
            ivw.md2doc("Scaling factor applied to the z axis"),
            1.0, increment=0.001, min=(1.0e-8, cb.Immutable), max=(10.0, cb.Ignore))

        self.zaxisRange = ivw.properties.DoubleVec2Property(
            "zaxisRange", "Z Axis Range",
            ivw.md2doc("Extent of the dataset along the z axis."),
            glm.dvec2(0.0, 1.0), increment=glm.dvec2(0.001),
            min=(glm.dvec2(-1.70e308), cb.Ignore),
            max=(glm.dvec2(1.79e308), cb.Ignore),
            semantics=semantics.Text, invalidationLevel=inv.Valid)
        self.zaxisRange.readOnly = True

        self.fitsParameters = ivwastrovis.createFitsCompositeProperty(
            "fitsParameters", "Fits Parameters")

        self.basis = ivwbase.properties.BasisProperty("basis", "Basis and offset")
        self.information = ivwbase.properties.VolumeInformationProperty(
            "information", "Data information")

        self.addProperties([self.filename, self.fitsParameters,
                            self.latLonCoords, self.axisType, self.zaxisRange, self.axisScaling,
                            self.basis, self.information])

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.FitsVolumeSource",
            displayName="FitsVolumeSource",
            category="Python",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags("PY, FITS, Astrophysics"),
            help=ivw.unindentMd2doc(FitsVolumeSource.__doc__)
        )

    def getProcessorInfo(self):
        return FitsVolumeSource.processorInfo()

    def initializeResources(self):
        pass

    @staticmethod
    def createVolume(fits_data: astroviscommon.FitsData, flip_z: bool = False) -> ivw.data.Volume:
        # data = np.nan_to_num(data)
        data_float = fits_data.data.astype(dtype=np.float32)
        # if flip_z:
        #     data_float = np.flip(data_float, axis=0)
        volumerep = ivw.data.VolumePy(np.copy(data_float, order='C'))

        volume = ivw.data.Volume(volumerep)
        volume.swizzlemask = ivw.data.SwizzleMask.defaultData(1)

        datarange = ivw.glm.dvec2(np.nanmin(fits_data.data), np.nanmax(fits_data.data))
        volume.dataMap.dataRange = datarange
        volume.dataMap.valueRange = datarange
        return volume

    def process(self):
        if not self.filename.value:
            return

        filename: Path = ivwbase.io.downloadAndCacheIfUrl(self.filename.valueAsString())
        if not filename.exists():
            return

        astroviscommon.readFITSData.cache_clear()

        t = time.perf_counter_ns()
        fits_data: astroviscommon.FitsData = astroviscommon.readFITSData(filename)
        dim = fits_data.data.shape
        elapsed = time.perf_counter_ns() - t
        print(f'{elapsed/1000.0} us')

        print(astroviscommon.readFITSData.cache_info())

        vel_func: Callable[[float], int] = astroviscommon.frequencyToVelocityFunc(fits_data.params)
        velocity: list = [vel_func(channel) for channel in range(dim[0])]
        print(f'Velocity matching frequency: {velocity}')

        extent_xy, offset_xy, unit = astroviscommon.getLatLongBasis(
            fits_data.params, LatLong(self.latLonCoords.value))

        axes = [("x", unit), ("y", unit), ("z", "")]

        extent_z: float = 0.0
        offset_z: float = 0.0
        # TODO: cannot swap basis function here as this will affect the slice order!
        match AxisType(self.axisType.value):
            case AxisType.SliceIndex:
                extent_z = float(dim[0] - 1)
                offset_z = 0
                axes[2] = ("Channel", "")
            case AxisType.Dataset:
                freq_func = astroviscommon.frequencyFunc(fits_data.params)
                freq_range = (freq_func(0), freq_func(dim[0] - 1))
                extent_z = freq_range[1] - freq_range[0]
                offset_z = freq_range[0]
                axes[2] = (fits_data.params.ctype[2], fits_data.params.cunit[2])
            case AxisType.Velocity:
                vel_range = (vel_func(0), vel_func(dim[0] - 1))
                extent_z = vel_range[1] - vel_range[0]
                offset_z = vel_range[0]
                axes[2] = ("Velocity", "km/s")
            case _:
                raise ValueError(f"Unexpected axis type {self.axisType.value}")

        volume_intensity = FitsVolumeSource.createVolume(fits_data)
        volume_intensity.dataMap.valueAxis.name = fits_data.params.btype
        # hdul[0].header['bunit'] is 'Jy/beam ', which is not a valid unit in Inviwo
        volume_intensity.dataMap.valueAxis.unit = ivw.data.Unit(
            fits_data.params.bunit.replace('/beam', ''))

        for i, (label, unit) in enumerate(axes):
            volume_intensity.axes[i].name = label
            volume_intensity.axes[i].unit = ivw.data.Unit(unit)

        m = ivw.glm.dmat4(1.0)
        m[0][0] = extent_xy[0]
        m[1][1] = extent_xy[1]
        m[2][2] = extent_z
        m[3] = ivw.glm.dvec4(offset_xy[0], offset_xy[1], offset_z, 1.0)

        # re-normalize model transformations using the world matrix
        w = ivw.glm.dmat4(1.0)
        w[0][0] = 1.0 / extent_xy[0]
        w[1][1] = 1.0 / extent_xy[1]
        w[2][2] = self.axisScaling.value / extent_z
        w[3] = [-0.5 - m[3][0] * w[0][0], -0.5 - m[3][1] * w[1][1], -0.5 - m[3][2] * w[2][2], 1.0]
        volume_intensity.modelMatrix = ivw.glm.mat4(m)
        volume_intensity.worldMatrix = ivw.glm.mat4(w)

        # print(w * m, extent_z * (self.axisScaling.value / extent_z))

        self.zaxisRange.value = glm.dvec2(offset_z, offset_z + extent_z)

        self.fitsParameters.objectName.value = fits_data.params.objectName
        self.fitsParameters.crpix3.value = fits_data.params.crpix[2]
        self.fitsParameters.crval3.value = fits_data.params.crval[2]
        self.fitsParameters.cdelt3.value = fits_data.params.cdelt[2]
        self.fitsParameters.restFrequency.value = fits_data.params.rest_frequency
        self.fitsParameters.valueName.value = volume_intensity.dataMap.valueAxis.name
        self.fitsParameters.valueUnit.value = str(volume_intensity.dataMap.valueAxis.unit)
        self.fitsParameters.dataRange.dataRange = volume_intensity.dataMap.dataRange
        # update data range of volume in case of a custom range
        volume_intensity.dataMap.dataRange = self.fitsParameters.dataRange.dataRange
        volume_intensity.dataMap.valueRange = self.fitsParameters.dataRange.dataRange

        # determine lat/long extent in au
        extent_xy, offset_xy, _ = astroviscommon.getLatLongBasis(
            fits_data.params, LatLong.RelativeArcsec)
        extent_xy *= self.fitsParameters.assumedDistance.value
        offset_xy *= self.fitsParameters.assumedDistance.value
        self.fitsParameters.dataExtent.value = glm.vec3(extent_xy[0], extent_xy[1], 0.0)
        self.fitsParameters.dataOffset.value = glm.vec3(offset_xy[0], offset_xy[1], 0.0)

        from inviwopy.properties import OverwriteState as ows

        self.basis.updateForNewEntity(volume_intensity, self.deserializing)
        self.information.updateForNewVolume(
            volume_intensity, ows.Yes if self.deserializing else ows.No)
        self.basis.updateEntity(volume_intensity)
        self.information.updateVolume(volume_intensity)
        self.deserializing = False

        self.outports.intensity.setData(volume_intensity)
        self.outports.fitsdata.setData(fits_data)

    def deserialize(self, deserializer: ivw.Deserializer):
        ivw.Processor.deserialize(self, deserializer)
        self.deserializing = True
