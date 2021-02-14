import numpy
import inspect
from pathlib import Path
from netCDF4 import Dataset, Dimension, Variable

import inviwopy as ivw
from inviwopy.properties import FileProperty, ButtonProperty, BoolProperty, \
    BoolCompositeProperty, CompositeProperty, IntMinMaxProperty, StringProperty


class GenericNetCDFSource(ivw.Processor):
    def log(self, msg):
        frame = inspect.getframeinfo(inspect.currentframe().f_back)
        ivw.log(msg=str(msg), source=self.identifier, file=frame.filename,
                line=frame.lineno, function=frame.function)

    def __init__(self, id, name, outputDimension):
        ivw.Processor.__init__(self, id, name)
        self.outputDimension = outputDimension

        self.displayInfo = ButtonProperty("displayInfo", "Log File Info")
        self.filePath = FileProperty("filepath", "NetCDF Path", "", "netcdfdata")
        self.variables = CompositeProperty("variables", "Exported Variables")
        self.dimensions = CompositeProperty("dimensions", "Restrict Dimensions")
        self.ignoreDimNames = BoolProperty('ignoreDimNames', 'Ignore Dim Names', False)
        self.toFloat = BoolProperty('toFloat', 'Convert to float', True)
        self.triggerLoad = ButtonProperty("load", "Load")

        self.addProperty(self.filePath)
        self.addProperty(self.displayInfo)
        self.addProperty(self.variables)
        self.addProperty(self.dimensions)
        self.addProperty(self.toFloat)
        self.addProperty(self.ignoreDimNames)
        self.addProperty(self.triggerLoad)

        self.displayInfo.onChange(self.displayDataInfo)

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
            minmax = IntMinMaxProperty(clean, dim.name, 0, len(dim) - 1, 0, len(dim) - 1)
            minmax.visible = False
            self.dimensions.addProperty(minmax, True)

    def addMinMaxProperties(self, dims: list[Dimension]):
        for dim in dims:
            self.addMinmaxProperty(dim)

    def addVariablePropery(self, variable: Variable):
        enabled = BoolCompositeProperty(self.cleanName(variable.name), variable.name, False)
        enabled.readOnly = len(variable.shape) < self.outputDimension
        enabled.collapsed = True
        dims = StringProperty("dimensions", "Dimensionss",
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
        if len(self.filePath.value) == 0 or not Path(self.filePath.value).exists():
            self.variables.clear()
            self.dimensions.clear()
            return

        with Dataset(self.filePath.value, "r", format="NETCDF4") as nc:
            # Update variables.
            if self.filePath.isModified:
                if self.variables.empty() or any(var.displayName not in nc.variables
                                                 for var in self.variables.properties):
                    self.variables.clear()
                    self.dimensions.clear()
                    for name, variable in nc.variables.items():
                        self.addVariablePropery(variable)
                        self.addMinMaxProperties(variable.get_dims())

            if self.variables.isModified:
                numComponents = 0
                showMinMax: list[IntMinMaxProperty] = []
                for var in filter(self.enabled, self.variables.properties):
                    comps = numpy.amax(1, nc.variables[var.displayName].datatype.ndim)

                    if numComponents + comps > 4:  # No space left
                        var.checked = False
                        self.log(f"No channels left, {numComponents} used, {comps} more needed")
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

            if self.triggerLoad.isModified:
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
                    buffer.shape = numpy.flip([1] + sizeDims)
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

    def displayDataInfo(self):
        if len(self.filePath.value) == 0:
            self.log("File name empty")
            return
        if not Path(self.filePath.value).exists():
            self.log(f"{self.filePath.value} does not exist")
            return

        def attrs(item, indent=0):
            for attr in item.ncattrs():
                self.log(f"{indent*' '}{attr}: {getattr(item, attr)}")

        self.log(f"File: {self.filePath.value}")
        with Dataset(self.filePath.value, "r", format="NETCDF4") as nc:
            attrs(nc)
            self.log(f"Dimensions: {', '.join(nc.dimensions)}")
            for name, var in nc.variables.items():
                self.log(f"{name}, {var.dimensions}, {var.shape}, {var.datatype}")
                attrs(var, 4)
