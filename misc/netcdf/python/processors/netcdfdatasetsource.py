# Name: NetCDFDataSetSource

import inviwopy as ivw
from ivwdiscretedata import DataSetInitializer, DataSet, GridPrimitive, Channel, DataSetOutport, Connectivity
from inviwopy.properties import IntVec3Property, FileProperty, OptionPropertyString, ButtonProperty, BoolProperty, CompositeProperty, DoubleMinMaxProperty, IntMinMaxProperty
from inviwopy.glm import dvec2, mat4, vec4
import netcdfutils
from genericnetcdfsource import GenericNetCDFSource

import numpy
from pathlib import Path
from netCDF4 import Dataset


class NetCDFDataSetSource(ivw.Processor):

    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)

        self.dataSetOutport = DataSetOutport("dataset")
        self.addOutport(self.dataSetOutport, owner=True)

        # self.data = []
        self.data = [[0.0, 1.0], [0.0, 1.0]]

        self.displayInfo = ButtonProperty("displayInfo", "Log File Info")

        self.filePath = ivw.properties.FileProperty(
            "filepath", "NetCDF Path", "", "netcdfdata")
        self.variables = CompositeProperty("variables", "Loaded Variables")
        self.variables.setSerializationMode(
            ivw.properties.PropertySerializationMode.All)

        # self.adjustDimensionsForStaggered = BoolProperty(
        #     'adjustForStaggered', 'Adjust for staggered climate grid', True)

        self.dimensions = ivw.properties.OptionPropertyString(
            "dimensions", "dimensions")

        self.triggerReload = ButtonProperty("reload", "Reload")
        self.autoReload = BoolProperty("autoReload", "Auto Reload", False)

        self.triggerReload.onChange(self.reloadData)
        self.autoReload.onChange(self.autoReloadData)
        self.displayInfo.onChange(self.displayDataInfo)

        self.addProperty(self.displayInfo)
        self.addProperty(self.filePath)
        self.addProperty(self.dimensions)
        self.addProperty(self.variables)
        # self.addProperty(self.toFloat)
        # self.addProperty(self.adjustDimensionsForStaggered)
        self.addProperty(self.triggerReload)
        self.addProperty(self.autoReload)

        # self.overwriteDataRange = BoolProperty(
        #     "overwriteDataRange", "Overwrite Data Range", False)

        # self.dataRange = DoubleMinMaxProperty(
        #     "dataRange", "Data Range", 0.0, 0.0, -1.70e308, 1.79e308)
        # self.dataRange.readOnly = True
        # self.addProperty(self.overwriteDataRange)
        # self.addProperty(self.dataRange)
        # self.dataRange.semantics = ivw.properties.PropertySemantics("Text")
        self.firstProcess = True

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.netcdf.netcdfdatasetsource",
            displayName="NetCDF DataSet Source",
            category="Source",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags([ivw.Tag.PY, ivw.Tag("NetCDF"),
                           ivw.Tag("DataSet")])
        )

    def getProcessorInfo(self):
        return NetCDFDataSetSource.processorInfo()

    @staticmethod
    def getDimensionStringFromVariable(nc, varName):
        ncDims = nc.variables[varName].get_dims()

        # No dimensions? Return empty string.
        if len(ncDims) == 0:
            return ''

        dimString = '('

        # Append all dimension names in one list.
        for dim in ncDims:
            dimString += dim.name
            dimString += ', '

        dimString = dimString[:-2]
        dimString += ')'
        return dimString

    def updateVariables(self, nc, dimString):
        print('==> Updating Vars!')

        # Remove all currently available variables.
        while self.variables.size() > 0:
            self.variables.removeProperty(
                self.variables.properties[self.variables.size()-1])

        # Find all variables that are defined on the selected dimensions.
        selectedDimString = self.dimensions.selectedValue

        for ncVar in nc.variables:
            varDimString = NetCDFDataSetSource.getDimensionStringFromVariable(
                nc, ncVar)

            # Compare strings of dimensions. Allow user to deselect available variables.
            if varDimString == selectedDimString:
                print("Available Var: ", ncVar)
                varProp = BoolProperty(
                    str(ncVar), str(ncVar), True)
                varProp.setSerializationMode(
                    ivw.properties.PropertySerializationMode.All)
                self.variables.addProperty(
                    varProp, True)

    def updateDimensions(self, nc):
        print('=> Updating Dims!')
        # Remove all currently available dimension combinations.
        self.dimensions.replaceOptions((), (), ())

        # Assemble dimension strings of all variables.
        for ncVar in nc.variables:
            varDimString = NetCDFDataSetSource.getDimensionStringFromVariable(
                nc, ncVar)

            # To get all values: self.dimensions.values
            if varDimString not in self.dimensions.values:
                print("Dim combo: ", varDimString)
                self.dimensions.addOption(
                    varDimString, varDimString, varDimString)

    def process(self):
        # Abort if file name is not valid.
        if len(self.filePath.value) == 0 or not Path(self.filePath.value).exists():
            while self.variables.size() > 0:
                self.variables.removeProperty(
                    self.variables.properties[self.variables.size()-1])
            self.dimensions.replaceOptions((), (), ())
            self.firstProcess = False
            return

        with Dataset(self.filePath.value, "r", format="NETCDF4") as nc:

            # Inital loading of dimenion combinations and available variables.
            if self.filePath.isModified and not self.firstProcess:
                self.updateDimensions(nc)
                self.updateVariables(nc, self.dimensions.value)

            # Switching the dimension combination means different variables.
            if self.dimensions.isModified and not self.firstProcess:
                self.updateVariables(nc, self.dimensions.value)

            # Potentially auto-reload data if selected variables change.
            if self.variables.isModified and not self.firstProcess:
                self.autoReloadData()

        self.firstProcess = False
        print('processed')

        # buffer.shape = numpy.flip([1] + sizeDims)

        # if len(self.filePath.value) == 0 or not Path(self.filePath.value).exists():
        #     return
        # with Dataset(self.filePath.value, "r", format = "NETCDF4") as nc:

        # def getProcessorInfo(self):
        #     return NetCDFDataSetSource.processorInfo()

    def autoReloadData(self):
        if self.autoReload.value:
            self.reloadData()

    def displayDataInfo(self):
        if len(self.filePath.value) == 0 or not Path(self.filePath.value).exists():
            return

        print(self.filePath.value)
        netcdfutils.printInfo(self.filePath.value)

    def reloadData(self):
        if len(self.filePath.value) == 0 or not Path(self.filePath.value).exists() or len(self.variables.properties) == 0:
            return

        with Dataset(self.filePath.value, "r", format="NETCDF4") as nc:
            print('Loading this file I guess! ', self.filePath.value)
            firstVar = nc.variables[self.variables.properties[0].identifier]
            ncDims = firstVar.get_dims()

            fieldSize = []
            fieldRange = []
            for ncDim in ncDims:
                fieldSize.append(len(ncDim))
                fieldRange.append(slice(0, len(ncDim)))
            print("Field size: ", fieldSize)

            # numpySize = numpy.array(fieldSize).astype('int32')
            grid = Connectivity.createStructured(fieldSize)
            dataset = DataSet(grid)

            for ncVar in self.variables.properties:
                if not ncVar.value:
                    continue

                rawBuffer = nc.variables[ncVar.identifier][tuple(fieldRange)]
                buffer = numpy.array(rawBuffer).astype('float32')

                channel = Channel.createChannel(
                    buffer, ncVar.identifier, GridPrimitive.Vertex)
                dataset.addChannel(channel)

            self.dataSetOutport.setData(dataset)

        print('reloaded - kinda')
        # Debug!
        # buffer = numpy.array(self.data).astype('float64')
        # print(type(buffer))
        # # help(buffer)
        # print('=====================')
        # help(DataSet)
        # dataset = DataSet.createFromArray(buffer, 'TestPos')
        # self.dataSetOutport.setData(dataset)
        print('reloaded - kinda')
        # extents = []
        # GenericNetCDFSource.reloadData(self, extents)

        # buffer = numpy.concatenate(self.data, axis=3)
        # volume = ivw.data.Volume(buffer)
        # if self.overwriteDataRange.value:
        #     volume.dataMap.dataRange = self.dataRange.value
        # else:
        #     minVal = numpy.amin(buffer)
        #     maxVal = numpy.amax(buffer)
        #     volume.dataMap.dataRange = dvec2(minVal, maxVal)
        # volume.dataMap.valueRange = dvec2(-10000, 10000.0)
        # volume.dataMap.valueRange = dvec2(-1000, 1000.0)
        # volume.modelMatrix = mat4(
        #     vec4(extents[2], 0, 0, 0),
        #     vec4(0, extents[1], 0, 0),
        #     vec4(0, 0, extents[0], 0),
        #     vec4(0, 0, 0, 1))
        # self.volumeOutport.setData(volume)
