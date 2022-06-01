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

import itertools
import numpy

from pathlib import Path

def parseCubeFile(file, flipSign=False, centerData=True):
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
    # If sizeX<0 the input cube coordinates are assumed to be in Bohr, otherwise, they are interpreted as Angstroms.
    # see http://gaussian.com/cubegen/
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

    offset = -0.5 * (basis[0] + basis[1] + basis[2]) if centerData else 0
    
    pos = []
    atoms = []
    for i in range(numAtoms):
        splitted = lines[6 + i].strip().split()
        atomID = int(splitted[0])
        atomPos = numpy.array(list(map(float, splitted[2:5])))
        if not unitsInAngstrom:
            atomPos = bohrToAngstrom * atomPos
        atomPos = atomPos - origin 
        pos.append(atomPos)
        atoms.append(ivwmolvis.atomicelement.element(atomID))

    sign = -1.0 if flipSign else 1.0

    chg = []
    for i in range(6 + numAtoms + extraLines, len(lines)):
        splitted = lines[i].strip().split()
        for val in splitted:
            chg.append(sign * float(val))
    
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

    volume.axes[0].name = "x"
    volume.axes[0].unit = ivw.data.Unit("Angstrom")
    volume.axes[1].name = "y"
    volume.axes[1].unit = ivw.data.Unit("Angstrom")
    volume.axes[2].name = "z"
    volume.axes[2].unit = ivw.data.Unit("Angstrom" )
    volume.dataMap.valueAxis.name = "Charge Density"
    volume.dataMap.valueAxis.unit = ivw.data.Unit("e/Angstrom^3")

    volume.basis = ivw.glm.mat3(basis)
    volume.offset = ivw.glm.vec3(offset)

    return (volume, pos, atoms)

def createMeshForCube(pos, elements, basis, offset, pm, margin, radiusScaling = 1.0):
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

def createMolecularStructure(pos, elements, margin, offset):
    positions = []
    indices = []
    atomicnumbers = []
    for i, p in enumerate(pos):
        element = elements[i]

        def addAtom(atompos):
            positions.append(ivw.glm.dvec3(atompos) + offset)
            indices.append(i)
            atomicnumbers.append(element)

        if margin > 0.0:
            for shift in itertools.product([-1, 0, 1], repeat=3):
                if all([x+s > -margin and x+s < 1 + margin for s, x in zip(shift, p)]):
                    addAtom([x+s for s, x in zip(shift, p)])
        else:
            addAtom(p)
    atoms = ivwmolvis.Atoms()
    atoms.positions = positions
    atoms.serialnumbers = indices
    atoms.atomicnumbers = atomicnumbers
    bonds = ivwmolvis.util.computeCovalentBonds(atoms)
    return ivwmolvis.MolecularStructure(ivwmolvis.MolecularData(source="chgcar_file", atoms=atoms, 
                                                                residues=[], chains=[], 
                                                                bonds=bonds))
def createDataFrameForCube(pos, elements):
    dataframe = df.DataFrame()
    ct = dataframe.addCategoricalColumn("type")
    cx = dataframe.addFloatColumn("x", 0, ivw.data.Unit("Angstrom"))
    cy = dataframe.addFloatColumn("y", 0, ivw.data.Unit("Angstrom"))
    cz = dataframe.addFloatColumn("z", 0, ivw.data.Unit("Angstrom"))
    r = dataframe.addFloatColumn("r", 0, ivw.data.Unit("Angstrom"))

    for elem, p in zip(elements, pos):        
        ct.add(ivwmolvis.atomicelement.symbol(elem))
        cx.add(p[0])
        cy.add(p[1])
        cz.add(p[2])
        r.add(ivwmolvis.atomicelement.vdwRadius(elem))

    dataframe.updateIndex()

    return dataframe
