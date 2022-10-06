# Name: StreamFunctionSolver

from cmath import inf
from sys import get_coroutine_origin_tracking_depth
import inviwopy as ivw
from inviwopy.properties import IntProperty, FloatMinMaxProperty  # FileProperty, ButtonProperty, BoolProperty, \
# BoolCompositeProperty, CompositeProperty, IntMinMaxProperty, StringProperty
# import scipy
import numpy as np
import scipy.sparse
import scipy.sparse.linalg


class MarkValueRange(ivw.Processor):
    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.fieldIn = ivw.data.ImageInport("fieldIn")
        self.markedFieldOut = ivw.data.ImageOutport("markedField")
        self.fieldIn.setOutportDeterminesSize(True)
        self.markedFieldOut.setHandleResizeEvents(False)
        self.addOutport(self.markedFieldOut)
        self.addInport(self.fieldIn)
        self.valueRange = ivw.properties.FloatMinMaxProperty(
            "valueRange", "Value Range", 1, 0, 10)
        self.addProperty(self.valueRange)

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.vectorvis.MarkValueRange",
            displayName="Mark Value Range",
            category="VectorVis",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags([ivw.Tag.PY, ivw.Tag("Image")])
        )

    def getProcessorInfo(self):
        return StreamFunctionSolver.processorInfo()

    def process(self):
        if (not self.fieldIn.hasData()):
            print("No field :(")
            return

        field = self.fieldIn.getData()
        fieldData = field.colorLayers[0].data.squeeze()

        fullFieldSize = fieldData.shape
        fieldSize = fullFieldSize[:2]
        numElements = fieldSize[0] * fieldSize[1]
        self.valueRange.minValue = ndarray.min(fieldData)
        self.valueRange.maxValue = ndarray.max(fieldData)

        # Mask
        result = fieldData.copy()
        result[result == 0] = self.valueRange.maxValue
        result[result < self.valueRange[0]] = 0
        result[result > self.valueRange[1]] = 0

        # result[fieldDataY == 0] = -100
        # result[validCells] = solved[0]
        fieldSize = np.flip(fieldSize)  # URRRGH! BIG URRRRGH!
        imageLayer = ivw.data.Layer(result.reshape(fieldSize))
        image = ivw.data.Image(imageLayer)
        self.markedFieldOut.setData(image)

        # At some point, solve: np.linalg.lstsq(gradientOperator, perpendicularVectorField)
