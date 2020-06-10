import inviwopy as ivw
from inviwopy.properties import IntVec3Property, FileProperty, OptionPropertyString, ButtonProperty, BoolProperty, CompositeProperty, DoubleMinMaxProperty, IntMinMaxProperty
from inviwopy.glm import dvec2, mat4, vec4
import netcdfutils

import numpy
from pathlib import Path
from netCDF4 import Dataset


class GenericNetCDFSource(ivw.Processor):

    def initGeneric(self, id, name):
        ivw.Processor.__init__(self, id, name)

        self.data = []

        self.displayInfo = ButtonProperty("displayInfo", "Log File Info")

        self.filePath = ivw.properties.FileProperty(
            "filepath", "NetCDF Path", "", "netcdfdata")
        self.variables = CompositeProperty("variables", "Exported Variables")
        self.dimensions = CompositeProperty(
            "dimensions", "Restrict Dimensions")
        self.adjustDimensionsForStaggered = BoolProperty(
            'adjustForStaggered', 'Adjust for staggered climate grid', True)
        self.toFloat = BoolProperty(
            'toFloat', 'Convert to float', True)

        self.triggerReload = ButtonProperty("reload", "Reload")

        self.triggerReload.onChange(self.reloadData)
        self.displayInfo.onChange(self.displayDataInfo)

        self.addProperty(self.displayInfo)
        self.addProperty(self.filePath)
        self.addProperty(self.variables)
        self.addProperty(self.toFloat)
        self.addProperty(self.dimensions)
        self.addProperty(self.adjustDimensionsForStaggered)
        self.addProperty(self.triggerReload)

        self.overwriteDataRange = BoolProperty(
            "overwriteDataRange", "Overwrite Data Range", False)

        self.dataRange = DoubleMinMaxProperty(
            "dataRange", "Data Range", 0.0, 0.0, -1.70e308, 1.79e308)
        self.dataRange.readOnly = True
        self.addProperty(self.overwriteDataRange)
        self.addProperty(self.dataRange)
        self.dataRange.semantics = ivw.properties.PropertySemantics("Text")

    def process(self):
        if len(self.filePath.value) == 0 or not Path(self.filePath.value).exists():
            while self.variables.size() > 0:
                self.variables.removeProperty(
                    self.variables.properties[self.variables.size()-1])
            while self.dimensions.size() > 0:
                self.dimensions.removeProperty(
                    self.dimensions.properties[self.dimensions.size()-1])
            return
        self.dataRange.readOnly = not self.overwriteDataRange.value

        with Dataset(self.filePath.value, "r", format="NETCDF4") as nc:

            # Update variables.
            if self.filePath.isModified:
                # Keep variables as is iff the new data has the same ones.
                reloadVariables = True
                if self.variables.size() > 0:
                    reloadVariables = False
                    for variable in self.variables.properties:
                        if variable not in nc.variables:
                            reloadVariables = True
                            break

                if reloadVariables:
                    while self.variables.size() > 0:
                        self.variables.removeProperty(
                            self.variables.properties[self.variables.size()-1])

                    while self.dimensions.size() > 0:
                        self.dimensions.removeProperty(
                            self.dimensions.properties[self.dimensions.size()-1])

                    for variable in nc.variables:
                        # At least n dimensions needed for an nD output.
                        if len(nc.variables[variable].dimensions) < self.outputDimension:
                            continue
                        self.variables.addProperty(
                            BoolProperty(str(variable), str(variable), False), True)

            if self.variables.isModified:
                selectedVarDims = []
                numComponents = 0
                for varProp in self.variables.properties:
                    if not varProp.value:
                        continue

                    numVarComps = numpy.amax(
                        1, nc.variables[varProp.identifier].datatype.ndim)
                    numComponents += numVarComps

                    # Collect all dimension names in one list.
                    for dim in nc.variables[varProp.identifier].get_dims():
                        adjustedDim = self.adjustForStaggered(dim.name)
                        if adjustedDim not in selectedVarDims:
                            selectedVarDims.append(adjustedDim)

                            if not next((x for x in self.dimensions.properties if x.identifier == adjustedDim), None):
                                self.dimensions.addProperty(IntMinMaxProperty(
                                    adjustedDim, adjustedDim + ' Range', 0, len(dim)-1, 0, len(dim)-1), True)

                for varProp in self.variables.properties:
                    if varProp.value:
                        continue

                    # Check whether there is still space to add this variable
                    # and it stretches over all dimensions  the others do.
                    couldBeAdded = True
                    ncVar = nc.variables[varProp.identifier]
                    numVarComps = numpy.amax(1, ncVar.datatype.ndim)
                    if numVarComps > 4 - numComponents:
                        couldBeAdded = False

                    varDims = map(self.adjustForStaggered, ncVar.dimensions)

                    for dim in selectedVarDims:
                        adjustedDim = self.adjustForStaggered(dim)
                        if adjustedDim not in varDims:
                            couldBeAdded = False
                            break

                    varProp.readOnly = not couldBeAdded

    def reloadData(self, extents):
        if len(self.filePath.value) == 0 or not Path(self.filePath.value).exists():
            print("Invalid path.")
            return False

        with Dataset(self.filePath.value, "r", format="NETCDF4") as nc:
            # Actually load data.
            if self.variables.size() <= 0:
                print("No known variables")
                return False

            sizeDims = []
            dimRanges = {}
            for dimProp in self.dimensions.properties:
                dimRange = dimProp.value
                dimRanges[dimProp.identifier] = dimRange
                if dimRange.x != dimRange.y:
                    sizeDims.append(dimRange.y - dimRange.x + 1)

            if len(sizeDims) != self.outputDimension:
                print("Wrong number of dimensions.\n\tRequire " +
                      str(self.outputDimension) + ", selected " + str(len(sizeDims)))
                return False

            varIDs = []
            self.data = []
            for varProp in self.variables.properties:
                if not varProp.value:
                    continue
                varIDs.append(varProp.identifier)

                # Single variable, simply load.
                ncVar = nc.variables[varIDs[0]]
                ncDims = ncVar.get_dims()
                dims = []
                for ncDim in ncDims:
                    propRange = dimRanges[self.adjustForStaggered(ncDim.name)]
                    dims.append(slice(propRange.x, propRange.y+1))
                varData = ncVar[tuple(dims)]
                buffer = numpy.array(varData).astype(
                    'float32' if self.toFloat.value else ncVar.datatype)

                buffer.shape = numpy.flip([1] + sizeDims)
                self.data.append(buffer)

            # Assemble data extent.
            for dim in dimRanges:
                dimRange = dimRanges[dim]
                if dimRange.y == dimRange.x:
                    continue
                ncVar = nc.variables[dim]
                cellExt = ncVar[1] - ncVar[0]
                cellNum = dimRanges[dim].y - dimRanges[dim].x
                extents.append(cellExt * cellNum)
        return True

    def displayDataInfo(self):
        print(self.filePath.value)
        netcdfutils.printInfo(self.filePath.value)

    def adjustForStaggered(self, dim):
        if self.adjustDimensionsForStaggered.value and len(dim) == 2 and dim[1] == 'G' and (dim[0] == 'X' or dim[0] == 'Y'):
            return str(dim[0]) + 'C'
        return dim
