# Name: TensorField3DExample

import inviwopy as ivw
import ivwtensorvis

import numpy


class TensorField3DExample(ivw.Processor):
    """
    Example processor in Python demonstrating the use of `ivwtensorvis.TensorField3D`.
    """

    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)

        self.addInport(ivwtensorvis.TensorField3DInport("tensors"))
        self.addProperty(ivw.properties.IntVec3Property("dim", "Dimensions",
                                                        ivw.glm.ivec3(1),
                                                        ivw.glm.ivec3(1),
                                                        ivw.glm.ivec3(20)))
        self.properties.dim.readOnly = True

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.TensorField3DExample",
            displayName="TensorField3D Example",
            category="Tensor",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags("PY, TensorVis, Example"),
            help=ivw.unindentMd2doc(TensorField3DExample.__doc__)
        )

    def getProcessorInfo(self):
        return TensorField3DExample.processorInfo()

    def initializeResources(self):
        pass

    def process(self):
        tensorfield = self.inports.tensors.getData()

        self.properties.dim.value = ivw.glm.ivec3(tensorfield.dimensions)

        print(f'Tensor data\nshape: {tensorfield.data.shape}\ndata: {tensorfield.data}')
