# Name: MergeTreeMesh

import inviwopy as ivw
import math


from inviwopy.glm import mat4, vec4

class MergeTreeMesh(ivw.Processor):
    """
    Documentation of MergeTreeMesh
    """

    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.inport = df.DataFrameInport("inport")
        self.addInport(self.inport, owner=False)

        self.nodeInfo = df.DataFrameInport("nodeInfo")
        self.addInport(self.nodeInfo, owner=False)

        self.cpBnlInport = ivwbnl.BrushingAndLinkingInport("cpBnL", [])
        self.addInport(self.cpBnlInport)

        self.atomBnlInport = ivwbnl.BrushingAndLinkingInport("atomBnL", [])
        self.addInport(self.atomBnlInport)

        self.outport = ivw.data.MeshOutport("outport")
        self.addOutport(self.outport, owner=False)

        self.mode = ivw.properties.OptionPropertyInt("mode", "Mode",
            [("tree", "Tree", 0),("bar", "Bar", 1)], 0)
        self.addProperty(self.mode, owner=False)

        self.colorTf = ivw.properties.TransferFunctionProperty("colorTf", "Critical Point Colors")
        self.addProperty(self.colorTf, owner=False)

        self.scale = ivw.properties.FloatProperty("scale", "Vertical Scale", 1, -2, 2, 0.01)
        self.addProperty(self.scale, owner=False)

        self.text = ivw.properties.StringProperty("text", "Text")
        self.addProperty(self.text, owner=False)

        self.pm = ivw.PickingMapper(self, 1, lambda x: self.pickCallback(x))

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.MergeTreeMesh",
            displayName="MergeTree Mesh",
            category="Python",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags.PY,
            help=ivw.unindentMd2doc(MergeTreeMesh.__doc__)
        )

    def getProcessorInfo(self):
        return MergeTreeMesh.processorInfo()

    @staticmethod
    def traverse(node, map, preorder = None, inorder = None, postorder = None):
        def recursive(n, parent, depth, index):
            children = map[n] if n in map else []
            if preorder is not None: preorder(n, depth, parent, index)
            if len(children) > 0: recursive(children[0], n, depth + 1, 0)
            if inorder is not None: inorder(n, depth, parent, index)
            if len(children) > 1: recursive(children[1], n, depth + 1, 1)
            if postorder is not None: postorder(n, depth, parent, index)
        recursive(node, None, 0, 0)

    def buildTree(self, info):
        picking = [ self.pm.pickingId(node) for node in info.getColumn("NodeId") ]
        colors = [ self.colorTf.value.sample(type / 3.0) for type in info.getColumn("CriticalType") ]
        positions = [ [0, s, 0] for s in info.getColumn("Scalar") ]
        labelInds = [ node for node in info.getColumn("NodeId") ]
        pointInds = [ node for node in info.getColumn("NodeId") ]

        root = next(iter(self.upMap))
        while root in self.upMap:
            root = self.upMap[root]

        xpos = 0
        def inorder(node, depth, parent, index):
            nonlocal xpos
            positions[node][0] = xpos
            xpos += 1

        MergeTreeMesh.traverse(root, self.downMap, inorder = inorder)

        lineInds = []
        def preorder(node, depth, parent, index):
            if parent is not None:
                lineInds.append(node)
                lineInds.append(parent)

        MergeTreeMesh.traverse(root, self.downMap, preorder = preorder)

        mesh = ivw.data.Mesh()

        scale = math.pow(10, self.scale.value)
        mesh.worldMatrix = mat4(vec4(1.0, 0.0, 0.0, 0.0),
                                vec4(0.0, scale, 0.0, 0.0),
                                vec4(0.0, 0.0, 1.0, 0.0),
                                vec4(0.0, 0.0, 0.0, 1.0))


        mesh.addBuffer(ivw.data.BufferType.PositionAttrib, ivw.data.BufferVec3FLOAT32(
            np.array(positions).astype(np.float32)))
        mesh.addBuffer(ivw.data.BufferType.ColorAttrib, ivw.data.BufferVec4FLOAT32(
            np.array(colors).astype(np.float32)))
        mesh.addBuffer(ivw.data.BufferType.PickingAttrib, ivw.data.BufferUINT32(
            np.array(picking).astype(np.uint32)))
        mesh.addBuffer(ivw.data.BufferType.IndexAttrib, ivw.data.BufferUINT32(
            np.array(labelInds).astype(np.uint32)))

        lineNone = ivw.data.MeshInfo(ivw.data.DrawType.Lines, ivw.data.ConnectivityType.Unconnected)
        mesh.addIndices(lineNone, ivw.data.IndexBufferUINT32(np.array(lineInds).astype(np.uint32)))

        pointNone = ivw.data.MeshInfo(ivw.data.DrawType.Points, ivw.data.ConnectivityType.Unconnected)
        mesh.addIndices(pointNone, ivw.data.IndexBufferUINT32(np.array(pointInds).astype(np.uint32)))
        return mesh


    def buildBars(self, info):

        picking = [ [self.pm.pickingId(node),self.pm.pickingId(node)]
                    for node in info.getColumn("NodeId") ]
        colors = [ [self.colorTf.value.sample(type / 3.0), self.colorTf.value.sample(type / 3.0) ]
                    for type in info.getColumn("CriticalType") ]
        positions = [ [[-10, s, 0], [10, s, 0]] for i,s in enumerate(info.getColumn("Scalar")) ]
        labelInds = [ [node, node] for node in info.getColumn("NodeId") ]

        lineInds =  [i for i in range(len(positions)*2) ]

        mesh = ivw.data.Mesh()

        scale = math.pow(10, self.scale.value)
        mesh.worldMatrix = mat4(vec4(1.0, 0.0, 0.0, 0.0),
                                vec4(0.0, scale, 0.0, 0.0),
                                vec4(0.0, 0.0, 1.0, 0.0),
                                vec4(0.0, 0.0, 0.0, 1.0))

        mesh.addBuffer(ivw.data.BufferType.PositionAttrib, ivw.data.BufferVec3FLOAT32(
            np.concatenate(positions, dtype=np.float32)))
        mesh.addBuffer(ivw.data.BufferType.ColorAttrib, ivw.data.BufferVec4FLOAT32(
            np.concatenate(colors, dtype=np.float32)))
        mesh.addBuffer(ivw.data.BufferType.PickingAttrib, ivw.data.BufferUINT32(
            np.concatenate(picking, dtype=np.uint32, casting='unsafe')))
        mesh.addBuffer(ivw.data.BufferType.IndexAttrib, ivw.data.BufferUINT32(
            np.concatenate(labelInds, dtype=np.uint32, casting='unsafe')))

        lineNone = ivw.data.MeshInfo(ivw.data.DrawType.Lines, ivw.data.ConnectivityType.Unconnected)
        mesh.addIndices(lineNone, ivw.data.IndexBufferUINT32(np.array(lineInds).astype(np.uint32)))

        return mesh


    def process(self):
        df = self.inport.getData()
        info = self.nodeInfo.getData()

        ups = df.getColumn("upNodeId")
        downs = df.getColumn("downNodeId") 

        self.downMap = {}
        self.upMap = {}
        for (up, down) in zip(ups, downs):
            if up in self.downMap:
                self.downMap[up].append(down)
            else:
                self.downMap[up] = [down]

            if down in self.upMap:
                print("ERROR multiple parents")
            else:
                self.upMap[down] = up


        self.pm.resize(len(self.upMap)+1)

        if self.mode.value == 0:
            mesh = self.buildTree(info)
            self.outport.setData(mesh)

        elif self.mode.value == 1:
            mesh = self.buildBars(info)
            self.outport.setData(mesh)

    def children(self, n):
        if n in self.downMap:
            item = [n]
            for c in self.downMap[n]:
                item.extend(self.children(c))
            return item
        else:
            return [n]


    def pickCallback(self, pickEvent):
        if pickEvent.pressState == ivw.PickingPressState.NoPress:
            if pickEvent.hoverState == ivw.PickingHoverState.Enter:
                i = pickEvent.pickedId
                children = self.children(i)
                self.cpBnlInport.highlight(ivw.data.BitSet(children))

                tooltip = f"NodeID {i}"
                if self.nodeInfo.hasData():
                    info = self.nodeInfo.getData()
                    for col in info:
                        tooltip += f"\n{col.header}: {col.getAsString(i)}"

                    atoms = []
                    for c in children:
                        if c not in self.downMap:
                            atoms.append(info.getColumn("assigned_atom_index")[c])

                    self.atomBnlInport.highlight(ivw.data.BitSet(atoms))
                    #self.atomBnlInport.filter("foo", ivw.data.BitSet(atoms))

                self.text.value = tooltip
                # pickEvent.setToolTip(tooltip)


            elif pickEvent.hoverState == ivw.PickingHoverState.Exit:
                self.cpBnlInport.highlight(ivw.data.BitSet())
                self.atomBnlInport.highlight(ivw.data.BitSet())
                # self.atomBnlInport.filter("foo", ivw.data.BitSet())
                # pickEvent.setToolTip("")
                self.text.value = ""

        if (pickEvent.pressState == ivw.PickingPressState.Release
            and pickEvent.pressItem == ivw.PickingPressItem.Primary
            and abs(pickEvent.deltaPressedPosition.x) < 0.01
                and abs(pickEvent.deltaPressedPosition.y) < 0.01):

            selection = self.cpBnlInport.getSelectedIndices()
            selection.flip(pickEvent.pickedId)
            # selection = inviwopy.data.BitSet([pickEvent.pickedId])
            self.cpBnlInport.select(selection)
