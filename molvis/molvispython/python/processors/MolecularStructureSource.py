# Name: MolecularStructureSource 

 #################################################################################
 #
 # Inviwo - Interactive Visualization Workshop
 #
 # Copyright (c) 2020 Inviwo Foundation
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

from inviwopy.glm import *
import ivwmolvis

from Bio.PDB.MMCIFParser import MMCIFParser
from Bio.PDB.PDBParser import PDBParser
from Bio.PDB import CaPPBuilder

import numpy as np


class MolecularStructureSource(ivw.Processor):
    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        
        self.addOutport(ivwmolvis.MolecularStructureOutport("molecule"))

        self.filename = ivw.properties.FileProperty("filename", "Structure Filename", "", "molecularstructure")
        self.filename.addNameFilter(ivw.properties.FileExtension("cif", "mmCIF file"))
        self.filename.addNameFilter(ivw.properties.FileExtension("pdb", "PDB file"))
        self.addProperty(self.filename, owner=False)

        self.dataset = ivw.properties.StringProperty("dataset", "Data")
        self.addProperty(self.dataset, owner=False)
        self.dataset.readOnly = True

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier = "org.inviwo.MolecularStructureSource", 
            displayName = "Molecular Structure Source",
            category = "MolVis",
            codeState = ivw.CodeState.Stable,
            tags = ivw.Tags.PY
        )

    def getProcessorInfo(self):
        return MolecularStructureSource.processorInfo()

    def initializeResources(self):
        pass

    def process(self):
        if not self.filename.value or not os.path.isfile(self.filename.value):
            return

        self.dataset.value = self.filename.value.split("/")[-1]

        self.outports.molecule.setData(self.parseCIF(self.filename.value))

    @staticmethod
    def parseCIF(filename):
        mesh = ivw.data.Mesh()

        ext = filename.split(".")[-1]
        if ext == 'gz' or ext == 'bz2':
            ext = filename.split(".")[-2]

        if (ext.startswith('pdb')):
            parser = PDBParser(PERMISSIVE=1)
        elif (ext.startswith('cif')):
            # initialize mmcif parser
            parser = MMCIFParser()
        else:
            ivw.LogError("unsupported extension '{}'".format(ext))
            return mesh

        structureName = filename.split("/")[-1]
        structure = parser.get_structure(structureName, filename)

        pos = []
        bfactors = []
        modelId = []
        chainId = []
        residueId = []
        atomFullName = []

        modelDict = dict()
        chainDict = dict()
        residueDict = dict()

        def addToDict(d, key):
            if key not in d:
                d[key] = len(d)
            return d[key]

        for atom in structure.get_atoms():
            info = atom.get_full_id()  # returns tuple (structure id, model id, chain id, residue id, atom name, altloc)
            pos.append(atom.coord)
            bfactors.append(atom.get_bfactor())
            if info[1] not in modelDict:
                modelDict[info[1]] = len(modelDict)
            modelId.append(modelDict[info[1]])

            if (info[2] not in chainDict):
                chainDict[info[2]] = len(chainDict)
            chainId.append(chainDict[info[2]])

            resId = info[3][1]
            if info[3][0] not in residueDict:
                residueDict[info[3][0]] = info[3][1]
            residueId.append(resId)
            atomFullName.append(atom.get_name())

        atoms = ivwmolvis.Atoms()
        dvec3pos = []
        for p in pos:
            dvec3pos.append(ivw.glm.dvec3(p[0], p[1], p[2]))
        atoms.positions = dvec3pos
        atoms.bfactors = bfactors
        atoms.modelids = modelId
        atoms.chainids = chainId
        atoms.residueids = residueId
        atoms.fullnames = atomFullName

        atoms.atomicnumbers = ivwmolvis.util.getAtomicNumbers(atoms)

        ## residues
        residues = []
        for res in structure.get_residues():
            info = res.get_id()
            residues.append(ivwmolvis.Residue(id=info[1], name=res.get_resname(), 
                fullname=info[0], chainid=chainDict[res.get_parent().get_full_id()[2]]))
        ## chains
        chains = []
        for chain in structure.get_chains():
            chains.append(ivwmolvis.Chain(id=chainDict[chain.get_id()], name=chain.get_id()))

        bonds = ivwmolvis.util.computeCovalentBonds(atoms)

        molstruct = ivwmolvis.MolecularStructure(ivwmolvis.MolecularData(source=structureName, atoms=atoms, 
                                                                         residues=residues, chains=chains, 
                                                                         bonds=bonds))
        return molstruct
