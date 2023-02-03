# Name: PythonBnLMeshExample

import inviwopy as ivw
import numpy as np

import ivwbnl


class PythonBnLMeshExample(ivw.Processor):
    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.outport = ivw.data.MeshOutport("outport")
        self.addOutport(self.outport, owner=False)

        self.bnlInport = ivwbnl.BrushingAndLinkingInport("bnl", [])
        self.addInport(self.bnlInport)

        self.size = ivw.properties.IntProperty("gridSize", "Grid Size", 10, 0, 1000, 1)
        self.addProperty(self.size, owner=False)

        self.tf = ivw.properties.TransferFunctionProperty("tf", "TF", ivw.doc.Document(),
                                                          ivw.data.TransferFunction())
        self.addProperty(self.tf, owner=False)

        self.pm = ivw.PickingMapper(self, 1, lambda x: self.callback(x))

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.PythonBnLMeshExample",
            displayName="Python BnL Mesh Example",
            category="Python",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags.PY
        )

    def getProcessorInfo(self):
        return PythonBnLMeshExample.processorInfo()

    def process(self):
        size = self.size.value

        self.positions = []
        colors = []
        picking = []
        index = []

        triangles = []

        self.pm.resize(size * size)
        count = 0
        for x in range(size):
            for y in range(size):
                i = len(self.positions)
                self.positions.append([x, y, 0])
                self.positions.append([x + 1, y, 0])
                self.positions.append([x, y + 1, 0])
                self.positions.append([x + 1, y + 1, 0])

                index.extend([count, count, count, count])
                color = self.tf.value.sample(count / (size * size))
                colors.extend([color, color, color, color])
                pid = self.pm.pickingId(count)
                picking.extend([pid, pid, pid, pid])

                triangles.extend([i, i + 1, i + 2])
                triangles.extend([i + 1, i + 3, i + 2])

                count += 1

        mesh = ivw.data.Mesh(ivw.data.DrawType.Triangles, ivw.data.ConnectivityType.Unconnected)

        mesh.addBuffer(ivw.data.BufferType.PositionAttrib, ivw.data.Buffer(
            np.asarray(self.positions, dtype=np.float32)))
        mesh.addBuffer(ivw.data.BufferType.ColorAttrib, ivw.data.Buffer(
            np.asarray(colors, dtype=np.float32)))
        mesh.addBuffer(ivw.data.BufferType.PickingAttrib, ivw.data.Buffer(
            np.asarray(picking, dtype=np.uint32)))
        mesh.addBuffer(ivw.data.BufferType.IndexAttrib, ivw.data.Buffer(
            np.asarray(index, dtype=np.uint32)))

        ib = mesh.addIndexBuffer(ivw.data.DrawType.Triangles,
                                 ivw.data.ConnectivityType.Unconnected)
        ib.size = len(triangles)
        ib.data = np.asarray(triangles, dtype=np.uint32)

        self.outport.setData(mesh)

    def callback(self, pickEvent):
        if pickEvent.pressState == ivw.PickingPressState.NoPress:
            if pickEvent.hoverState == ivw.PickingHoverState.Enter:
                i = pickEvent.pickedId
                self.bnlInport.highlight(ivw.data.BitSet([i]))

                p1 = self.positions[i * 4 + 0]
                p2 = self.positions[i * 4 + 1]
                p3 = self.positions[i * 4 + 2]
                p4 = self.positions[i * 4 + 2]
                pickEvent.setToolTip(f"id: {i} Pos: {p1}, {p2}, {p3} {p4}")

            elif pickEvent.hoverState == ivw.PickingHoverState.Exit:
                self.bnlInport.highlight(ivw.data.BitSet())
                pickEvent.setToolTip("")

        if (pickEvent.pressState == ivw.PickingPressState.Release
            and pickEvent.pressItem == ivw.PickingPressItem.Primary
            and pickEvent.deltaPressedPosition.x == 0
                and pickEvent.deltaPressedPosition.y == 0):

            selection = self.bnlInport.getSelectedIndices()
            selection.flip(pickEvent.pickedId)
            self.bnlInport.select(selection)
