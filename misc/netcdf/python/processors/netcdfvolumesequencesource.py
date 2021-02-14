# Name: NetCDFVolumeSequenceSource

import inviwopy as ivw
from inviwopy.glm import dvec2, mat4, vec4
from inviwopy.properties import BoolProperty, DoubleMinMaxProperty
import genericnetcdfsource
import numpy
import importlib
importlib.reload(genericnetcdfsource)


class NetCDFVolumeSequenceSource(genericnetcdfsource.GenericNetCDFSource):
    def __init__(self, id, name):
        genericnetcdfsource.GenericNetCDFSource.__init__(self, id, name, outputDimension=4)
        self.volumeOutport = ivw.data.VolumeSequenceOutport("data4D")
        self.addOutport(self.volumeOutport, owner=False)
        self.overwriteDataRange = BoolProperty("overwriteDataRange", "Overwrite Data Range", False)
        self.dataRange = DoubleMinMaxProperty(
            "dataRange", "Data Range", 0.0, 0.0, -1.70e308, 1.79e308)
        self.addProperty(self.overwriteDataRange)
        self.addProperty(self.dataRange)
        self.dataRange.readOnly = True
        self.dataRange.semantics = ivw.properties.PropertySemantics("Text")
        self.dataRange.readonlyDependsOn(self.overwriteDataRange, lambda x: not x.value)

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

    def dataLoaded(self, data, extents):
        volumeSequence = []
        for timeStep in range(data[0].shape[3]):
            subData = []
            for comp in data:
                # A bit brute force, but delivers correct data.
                numElems = comp.shape[0] * comp.shape[1] * comp.shape[2]
                subBuffer = comp.flat[(numElems * timeStep): (numElems * (timeStep + 1))]
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
            volume.modelMatrix = mat4(
                vec4(extents[3], 0, 0, 0),
                vec4(0, extents[2], 0, 0),
                vec4(0, 0, extents[1], 0),
                vec4(0, 0, 0, 1))
            volumeSequence.append(volume)

        sequence = ivw.data.VolumeSequence(volumeSequence)
        self.volumeOutport.setData(sequence)
