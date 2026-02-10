# Name: FitsVolumeSource

import inviwopy as ivw
from inviwopy import glm
import ivwbase

import numpy as np
from pathlib import Path
from collections.abc import Callable
from enum import Enum
from dataclasses import dataclass
from contextlib import suppress
from astropy.io import fits


class LatLong(Enum):
    AbsoluteDeg = 0
    RelativeDeg = 1
    RelativeArcsec = 2


@dataclass
class FitsParams:
    naxis: tuple[int, ...]
    ctype: tuple[str, str, str]
    cunit: tuple[str, str, str]
    crpix: tuple[float, float, float]
    crval: tuple[float, float, float]
    cdelt: tuple[float, float, float]
    btype: str
    bunit: str
    rest_frequency: float  # [Hz]


def extractFitsParams(header: fits.header.Header) -> FitsParams:
    rest_frequency: float = header['restfrq']  # equals 345_339_760_000
    # mean difference 0.0080740896
    # rest_frequency = 345_339_769_300  # [hz]

    return FitsParams(
        naxis=tuple(header[f'naxis{i}'] for i in range(1, header['naxis'] + 1)),
        ctype=tuple(header[f'ctype{i}'].lower() for i in range(1, 4)),
        cunit=tuple(header[f'cunit{i}'] for i in range(1, 4)),
        crpix=tuple(header[f'crpix{i}'] for i in range(1, 4)),
        crval=tuple(header[f'crval{i}'] for i in range(1, 4)),
        cdelt=tuple(header[f'cdelt{i}'] for i in range(1, 4)),
        btype=header['btype'],
        bunit=header['bunit'],
        rest_frequency=rest_frequency)


def pixelCoordinatesFunc(params: FitsParams) -> Callable[[np.ndarray], [int, int]]:
    def func(i: int, j: int) -> np.ndarray:
        # FIXME: use i or (i+1) ?
        alpha = params.crval[0] + params.cdelt[0] * (i - params.crpix[0])
        beta = params.crval[1] + params.cdelt[1] * (j - params.crpix[1])
        return np.array([alpha, beta])

    if not params.ctype[0] == 'ra---sin':
        raise ValueError(
            f'Expected unit type "ctype1" to be "RA---SIN". Found "{params.ctype[0]}".')
    if not params.ctype[1] == 'dec--sin':
        raise ValueError(
            f'Expected unit type "ctype2" to be "DEC--SIN". Found "{params.ctype[1]}".')
    return func


"""
Conversion of velocity axis to 3rd coordinate:
- In this case via the assumption of a radial velocity law of the form:
 |V - V*| = Vinf * (1-((Rw-r)/Rw)^2.5) for r<Rw and
 |V - V*| = Vinf for r>Rw
Vinf = 14.5 km/s
V* = -26.5 km/s
Rw = 54 au
(assumed distance 123 pc, so 1" = 123 au)

Since in this particular case, we're at scales larger than Rw we're in the
simplest case where the velocity is constant V=Vinf=14.5 km/s.

So the line of sight velocity Vlos = V - V* = Vinf * (z/r) where r=sqrt(x^2+y^2+z^2)

taking rho = sqrt(x^2+y^2) and mu = Vlos/Vinf we have
Z = +- rho * mu/sqrt(1-mu^2)

The positive and negative sign are for the blue-shifted (in front) and red-shifted
(behind the plane of the sky) parts of the envelope.
We should in this case disregard all velocity channels with |V-V*|=|Vlos|>Vinf.
And remember for the coordinate transformation from (relative to the center of
the emission) arcseconds to au is using 1"=123 au).
"""


