# Name: NetCDFSource

import inviwopy as ivw
from inviwopy.properties import IntVec3Property, FileProperty, OptionPropertyString, ButtonProperty
from inviwopy.glm import dvec2, mat4, vec4
import netcdfutils

# # import functools
# # import math
import numpy
from pathlib import Path
from netCDF4 import Dataset


class NetCDFSource(ivw.Processor):

    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)

        self.volumeOutport = ivw.data.VolumeOutport("data3D")
        self.addOutport(self.volumeOutport, owner=False)

        self.displayInfo = ButtonProperty("displayInfo", "Log File Info")

        self.filePath = ivw.properties.FileProperty("filepath", "NetCDF Path")
        self.variable = OptionPropertyString("variable", "Variable")
        # self.dimensionIds = [OptionPropertyString("dimX", "Dim X"), OptionPropertyString(
        #     "dimY", "Dim Y"), OptionPropertyString("dimZ", "Dim Z")]

        self.triggerReload = ButtonProperty("reload", "Reload")

        self.triggerReload.onChange(self.reloadData)
        self.displayInfo.onChange(self.displayDataInfo)

        self.addProperty(self.displayInfo)
        self.addProperty(self.filePath)
        self.addProperty(self.variable)
        # for dimId in self.dimensionIds:
        #     self.addProperty(dimId)
        self.addProperty(self.triggerReload)

        # self.dataRange = ivw.properties.DoubleMinMaxProperty(
        #     "dataRange", "Data Range", 0.0, 0.0, -1.70e308, 1.79e308)
        # self.addProperty(self.dataRange)
        # self.dataRange.semantics = ivw.properties.PropertySemantics("Text")
        # self.dataRange.readOnly = True

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.netcdf.netcdfsource",
            displayName="NetCDF Source",
            category="Source",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags([ivw.Tag.PY, ivw.Tag("NetCDF"),
                           ivw.Tag("VolumeSequence")])
        )

    def getProcessorInfo(self):
        return NetCDFSource.processorInfo()

    def initializeResources(self):
        print("initalizing")
        pass

    def process(self):
        print("Process called")

        if len(self.filePath.value) == 0 or not Path(self.filePath.value).exists():
            print("No valid path given")
            return

        with Dataset(self.filePath.value, "r", format="NETCDF4") as nc:
            if self.filePath.isModified or self.variable.isModified:
                # print("Something happened!")

                # Update variables.
                if self.filePath.isModified:
                    print("Path is modified")
                    # Save last selected option to set again.
                    lastSelected = ""
                    if len(self.variable.values) > 0:
                        lastSelected = self.variable.value

                    # Update all variables as options.
                    for option in self.variable.values:
                        self.variable.removeOption(option)
                    for variable in nc.variables:
                        self.variable.addOption(
                            str(variable), str(variable), str(variable))
                        if variable == lastSelected:
                            self.variable.value = variable

    #             for dimId in self.dimensionIds:
    #                 # Save last selected option to set again.
    #                 lastSelected = ""
    #                 if len(dimId.values) > 0:
    #                     lastSelected = dimId.value

    #                 # Update all dimensions as options.
    #                 for option in dimId.values:
    #                     dimId.removeOption(option)
    #                 for dim in nc.dimensions:
    #                     dimId.addOption(str(dim), str(dim), str(dim))
    #                     if dim == lastSelected:
    #                         dimId.value = dim

    def reloadData(self):
        if len(self.filePath.value) == 0 or not Path(self.filePath.value).exists():
            print("invalid path")
            return

        with Dataset(self.filePath.value, "r", format="NETCDF4") as nc:
            # Actually load data.
            if len(self.variable.values) <= 0:
                print("No known variables")
                return

            ncVar = nc.variables[self.variable.value]
            ncDims = ncVar.get_dims()
            # print("Var " + str(self.variable.value) +
            #       " with dims " + str(ncDims))
            # for dim in ncDims:
            #     print("Dim " + str(dim) + " of size " + str(len(dim)))

            dims = []
            for dim in ncDims:
                dims.append(len(dim))

            # 4D data, i.e., a volume sequence?
            if len(ncDims) == 4:
                print("4D!")
                timeLength = dims[0]
                print("Time " + str(timeLength))
                for timeStep in range(1):  # timeLength:
                    varData = ncVar[timeStep, :, :, :]
                    buffer = numpy.array(varData).astype('float32')
                    # ncVar.datatype)
                    buffer.shape = [dims[3], dims[2], dims[1]]
                    minVal = numpy.amin(buffer)
                    maxVal = numpy.amax(buffer)
                    print("Min " + str(minVal) + ", max " + str(maxVal))
                    # print(buffer.shape)
                    volume = ivw.data.Volume(buffer)
                    volume.dataMap.dataRange = dvec2(minVal, maxVal)
                    volume.dataMap.valueRange = dvec2(0.0, 1000.0)
                    volume.modelMatrix = mat4(vec4(dims[3], 0, 0, 0),
                                              vec4(0, dims[2], 0, 0),
                                              vec4(0, 0, dims[1], 0),
                                              vec4(0, 0, 0, 1))
                    self.volumeOutport.setData(volume)

    def displayDataInfo(self):
        print(self.filePath.value)
        netcdfutils.printInfo(self.filePath.value)
