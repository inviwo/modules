import numpy
from pathlib import Path
import sys

from netCDF4 import Dataset


"""
This script is part of the NetCDF Source processor.
It will be run on construction and whenever this script changes.
The PythonScriptProcessor is exposed as the local variable 'self'.
We bind functions here to onChange for properties.
"""


def printInfo(filePath):
    with Dataset(filePath, "r", format="NETCDF4") as nc:
        print(nc)
        print("- Dimensions -")
        for dim in nc.dimensions:
            print(dim)
        print("- Variables -")
        for v in nc.variables:
            print(v)
            print('----------')
