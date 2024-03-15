# Name: TensorField3DSource

import inviwopy as ivw
import ivwtensorvis

import numpy


class TensorField3DSource(ivw.Processor):
    """
    Example processor in Python demonstrating the use of `ivwtensorvis.TensorField3D`
    and its matching outport.
    """

    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)

        self.addOutport(ivwtensorvis.TensorField3DOutport("tensors"))
        self.dim = ivw.properties.IntVec3Property("dim", "Dimensions",
                                                  ivw.glm.ivec3(4, 3, 1),
                                                  ivw.glm.ivec3(1),
                                                  ivw.glm.ivec3(20))
        self.addProperty(self.dim)

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.TensorField3DSource",
            displayName="TensorField3D Source Python",
            category="Data Input",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags("PY, TensorVis, Example"),
            help=ivw.unindentMd2doc(TensorField3DSource.__doc__)
        )

    def getProcessorInfo(self):
        return TensorField3DSource.processorInfo()

    def initializeResources(self):
        pass

    def process(self):
        dim = self.dim.value
        # create a dim.x x dim.y x dim.z volume with 3x3 tensors
        # tensor values are stored in column-major and increase continuously
        #
        # Note: TensorField3D will convert tensor data to double precision
        arr = numpy.arange(9 * ivw.glm.compMul(dim), dtype=numpy.uint8)
        arr.shape = (dim.z, dim.y, dim.x, 9)

        tensorfield = ivwtensorvis.TensorField3D(arr)

        self.outports.tensors.setData(tensorfield)
