# Name: MergeTreeBar

import inviwopy as ivw
import ivwdataframe as df


class MergeTreeBar(ivw.Processor):
    """
    Documentation of MergeTreeBar
    """

    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.inport = df.DataFrameInport("inport")
        self.addInport(self.inport, owner=False)
        self.outport = ivw.data.MeshOutport("outport")
        self.addOutport(self.outport, owner=False)

        self.tf = ivw.properties.TransferFunctionProperty("tf", "TF")
        self.addProperty(self.tf, owner=False)

        self.width = ivw.properties.FloatProperty("width", "Width", 1.0, 0.0, 100.0, 1)
        self.addProperty(self.width, owner=False)

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.MergeTreeBar",
            displayName="MergeTree Bar",
            category="Python",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags.PY,
            help=ivw.unindentMd2doc(MergeTreeBar.__doc__)
        )

    def getProcessorInfo(self):
        return MergeTreeBar.processorInfo()

    def process(self):
        df = self.inport.getData()

        scalars = df.getColumn("Scalar")
        criticalTypes = df.getColumn("CriticalType") 

        positions = []
        colors = []
        lines = []
        for (i,(scalar, criticalType)) in enumerate(zip(scalars, criticalTypes)):
            positions.append([0.0, scalar, i*10.0])
            positions.append([self.width.value, scalar, i*10.0])
            color = self.tf.value.sample(criticalType / 6.0);
            colors.append(color)
            colors.append(color)
            lines.append(2*i)
            lines.append(2*i+1)
            print(i)

        mesh = ivw.data.Mesh()


        mesh.addBuffer(ivw.data.BufferType.PositionAttrib, ivw.data.BufferVec3FLOAT32(
            np.array(positions).astype(np.float32)))
        mesh.addBuffer(ivw.data.BufferType.ColorAttrib, ivw.data.BufferVec4FLOAT32(
            np.array(colors).astype(np.float32)))

        lineNone = ivw.data.MeshInfo(ivw.data.DrawType.Lines, ivw.data.ConnectivityType.Unconnected)

        mesh.addIndices(lineNone, ivw.data.IndexBufferUINT32(np.array(lines).astype(np.uint32)))

        self.outport.setData(mesh)

