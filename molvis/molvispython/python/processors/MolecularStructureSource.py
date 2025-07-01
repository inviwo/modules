# Name: MolecularStructureSource

#################################################################################
#
# Inviwo - Interactive Visualization Workshop
#
# Copyright (c) 2020-2025 Inviwo Foundation
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

import builtins
import os
import inviwopy as ivw

import ivwmolvis

from Bio.PDB.MMCIFParser import MMCIFParser
from Bio.PDB.PDBParser import PDBParser

import xpdb


class MolecularStructureSource(ivw.Processor):
    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)

        self.addOutport(ivwmolvis.MolecularStructureOutport("molecule"))

        self.filename = ivw.properties.FileProperty(
            "filename", "Structure Filename", "", "molecularstructure")
        self.filename.addNameFilter(ivw.properties.FileExtension("cif", "mmCIF file"))
        self.filename.addNameFilter(ivw.properties.FileExtension("pdb", "PDB file"))
        self.addProperty(self.filename, owner=False)

        self.dataset = ivw.properties.StringProperty("dataset", "Data")
        self.addProperty(self.dataset, owner=False)
        self.dataset.readOnly = True
        self.dataset.invalidationLevel = ivw.properties.InvalidationLevel.Valid

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.MolecularStructureSourcePython",
            displayName="MolecularStructure Source Python",
            category="Data Input",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags("PY, MolVis, Source, PDB, mmCIF")
        )

    def getProcessorInfo(self):
        return MolecularStructureSource.processorInfo()

    def initializeResources(self):
        pass

    def process(self):
        if not self.filename.value or not os.path.isfile(self.filename.value):
            return

        self.dataset.value = self.filename.value.name

        self.outports.molecule.setData(self.parseFile(self.filename.value))

    @staticmethod
    def parseFile(filename):
        ext = filename.suffix
        if ext == '.gz' or ext == '.bz2':
            ext = filename.suffixes[0]

        if (ext.startswith('.pdb')):
            parser = PDBParser(PERMISSIVE=1, structure_builder=xpdb.SloppyStructureBuilder())
        elif (ext.startswith('.cif')):
            parser = MMCIFParser()
        else:
            raise Exception("unsupported extension '{}'".format(ext))

        structureName = filename.name
        structure = parser.get_structure(structureName, filename)

        pos = []
        serialNumbers: ivw.intVector = ivw.intVector()
        bfactors: ivw.doubleVector = ivw.doubleVector()
        modelId: ivw.intVector = ivw.intVector()
        chainId: ivw.intVector = ivw.intVector()
        residueId: ivw.intVector = ivw.intVector()
        atomFullName: ivw.StringVector = ivw.StringVector()
        elements = []

        modelDict = dict()
        chainDict = dict()
        residueDict = dict()

        for atom in structure.get_atoms():
            structureid, modelid, chainid, resid, _ = atom.get_full_id()
            pos.append(atom.coord)
            serialNumbers.append(atom.get_serial_number())
            bfactors.append(atom.get_bfactor())
            if modelid not in modelDict:
                modelDict[modelid] = len(modelDict)
            modelId.append(modelDict[modelid])

            if (chainid not in chainDict):
                chainDict[chainid] = len(chainDict)
            chainId.append(chainDict[chainid])

            resId = resid[1]
            if resid[0] not in residueDict:
                residueDict[resid[0]] = resid[1]
            residueId.append(resId)
            atomFullName.append(atom.get_name())
            elements.append(ivwmolvis.atomicelement.fromAbbr(atom.element))

        atoms = ivwmolvis.Atoms()
        dvec3pos: ivw.dvec3Vector = ivw.dvec3Vector()
        for p in pos:
            dvec3pos.append(ivw.glm.dvec3(p[0], p[1], p[2]))
        atoms.positions = dvec3pos
        # check serial numbers, might be None for some CIF structures
        atoms.serialnumbers = serialNumbers if builtins.all(
            serialNumbers) else [x for x in range(len(pos))]
        atoms.bfactors = bfactors
        atoms.modelids = modelId
        atoms.chainids = chainId
        atoms.residueids = residueId
        atoms.fullnames = atomFullName
        atoms.atomicnumbers = elements

        # residues
        residues = []
        for res in structure.get_residues():
            info = res.get_id()
            residues.append(ivwmolvis.Residue(id=info[1], name=res.get_resname(),
                                              fullname=info[0],
                                              chainid=chainDict[res.get_parent().get_full_id()[2]]))
        # chains
        chains = []
        for chain in structure.get_chains():
            chains.append(ivwmolvis.Chain(id=chainDict[chain.get_id()], name=chain.get_id()))

        bonds = ivwmolvis.util.computeCovalentBonds(atoms)

        molstruct = ivwmolvis.MolecularStructure(ivwmolvis.MolecularData(source=structureName,
                                                                         atoms=atoms,
                                                                         residues=residues,
                                                                         chains=chains,
                                                                         bonds=bonds))
        return molstruct