def estimateDepth(params: FitsParams,
                  velocity: float,
                  assumed_distance: float = 123.0,
                  velocity_inf: float = 14.5,
                  velocity_star: float = -26.5) -> np.ndarray:
    """

    Parameters
    ----------
    params : FitsParams
        Parameters extracted from a fits.header.Header
    velocity : float
        in km/s.
    assumed_distance : float, optional
        in parsecs. The default is 123.0.
    velocity_inf: float, optional
        in km/s. The default is 14.5.
    velocity_star: float, optional
        in km/s. The default is -26.5.


    Returns
    -------
    2D array of depth values for the given velocity

    """
    v_los: float = velocity - velocity_star
    if np.abs(v_los) > velocity_inf:
        raise ValueError(f'v_los={v_los} exceeds v_inf={velocity_inf}')

    mu: float = v_los / velocity_inf

    coords: Callable[[np.ndarray], [int, int]] = pixelCoordinatesFunc(params)
    # coordinates are given in degree, convert to arcseconds then parsecs (au)
    delta: np.ndarray = (coords(1, 1) - coords(0, 0)) * \
        3600.0 * assumed_distance
    dims = np.array(params.naxis[:2])
    center = (dims - 1) / 2.0

    depths = np.empty(dims, dtype=float)
    with np.nditer(depths, flags=['multi_index'], op_flags=['writeonly']) as it:
        for value in it:
            d = (it.multi_index - center) * delta
            rho: float = np.sqrt(d[0]**2 + d[1]**2)
            value[...] = rho * mu / np.sqrt(1.0 - mu**2)
    return depths


def getLatLongBasis(params: FitsParams,
                    lat_long: LatLong = LatLong.RelativeArcsec
                    ) -> tuple[np.ndarray, np.ndarray]:
    """
    Calculate the lat/long extent and offset given a fits header for pixel centers

    Parameters
    ----------
    params : FitsParams
        Parameters extracted from a fits.header.Header
    lat_long : LatLong, optional
        Determines the coordinate system (absolute/relative, degree/arcsec, ...).
        The default is LatLong.RelativeArcsec.

    Returns
    -------
    tuple[np.ndarray, np.ndarray]
        lat/long extent and offset

    """
    coords: Callable[[np.ndarray], [int, int]] = pixelCoordinatesFunc(params)
    origin: np.ndarray = coords(0, 0)
    delta: np.ndarray = coords(1, 1) - origin
    extent: np.ndarray = coords(params.naxis[0], params.naxis[1]) - origin

    for i in range(2):
        if extent[i] < 0.0:
            origin[i] += extent[i]
            extent[i] = -extent[i]
            delta[i] = -delta[i]

    if lat_long == LatLong.RelativeArcsec:
        extent *= 3600  # convert from degree to arcseconds

    if lat_long == LatLong.AbsoluteDeg:
        offset = origin - delta * 0.5
    elif lat_long in [LatLong.RelativeDeg, LatLong.RelativeArcsec]:
        offset = extent * 0.5
    else:
        raise ValueError(f'Invalid Lat/Long value {lat_long}')

    return (extent, offset)


def createDepthMesh(params: FitsParams,
                    velocity_func: Callable[[float], int],
                    tf: ivw.data.TransferFunction,
                    lat_long: LatLong,
                    assumed_distance: float = 123.0,
                    velocity_inf: float = 14.5,
                    velocity_star: float = -26.5
                    ) -> (ivw.data.Mesh, np.ndarray):
    dims = np.array(params.naxis)

    positions: list[np.ndarray] = []

    mesh_count: int = 0
    mesh = ivw.data.Mesh(dt=ivw.data.DrawType.Triangles, ct=ivw.data.ConnectivityType.Unconnected)
    for i in range(0, dims[2]):
        with suppress(ValueError):
            with np.nditer(estimateDepth(params, velocity_func(i)), flags=['multi_index']) as it:
                for value in it:
                    p = np.array([it.multi_index[0], it.multi_index[1], value])
                    positions.append(p)

            offset: int = mesh_count * dims[0] * dims[1]
            for y in range(dims[1] - 1):
                indices: list[int] = []
                for x in range(dims[0]):
                    indices.append(y * dims[0] + x + offset)
                    indices.append((y + 1) * dims[0] + x + offset)
                mesh.addIndices(ivw.data.MeshInfo(dt=ivw.data.DrawType.Triangles,
                                                  ct=ivw.data.ConnectivityType.Strip),
                                ivw.data.IndexBufferUINT32(np.array(indices, dtype=np.uint32)))
            mesh_count += 1

    if len(positions) == 0:
        return (mesh, np.array([0, 0]))

    positions_np = np.array(positions)
    min_max = [np.min(positions_np[:, 2]), np.max(positions_np[:, 2])]
    ivw.logInfo(f'min/max: {min_max}')
    depth_scaling = np.max(np.abs(min_max))
    # scaling = np.array([1.0 / dims[0], 1.0 / dims[1], 1.0 / depth_scaling])

    colors = [tf.sample(value / depth_scaling * 0.5 + 0.5) for value in positions_np[:, 2]]

    extent, offset = getLatLongBasis(params, lat_long)
    ivw.logInfo(f'{extent=}\n{offset=}')
    positions_np = (positions_np * np.array([extent[0] / (dims[0] - 1),
                                             extent[1] / (dims[1] - 1), 1.0])
                    - np.array([offset[0], offset[1], 0.0]))

    mesh.addBuffer(ivw.data.BufferType.PositionAttrib,
                   ivw.data.Buffer(np.array(positions_np, dtype=np.float32)))
    mesh.addBuffer(ivw.data.BufferType.ColorAttrib,
                   ivw.data.Buffer(np.array(colors, dtype=np.float32)))

    # m = ivw.glm.mat4([[1.0 / dims[0], 0.0, 0.0, 0.0],
    #                   [0.0, 1.0 / dims[1], 0.0, 0.0],
    #                   [0.0, 0.0, 1.0 / depth_scaling, 0.0],
    #                   [-0.5, -0.5, 0.0, 1.0]])

    m = ivw.glm.mat4(1.0)

    w = ivw.glm.mat4(1.0)
    w[2][2] = 1.0 / depth_scaling * 20.0
    # w[3] = [-0.5, -0.5, 0.0, 1.0]

    mesh.modelMatrix = m
    mesh.worldMatrix = w

    return (mesh, np.array(min_max))


