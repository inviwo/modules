 #################################################################################
 #
 # Inviwo - Interactive Visualization Workshop
 #
 # Copyright (c) 2020-2021 Inviwo Foundation
 # All rights reserved.
 #
 # Redistribution and use in source and binary forms, with or without
 # modification, are permitted provided that the following conditions are met:
 #
 # 1. Redistributions of source code must retain the above copyright notice, this
 # list of conditions and the following disclaimer.
 # 2. Redistributions in binary form must reproduce the above copyright notice,
 # this list of conditions and the following disclaimer in the documentation
 # and/or other materials provided with the distribution.
 #
 # THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 # ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 # WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 # DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 # ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 # (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 # LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 # ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 # (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 # SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 #
 #################################################################################

import inviwopy as ivw
import ivwdataframe as df
import ivwmolvis

import numpy

from pathlib import Path

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
    
    pos = []
    atomType = []
    for i in range(numAtoms):
        splitted = lines[6 + i].strip().split()
        atomID = int(splitted[0])
        atomPos = numpy.array(list(map(float, splitted[2:5])))
        if not unitsInAngstrom:
            atomPos = bohrToAngstrom * atomPos
        atomPos = atomPos - origin 
        pos.append(atomPos)
        element = ivwmolvis.atomicelement.element(atomID)
        atomType.append(ivwmolvis.atomicelement.symbol(element))
    
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
    volume.offset = ivw.glm.vec3(0.0)

    return (volume, pos, atomType)

def createMeshForCube(pos, elemtype, basis, offset, pm):
    position = []
    color = []
    radius = []
    picking = []
    index = []

    pm.resize(len(elemtype))

    for i, p in enumerate(pos):
        element = ivwmolvis.atomicelement.fromAbbr(elemtype[i])
        c = numpy.array(ivwmolvis.atomicelement.color(element))
        r = ivwmolvis.atomicelement.vdwRadius(element)
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
        element = ivwmolvis.atomicelement.fromAbbr(et)
        ct.add(et)
        cx.add(p[0])
        cy.add(p[1])
        cz.add(p[2])
        r.add(ivwmolvis.atomicelement.vdwRadius(element))

    dataframe.updateIndex()

    return dataframe
