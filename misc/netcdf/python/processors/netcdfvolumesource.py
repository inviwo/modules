# Name: NetCDFVolumeSource

import inviwopy as ivw
from inviwopy.properties import IntVec3Property, FileProperty, OptionPropertyString, ButtonProperty, BoolProperty, CompositeProperty, DoubleMinMaxProperty, IntMinMaxProperty
from inviwopy.glm import dvec2, mat4, vec4
import netcdfutils
from genericnetcdfsource import GenericNetCDFSource

import numpy
from pathlib import Path
from netCDF4 import Dataset


class NetCDFVolumeSource(GenericNetCDFSource):

    def __init__(self, id, name):
        self.outputDimension = 3
        GenericNetCDFSource.initGeneric(self, id, name)

        self.volumeOutport = ivw.data.VolumeOutport("data3D")
        self.addOutport(self.volumeOutport, owner=False)

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

    def reloadData(self):
        extents = []
        GenericNetCDFSource.reloadData(self, extents)

        buffer = numpy.concatenate(self.data, axis=3)
        volume = ivw.data.Volume(buffer)
        if self.overwriteDataRange.value:
            volume.dataMap.dataRange = self.dataRange.value
        else:
            minVal = numpy.amin(buffer)
            maxVal = numpy.amax(buffer)
            volume.dataMap.dataRange = dvec2(minVal, maxVal)
        volume.dataMap.valueRange = dvec2(-10000, 10000.0)
        volume.dataMap.valueRange = dvec2(-1000, 1000.0)
        volume.modelMatrix = mat4(
            vec4(extents[2], 0, 0, 0),
            vec4(0, extents[1], 0, 0),
            vec4(0, 0, extents[0], 0),
            vec4(0, 0, 0, 1))
        self.volumeOutport.setData(volume)
