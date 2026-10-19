# Name: PlotColumnConfiguration

import inviwopy as ivw
import ivwdataframe
from enum import Enum


class Coloring(Enum):
    Regular = 0
    Sequence = 1


def createColorMappingComposite(identifier: str,
                                displayName: str,
                                dataFrameInport: ivwdataframe.DataFrameInport
                                ) -> ivw.properties.CompositeProperty:
    # cb = ivw.properties.ConstraintBehavior
    IntOption = ivw.properties.IntOption
    # inv = ivw.properties.InvalidationLevel
    # semantics = ivw.properties.PropertySemantics

    coloring = ivw.properties.OptionPropertyInt(
        "coloring", "Coloring",
        options=[IntOption("regular", "Regular", Coloring.Regular.value),
                 IntOption("sequence", "Sequence", Coloring.Sequence.value)],
        selectedIndex=0)

    colorColumn = ivwdataframe.ColumnOptionProperty(
        "colorColumn", "Color Column",
        help=ivw.md2doc("DataFrame Column used for coloring"),
        port=dataFrameInport)
    sequenceColorColumn = ivw.properties.StringProperty("seqColorColumn", "Sequence Color Column",
                                                        "sequenceIndex")
    tf = ivw.properties.TransferFunctionProperty("tf", "Color Map")

    prop = ivw.properties.CompositeProperty(identifier, displayName)
    prop.addProperties([tf, coloring, colorColumn, sequenceColorColumn])
    return prop


class PlotColumnConfiguration(ivw.Processor):
    """
    This processor provides various properties to configure the plotting of C3D sequence data.
    """

    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.inport = ivwdataframe.DataFrameInport("inport")
        self.addInport(self.inport)

        self.plotProps = [createColorMappingComposite("xyPlot", "XY Plot", self.inport),
                          createColorMappingComposite("depthPlot", "Depth Plot", self.inport),
                          createColorMappingComposite("distPlot", "Distance Plot", self.inport)]

        self.output = ivw.properties.CompositeProperty("output", "Output")
        self.output.collapsed = True

        self.sequenceTf = ivw.properties.TransferFunctionProperty("seqTf", "Sequence Color Map")

        for p in self.plotProps:
            col = ivw.properties.StringProperty(
                f"{p.identifier}ColorColumn", f"{p.displayName} Color Column")
            tf = ivw.properties.TransferFunctionProperty(
                f"{p.identifier}Tf", f"{p.displayName} Color Map")

            def update(p: ivw.properties.CompositeProperty, column, tf):
                def updateOutput():
                    if not self.inport.hasData():
                        return

                    if Coloring(p.properties.coloring.value) == Coloring.Sequence:
                        column.value = p.properties.seqColorColumn.value
                        tf.value = self.properties.seqTf.value
                    else:
                        column.value = p.properties.colorColumn.selectedColumnHeader
                        tf.value = p.properties.tf.value

                return updateOutput

            p.onChange(update(p, col, tf))
            self.output.addProperties([col, tf])

        def updateAll():
            for p in self.plotProps:
                p.propertyModified()

        self.sequenceTf.onChange(updateAll)

        self.output.readOnly = True
        self.addProperties([self.sequenceTf] + self.plotProps + [self.output])

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.PlotColumnConfiguration",
            displayName="PlotColumnConfiguration",
            category="Python",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags([ivw.Tags.PY, ivw.Tag("Utility"), ivw.Tag("InfraVis")]),
            help=ivw.unindentMd2doc(PlotColumnConfiguration.__doc__)
        )

    def getProcessorInfo(self):
        return PlotColumnConfiguration.processorInfo()

    def initializeResources(self):
        pass

    def process(self):
        pass
