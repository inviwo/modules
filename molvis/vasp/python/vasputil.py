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

import functools
import itertools
import math
import numpy

from pathlib import Path

def parseFile(file, flipSign=False, centerData=True):
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

    offset = -0.5 * (basis[0] + basis[1] + basis[2]) if centerData else 0

    if not direct:
        inv = numpy.linalg.inv(basis)
        pos = numpy.array([numpy.dot(inv, p) for p in pos])

    dims = list(map(int, lines[9+ntot].split()))
    voxels = functools.reduce(lambda x, y: x * y, dims)
    perrow = len(lines[10+ntot].strip().split())
    rows = math.ceil(voxels/perrow)

    sign = -1.0 if flipSign else 1.0

    chg = []
    for line in lines[10+ntot:10+ntot+rows]:
        for c in line.strip().split():
            chg.append(sign * float(c))

    chgdata = numpy.array(chg).astype(numpy.float32)
    chgdata.shape = [dims[0], dims[1], dims[2]]

    volume = ivw.data.Volume(chgdata)
    volume.data = chgdata
    volume.dataMap.dataRange = ivw.glm.dvec2(chgdata.min(), chgdata.max())
    volume.dataMap.valueRange = volume.dataMap.dataRange

    volume.axes[0].name = "x"
    volume.axes[0].unit = ivw.data.Unit("Angstrom")
    volume.axes[1].name = "y"
    volume.axes[1].unit = ivw.data.Unit("Angstrom")
    volume.axes[2].name = "z"
    volume.axes[2].unit = ivw.data.Unit("Angstrom" )
    volume.dataMap.valueAxis.name = "Charge Density"
    volume.dataMap.valueAxis.unit = ivw.data.Unit("eV")

    volume.basis = ivw.glm.mat3(basis)
    volume.offset = ivw.glm.vec3(offset)

    atoms = []
    for i, n in enumerate(nelem):
        element = ivwmolvis.atomicelement.fromAbbr(elem[i])
        for x in range(n):
            atoms.append(element)

    return (volume, pos, elem, nelem, atoms)

def createMesh(pos, elements, basis, offset, pm, margin, radiusScaling = 1.0):
    position = []
    color = []
    radius = []
    picking = []
    index = []

    pm.resize(len(elements))

    for i, p in enumerate(pos):
        c = numpy.array(ivwmolvis.atomicelement.color(elements[i]))
        r = ivwmolvis.atomicelement.vdwRadius(elements[i]) * radiusScaling
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

def createDataFrame(pos, elements, modelMat):
    dataframe = df.DataFrame()
    ct = dataframe.addCategoricalColumn("type")
    cx = dataframe.addFloatColumn("x", 0, ivw.data.Unit("Angstrom"))
    cy = dataframe.addFloatColumn("y", 0, ivw.data.Unit("Angstrom"))
    cz = dataframe.addFloatColumn("z", 0, ivw.data.Unit("Angstrom"))
    r = dataframe.addFloatColumn("r", 0, ivw.data.Unit("Angstrom"))

    for elem, p in zip(elements, pos):
        mp = modelMat * ivw.glm.vec4(p[0], p[1], p[2], 1.0)

        ct.add(ivwmolvis.atomicelement.symbol(elem))
        cx.add(mp[0])
        cy.add(mp[1])
        cz.add(mp[2])
        r.add(ivwmolvis.atomicelement.vdwRadius(elem))

    dataframe.updateIndex()

    return dataframe
    
