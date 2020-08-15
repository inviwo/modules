# Name: NetCDFImageSource

import inviwopy as ivw
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

    def dataLoaded(self, data, extents):
        buffer = numpy.concatenate(data, axis=2)
        imageLayer = ivw.data.Layer(buffer)
        image = ivw.data.Image(imageLayer)
        self.imageOutport.setData(image)
