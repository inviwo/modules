# Name: NetCDFImageSource

import inviwopy as ivw
<<<<<<< HEAD
=======
from inviwopy.properties import IntVec3Property, FileProperty, ButtonProperty, BoolProperty, CompositeProperty, DoubleMinMaxProperty, IntMinMaxProperty
from inviwopy.glm import dvec2, mat3, vec3
import netcdfutils
>>>>>>> 35eeab5 (WIP)
from genericnetcdfsource import GenericNetCDFSource
import numpy


class NetCDFImageSource(GenericNetCDFSource):
    def __init__(self, id, name):
        GenericNetCDFSource.__init__(self, id, name, outputDimension=2)
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

<<<<<<< HEAD
    def dataLoaded(self, data, extents):
        buffer = numpy.concatenate(data, axis=2)
=======
    def process(self):
        GenericNetCDFSource.process(self)
        if len(self.filePath.value) == 0 or not Path(self.filePath.value).exists() or self.variables.size() <= 0:
            return
        self.reloadData()

    def reloadData(self):
        GenericNetCDFSource.genReloadData(self)

        buffer = numpy.concatenate(self.data, axis=2)
>>>>>>> ea95e44 (WIP 2)
        imageLayer = ivw.data.Layer(buffer)
        image = ivw.data.Image(imageLayer)
        self.imageOutport.setData(image)
