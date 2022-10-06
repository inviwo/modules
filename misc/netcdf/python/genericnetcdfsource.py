from inviwopy.properties import FileProperty, ButtonProperty, BoolProperty, \
    BoolCompositeProperty, CompositeProperty, IntMinMaxProperty, StringProperty
from netCDF4 import Dataset, Dimension, Variable
from pathlib import Path
import inspect
import numpy
import netcdfutils
from inviwopy.glm import dvec2, mat4, vec4
from inviwopy.properties import IntVec3Property, FileProperty, ButtonProperty, BoolProperty, DoubleProperty, CompositeProperty, CompositeProperty, DoubleMinMaxProperty, IntMinMaxProperty
import inviwopy as ivw

class GenericNetCDFSource(ivw.Processor):
    def log(self, msg):
        frame = inspect.getframeinfo(inspect.currentframe().f_back)
        ivw.log(msg=str(msg), source=self.identifier, file=frame.filename,
                line=frame.lineno, function=frame.function)

    def __init__(self, id, name, outputDimension):
        ivw.Processor.__init__(self, id, name)
        self.outputDimension = outputDimension

        self.displayInfo = ButtonProperty("displayInfo", "Log File Info")
        self.filePath = FileProperty(
            "filepath", "NetCDF Path", "", "netcdfdata")
        self.variables = CompositeProperty("variables", "Exported Variables")
        self.variables.setSerializationMode(
            ivw.properties.PropertySerializationMode.All)
        self.dimensions = CompositeProperty(
            "dimensions", "Restrict Dimensions")
        self.dimensions.setSerializationMode(
            ivw.properties.PropertySerializationMode.All)
        self.adjustDimensionsForStaggered = BoolProperty(
            'adjustForStaggered', 'Adjust for staggered climate grid', True)
        self.toFloat = BoolProperty(
            'toFloat', 'Convert to float', True)
        self.doScale = BoolProperty("doScale", "Scale Values?", False)
        self.scale = CompositeProperty("scale", "Scale Values")

        self.triggerReload = ButtonProperty("reload", "Reload")
        self.autoReload = BoolProperty("autoReload", "Auto Reload", False)

        self.triggerReload.onChange(self.reloadData)
        self.autoReload.onChange(self.autoReloadData)
        self.displayInfo.onChange(self.displayDataInfo)

        self.addProperty(self.filePath)
        self.addProperty(self.displayInfo)
        self.addProperty(self.variables)
        self.addProperty(self.doScale)
        self.addProperty(self.scale)
        self.addProperty(self.dimensions)
        self.addProperty(self.toFloat)
        self.addProperty(self.ignoreDimNames)
        self.addProperty(self.triggerLoad)

        self.displayInfo.onChange(self.displayDataInfo)
        self.firstRun = True

    @staticmethod
    def enabled(prop: BoolCompositeProperty):
        return prop.checked

    @staticmethod
    def cleanName(name: str):
        return name.replace(" ", "_")

    def minmax(self, dim: Dimension):
        return self.dimensions.properties[self.cleanName(dim.name)].value

    def collapsed(self, dim: Dimension):
        minmax = self.minmax(dim)
        return minmax.x == minmax.y

    def addMinmaxProperty(self, dim: Dimension):
        clean = self.cleanName(dim.name)
        if clean not in self.dimensions.properties:
            # Create a minmax property to mark the __inclusive__ range to load
            # If the min == max then the dimensions is __collapsed__ and will not
            # contribute to the outputDimension i.e. collpsing one dim of a 4D dataset
            # makes it a 3D dataset.
            minmax = IntMinMaxProperty(
                clean, dim.name, 0, len(dim) - 1, 0, len(dim) - 1)
            minmax.visible = False  # Only show when a corresponding variable is enabled
            self.dimensions.addProperty(minmax, True)

    def addMinMaxProperties(self, dims: list[Dimension]):
        for dim in dims:
            self.addMinmaxProperty(dim)

    def addVariablePropery(self, variable: Variable):
        enabled = BoolCompositeProperty(
            self.cleanName(variable.name), variable.name, False)
        enabled.readOnly = len(variable.shape) < self.outputDimension
        enabled.collapsed = True
        dims = StringProperty("dimensions", "Dimensions",
                              ", ".join(str(d.name) for d in variable.get_dims()))
        dims.readOnly = True
        enabled.addProperty(dims)
        shape = StringProperty("shape", "Shape",
                               ", ".join(str(d) for d in variable.shape))
        shape.readOnly = True
        enabled.addProperty(shape)

        format = StringProperty("format", "Format", str(variable.datatype))
        format.readOnly = True
        enabled.addProperty(format)

        attrstr = '\n'.join(f"{attr}: {getattr(variable, attr)}"
                            for attr in variable.ncattrs())
        attr = StringProperty("attributes", "Attributes", attrstr,
                              ivw.properties.InvalidationLevel.Valid,
                              ivw.properties.PropertySemantics.Multiline)
        attr.readOnly = True
        enabled.addProperty(attr)

        self.variables.addProperty(enabled, True)

    def process(self):
        self.scale.visible = self.doScale.value
        if len(self.filePath.value) == 0 or not Path(self.filePath.value).exists():
            self.variables.clear()
            self.dimensions.clear()
            return

        with Dataset(self.filePath.value, "r", format="NETCDF4") as nc:
            # Update variables.
            if self.filePath.isModified and not self.firstProcess:
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

                    while self.scale.size() > 0:
                        self.scale.removeProperty(
                            self.scale.properties[self.scale.size()-1])

                    for variable in nc.variables:
                        # At least n dimensions needed for an nD output.
                        if len(nc.variables[variable].dimensions) < self.outputDimension:
                            continue
                        varProp = BoolProperty(
                            str(variable), str(variable), False)
                        varProp.setSerializationMode(
                            ivw.properties.PropertySerializationMode.All)
                        self.variables.addProperty(
                            varProp, True)

                        # Add scale.
                        scProp = DoubleProperty(
                            str(variable) + "Scale", str(variable) + " Scale", 1.0, 0, 100)
                        scProp.setSerializationMode(
                            ivw.properties.PropertySerializationMode.All)
                        self.scale.addProperty(scProp, True)

            if self.variables.isModified and not self.firstProcess:
                selectedVarDims = []
                numComponents = 0
                showMinMax: list[IntMinMaxProperty] = []
                for var in filter(self.enabled, self.variables.properties):
                    comps = numpy.amax(
                        1, nc.variables[var.displayName].datatype.ndim)

                    if numComponents + comps > 4:  # No space left
                        var.checked = False
                        self.log(
                            f"No channels left, {numComponents} used, {comps} more needed")
                        continue

                    numComponents += comps

                    for dim in nc.variables[var.displayName].get_dims():
                        clean = self.cleanName(dim.name)
                        showMinMax.append(self.dimensions.properties[clean])
                for minmax in self.dimensions.properties:
                    minmax.visible = minmax in showMinMax

            request: list[Variable] = [nc.variables[prop.displayName]
                                       for prop in filter(self.enabled, self.variables.properties)]

            nonZeroDims: list[tuple[Variable, list[Dimension]]] = \
                [(var, list(filter(lambda d: not self.collapsed(d), var.get_dims())))
                 for var in request]

            for var, dims in nonZeroDims:
                if len(dims) != self.outputDimension:
                    self.log(f"Invalid output dimensions: {len(dims)} for {var.name} "
                             f"expected {self.outputDimension}")
                    return

                for dimi, dim0 in zip(dims, nonZeroDims[0][1]):
                    if not self.ignoreDimNames and dimi != dim0:
                        self.log(f"dimension missmatch for {var.name}, "
                                 f"expexted {dim0.name} got {dimi.name}")
                        return
                    else:
                        minmaxi = self.minmax(dimi)
                        minmax0 = self.minmax(dim0)
                        if minmaxi != minmax0:
                            self.log(f"Expected the range of {dimi.name}"
                                     f" to be {minmax0} got {minmaxi}")
                            return

            if self.triggerLoad.isModified or first:
                if len(nonZeroDims) == 0:
                    self.log("No variables selected")
                    return

                data = []
                for var in request:
                    ranges = [self.minmax(dim) for dim in var.get_dims()]
                    sizeDims = [r.y - r.x + 1 for r in ranges if r.x != r.y]
                    dims = [slice(item.x, item.y + 1) for item in ranges]
                    varData = var[tuple(dims)]
                    buffer = numpy.array(varData).astype(
                        'float32' if self.toFloat.value else var.datatype)
                    buffer.shape = numpy.flip(
                        [numpy.amax(1, var.datatype.ndim)] + sizeDims)
                    data.append(buffer)

                # Assemble data extent.
                extents = []
                for dim in nonZeroDims[0][1]:
                    dimRange = self.minmax(dim)
                    if dimRange.x == dimRange.y:
                        continue
                    ncDimVar = nc.variables[dim.name]
                    cellExt = ncDimVar[1] - ncDimVar[0]
                    cellNum = dimRange.y - dimRange.x
                    extents.append(cellExt * cellNum)

                self.dataLoaded(data, extents)  # implemented in child

    def autoReloadData(self):

        if self.autoReload.value:
            print("Disabled Auto Reload!")
            self.reloadData()

    def reloadData(self):
        return

    def genReloadData(self):
        if len(self.filePath.value) == 0 or not Path(self.filePath.value).exists():
            raise Exception("Invalid path.")

        self.log(f"File: {self.filePath.value}")
        with Dataset(self.filePath.value, "r", format="NETCDF4") as nc:

            # Actually load data.
            if self.variables.size() <= 0:
                # raise Exception("No known variables")
                self.invalidate(ivw.properties.InvalidationLevel.InvalidOutput)
                self.firstProcess = True

            sizeDims = []
            dimRanges = {}
            for dimProp in self.dimensions.properties:
                dimRange = dimProp.value
                dimRanges[dimProp.identifier] = dimRange
                if dimRange.x != dimRange.y:
                    sizeDims.append(dimRange.y - dimRange.x + 1)

            if len(sizeDims) != self.outputDimension:
                raise Exception("Wrong number of dimensions.\n\tRequire " +
                                str(self.outputDimension) + ", selected " + str(len(sizeDims)))

            self.data = []
            numProps = 0
            doScale = self.doScale.value
            for varProp in self.variables.properties:
                if not varProp.value:
                    continue

                # Single variable, simply load.
                ncVar = nc.variables[varProp.identifier]
                ncDims = ncVar.get_dims()
                dims = []
                for ncDim in ncDims:
                    propRange = dimRanges[self.adjustForStaggered(ncDim.name)]
                    dims.append(slice(propRange.x, propRange.y+1))
                varData = ncVar[tuple(dims)]
                buffer = numpy.array(varData).astype(
                    'float32' if self.toFloat.value else ncVar.datatype)

                buffer.shape = numpy.flip([1] + sizeDims)
                if (doScale):
                    for sc in self.scale.properties:
                        if(sc.identifier == varProp.identifier + "Scale"):
                            buffer *= sc.value
                            print("Scaled ", varProp.identifier,
                                  " by ", sc.value)
                            break
                self.data.append(buffer)
                numProps += 1

            # Assemble data extent.
            extents = []
            for dim in dimRanges:
                dimRange = dimRanges[dim]
                if dimRange.y == dimRange.x:
                    continue
                ncVar = nc.variables[dim]
                cellExt = ncVar[1] - ncVar[0]
                cellNum = dimRanges[dim].y - dimRanges[dim].x
                extents.append(cellExt * cellNum)
            return extents

    def displayDataInfo(self):
        print(self.filePath.value)
        netcdfutils.printInfo(self.filePath.value)

    def adjustForStaggered(self, dim):
        if self.adjustDimensionsForStaggered.value and len(dim) == 2 and dim[1] == 'G' and (dim[0] == 'X' or dim[0] == 'Y'):
            return str(dim[0]) + 'C'
        return dim
