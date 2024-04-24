# Name: ExtractFilename

import inviwopy as ivw


class ExtractFilename(ivw.Processor):
    """
    Extracts the different parts from a file path and lists them as read-only
    string properties.
    """

    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.sourcefile = ivw.properties.FileProperty("sourcefile", "File Name")
        self.addProperty(self.sourcefile)

        self.addProperty(ivw.properties.CompositeProperty("pathcomponents", "Path Components"))

        self.path = ivw.properties.StringProperty("filepath", "Path")
        self.path.readOnly = True
        self.name = ivw.properties.StringProperty("filename", "Name")
        self.name.readOnly = True
        self.stem = ivw.properties.StringProperty("filestem", "stem")
        self.stem.readOnly = True
        self.extension = ivw.properties.StringProperty("fileextension", "extension")
        self.extension.readOnly = True

        props = [self.path, self.name, self.stem, self.extension]
        self.properties.pathcomponents.addProperties(props)

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.ExtractFilename",
            displayName="ExtractFilename",
            category="Information",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags([ivw.Tags.PY, ivw.Tag("Information"), ivw.Tag("Utility")]),
            help=ivw.unindentMd2doc(ExtractFilename.__doc__)
        )

    def getProcessorInfo(self):
        return ExtractFilename.processorInfo()

    def initializeResources(self):
        pass

    def process(self):
        self.path.value = str(self.sourcefile.value.parent)
        self.name.value = str(self.sourcefile.value.name)
        self.stem.value = str(self.sourcefile.value.stem)
        self.extension.value = str(self.sourcefile.value.suffix)
