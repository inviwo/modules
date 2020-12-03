# Name: NetCDFVolumeSequenceSource

import inviwopy as ivw
<<<<<<< HEAD
=======
from inviwopy.properties import IntVec3Property, FileProperty, ButtonProperty, BoolProperty, CompositeProperty, DoubleMinMaxProperty, IntMinMaxProperty
>>>>>>> 35eeab5 (WIP)
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


class NetCDFVolumeSequenceSource(genericnetcdfsource.GenericNetCDFSource):
    def __init__(self, id, name):
        genericnetcdfsource.GenericNetCDFSource.__init__(self, id, name, outputDimension=4)
        self.volumeOutport = ivw.data.VolumeSequenceOutport("data4D")
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
            classIdentifier="org.inviwo.netcdf.netcdfvolumesequencesource",
            displayName="NetCDF Volume Sequence Source",
            category="Source",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags([ivw.Tag.PY, ivw.Tag("NetCDF"),
                           ivw.Tag("Volume Sequence")])
        )

    def getProcessorInfo(self):
        return NetCDFVolumeSequenceSource.processorInfo()

<<<<<<< HEAD
    def dataLoaded(self, data, extents):
=======
    def reloadData(self):
        # extents = []
        extents = GenericNetCDFSource.genReloadData(self)

>>>>>>> ea95e44 (WIP 2)
        volumeSequence = []
        for timeStep in range(data[0].shape[3]):
            subData = []
            for comp in data:
                # A bit brute force, but delivers correct data.
                numElems = comp.shape[0] * comp.shape[1] * comp.shape[2]
<<<<<<< HEAD
                subBuffer = comp.flat[(numElems * timeStep): (numElems * (timeStep + 1))]
=======
                subBuffer = comp.flat[(numElems * timeStep)                                      : (numElems * (timeStep+1))]
>>>>>>> ea95e44 (WIP 2)
                subBuffer.shape = (comp.shape[0],
                                   comp.shape[1], comp.shape[2], 1)
                subData.append(subBuffer)
            buffer = numpy.concatenate(subData, axis=3)
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
            volumeSequence.append(volume)

        sequence = ivw.data.VolumeSequence(volumeSequence)
        self.volumeOutport.setData(sequence)
