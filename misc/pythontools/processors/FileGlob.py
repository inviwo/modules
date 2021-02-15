# Name: FileGlob

import inviwopy as ivw
from inviwopy.properties import FileProperty, FilePatternProperty, ButtonProperty, IntProperty


class FileGlob(ivw.Processor):
    def __init__(self, id, name):
        try:
            ivw.Processor.__init__(self, id, name)

            self.slider = IntProperty("slider", "slider", 0, 0, 100, 1)
            self.addProperty(self.slider, owner=False)
            self.pattern = FilePatternProperty("pattern", "pattern", "", "")
            self.addProperty(self.pattern)

            self.out = FileProperty("out", "out", "", "")
            self.addProperty(self.out)
            self.out.readOnly = True

            self.triggerLoad = ButtonProperty("load", "Load")
            self.addProperty(self.triggerLoad)

            def patternChange():
                self.slider.maxValue = len(self.pattern.fileList)
            self.pattern.onChange(patternChange)

            def sliderChange():
                if self.slider.value < len(self.pattern.fileList):
                    self.out.value = self.pattern.fileList[self.slider.value]
                    self.load.press()
            self.slider.onChange(sliderChange)
        except Exception as e:
            print(f"Error: {e}")

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.FileGlob",
            displayName="FileGlob",
            category="Python",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags.PY
        )

    def getProcessorInfo(self):
        return FileGlob.processorInfo()

    def initializeResources(self):
        pass

    def process(self):
        pass
