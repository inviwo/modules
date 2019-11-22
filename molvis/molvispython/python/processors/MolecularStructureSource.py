# Name: MolecularStructureSource 

import inviwopy as ivw

#from inviwopy.properties import *
#from inviwopy.data import *
from inviwopy.glm import *
#from inviwopy.data.formats import *
import ivwmolvis

import molvisdata

from Bio.PDB.MMCIFParser import MMCIFParser
from Bio.PDB.PDBParser import PDBParser
from Bio.PDB import CaPPBuilder

import numpy as np


class MolecularStructureSource(ivw.Processor):
    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        
        self.addOutport(ivw.data.MeshOutport("mesh"))
        self.addOutport(ivwmolvis.MolecularStructureOutport("molecule"))

        self.filename = ivw.properties.FileProperty("filename", "CIF Filename")
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

        mesh, molstruct = self.parseCIF(self.filename.value)
        self.outports.mesh.setData(mesh)
        self.outports.molecule.setData(molstruct)

    # @staticmethod
    # def extractAtomName(atom):
    #     return molvisdata.extractAtomName(atom.get_name())

    @staticmethod
    def parseCIF(filename):
        mesh = ivw.data.Mesh()

        ext = filename.split(".")[-1]
        if ext == 'gz' or ext == 'bz2':
            ext = filename.split(".")[-2]
        print(ext)

        if (ext.startswith('pdb')):
            print('pdb...')
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
        #structureId = []
        modelId = []
        chainId = []
        residueId = []
        atomNumber = []
        atomFullName = []

        modelDict = dict()
        chainDict = dict()
        residueDict = dict()

        def addToDict(d, key):
            if key not in d:
                d[key] = len(d)
            return d[key]

        """def get_full_id(self): 
        Return the full id of the atom. 

        The full id of an atom is the tuple 
        (structure id, model id, chain id, residue id, atom name, altloc). 
        """ 
        for atom in structure.get_atoms():
            info = atom.get_full_id()
            pos.append(atom.coord)
            #structureId.append(info[0])
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
            #atomNumber.append()
            atomFullName.append(atom.get_name())

        atoms = ivwmolvis.Atoms()
        #atoms.positions = np.array(pos, dtype=np.double)
        dvec3pos = []
        for p in pos:
            dvec3pos.append(ivw.glm.dvec3(p[0], p[1], p[2]))
        atoms.positions = dvec3pos
        #atoms.structureids = structureId
        atoms.modelids = modelId
        atoms.chainids = chainId
        atoms.residueids = residueId
        #atoms.atomNumber = atomNumber
        atoms.fullnames = atomFullName

        atoms.updateAtomNumbers()

        molstruct = ivwmolvis.MolecularStructure()
        molstruct.atoms = atoms

        positions = []
        colors = []
        radii = []
        modelIndices = []

        numColors = 12

        modelIndex = -1;
        for model in structure:
            modelIndex = (modelIndex + 1) % numColors
            i = 1
            for atom in model.get_atoms():
                #name = MolecularStructureSource.extractAtomName(atom)
                name = molvisdata.extractAtomName(atom.get_name())
                color = molvisdata.color(name)
                radius = molvisdata.radius(name)
                modelIndices.append(modelIndex)

                # print("atom {}: '{}' '{}' '{}', resID {}".format(i, atom.get_name(), atom.get_id(), atom.get_full_id(), atom.get_full_id()[3][1]))

                # i = i + 1
                # if i > 10: 
                #     break

                positions.append(atom.coord)
                colors.append(color)
                radii.append(radius)
                modelIndices.append(modelIndex)

        nppos = np.array(positions)
        posBuffer = ivw.data.Buffer(nppos)
        # normalize colors to '1' and add alpha '1'
        #colornumpy = np.concatenate((np.array(colors, dtype=np.single) / 255.0, np.ones((len(colors), 1), dtype=np.single)), axis=1)
        #colorBuffer = ivw.data.Buffer(colornumpy)
        colorBuffer = ivw.data.Buffer(np.array(colors, dtype=np.single))
        radiiBuffer = ivw.data.Buffer(np.array(radii).astype(np.float32))

        modeldata = np.array(modelIndices).astype(np.float32) / numColors
        modelBuffer = ivw.data.Buffer(modeldata)
        
        mesh.addBuffer(ivw.data.BufferType.PositionAttrib, posBuffer)
        mesh.addBuffer(ivw.data.BufferType.ColorAttrib, colorBuffer)
        mesh.addBuffer(ivw.data.BufferType.RadiiAttrib, radiiBuffer)
        mesh.addBuffer(ivw.data.BufferType.ScalarMetaAttrib, modelBuffer)

        return mesh, molstruct