def createFitsCompositeProperty(identifier: str,
                                displayName: str) -> ivw.properties.CompositeProperty:
    cb = ivw.properties.ConstraintBehavior
    inv = ivw.properties.InvalidationLevel
    semantics = ivw.properties.PropertySemantics

    assumedDistance = ivw.properties.FloatProperty("assumedDistance", "Assumed Distance [pc]",
                                                   ivw.md2doc("Assumed distance of the star in parsec [pc]."),  # noqa e501
                                                   123.0, increment=0.001,
                                                   min=(-100.0, cb.Ignore), max=(100.0, cb.Ignore),
                                                   semantics=semantics.Text)
    velocityInf = ivw.properties.FloatProperty("velocityInf", "v_infinity [km/s]",
                                               ivw.md2doc(
                                                   "The hyperbolic excess speed v_inf [km/s]."),
                                               14.5, increment=0.001,
                                               min=(-100.0, cb.Ignore), max=(100.0, cb.Ignore),
                                               semantics=semantics.Text)
    velocityStar = ivw.properties.FloatProperty("velocityStar", "v_star [km/s]",
                                                ivw.md2doc("v_star [km/s]"),
                                                -26.5, increment=0.001,
                                                min=(-100.0, cb.Ignore), max=(100.0, cb.Ignore),
                                                semantics=semantics.Text)
    crpix3 = ivw.properties.FloatProperty("crpix3", "crpix3",
                                          ivw.md2doc("Reference pixel in axis3"),
                                          0.0, increment=0.001,
                                          min=(-100.0, cb.Ignore), max=(100.0, cb.Ignore),
                                          semantics=semantics.Text,
                                          invalidationLevel=inv.Valid)
    crval3 = ivw.properties.FloatProperty("crval3", "crval3",
                                          ivw.md2doc("Physical value of the reference pixel"),
                                          0.0, increment=0.001,
                                          min=(-100.0, cb.Ignore), max=(100.0, cb.Ignore),
                                          semantics=semantics.Text,
                                          invalidationLevel=inv.Valid)
    cdelt3 = ivw.properties.FloatProperty("cdelt3", "cdelt3",
                                          ivw.md2doc(""),
                                          0.0, increment=0.001,
                                          min=(-100.0, cb.Ignore), max=(100.0, cb.Ignore),
                                          semantics=semantics.Text,
                                          invalidationLevel=inv.Valid)
    restFrequency = ivw.properties.FloatProperty("restFrequency", "Rest Frequency [Hz",
                                                 ivw.md2doc("restfreq [Hz]"),
                                                 0.0, increment=0.001,
                                                 min=(-100.0, cb.Ignore), max=(100.0, cb.Ignore),
                                                 semantics=semantics.Text,
                                                 invalidationLevel=inv.Valid)
    extent = ivw.properties.FloatVec3Property("dataExtent", "Extent [au]",
                                              ivw.md2doc("Extent of the dataset in au."),
                                              glm.vec3(0.0), increment=glm.vec3(0.001),
                                              min=(glm.vec3(-100.0), cb.Ignore),
                                              max=(glm.vec3(100.0), cb.Ignore),
                                              semantics=semantics.Text,
                                              invalidationLevel=inv.Valid)
    offset = ivw.properties.FloatVec3Property("dataOffset", "Offset [au]",
                                              ivw.md2doc("Coordinate of the lower left corner of the dataset in au."),  # noqa e501
                                              glm.vec3(0.0), increment=glm.vec3(0.001),
                                              min=(glm.vec3(-100.0), cb.Ignore),
                                              max=(glm.vec3(100.0), cb.Ignore),
                                              semantics=semantics.Text,
                                              invalidationLevel=inv.Valid)
    dataRange = ivw.properties.DoubleMinMaxProperty("dataRange",
                                                    "Data Range",
                                                    0.0, 1.0, -1.70e308, 1.79e308,
                                                    increment=0.0001,
                                                    semantics=semantics.Text,
                                                    invalidationLevel=inv.Valid)

    for p in [crpix3, crval3, cdelt3, restFrequency, extent, offset, dataRange]:
        p.readOnly = True

    prop = ivw.properties.CompositeProperty(identifier, displayName)
    prop.addProperties([assumedDistance, velocityInf, velocityStar,
                        crpix3, crval3, cdelt3, restFrequency,
                        extent, offset, dataRange])
    return prop


