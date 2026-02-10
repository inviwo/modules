
import numpy as np
from pathlib import Path
from collections.abc import Callable
from enum import Enum
from dataclasses import dataclass
from functools import lru_cache
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


def extractFITSParams(header: fits.header.Header) -> FitsParams:
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


@lru_cache(maxsize=2)
def readFITSData(filepath: Path,
                 dataset_index: int | None = None,
                 **kwargs) -> tuple[FitsParams, np.ndarray]:
    if not dataset_index:
        dataset_index = 0
    with fits.open(filepath, **kwargs) as hdul:
        fits_parameters: FitsParams = extractFITSParams(hdul[dataset_index].header)
        data: np.ndarray = hdul[dataset_index].data  # TODO: copy data?
        return (fits_parameters, data)


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


def estimateDepth(params: FitsParams,
                  velocity: float,
                  assumed_distance: float = 123.0,
                  velocity_inf: float = 14.5,
                  velocity_star: float = -26.5) -> np.ndarray:
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
