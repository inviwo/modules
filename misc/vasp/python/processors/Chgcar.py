# Name: Chgcar

import inviwopy as ivw
import atomdata

import functools
import math
import numpy


def parseFile(file):
    with open(file) as f:
        lines = f.readlines()

    scale = float(lines[1].strip())
    a1 = numpy.array(list(map(float, lines[2].strip().split())))
    a2 = numpy.array(list(map(float, lines[3].strip().split())))
    a3 = numpy.array(list(map(float, lines[4].strip().split())))

    elem = lines[5].strip().split()
    nelem = list(map(int, lines[6].strip().split()))
    direct = not {lines[7].strip()[0]}.issubset({"K", "k", "C", "c"})
    ntot = functools.reduce(lambda x, y: x + y, nelem)
    pos = []
    for line in lines[8:8+ntot]:
        pos.append(list(map(float, line.strip().split())))
    pos = numpy.array(pos).astype(numpy.float32)

    factor = scale if scale >= 0.0 else np.power(
        -scale / np.dot(a3, np.cross(a2, a1)), 1/3)
    basis = numpy.array([a1, a2, a3]) * factor
    offset = -0.5 * (basis[0] + basis[1] + basis[2])
    if not direct:
        inv = numpy.linalg.inv(basis)
        pos = numpy.array([numpy.dot(inv, p) for p in pos])

    dims = list(map(int, lines[9+ntot].split()))
    voxels = functools.reduce(lambda x, y: x * y, dims)
    perrow = len(lines[10+ntot].strip().split())
    rows = math.ceil(voxels/perrow)

    chg = []
    for line in lines[10+ntot:10+ntot+rows]:
        for c in line.strip().split():
            chg.append(float(c))

    chgdata = numpy.array(chg).astype(numpy.float32)
    chgdata.shape = [dims[0], dims[1], dims[2]]

    volume = ivw.data.Volume(chgdata)
    volume.data = chgdata
    volume.dataMap.dataRange = ivw.glm.dvec2(chgdata.min(), chgdata.max())
    volume.dataMap.valueRange = volume.dataMap.dataRange

    volume.basis = ivw.glm.mat3(basis)
    volume.offset = ivw.glm.vec3(offset)

    elemtype = []
    for i, n in enumerate(nelem):
        for x in range(n):
            elemtype.append(elem[i])

    print("Loaded CHGCAR: {}\nDims:  {}\nElem:  {}\nNElem  {}\nRange: {}".format(
        file, dims, elem, nelem, volume.dataMap.dataRange))

    return (volume, pos, elem, nelem, elemtype)


def createMesh(pos, elemtype, basis, offset, pm):
    position = []
    color = []
    radius = []
    picking = []

    pm.resize(len(elemtype))

    for i, p in enumerate(pos):
        c = atomdata.color(elemtype[i])
        r = atomdata.radius(elemtype[i])
        pi = pm.pickingId(i)

        def addVertex(offset):
            position.append(
                [p[0] + offset[0], p[1] + offset[1], p[2] + offset[2]])
            color.append(c)
            radius.append(r)
            picking.append(pi)

        addVertex([0, 0, 0])

        margin = 0.2
        boundary = [(1 if x < margin else (-1 if x > 1 - margin else 0))
                    for x in p]
        for d in range(3):
            if boundary[d] != 0:
                shift = [0, 0, 0]
                shift[d] = boundary[d]
                addVertex(shift)
        for d1 in range(3):
            for d2 in range(d1 + 1, 3):
                if boundary[d1] != 0 and boundary[d2] != 0:
                    shift = [0, 0, 0]
                    shift[d1] = boundary[d1]
                    shift[d2] = boundary[d2]
                    addVertex(shift)
        if boundary[0] != 0 and boundary[1] != 0 and boundary[2] != 0:
            addVertex(boundary)

    mesh = ivw.data.Mesh()
    mesh.basis = ivw.glm.mat3(basis)
    mesh.offset = ivw.glm.vec3(offset)

    mesh.addBuffer(ivw.data.BufferType.PositionAttrib, ivw.data.Buffer(
        numpy.array(position).astype(numpy.float32)))
    mesh.addBuffer(ivw.data.BufferType.ColorAttrib, ivw.data.Buffer(
        numpy.array(color).astype(numpy.float32)))
    mesh.addBuffer(ivw.data.BufferType.RadiiAttrib, ivw.data.Buffer(
        numpy.array(radius).astype(numpy.float32)))
    mesh.addBuffer(ivw.data.BufferType.PickingAttrib, ivw.data.Buffer(
        numpy.array(picking).astype(numpy.uint32)))
    return mesh


class Chgcar(ivw.Processor):
    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)

        self.volumeOutport = ivw.data.VolumeOutport("chargedensity")
        self.addOutport(self.volumeOutport, owner=False)

        self.meshOutport = ivw.data.MeshOutport("atoms")
        self.addOutport(self.meshOutport)

        self.chgcar = ivw.properties.FileProperty("chgcar", "CHGCAR")
        self.addProperty(self.chgcar)

        self.dataRange = ivw.properties.DoubleMinMaxProperty(
            "dataRange", "Data Range", 0.0, 1.0, -1.70e308, 1.79e308)
        self.addProperty(self.dataRange)
        self.dataRange.semantics = ivw.properties.PropertySemantics("Text")
        self.dataRange.readOnly = True

        self.useCustomRange = ivw.properties.BoolProperty(
            "useCustomRange", "Overwrite Data Range", False)
        self.addProperty(self.useCustomRange)

        self.customDataRange = ivw.properties.DoubleMinMaxProperty(
            "customDataRange", "Custom Data Range", 0.0, 1.0, -1.70e308, 1.79e308)
        self.addProperty(self.customDataRange)
        self.properties.customDataRange.semantics = ivw.properties.PropertySemantics(
            "Text")

        self.dataset = ivw.properties.StringProperty("dataset", "Data")
        self.addProperty(self.dataset)
        self.properties.dataset.readOnly = True

        self.pm = inviwopy.PickingMapper(self, 1, lambda x: self.callback(x))

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.vasp.Chgcar",
            displayName="Chgcar Source",
            category="Source",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags([ivw.Tag.PY, ivw.Tag("VASP"), ivw.Tag("Volume")])
        )

    def getProcessorInfo(self):
        return Chgcar.processorInfo()

    def initializeResources(self):
        pass

    def process(self):
        self.volume, self.atomPos, self.elem, self.nelem, self.elemtype = parseFile(
            self.chgcar.value)
        self.volumeDataRange = self.volume.dataMap.dataRange

        self.volume.dataMap.dataRange = self.customDataRange.value if self.useCustomRange.value else self.volumeDataRange
        self.volume.dataMap.valueRange = self.customDataRange.value if self.useCustomRange.value else self.volumeDataRange

        self.mesh = createMesh(self.atomPos, self.elemtype,
                               self.volume.basis, self.volume.offset, self.pm)

        self.volumeOutport.setData(self.volume)
        self.meshOutport.setData(self.mesh)

    def callback(self, pickevent):
        if (pickevent.state == inviwopy.PickingState.Updated):
            i = pickevent.pickedId
            pos = numpy.dot(numpy.array(self.volume.basis), self.atomPos[i])
            pickevent.setToolTip("Atom id: {}\nPosition {}".format(i, pos))
            #
            #pickevent.setToolTip("Atom id: {}\nElement: {}\nPosition {}\n Fractional {}".format(
            #    i, self.elemtype[i], pos, self.atomPos[i]))

