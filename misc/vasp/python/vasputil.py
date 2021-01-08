import inviwopy as ivw
import ivwdataframe as df
import atomdata

import functools
import itertools
import math
import numpy

from pathlib import Path

def parseFile(file):
    file = Path(file)
    if file.suffix == ".xz":
        import lzma
        with lzma.open(file, mode='rt') as f:
            lines = f.readlines()
    elif file.suffix == ".bzip2" or file.suffix == ".bz2":
        import bz2
        with bz2.open(file, mode='rt') as f:
            lines = f.readlines()
    elif file.suffix == ".gz":
        import gzip
        with gzip.open(file, mode='rt') as f:
            lines = f.readlines()
    else:
        with open(file, mode='r') as f:
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

    factor = scale if scale >= 0.0 else numpy.power(
        -scale / numpy.dot(a3, numpy.cross(a2, a1)), 1/3)
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

    return (volume, pos, elem, nelem, elemtype)

def parseCubeFile(file):
    file = Path(file)
    if file.suffix == ".xz":
        import lzma
        with lzma.open(file, mode='rt') as f:
            lines = f.readlines()
    elif file.suffix == ".bzip2" or file.suffix == ".bz2":
        import bz2
        with bz2.open(file, mode='rt') as f:
            lines = f.readlines()
    elif file.suffix == ".gz":
        import gzip
        with gzip.open(file, mode='rt') as f:
            lines = f.readlines()
    else:
        with open(file, mode='r') as f:
            lines = f.readlines()

    nVal = 1
    extraLines = 0
    splitted = lines[2].strip().split()
    numAtoms = int(splitted[0])
    origin = numpy.array(list(map(float, splitted[1:4])))
    if numAtoms < 0:
        numAtoms = -numAtoms
        extraLines = 1
    if len(splitted) > 4:
        nVal = int(splitted[4])
    
    unitsInAngstrom = False
    bohrToAngstrom = 0.529177249
    splitted = lines[3].strip().split()
    sizeX = int(splitted[0])
    if sizeX < 0:
        sizeX = -sizeX
        unitsInAngstrom = True
    a1 = sizeX * numpy.array(list(map(float, splitted[1:4])))

    splitted = lines[4].strip().split()
    sizeY = abs(int(splitted[0]))
    a2 = sizeY * numpy.array(list(map(float, splitted[1:4])))

    splitted = lines[5].strip().split()
    sizeZ = abs(int(splitted[0]))
    a3 = sizeZ * numpy.array(list(map(float, splitted[1:4])))

    basis = numpy.array([a1, a2, a3]).astype(numpy.float32)
    if not unitsInAngstrom:
        basis = bohrToAngstrom * basis
        origin = bohrToAngstrom * origin
    offset = -0.5 * (basis[0] + basis[1] + basis[2])
    
    pos = []
    atomNo = []
    for i in range(numAtoms):
        splitted = lines[6 + i].strip().split()
        atomID = int(splitted[0])
        atomPos = numpy.array(list(map(float, splitted[2:5])))
        if not unitsInAngstrom:
            atomPos = bohrToAngstrom * atomPos
        atomPos = atomPos - origin 
        pos.append(atomPos)
        atomNo.append(atomdata.atomicSymbol(atomID))
    
    chg = []
    for i in range(6 + numAtoms + extraLines, len(lines)):
        splitted = lines[i].strip().split()
        for val in splitted:
            chg.append(float(val))
    
    chgdata = numpy.array(chg).astype(numpy.float32)
    chosenIdx = 0
    chgdata = chgdata[chosenIdx::nVal]
    size = (sizeX, sizeY, sizeZ)
    chgdata = chgdata.reshape(size)
    chgdata = chgdata.flatten('F')
    chgdata = chgdata.reshape(size)

    volume = ivw.data.Volume(chgdata)
    volume.data = chgdata
    volume.dataMap.dataRange = ivw.glm.dvec2(chgdata.min(), chgdata.max())
    volume.dataMap.valueRange = volume.dataMap.dataRange

    volume.basis = ivw.glm.mat3(basis)
    volume.offset = ivw.glm.vec3(offset)

    return (volume, pos, atomNo)

