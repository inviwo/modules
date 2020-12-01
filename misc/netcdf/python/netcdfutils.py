# import inviwopy
import numpy
from pathlib import Path
# from scipy.io import netcdf
import sys

from netCDF4 import Dataset
# from inviwopy.glm import dvec2, vec4, mat4
# from inviwopy.data import VolumeOutport
# from inviwopy.data import Volume


"""
This script is part of the NetCDF Source processor.
It will be run on construction and whenever this script changes.
The PythonScriptProcessor is exposed as the local variable 'self'.
We bind functions here to onChange for properties.
"""


def printInfo(filePath):
    with Dataset(filePath, "r", format="NETCDF4") as nc:
        print(nc)
        # for dim in nc.dimensions:
        #     print(dim)
