# Name: NetCDFImageSource

import inviwopy as ivw
from inviwopy.properties import IntVec3Property, FileProperty, OptionPropertyString, ButtonProperty, BoolProperty, CompositeProperty, DoubleMinMaxProperty, IntMinMaxProperty
from inviwopy.glm import dvec2, mat3, vec3
import netcdfutils
from genericnetcdfsource import GenericNetCDFSource

import numpy
from pathlib import Path
from netCDF4 import Dataset


class NetCDFImageSource(GenericNetCDFSource):

    def __init__(self, id, name):
        self.outputDimension = 2
        GenericNetCDFSource.initGeneric(self, id, name)

        self.imageOutport = ivw.data.ImageOutport("data3D")
        self.addOutport(self.imageOutport, owner=False, group="NetCDF")

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.netcdf.netcdfimagesource",
            displayName="NetCDF Image Source",
            category="Source",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags([ivw.Tag.PY, ivw.Tag("NetCDF"),
                           ivw.Tag("Image")])
        )

    def getProcessorInfo(self):
        return NetCDFImageSource.processorInfo()

    def process(self):
        GenericNetCDFSource.process(self)
        if len(self.filePath.value) == 0 or not Path(self.filePath.value).exists() or self.variables.size() <= 0:
            return
        self.reloadData()

    def reloadData(self):
        extents = []
        GenericNetCDFSource.reloadData(self, extents)

        buffer = numpy.concatenate(self.data, axis=2)
        imageLayer = ivw.data.Layer(buffer)
        image = ivw.data.Image(imageLayer)
        self.imageOutport.setData(image)
