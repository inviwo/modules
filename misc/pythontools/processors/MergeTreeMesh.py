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

        self.dataRange = ivw.properties.DoubleMinMaxProperty("dataRange", "Data Range")
        self.addProperty(self.dataRange, owner=False)
        self.dataRange.semantics = inviwopy.properties.PropertySemantics.Text

        self.tf = ivw.properties.TransferFunctionProperty("tf", "TF")
        self.addProperty(self.tf, owner=False)
        self.tf.invalidationLevel = inviwopy.properties.InvalidationLevel.Valid

        self.iso = ivw.properties.FloatProperty("iso", "Critical Point ISO", 0.0, -10.0, 10.0, 0.001)
        self.addProperty(self.iso, owner=False)
        self.iso.readOnly = True

        self.shift = ivw.properties.FloatProperty("shift", "ISO Shift", 0.0, -0.10, 0.10, 0.001)
        self.addProperty(self.shift, owner=False)

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
    def traverse(node, downMap, start, end, leaf, parent = -1, depth = 0, index = 0, count = 1):
        if node in downMap:
            start(node, depth, parent, index, count)
            for i, c in enumerate(downMap[node]):
                MergeTreeMesh.traverse(c, downMap, start, end, leaf, node, depth+1,i, len(downMap[node]))
            end(node, depth, parent, index, count)
        else:
            leaf(node, depth, parent, index, count)

    def updateTF(self, iso):
        tfd = inviwopy.data.TFPrimitiveData

        iso = 0 if iso < 0 else iso
        iso = 1 if iso > 1 else iso

        self.tf.value.set([
            tfd(iso, 0.0, [0,0,0]),
            tfd(iso + 0.00000001, 1.0, [1,1,1])
        ])


    def calcNodeSpace(self):
        def space(n):
            if n in self.downMap:
                return 1 + sum(space(c) for c in self.downMap[n])
            else:
                return 1

        self.nodeSpace = [ space(n) for n in range(len(self.upMap)+1) ]

        for key, value in self.downMap.items():
            if len(value) == 2:
                r = self.nodeSpace[value[0]]
                l = self.nodeSpace[value[1]]
                if r < l:
                    self.downMap[key] = [value[1], value[0]]


    def buildTree(self, info):
        picking = [ self.pm.pickingId(node) for node in info.getColumn("NodeId") ]
        colors = [ self.colorTf.value.sample(type / 3.0) for type in info.getColumn("CriticalType") ]

        positions = [ [0, s, 0] for i,s in enumerate(info.getColumn("Scalar")) ]
        labelInds = [ node for node in info.getColumn("NodeId") ]
        pointInds = [ node for node in info.getColumn("NodeId") ]

        lineInds = []
        def start(node, depth, parent, index, count):
            ppos = positions[parent][0] if parent >=0 else 0

            if parent >= 0:
                lineInds.append(node)
                lineInds.append(parent)

            if index == 0:
                if node in self.downMap and len(self.downMap[node]) > 1:
                    positions[node][0] = ppos - 1 - self.nodeSpace[self.downMap[node][1]]
                else:
                    positions[node][0] = ppos - 1

            elif index == 1:
                if node in self.downMap and len(self.downMap[node]) > 0:
                    positions[node][0] = ppos + 1 + self.nodeSpace[self.downMap[node][0]]
                else:
                    positions[node][0] = ppos + 1

            else:
                raise Exception("Unexpected tree structure")


        def end(node, depth, parent, index, count):
            pass


        root = next(iter(self.upMap))
        while root in self.upMap:
            root = self.upMap[root]

        MergeTreeMesh.traverse(root, self.downMap, start, end, start)

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
            self.calcNodeSpace()
            mesh = self.buildTree(info)
            self.outport.setData(mesh)

        elif self.mode.value == 1:
            mesh = self.buildBars(info)
            self.outport.setData(mesh)

        [min, max] = self.dataRange.value
        self.updateTF((self.iso.value + self.shift.value - min) / (max - min))

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

            [min, max] = self.dataRange.value

            if selection.contains(pickEvent.pickedId):
                iso = self.nodeInfo.getData().getColumn("Scalar").get(pickEvent.pickedId)
            elif not selection.empty():
                iso = self.nodeInfo.getData().getColumn("Scalar").get(selection.min())
            else:
                iso = min

            self.iso.value = iso
            self.updateTF((iso + self.shift.value - min) / (max - min))
