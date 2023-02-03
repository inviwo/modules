# Name: IntToSelection

import inviwopy as ivw
import ivwbnl as bl


class IntToSelection(ivw.Processor):
    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.inport = bl.BrushingAndLinkingInport("inport")
        self.addInport(self.inport, owner=False)

        self.slider = ivw.properties.IntProperty("selection", "selection", 0, 0, 100, 1)
        self.addProperty(self.slider, owner=False)

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.IntToSelection",
            displayName="IntToSelection",
            category="Python",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags.PY
        )

    def getProcessorInfo(self):
        return IntToSelection.processorInfo()

    def process(self):
        if self.slider.isModified:
            self.inport.select(ivw.data.BitSet([self.slider.value]), bl.BrushingTarget.Row)
        if self.inport.isSelectionModified():
            selection = self.inport.getSelectedIndices(bl.BrushingTarget.Row)
            if not selection.empty():
                self.slider.value = next(iter(selection))
