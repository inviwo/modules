# Name: FitsPolarizationSource

import inviwopy as ivw
import ivwbase

import os
import numpy as np
from astropy.io import fits


class FitsPolarizationSource(ivw.Processor):
    """
    Reads two FITS files containing the angle and intensity of the polarization and
    outputs the resulting 2D vector field
    """

    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.deserializing = False

        self.outport = ivw.data.VolumeOutport("polarization")
        self.addOutport(self.outport)

        self.filenameAngle = ivw.properties.FileProperty(
            "filenameAngle", "Angle", "", "fits")
        self.filenameAngle.addNameFilter(ivw.properties.FileExtension("fits", "FITS file format"))
        self.filenameIntensity = ivw.properties.FileProperty(
            "filenameIntensity", "Intensity", "", "fits")
        self.filenameIntensity.addNameFilter(
            ivw.properties.FileExtension("fits", "FITS file format"))
        self.basis = ivwbase.properties.BasisProperty("basis", "Basis and offset")
        self.information = ivwbase.properties.VolumeInformationProperty(
            "information", "Data information")
        self.addProperties([self.filenameAngle, self.filenameIntensity,
                            self.basis, self.information])

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.FitsPolarizationSource",
            displayName="FitsPolarizationSource",
            category="Python",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags.PY,
            help=ivw.unindentMd2doc(FitsPolarizationSource.__doc__)
        )

    def getProcessorInfo(self):
        return FitsPolarizationSource.processorInfo()

    def initializeResources(self):
        pass

    @staticmethod
    def createVectorField(angle: np.array, length: np.array,
                          use_degree: bool = True) -> ivw.data.Volume:

        # angle = np.ma.fix_invalid(angle, fill_value=0.).data
        # length = np.ma.fix_invalid(length, fill_value=1.0).data

        if use_degree:
            angle = angle * np.pi / 180.0

        v = np.stack([np.cos(angle + np.pi / 2.0) * length,
                      np.sin(angle + np.pi / 2.0) * length], axis=3)

        volumerep = ivw.data.VolumePy(np.copy(v.astype(dtype=np.float32), order='C'))

        volume = ivw.data.Volume(volumerep)

        # TODO: extract extent from hdul[0].header
        volume.basis = ivw.glm.mat3(1)
        volume.offset = ivw.glm.vec3(-0.5, -0.5, -0.5)

        max_len: float = np.nanmax(length)
        datarange = ivw.glm.dvec2(-max_len, max_len)
        volume.dataMap.dataRange = datarange
        volume.dataMap.valueRange = datarange
        return volume

    def process(self):
        if not self.filenameAngle.value or not self.filenameIntensity:
            return

        filenameAngle = ivwbase.io.downloadAndCacheIfUrl(self.filenameAngle.valueAsString())
        if not os.path.isfile(filenameAngle):
            return
        filenameIntensity = ivwbase.io.downloadAndCacheIfUrl(self.filenameIntensity.valueAsString())
        if not os.path.isfile(filenameIntensity):
            return

        with fits.open(filenameAngle) as hdul_angle, fits.open(filenameIntensity) as hdul_intensity:
            if hdul_angle[0].data.shape != hdul_intensity[0].data.shape:
                raise ValueError(f'Shape differs between polarization angles '
                                 f'({hdul_angle[0].data.shape}) and intensity '
                                 f'({hdul_intensity[0].data.shape})')
            use_degree: bool = hdul_angle[0].header['bunit'].lower() == "deg"

            volume: ivw.data.Volume = FitsPolarizationSource.createVectorField(
                hdul_angle[0].data,
                hdul_intensity[0].data,
                use_degree=use_degree)

            volume.dataMap.valueAxis.name = "Polarity"
            # hdul_intensity[0].header['bunit'] is 'Jy/beam ', which is not a valid unit
            volume.dataMap.valueAxis.unit = ivw.data.Unit("Jy")
            volume.swizzlemask = ivw.data.SwizzleMask.defaultData(2)

            from inviwopy.properties import OverwriteState as ows

            self.basis.updateForNewEntity(volume, self.deserializing)
            self.information.updateForNewVolume(volume, ows.Yes if self.deserializing else ows.No)
            self.deserializing = False

            self.basis.updateEntity(volume)
            self.information.updateVolume(volume)

            self.outport.setData(volume)

    def deserialize(self, deserializer: ivw.Deserializer):
        ivw.Processor.deserialize(self, deserializer)
        self.deserializing = True
