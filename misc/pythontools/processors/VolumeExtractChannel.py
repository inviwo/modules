# Name: VolumeExtractChannel

import inviwopy as ivw
import numpy as np


class VolumeExtractChannel(ivw.Processor):
    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.inport = ivw.data.VolumeInport("inport")
        self.addInport(self.inport, owner=False)
        self.outport = ivw.data.VolumeOutport("outport")
        self.addOutport(self.outport, owner=False)

        self.channel = ivw.properties.IntProperty("channel", "channel", 0, 0, 4, 1)
        self.addProperty(self.channel, owner=False)

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.VolumeExtractChannel",
            displayName="Volume Extract Channel",
            category="Volume Operation",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags.PY
        )

    def getProcessorInfo(self):
        return VolumeExtractChannel.processorInfo()

    def process(self):
        volume = self.inport.getData()
        if len(volume.data.shape) <= 3:
            self.outport.setData(volume)
            return

        channels = volume.data.shape[3]

        volumeSlice = volume.data[:, :, :, np.clip(self.channel.value, 0, channels - 1)]
        newVolume = ivw.data.Volume(np.copy(volumeSlice))
        newVolume.dataMap = volume.dataMap
        newVolume.modelMatrix = volume.modelMatrix
        newVolume.worldMatrix = volume.worldMatrix
        newVolume.copyMetaDataFrom(volume)
        newVolume.swizzlemask = volume.swizzlemask
        newVolume.interpolation = volume.interpolation
        newVolume.wrapping = volume.wrapping

        self.outport.setData(newVolume)
