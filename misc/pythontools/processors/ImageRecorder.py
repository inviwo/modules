# Name: ImageRecorder

import imageio
import inviwopy as ivw


class ImageRecorder(ivw.Processor):
    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.inport = ivw.data.ImageInport("inport")
        self.addInport(self.inport, owner=False)
        self.outport = ivw.data.ImageOutport("outport")
        self.addOutport(self.outport, owner=False)

        self.file = ivw.properties.FileProperty("file", "File")
        self.addProperty(self.file, owner=False)

        self.start = ivw.properties.ButtonProperty("start", "Start Recording")
        self.addProperty(self.start, owner=False)
        self.stop = ivw.properties.ButtonProperty("stop", "Stop Recording")
        self.addProperty(self.stop, owner=False)
        self.save = ivw.properties.ButtonProperty("save", "Save Recording")
        self.addProperty(self.save, owner=False)
        self.recoding = False
        self.writer = None

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.ImageRecorder",
            displayName="Image Recorder",
            category="Python",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags.PY
        )

    def getProcessorInfo(self):
        return ImageRecorder.processorInfo()

    def initializeResources(self):
        pass

    def process(self):
        if self.start.isModified and not self.recoding:
            ivw.logInfo("Recording to : " + self.file.value)
            self.writer = imageio.get_writer(self.file.value, fps=30)
            self.recoding = True

        if self.stop.isModified and self.recoding:
            self.recoding = False

        if self.save.isModified and self.writer is not None:
            self.writer.close()
            self.writer = None
            self.recoding = False

        if self.recoding:
            data = self.inport.getData()
            self.writer.append_data(data.colorLayers[0].data)

        self.outport.setData(self.inport.getData())
