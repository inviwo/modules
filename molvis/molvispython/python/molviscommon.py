 #################################################################################
 #
 # Inviwo - Interactive Visualization Workshop
 #
 # Copyright (c) 2022 Inviwo Foundation
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
from typing import List
from collections.abc import Callable

def _adjustMargins(pos, margin : float, func : Callable[[List[float]], None]):
    if margin > 0.0:
        for shift in itertools.product([-1, 0, 1], repeat=3):
            if all([x+s > -margin and x+s < 1 + margin for s, x in zip(shift, pos)]):
                func([x+s for s, x in zip(shift, pos)])
    else:
        func(pos)

def createMesh(pos : List[numpy.array], 
               elements : List[ivwmolvis.atomicelement.element],
               basis : ivw.glm.mat3=None, offset : ivw.glm.vec3=ivw.glm.vec3(0, 0, 0),
               pm : ivw.PickingMapper=None,
               margin : float=0.0,
               radiusscaling : float=1.0):
    """
    Create a sphere Mesh from a list of 3D positions.

    :param pos:        list of 3D positions
    :param elements:   atomic elements matching each position for determining the sphere radii
    :param basis:      basis of the created mesh
    :param offset:     offset of the created mesh
    :param pm:         pickingmapper used for creating picking IDs, if present
    :param margin:     if larger than 0, positions within margin distance to a boundingbox surface
                       of a [0,0,0]-[1,1,1] cube will be duplicated
    :param radiusscaling:  scaling factor for sphere radii
    :return: inviwopy.data.Mesh
    """
    position = []
    color = []
    radius = []
    picking = []
    index = []

    if pm:
        pm.resize(len(elements))

    for i, p in enumerate(pos):
        c = numpy.array(ivwmolvis.atomicelement.color(elements[i]))
        r = ivwmolvis.atomicelement.vdwRadius(elements[i]) * radiusscaling
        pi = pm.pickingId(i) if pm else None

        def addVertex(vertexpos):
            position.append(vertexpos)
            color.append(c)
            radius.append(r)
            if pi:
                picking.append(pi)
            index.append(i)

        _adjustMargins(p, margin, addVertex)

    mesh = ivw.data.Mesh()
    if basis:
        mesh.basis = ivw.glm.mat3(basis)
    mesh.offset = ivw.glm.vec3(offset)

    mesh.addBuffer(ivw.data.BufferType.PositionAttrib, ivw.data.Buffer(
        numpy.array(position).astype(numpy.float32)))
    mesh.addBuffer(ivw.data.BufferType.ColorAttrib, ivw.data.Buffer(
        numpy.array(color).astype(numpy.float32)))
    mesh.addBuffer(ivw.data.BufferType.RadiiAttrib, ivw.data.Buffer(
        numpy.array(radius).astype(numpy.float32)))
    if pm:
        mesh.addBuffer(ivw.data.BufferType.PickingAttrib, ivw.data.Buffer(
            numpy.array(picking).astype(numpy.uint32)))
    mesh.addBuffer(ivw.data.BufferType.IndexAttrib, ivw.data.Buffer(
        numpy.array(index).astype(numpy.uint32)))
    return mesh

def createMolecularStructure(pos : List[numpy.array], 
                             elements : List[ivwmolvis.atomicelement.element],
                             margin : float=0.0, offset : ivw.glm.vec3=ivw.glm.vec3(0, 0, 0)):
    """
    Create a molecular representation from a list of 3D positions.

    :param pos:        list of 3D positions
    :param elements:   atomic elements matching each position
    :param margin:     if larger than 0, positions within margin distance to a boundingbox surface
                       of a [0,0,0]-[1,1,1] cube will be duplicated
    :param offset:     offset of the created mesh
    :return: ivwmolvis.MolecularStructure
    """

    positions = []
    indices = []
    atomicnumbers = []
    for i, p in enumerate(pos):
        element = elements[i]

        def addAtom(atompos):
            positions.append(ivw.glm.dvec3(atompos) + offset)
            indices.append(i)
            atomicnumbers.append(element)

        _adjustMargins(p, margin, addAtom)

    atoms = ivwmolvis.Atoms()
    atoms.positions = positions
    atoms.serialnumbers = indices
    atoms.atomicnumbers = atomicnumbers
    bonds = ivwmolvis.util.computeCovalentBonds(atoms)
    return ivwmolvis.MolecularStructure(ivwmolvis.MolecularData(source="chgcar_file", atoms=atoms,
                                                                residues=[], chains=[],
                                                                bonds=bonds))

def createDataFrame(pos : List[numpy.array], 
                    elements : List[ivwmolvis.atomicelement.element],
                    modelmat : ivw.glm.mat3=ivw.glm.mat3(1.0)):
    """
    Create a DataFrame from a list of 3D positions and elements.

    :param pos:        list of 3D positions
    :param elements:   atomic elements matching each position
    :param modelmat:   transformation applied to each position
    :return: ivwdataframe.DataFrame
    """
    dataframe = df.DataFrame()
    ct = dataframe.addCategoricalColumn("type")
    cx = dataframe.addFloatColumn("x", 0, ivw.data.Unit("Angstrom"))
    cy = dataframe.addFloatColumn("y", 0, ivw.data.Unit("Angstrom"))
    cz = dataframe.addFloatColumn("z", 0, ivw.data.Unit("Angstrom"))
    r = dataframe.addFloatColumn("r", 0, ivw.data.Unit("Angstrom"))

    for elem, p in zip(elements, pos):
        mp = modelmat * ivw.glm.vec4(p[0], p[1], p[2], 1.0)

        ct.add(ivwmolvis.atomicelement.symbol(elem))
        cx.add(mp[0])
        cy.add(mp[1])
        cz.add(mp[2])
        r.add(ivwmolvis.atomicelement.vdwRadius(elem))

    dataframe.updateIndex()

    return dataframe
