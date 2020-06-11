# Name: NetCDFVolumeSequenceSource

import inviwopy as ivw
from inviwopy.properties import IntVec3Property, FileProperty, OptionPropertyString, ButtonProperty, BoolProperty, CompositeProperty, DoubleMinMaxProperty, IntMinMaxProperty
from inviwopy.glm import dvec2, mat4, vec4
import netcdfutils
from genericnetcdfsource import GenericNetCDFSource

import numpy
from pathlib import Path
from netCDF4 import Dataset


class NetCDFVolumeSequenceSource(GenericNetCDFSource):

    def __init__(self, id, name):
        self.outputDimension = 4
        GenericNetCDFSource.initGeneric(self, id, name)

        self.volumeOutport = ivw.data.VolumeSequenceOutport("data4D")
        self.addOutport(self.volumeOutport, owner=False)

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

    def reloadData(self):
        extents = []
        GenericNetCDFSource.reloadData(self, extents)

        volumeSequence = []
        for timeStep in range(self.data[0].shape[3]):
            subData = []
            for comp in self.data:
                # A bit brute force, but delivers correct data.
                numElems = comp.shape[0] * comp.shape[1] * comp.shape[2]
                subBuffer = comp.flat[(numElems * timeStep): (numElems * (timeStep+1))]
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
            volume.dataMap.valueRange = dvec2(-10000, 10000.0)
            volume.modelMatrix = mat4(
                vec4(extents[3], 0, 0, 0),
                vec4(0, extents[2], 0, 0),
                vec4(0, 0, extents[1], 0),
                vec4(0, 0, 0, 1))
            volumeSequence.append(volume)
        sequence = ivw.data.VolumeSequence(volumeSequence)
        self.volumeOutport.setData(sequence)