def createMesh(pos, elemtype, basis, offset, pm, margin):
    position = []
    color = []
    radius = []
    picking = []
    index = []

    pm.resize(len(elemtype))

    for i, p in enumerate(pos):
        c = atomdata.color(elemtype[i])
        r = atomdata.radius(elemtype[i])
        pi = pm.pickingId(i)

        def addVertex(vertexpos):
            position.append(vertexpos)
            color.append(c)
            radius.append(r)
            picking.append(pi)
            index.append(i)

        if margin > 0.0:
            for shift in itertools.product([-1, 0, 1], repeat=3):
                if all([x+s > -margin and x+s < 1 + margin for s, x in zip(shift, p)]):
                    addVertex([x+s for s, x in zip(shift, p)])
        else:
            addVertex(p)

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
    mesh.addBuffer(ivw.data.BufferType.IndexAttrib, ivw.data.Buffer(
        numpy.array(index).astype(numpy.uint32)))
    return mesh

def createMeshForCube(pos, elemtype, basis, offset, pm):
    position = []
    color = []
    radius = []
    picking = []
    index = []

    pm.resize(len(elemtype))

    for i, p in enumerate(pos):
        c = atomdata.color(elemtype[i])
        r = atomdata.radius(elemtype[i])
        pi = pm.pickingId(i)

        def addVertex(vertexpos):
            position.append(vertexpos)
            color.append(c)
            radius.append(r)
            picking.append(pi)
            index.append(i)

        addVertex(p)

    mesh = ivw.data.Mesh()
    #mesh.basis = ivw.glm.mat3(basis)
    mesh.offset = ivw.glm.vec3(offset)

    mesh.addBuffer(ivw.data.BufferType.PositionAttrib, ivw.data.Buffer(
        numpy.array(position).astype(numpy.float32)))
    mesh.addBuffer(ivw.data.BufferType.ColorAttrib, ivw.data.Buffer(
        numpy.array(color).astype(numpy.float32)))
    mesh.addBuffer(ivw.data.BufferType.RadiiAttrib, ivw.data.Buffer(
        numpy.array(radius).astype(numpy.float32)))
    mesh.addBuffer(ivw.data.BufferType.PickingAttrib, ivw.data.Buffer(
        numpy.array(picking).astype(numpy.uint32)))
    mesh.addBuffer(ivw.data.BufferType.IndexAttrib, ivw.data.Buffer(
        numpy.array(index).astype(numpy.uint32)))
    return mesh


def createDataFrame(pos, elemtype, modelMat):
    dataframe = df.DataFrame()
    ct = dataframe.addCategoricalColumn("type")
    cx = dataframe.addFloatColumn("x")
    cy = dataframe.addFloatColumn("y")
    cz = dataframe.addFloatColumn("z")

    for et, p in zip(elemtype, pos):
        mp = modelMat * ivw.glm.vec4(p[0], p[1], p[2], 1.0)

        ct.add(et)
        cx.add(mp[0])
        cy.add(mp[1])
        cz.add(mp[2])

    dataframe.updateIndex()

    return dataframe
    
def createDataFrameForCube(pos, elemtype):
    dataframe = df.DataFrame()
    ct = dataframe.addCategoricalColumn("type")
    cx = dataframe.addFloatColumn("x")
    cy = dataframe.addFloatColumn("y")
    cz = dataframe.addFloatColumn("z")
    r = dataframe.addFloatColumn("r")

    for et, p in zip(elemtype, pos):
        ct.add(et)
        cx.add(p[0])
        cy.add(p[1])
        cz.add(p[2])
        r.add(atomdata.radius(et))

    dataframe.updateIndex()

    return dataframe
