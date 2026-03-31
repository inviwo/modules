import astroviscommon
from astroviscommon import FitsParams, LatLong

import inviwopy as ivw
import ivwbase
from inviwopy import glm

import numpy as np
from contextlib import suppress
from collections.abc import Callable


def createDepthMesh(params: FitsParams,
                    tf: ivw.data.TransferFunction,
                    lat_long: LatLong,
                    assumed_distance: float = 123.0,
                    velocity_inf: float = 14.5,
                    velocity_star: float = -26.5
                    ) -> (ivw.data.Mesh, np.ndarray, (int, int)):
    """
    Create a Mesh containing the individual channels with the distorted z axis based on a
    depth estimation of the channel velocity

    Parameters
    ----------
    params : FitsParams
        Parameters extracted from a fits.header.Header
    tf : ivw.data.TransferFunction
        Transferfunction to color the mesh according to depth
    assumed_distance : float, optional
        in parsecs. The default is 123.0.
    velocity_inf: float, optional
        in km/s. The default is 14.5.
    velocity_star: float, optional
        in km/s. The default is -26.5.

    Returns
    -------
    A tuple containing the Mesh, min/max depth values (np.ndarray), and
    first and last valid channel index (int, int) where |velocity - velocity_star| <= velocity_inf
    """

    velocity_func: Callable[[float], int] = astroviscommon.frequencyToVelocityFunc(params)

    dims = np.array(params.naxis)

    positions: list[np.ndarray] = []

    v = np.array([velocity_func(i) for i in range(0, dims[2])])
    valid_channels = np.argwhere(np.abs(v - velocity_star) <= velocity_inf).squeeze()
    if len(valid_channels) == 0:
        raise ValueError(
            'No valid velocity channels found based on |velocity - velocity_star| <= velocity_inf')

    mesh_count: int = 0
    mesh = ivw.data.Mesh(dt=ivw.data.DrawType.Triangles, ct=ivw.data.ConnectivityType.Unconnected)
    for i in valid_channels:
        with suppress(ValueError):
            with np.nditer(astroviscommon.estimateDepth(params, velocity_func(i)),
                           flags=['multi_index']) as it:
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
        return (mesh, np.array([0, 0]), (valid_channels[0], valid_channels[-1]))

    positions_np = np.array(positions)
    min_max = [np.min(positions_np[:, 2]), np.max(positions_np[:, 2])]
    # ivw.logInfo(f'min/max: {min_max}')
    depth_scaling = np.max(np.abs(min_max))

    colors = [tf.sample(value / depth_scaling * 0.5 + 0.5) for value in positions_np[:, 2]]

    extent, offset, _ = astroviscommon.getLatLongBasis(params, lat_long)
    ivw.logInfo(f'{extent=}\n{offset=}')
    positions_np = (positions_np * np.array([extent[0] / (dims[0] - 1),
                                             extent[1] / (dims[1] - 1), 1.0])
                    + np.array([offset[0], offset[1], 0.0]))

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
    # w[0][0] = 1.0 / extent[0]
    # w[1][1] = 1.0 / extent[1]
    # w[2][2] = 2.0 / (min_max[1] - min_max[0])
    # w[3] = [-0.5, -0.5, -0.5, 1.0]

    mesh.modelMatrix = m
    mesh.worldMatrix = w

    return (mesh, np.array(min_max), (valid_channels[0], valid_channels[-1]))


def createFitsCompositeProperty(identifier: str,
                                displayName: str) -> ivw.properties.CompositeProperty:
    cb = ivw.properties.ConstraintBehavior
    inv = ivw.properties.InvalidationLevel
    semantics = ivw.properties.PropertySemantics

    objectName = ivw.properties.StringProperty(
        "objectName", "Object",
        ivw.md2doc(
            "Contents of the 'objects' field in the FITS Header."), "",
        invalidationLevel=inv.Valid)
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
    restFrequency = ivw.properties.FloatProperty("restFrequency", "Rest Frequency [Hz]",
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
    valueName = ivw.properties.StringProperty(
        "valueName", "btype",
        ivw.md2doc("Corresponds to the 'btype' field in the FITS Header."), "",
        invalidationLevel=inv.Valid)
    valueUnit = ivw.properties.StringProperty(
        "valueUnit", "bunit",
        ivw.md2doc("Corresponds to the 'bunit' field in the FITS Header."), "",
        invalidationLevel=inv.Valid)

    dataRange = ivwbase.properties.DataRangeProperty("dataRange", "Data Range")
    dataRange.help = ivw.md2doc("Min/max values of the dataset, ignoring NaN")
    dataRange.getPropertyByIdentifier("valueRange", True).visible = False
    dataRange.getPropertyByIdentifier("customValueRange", True).visible = False

    for p in [objectName, crpix3, crval3, cdelt3, restFrequency, extent, offset,
              valueName, valueUnit]:
        p.readOnly = True

    prop = ivw.properties.CompositeProperty(identifier, displayName)
    prop.addProperties([objectName, assumedDistance, velocityInf, velocityStar,
                        crpix3, crval3, cdelt3, restFrequency,
                        extent, offset, valueName, valueUnit, dataRange])
    return prop
