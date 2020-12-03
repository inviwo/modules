# Name: NetCDFVolumeSource

import inviwopy as ivw
from inviwopy.glm import dvec2, mat4, vec4
from inviwopy.properties import BoolProperty, DoubleMinMaxProperty, OptionPropertyInt, \
    FloatMat4Property
import genericnetcdfsource
import numpy
import importlib
importlib.reload(genericnetcdfsource)


def options(enum):
    return [ivw.properties.IntOption(str(e), str(e), v.__int__())
            for e, v in enum.__members__.items()]


class NetCDFVolumeSource(genericnetcdfsource.GenericNetCDFSource):
    def __init__(self, id, name):
        genericnetcdfsource.GenericNetCDFSource.__init__(self, id, name, outputDimension=3)
        self.volumeOutport = ivw.data.VolumeOutport("data3D")
        self.addOutport(self.volumeOutport, owner=False)

        self.interpolation = OptionPropertyInt(
            "interpolation", "Interpolation", options(ivw.data.InterpolationType), 0)
        self.addProperty(self.interpolation)
        self.wrappingX = OptionPropertyInt(
            "wrappingX", "Wrapping X", options(ivw.data.Wrapping), 0)
        self.addProperty(self.wrappingX)
        self.wrappingY = OptionPropertyInt(
            "wrappingY", "Wrapping Y", options(ivw.data.Wrapping), 0)
        self.addProperty(self.wrappingY)
        self.wrappingZ = OptionPropertyInt(
            "wrappingZ", "Wrapping Z", options(ivw.data.Wrapping), 0)
        self.addProperty(self.wrappingZ)

        self.overwriteDataRange = BoolProperty("overwriteDataRange", "Overwrite Data Range", False)
        self.addProperty(self.overwriteDataRange)
        self.dataRange = DoubleMinMaxProperty(
            "dataRange", "Data Range", 0.0, 0.0, -1.70e308, 1.79e308)
        self.addProperty(self.dataRange)
        self.dataRange.readOnly = True
        self.dataRange.semantics = ivw.properties.PropertySemantics("Text")
        self.dataRange.readonlyDependsOn(self.overwriteDataRange, lambda x: not x.value)

        self.overwriteModel = BoolProperty("overwriteModel", "Overwrite Model Matrix", False)
        self.addProperty(self.overwriteModel)
        self.modelMatrix = FloatMat4Property(
            "modelMatrix", "Model Matrix", mat4(10.0), mat4(0) - 10e20, mat4(0) + 10e20)
        self.addProperty(self.modelMatrix)
        self.modelMatrix.readOnly = True
        self.modelMatrix.semantics = ivw.properties.PropertySemantics("Text")
        self.modelMatrix.readonlyDependsOn(self.overwriteModel, lambda x: not x.value)

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.netcdf.netcdfvolumesource",
            displayName="NetCDF Volume Source",
            category="Source",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags([ivw.Tag.PY, ivw.Tag("NetCDF"),
                           ivw.Tag("Volume")])
        )

    def getProcessorInfo(self):
        return NetCDFVolumeSource.processorInfo()

<<<<<<< HEAD
    def dataLoaded(self, data, extents):
        buffer = numpy.concatenate(data, axis=3)
=======
    def reloadData(self):
        extents = GenericNetCDFSource.genReloadData(self)

        buffer = numpy.concatenate(self.data, axis=3)
        print(type(buffer))
>>>>>>> 35eeab5 (WIP)
        volume = ivw.data.Volume(buffer)
        if self.overwriteDataRange.value:
            volume.dataMap.dataRange = self.dataRange.value
        else:
            minVal = numpy.amin(buffer)
            maxVal = numpy.amax(buffer)
            volume.dataMap.dataRange = dvec2(minVal, maxVal)
        volume.dataMap.valueRange = volume.dataMap.dataRange
        
        if self.overwriteModel:
            volume.modelMatrix = self.modelMatrix.value
        else:
            volume.modelMatrix = mat4(
                vec4(extents[2], 0, 0, 0),
                vec4(0, extents[1], 0, 0),
                vec4(0, 0, extents[0], 0),
                vec4(0, 0, 0, 1)
            )

        volume.interpolation = ivw.data.InterpolationType(self.interpolation.value)
        volume.wrapping = [
            ivw.data.Wrapping(self.wrappingX.value),
            ivw.data.Wrapping(self.wrappingY.value),
            ivw.data.Wrapping(self.wrappingZ.value)
        ]
        self.volumeOutport.setData(volume)
