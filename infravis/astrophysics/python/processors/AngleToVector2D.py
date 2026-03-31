# Name: AngleToVector2D

import inviwopy as ivw
import numpy as np


class AngleToVector2D(ivw.Processor):
    """
    Converts the angles of the input volume into vectors and scales these with an optional magnitude
    """

    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.addInport(ivw.data.VolumeInport("angle"))
        self.addInport(ivw.data.VolumeInport("magnitude"))
        self.inports.magnitude.optional = True

        self.addOutport(ivw.data.VolumeOutport("outport"))

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.AngleToVector2D",
            displayName="AngleToVector2D",
            category="Python",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags("PY, FITS, Astrophysics"),
            help=ivw.unindentMd2doc(AngleToVector2D.__doc__)
        )

    def getProcessorInfo(self):
        return AngleToVector2D.processorInfo()

    def initializeResources(self):
        pass

    def process(self):
        volume_angle = self.inports.angle.getData()
        volume_magnitude = self.inports.magnitude.getData()

        use_degree: bool = True
        # assume degree if anything other than radians is stated as unit
        if volume_angle.dataMap.valueAxis.unit == ivw.data.Unit("rad"):
            use_degree = False

        angle = volume_angle.data
        magnitude = volume_magnitude.data

        if use_degree:
            angle *= np.pi / 180.0
        data = np.stack([np.cos(angle + np.pi / 2.0) * magnitude,
                         np.sin(angle + np.pi / 2.0) * magnitude], axis=3)
        volumerep = ivw.data.VolumePy(np.copy(data.astype(dtype=np.float32), order='C'))
        volume = ivw.data.Volume(volumerep)

        max_len: float = np.nanmax(magnitude)
        datarange = ivw.glm.dvec2(-max_len, max_len)
        volume.dataMap.dataRange = datarange
        volume.dataMap.valueRange = datarange

        volume.dataMap.valueAxis = volume_magnitude.dataMap.valueAxis
        volume.modelMatrix = volume_magnitude.modelMatrix
        volume.worldMatrix = volume_magnitude.worldMatrix
        volume.axes = volume_magnitude.axes
        volume.copyMetaDataFrom(volume_magnitude)

        self.outports.outport.setData(volume)
