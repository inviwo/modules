# Name: AxisSliceAnnotation

import inviwopy as ivw

customLabelHelp: str = r'''Custom formatted label. The following placeholders are defined:
+ `{i}` slice index
+ `{n}` axis name
+ `{v}` axis value of the current slice index
+ `{u}` axis unit'''


class AxisSliceAnnotation(ivw.Processor):
    """
    Composes a formatted string for a given slice index and axis using the axis information
    of the input volume.
    """

    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.inport = ivw.data.VolumeInport("volume")
        self.addInport(self.inport)

        cb = ivw.properties.ConstraintBehavior
        self.sliceAxis = \
            ivw.properties.OptionPropertyInt(
                "sliceAxis", "Slice Axis",
                ivw.md2doc("Defines the volume axis of the slice"),
                [ivw.properties.IntOption("x", "X axis", 0),
                 ivw.properties.IntOption("y", "Y axis", 1),
                 ivw.properties.IntOption("z", "Z axis", 2)])
        self.slice = ivw.properties.IntProperty("sliceNumber", "Slice",
                                                ivw.md2doc("Index of the slice"),
                                                128, min=(1, cb.Immutable), max=(256, cb.Ignore))
        self.labelType = \
            ivw.properties.OptionPropertyString(
                "labelType", "Type of Slice Label",
                [ivw.properties.StringOption("slice", "Slice Index", "{i}"),
                 ivw.properties.StringOption("axisValueUnit", "Axis Value + Unit", "{v} [{u}]"),
                 ivw.properties.StringOption("axisValue", "Axis Value only", "{v}"),
                 ivw.properties.StringOption("custom",
                                             "Custom Format (example 'Slice {i}, {n} {v} [{u}]')",
                                             "custom")])
        self.customLabel = ivw.properties.StringProperty(
            "customLabel", "Custom Label", ivw.md2doc(customLabelHelp), "Slice {i}, {n} {v} [{u}]")
        self.output = ivw.properties.StringProperty(
            "output", "Output",
            ivw.md2doc("Resulting label for the given slice and axis."), "")
        self.output.readOnly = True

        self.addProperties([self.sliceAxis, self.slice, self.labelType,
                            self.customLabel, self.output])

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.AxisSliceAnnotation",
            displayName="Axis Slice Annotation",
            category="Python",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags([ivw.Tags.PY, ivw.Tag("Volume")]),
            help=ivw.unindentMd2doc(AxisSliceAnnotation.__doc__)
        )

    def getProcessorInfo(self):
        return AxisSliceAnnotation.processorInfo()

    def initializeResources(self):
        pass

    def process(self):
        volume = self.inport.getData()

        axis = self.sliceAxis.value
        value: float = (volume.basis[axis] * (self.slice.value - 1)
                        / volume.dimensions[axis] + volume.offset)[axis]

        formatString = self.labelType.selectedValue \
            if self.labelType.selectedValue != "custom" else self.customLabel.value

        self.output.value = formatString.format(
            i=self.slice.value,
            v=value,
            n=volume.axes[axis].name,
            u=volume.axes[axis].unit
        )
