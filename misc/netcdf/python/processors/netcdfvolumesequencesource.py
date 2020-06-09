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
        if not GenericNetCDFSource.reloadData(self, extents):
            return

        buffer = numpy.concatenate(self.data, axis=4)
        volumeSequence = []
        print(str(buffer.shape[3]) + " time steps")
        print("Dims: " + str(buffer.shape))
        for timeStep in range(buffer.shape[3]):
            subBuffer = buffer[:, :, :, timeStep, :]
            print(buffer.shape)
            print(subBuffer.shape)
            minVal = numpy.amin(subBuffer)
            maxVal = numpy.amax(subBuffer)
            volume = ivw.data.Volume(subBuffer)
            volume.dataMap.dataRange = dvec2(minVal, maxVal)
            volume.dataMap.valueRange = dvec2(-1000, 1000.0)
            volume.modelMatrix = mat4(
                vec4(extents[3], 0, 0, 0),
                vec4(0, extents[2], 0, 0),
                vec4(0, 0, extents[1], 0),
                vec4(0, 0, 0, 1))
            volumeSequence.append(volume)
        sequence = ivw.data.VolumeSequence(volumeSequence)
        self.volumeOutport.setData(sequence)