class FitsVolumeSource(ivw.Processor):
    """
    Reads a FITS file and outputs the first array as a Volume
    """

    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.deserializing = False

        self.outport = ivw.data.VolumeOutport("Intensity")
        self.addOutport(self.outport)

        self.depth = ivw.data.MeshOutport("depth")
        self.addOutport(self.depth)

        self.filename = ivw.properties.FileProperty(
            "filename", "Filename", "", "fits")
        self.filename.addNameFilter(ivw.properties.FileExtension("fits", "FITS file format"))

        options = [ivw.properties.IntOption("absolute", "Absolute (degree)", LatLong.AbsoluteDeg.value),  # noqa e501
                   ivw.properties.IntOption("relativeDeg", "Relative (degree)", LatLong.RelativeDeg.value),  # noqa e501
                   ivw.properties.IntOption("relativeArcsec", "Relative (arcsec)", LatLong.RelativeArcsec.value)]  # noqa e501
        self.latLonCoords = ivw.properties.OptionPropertyInt(
            "latLonCoords", "Lat/Long Coordinates",
            help=ivw.unindentMd2doc(r'''Defines how Latitude and Longitude coordinates are shown.

* _Absolute_ full coordinate in decimal degrees
* _Relative (degree)_ coordinates in arcseconds relative to the star/center
* _Relative (arcsec)_ coordinates in arcseconds relative to the star/center'''),
            options=options, selectedIndex=2)

        self.fitsParameters = createFitsCompositeProperty("fitsParameters", "Fits Parameters")

        self.basis = ivwbase.properties.BasisProperty("basis", "Basis and offset")
        self.information = ivwbase.properties.VolumeInformationProperty(
            "information", "Data information")

        self.tf = ivw.properties.TransferFunctionProperty("colormap", "Mesh Colormap",
                                                          ivw.doc.Document("Colormap used for coloring the frequency slices according to depth"))  # noqa e501
        self.addProperties([self.filename, self.fitsParameters, self.latLonCoords, self.tf,
                            self.basis, self.information])

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.FitsVolumeSource",
            displayName="FitsVolumeSource",
            category="Python",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags.PY,
            help=ivw.unindentMd2doc(FitsVolumeSource.__doc__)
        )

    def getProcessorInfo(self):
        return FitsVolumeSource.processorInfo()

    def initializeResources(self):
        pass

    @staticmethod
    def createVolume(data: np.array) -> ivw.data.Volume:
        # data = np.nan_to_num(data)
        volumerep = ivw.data.VolumePy(np.copy(data.astype(dtype=np.float32), order='C'))

        volume = ivw.data.Volume(volumerep)
        volume.swizzlemask = ivw.data.SwizzleMask.defaultData(1)

        # TODO: extract extent from hdul[0].header
        volume.basis = ivw.glm.mat3(1)
        volume.offset = ivw.glm.vec3(-0.5, -0.5, -0.5)

        datarange = ivw.glm.dvec2(np.nanmin(data), np.nanmax(data))
        volume.dataMap.dataRange = datarange
        volume.dataMap.valueRange = datarange
        return volume

    @staticmethod
    def frequencyToVelocityFunc(params: FitsParams) -> Callable[[float], int]:
        def func(channel: int) -> float:
            freq: int = (params.rest_frequency
                         - ((channel + 1 - params.crpix[2]) * params.cdelt[2] + params.crval[2]))
            return vc * freq / params.rest_frequency / 1000

        vc = 299_792_458  # speed of light
        return func

    def process(self):
        if not self.filename.value:
            return

        filename: Path = ivwbase.io.downloadAndCacheIfUrl(self.filename.valueAsString())
        if not filename.exists():
            return

        with fits.open(filename) as hdul:
            dim = hdul[0].data.shape

            fits_parameters: FitsParams = extractFitsParams(hdul[0].header)

            func: Callable[[float], int] = FitsVolumeSource.frequencyToVelocityFunc(fits_parameters)
            velocity: list = [func(channel) for channel in range(dim[0])]
            print(f'Velocity matching frequency: {velocity}')

            depth_mesh, min_max = \
                createDepthMesh(fits_parameters, func, self.tf.value,
                                LatLong(self.latLonCoords.value),
                                assumed_distance=self.fitsParameters.assumedDistance.value,
                                velocity_inf=self.fitsParameters.velocityInf.value,
                                velocity_star=self.fitsParameters.velocityStar.value)

            volume_intensity = FitsVolumeSource.createVolume(hdul[0].data)
            volume_intensity.dataMap.valueAxis.name = 'Intensity'
            # hdul[0].header['bunit'] is 'Jy/beam ', which is not a valid unit in Inviwo
            volume_intensity.dataMap.valueAxis.unit = ivw.data.Unit(
                fits_parameters.bunit.replace('/beam', ''))

            extent, offset = getLatLongBasis(fits_parameters, LatLong(self.latLonCoords.value))

            m = ivw.glm.mat4(1.0)
            m[0][0] = extent[0]
            m[1][1] = extent[1]
            m[2][2] = np.abs(velocity[-1] - velocity[0])
            m[3] = ivw.glm.vec4(offset[0], offset[1], velocity[0], 1.0)

            axes = [("x", "\""), ("y", "\""), ("Velocity", "km/s")]
            for i, (label, unit) in enumerate(axes):
                volume_intensity.axes[i].name = label
                volume_intensity.axes[i].unit = ivw.data.Unit(unit)

            w = ivw.glm.mat4(1.0)
            w[2][2] = 2.0 / m[2][2]
            # w[3] = [-0.5, -0.5, 0.0, 1.0]
            volume_intensity.modelMatrix = m
            volume_intensity.worldMatrix = w

            self.fitsParameters.crpix3.value = fits_parameters.crpix[2]
            self.fitsParameters.crval3.value = fits_parameters.crval[2]
            self.fitsParameters.cdelt3.value = fits_parameters.cdelt[2]
            self.fitsParameters.restFrequency.value = fits_parameters.rest_frequency
            self.fitsParameters.dataRange.value = volume_intensity.dataMap.dataRange

            extent, offset = getLatLongBasis(fits_parameters, LatLong.RelativeArcsec)
            extent *= self.fitsParameters.assumedDistance.value
            offset *= self.fitsParameters.assumedDistance.value
            self.fitsParameters.dataExtent.value = glm.vec3(extent[0], extent[1],
                                                            min_max[1] - min_max[0])
            self.fitsParameters.dataOffset.value = glm.vec3(offset[0], offset[1], min_max[0])

            from inviwopy.properties import OverwriteState as ows

            self.basis.updateForNewEntity(volume_intensity, self.deserializing)
            self.information.updateForNewVolume(
                volume_intensity, ows.Yes if self.deserializing else ows.No)
            self.basis.updateEntity(volume_intensity)
            self.information.updateVolume(volume_intensity)
            self.deserializing = False

            self.outport.setData(volume_intensity)
            self.depth.setData(depth_mesh)

    def deserialize(self, deserializer: ivw.Deserializer):
        ivw.Processor.deserialize(self, deserializer)
        self.deserializing = True
