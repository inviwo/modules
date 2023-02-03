# Name: ScalarGenerator

import inviwopy as ivw
import numpy as np
import math


class ScalarGenerator(ivw.Processor):
    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.outport = ivw.floatVectorOutport("outport")
        self.addOutport(self.outport, owner=False)

        self.count = ivw.properties.IntProperty("count", "count", 0, 0, 1000, 1)
        self.amplitude = ivw.properties.FloatProperty("amplitude", "Amplitude", 1, -100, 100, 1)
        self.frequency = ivw.properties.FloatProperty("frequency", "Frequency", 1, 0.001, 100, 1)
        self.phase = ivw.properties.FloatProperty("phase", "Phase", 0, -4 * math.pi, 4 * math.pi, 1)
        self.addProperty(self.count, owner=False)
        self.addProperty(self.amplitude, owner=False)
        self.addProperty(self.frequency, owner=False)
        self.addProperty(self.phase, owner=False)

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.ScalarGenerator",
            displayName="ScalarGenerator",
            category="Python",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags.PY
        )

    def getProcessorInfo(self):
        return ScalarGenerator.processorInfo()

    def process(self):
        self.outport.setData(ivw.floatVector(self.amplitude.value * np.sin(
            self.phase.value + math.pi * 2.0 * self.frequency.value * np.arange(
                0, self.count.value, 1, dtype=float))))
