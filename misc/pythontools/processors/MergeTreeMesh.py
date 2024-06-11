# Name: MergeTreeMesh

import inviwopy as ivw


class MergeTreeMesh(ivw.Processor):
    """
    Documentation of MergeTreeMesh
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

    def process(self):
        df = self.inport.getData()

        ups = df.getColumn("upNodeId")
        downs = df.getColumn("downNodeId") 

        downMap = {}
        upMap = {}
        for (up, down) in zip(ups, downs):
            if up in downMap:
                downMap[up].append(down)
            else:
                downMap[up] = [down]

            if down in upMap:
                print("ERROR multiple parents")
            else:
                upMap[down] = up

        print("Size: ", len(downMap), len(upMap))

        n = next(iter(upMap))
        while n in upMap:
            n = upMap[n]
        print("Root ", n) 

        positions = []
        colors = []
        lineInds = []
        pointInds = []

        indexMap = {}

        width = {}
        def countLeaf(node, depth, parent, index, count):
            print(f"leaf {' ' * depth} {index}/{count} {node} -> {parent}"),
            if parent not in width:
                width[parent] = [0,0]
            width[parent][index] += 1

        def countSize(node, depth, parent, index, count):
            if parent not in width:
                width[parent] = [0,0]

            size = width[node][index] if node in width else 1
            width[parent][index] += size + 1

            print(f"node {' ' * depth} {index}/{count} {node} -> {parent} {size}, {width[parent]}"),

        MergeTreeMesh.traverse(n, downMap, lambda n,d,p,i,c : 0, countSize, countLeaf)

        print("width", width.values())

        def start(node, depth, parent, index, count):
            ppos = positions[indexMap[parent]] if parent >=0 else [0.0, 0.0, 0.0]

            offset = (index*2-1) * (width[node][index] if node in width else 0)

            positions.append([ppos[0] + offset * 10, ppos[1] + 50.0, ppos[2]])
            colors.append([1.0, 0.0, 0.01*depth, 1.0])
            pointInds.append(len(positions)-1)
            indexMap[node] = len(positions)-1
            if parent >= 0:
                lineInds.append(len(positions)-1)
                lineInds.append(indexMap[parent])

        def end(node, depth, parent, index, count):
            pass

        #MergeTreeMesh.traverse(n, downMap,
        #    lambda x,d,p,i,c : print(f"start {' ' * d} {d} {i} {c} {x} {p}"),
        #    lambda x,d,p,i,c : print(f"end   {' ' * d} {d} {i} {c} {x} {p}"),
        #    lambda x,d,p,i,c : print(f"leaf  {' ' * d} {d} {i} {c} {x} {p}")) 

        MergeTreeMesh.traverse(n, downMap, start, end, start) 

        mesh = ivw.data.Mesh()

        mesh.addBuffer(ivw.data.BufferType.PositionAttrib, ivw.data.BufferVec3FLOAT32(
            np.array(positions).astype(np.float32)))
        mesh.addBuffer(ivw.data.BufferType.ColorAttrib, ivw.data.BufferVec4FLOAT32(
            np.array(colors).astype(np.float32)))

        lineNone = ivw.data.MeshInfo(ivw.data.DrawType.Lines, ivw.data.ConnectivityType.Unconnected)
        mesh.addIndices(lineNone, ivw.data.IndexBufferUINT32(np.array(lineInds).astype(np.uint32)))

        pointNone = ivw.data.MeshInfo(ivw.data.DrawType.Points, ivw.data.ConnectivityType.Unconnected)
        mesh.addIndices(pointNone, ivw.data.IndexBufferUINT32(np.array(pointInds).astype(np.uint32)))

        self.outport.setData(mesh)



