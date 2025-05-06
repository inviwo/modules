# Name: MergeTreeMesh

import inviwopy as ivw
import ivwmolvis
from inviwopy.glm import mat4, vec4
import ivwdataframe

from functools import cmp_to_key
import math
import enum

class Marker(enum.Enum):
    Circle = 0
    Diamond = 1
    Square = 2
    Hexagon = 3
    Plus = 4
    Cross = 5
    TriangleUp = 6
    TriangleLeft = 7
    TriangleDown = 8
    TriangleRight = 9


class MergeTreeMesh(ivw.Processor):
    """
    Documentation of MergeTreeMesh
    """

    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.inport = ivwdataframe.DataFrameInport("inport")
        self.addInport(self.inport, owner=False)

        self.nodeInfo = df.DataFrameInport("nodeInfo")
        self.addInport(self.nodeInfo, owner=False)

        self.cpBnlInport = ivwbnl.BrushingAndLinkingInport("cpBnL", [])
        self.addInport(self.cpBnlInport)

        self.atomBnlInport = ivwbnl.BrushingAndLinkingInport("atomBnL", [])
        self.addInport(self.atomBnlInport)

        self.outport = ivw.data.MeshOutport("outport")
        self.addOutport(self.outport, owner=False)

        self.labels = ivw.StringVectorOutport("labels")
        self.addOutport(self.labels, owner=False)

        self.mode = ivw.properties.OptionPropertyInt("mode", "Mode",
            [("tree", "Tree", 0),("bar", "Bar", 1)], 0)
        self.addProperty(self.mode, owner=False)

        self.colorTf = ivw.properties.TransferFunctionProperty("colorTf", "Critical Point Colors")
        self.addProperty(self.colorTf, owner=False)

        self.scale = ivw.properties.FloatProperty("scale", "Vertical Scale", 1, -2, 2, 0.01)
        self.addProperty(self.scale, owner=False)

        self.filter = ivw.properties.StringProperty("filter", "Filter")
        self.addProperty(self.filter, owner=False)

        self.collapse = ivw.properties.BoolProperty("collapse", "Collapse Nodes", False)
        self.addProperty(self.collapse, owner=False)

        self.clampPositions = ivw.properties.BoolCompositeProperty("clampPositions", "Clamp Positions", False)
        self.addProperty(self.clampPositions, owner=False)

        self.clampRange = ivw.properties.DoubleMinMaxProperty("clampRange", "Clamp Range",
            valueMin=0, valueMax=2, rangeMin=-1000, rangeMax=1000, increment=0.001,
            semantics=inviwopy.properties.PropertySemantics.Text)
        self.clampPositions.addProperty(self.clampRange, owner=False)

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
            if len(children) > 2: recursive(children[2], n, depth + 1, 1)
            if postorder is not None: postorder(n, depth, parent, index)
        recursive(node, None, 0, 0)


    @staticmethod
    def atomColor(type):
        return ivwmolvis.atomicelement.color(ivwmolvis.atomicelement.fromAbbr(type), ivwmolvis.atomicelement.Colormap.RasmolCPKnew)

    def clamp(self, value):
        if self.clampPositions.checked:
            return max(self.clampRange.start, min(value, self.clampRange.end))
        else:
            return value

    def buildTree(self, root, tree, info):

        def findColor(n):
            if self.collapse.value:
                if len(self.atoms[n]) == 1:
                    (_, type) = next(iter(self.atoms[n]))
                    color = MergeTreeMesh.atomColor(type)
                else:
                    color = self.colorTf.value.sample(criticalTypes[n] / 4.0)
            else:
                color = self.colorTf.value.sample(criticalTypes[n] / 4.0)

            #s = scalars[n]
            #if s != self.clamp(s):
            #    color[3] /= 2
            return color



        criticalTypes = info.getColumn("CriticalType")
        scalars = info.getColumn("Scalar")

        nods = set([i for sub in tree.values() for i in sub]) | set(tree.keys())
        indexMap = {n:i for i,n in enumerate(nods)}

        picking = [ self.pm.pickingId(n) for n in nods]
        colors = [ findColor(n) for n in nods]
        positions = [ [0, self.clamp(scalars[n]), 0] for n in nods]
        labelInds = [ node for node in nods]

        def findMarker(n):
            s = scalars[n]
            if s < self.clampRange.start:
                return Marker.TriangleUp.value

            if s > self.clampRange.end:
                return Marker.TriangleDown.value

            if n == root:
                return Marker.Hexagon.value

            if len(self.atoms[n]) == 1:
                if self.collapse.value:
                    return Marker.Circle.value
                else:
                    return Marker.Square.value
            else:
                return Marker.Diamond.value

        markers = [findMarker(n) for n in nods]

        pointInds = [ indexMap[node] for node in nods]


        xpos = 0
        def inorder(node, depth, parent, index):
            nonlocal xpos
            positions[indexMap[node]][0] = xpos
            xpos += 1

        MergeTreeMesh.traverse(root, tree, inorder = inorder)

        # fix for the root item, which only have 1 child.
        positions[indexMap[root]][0] = positions[indexMap[tree[root][0]]][0]

        lineInds = []
        def straitLines(node, depth, parent, index):
            nonlocal lineInds
            if parent is not None:
                lineInds.append(indexMap[node])
                lineInds.append(indexMap[parent])

        def kneeLines(node, depth, parent, index):
            nonlocal lineInds
            if parent is not None:
                np = positions[indexMap[node]]
                pp = positions[indexMap[parent]]

                kp = [np[0], pp[1], 0]
                kIndex = len(positions)

                picking.append(0)
                colors.append([0,0,0,1])
                positions.append(kp)
                labelInds.append(0)
                markers.append(0)

                lineInds.append(indexMap[parent])
                lineInds.append(kIndex)
                lineInds.append(kIndex)
                lineInds.append(indexMap[node])

        MergeTreeMesh.traverse(root, tree, preorder = kneeLines)

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
        mesh.addBuffer(ivw.data.BufferType.IntMetaAttrib, ivw.data.BufferUINT32(
            np.array(markers).astype(np.uint32)))

        lineNone = ivw.data.MeshInfo(ivw.data.DrawType.Lines, ivw.data.ConnectivityType.Unconnected)
        mesh.addIndices(lineNone, ivw.data.IndexBufferUINT32(np.array(lineInds).astype(np.uint32)))

        pointNone = ivw.data.MeshInfo(ivw.data.DrawType.Points, ivw.data.ConnectivityType.Unconnected)
        mesh.addIndices(pointNone, ivw.data.IndexBufferUINT32(np.array(pointInds).astype(np.uint32)))
        return mesh


    def findNodeMeta(self, info):
        ct = info.getColumn("CriticalType")
        types = info.getColumn("Type");
        ind = info.getColumn("assigned_atom_index");
        atomsTypes = {}
        atoms = {}
        childCount = {}


        def postorder(node, depth, parent, index):
            if not node in atomsTypes:
                atomsTypes[node] = set()
                atoms[node] = set()
            if not parent in atomsTypes:
                atomsTypes[parent] = set()
                atoms[parent] = set()

            if ct[node] == 3: # only assign atoms from max points
                atomsTypes[node].add(types[node])
                atoms[node].add((ind[node], types[node]))

            atomsTypes[parent].update(atomsTypes[node])
            atoms[parent].update(atoms[node])

            if not node in childCount:
                childCount[node] = 1
            if not parent in childCount:
                childCount[parent] = 1

            childCount[parent] += childCount[node]

        MergeTreeMesh.traverse(self.root, self.downMap, postorder = postorder)
        self.atoms = atoms
        self.atomsTypes = atomsTypes
        self.childCount = childCount


    def buildBars(self, info):

        def sizeFun(x):
            return 10*(1 - math.exp(-x/6))

        picking = []
        colors = []
        positions = []
        labelInds = []

        if len(self.filter.value) != 0:
            for i, node in enumerate(info.getColumn("NodeId")):
                if (node in self.atomsTypes and self.filter.value in self.atomsTypes[node]):
                    na = len(self.atoms[i])
                    satoms = sorted(self.atoms[i], key=lambda x: x[1])
                    for ia, a in enumerate(satoms):
                        pi = self.pm.pickingId(node)
                        picking.append([pi, pi])
                        color = ivwmolvis.atomicelement.color(ivwmolvis.atomicelement.fromAbbr(a[1]), ivwmolvis.atomicelement.Colormap.RasmolCPKnew)
                        colors.append([color, color])
                        s = info.getColumn("Scalar")[i]
                        positions.append([[sizeFun(ia), s, 0], [sizeFun(ia + 0.9), s, 0]])
                        labelInds.append([node, node])

        if len(positions) == 0:
            print(f"No critical points found matching '{self.filter.value}'")

        if len(positions) == 0:
            picking = [ [self.pm.pickingId(node),self.pm.pickingId(node)]
                        for node in info.getColumn("NodeId") ]
            colors = [ [self.colorTf.value.sample(type / 4.0), self.colorTf.value.sample(type / 4.0) ]
                        for type in info.getColumn("CriticalType") ]
            positions = [ [[-2, s, 0], [-0.5, s, 0]] for i,s in enumerate(info.getColumn("Scalar")) ]
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


    def orderTree(self, treeMap, prioAtomType):
        def sortFun(n):
            return (-1000 * sum(1 for x in self.atoms[n] if x[1] == prioAtomType)
                    - 1 * self.childCount[n]
                    + len(self.atoms[n]))

        def cmp(n1, n2):
            s1 = sum(1 for x in self.atoms[n1] if x[1] == prioAtomType)
            s2 = sum(1 for x in self.atoms[n2] if x[1] == prioAtomType)
            if s1 < s2:
                return 1
            elif s2 < s1:
                return -1
            c1 = self.childCount[n1]
            c2 = self.childCount[n2]
            if s1 > 0:
                if c1 < c2:
                    return 1
                elif c2 < c1:
                    return -1
            else:
                if c1 < c2:
                    return -1
                elif c2 < c1:
                    return 1

            return 0

        sortedTreeMap = {}
        for node in treeMap.keys():
            sortedTreeMap[node] = sorted(treeMap[node], key=cmp_to_key(cmp))

        return sortedTreeMap


    def collapseNodes(self, root, tree):
        collapsed = {}
        def postorder(node, depth, parent, index):
            nonlocal collapsed
            if not parent is None:
                if len(self.atoms[parent]) > 1:
                    if parent in collapsed:
                        collapsed[parent].append(node)
                    else:
                        collapsed[parent] = [node]
        MergeTreeMesh.traverse(root, tree, postorder = postorder)
        return collapsed


    def process(self):
        df = self.inport.getData()
        info = self.nodeInfo.getData()

        ups = df.getColumn("upNodeId")
        downs = df.getColumn("downNodeId") 

        self.nodeLabels = []

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

        root = next(iter(self.upMap))
        while root in self.upMap:
            root = self.upMap[root]
        self.root = root

        self.pm.resize(len(self.upMap) + 1)

        self.findNodeMeta(info)

        if len(self.filter.value) != 0:
            self.downMap = self.orderTree(self.downMap, self.filter.value)

        if self.collapse.value:
            tree = self.collapseNodes(self.root, self.downMap)
        else:
            tree = self.downMap

        if self.mode.value == 0:
            mesh = self.buildTree(self.root, tree, info)
            self.outport.setData(mesh)

            nods = set([i for sub in self.downMap.values() for i in sub]) | set(self.downMap.keys())
            nodeLabels = [str(list(self.atoms[node])[0][1]) if len(self.atoms[node]) == 1 else ""
                            for node in nods]
            self.labels.setData(nodeLabels)

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

                pickEvent.setToolTip(f"NodeID {i} {self.atoms[i] if i in self.atoms else '-'}")

            elif pickEvent.hoverState == ivw.PickingHoverState.Exit:
                self.cpBnlInport.highlight(ivw.data.BitSet())
                self.atomBnlInport.highlight(ivw.data.BitSet())
                # self.atomBnlInport.filter("foo", ivw.data.BitSet())

                self.text.value = ""
                pickEvent.setToolTip("")

        if (pickEvent.pressState == ivw.PickingPressState.Release
            and pickEvent.pressItem == ivw.PickingPressItem.Primary
            and abs(pickEvent.deltaPressedPosition.x) < 0.01
                and abs(pickEvent.deltaPressedPosition.y) < 0.01):

            selection = self.cpBnlInport.getSelectedIndices()
            selection.flip(pickEvent.pickedId)
            # selection = inviwopy.data.BitSet([pickEvent.pickedId])
            self.cpBnlInport.select(selection)
