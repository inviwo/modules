# Name: CubeSourceMolvis 

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
import gaussianutil

from pathlib import Path

class CubeSourceMolvis(ivw.Processor):
    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)

        self.volumeOutport = ivw.data.VolumeOutport("chargedensity")
        self.addOutport(self.volumeOutport, owner=False)

        self.meshOutport = ivw.data.MeshOutport("atoms")
        self.addOutport(self.meshOutport)

        self.dataframeOutport = df.DataFrameOutport("atomInformation")
        self.addOutport(self.dataframeOutport)

        self.addOutport(ivwmolvis.MolecularStructureOutport("molecule"))

        self.cubeFilePath = ivw.properties.FileProperty("cube", "cube")
        self.addProperty(self.cubeFilePath)

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

        self.flipSign = ivw.properties.BoolProperty(
            "flipSign", "Flip Sign of Charge", False)
        self.addProperty(self.flipSign)

        self.centerData = ivw.properties.BoolProperty(
            "centerData", "Center Data", True)
        self.addProperty(self.centerData)

        self.pm = ivw.PickingMapper(self, 1, lambda x: self.callback(x))

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
    		classIdentifier = "org.inviwo.CubeSourceMolvis", 
    		displayName = "Cube Source Molvis",
    		category = "Python",
    		codeState = ivw.CodeState.Stable,
    		tags=ivw.Tags([ivw.Tag.PY, ivw.Tag("Cube"), 
                           ivw.Tag("Gaussian"), ivw.Tag("Volume"), ivw.Tag("Mesh")])
        )

    def getProcessorInfo(self):
        return CubeSourceMolvis.processorInfo()

    def initializeResources(self):
        print("init")

    def process(self):
        print("process")
        if len(self.cubeFilePath.value) == 0 or not Path(self.cubeFilePath.value).exists():
            return

        self.volume, self.atomPos, self.atomType = gaussianutil.parseCubeFile(
            self.cubeFilePath.value, self.flipSign.value, self.centerData.value)
        self.volumeDataRange = self.volume.dataMap.dataRange

        self.volume.dataMap.dataRange = self.customDataRange.value if self.useCustomRange.value else self.volumeDataRange
        self.volume.dataMap.valueRange = self.customDataRange.value if self.useCustomRange.value else self.volumeDataRange

        self.mesh = gaussianutil.createMeshForCube(self.atomPos, self.atomType,
                                        self.volume.basis, self.volume.offset, self.pm)

        self.dataframe = gaussianutil.createDataFrameForCube(self.atomPos, self.atomType)

        atoms = ivwmolvis.Atoms()
        dvec3pos = []

        for p in self.atomPos:
            dvec3pos.append(ivw.glm.dvec3(p))

        if self.centerData.value:
            offset = ivw.glm.dvec3(self.volume.offset)
            dvec3pos = [ x + offset for x in dvec3pos ]
        
        elements = []
        for t in self.atomType:
            elements.append(t)

        atoms.positions = dvec3pos
        atoms.atomicnumbers = elements
        bonds = ivwmolvis.util.computeCovalentBonds(atoms)

        self.molecule = ivwmolvis.MolecularStructure(ivwmolvis.MolecularData(source="cube_file", atoms=atoms, 
                                                                             residues=[], chains=[], 
                                                                             bonds=bonds))
        
        print("Loaded Cube file testest: {}\nDims:  {}\nRange: {}".format(
            self.cubeFilePath.value, self.volume.dimensions, self.volume.dataMap.dataRange))

        self.volumeOutport.setData(self.volume)
        self.meshOutport.setData(self.mesh)
        self.dataframeOutport.setData(self.dataframe)

        self.outports.molecule.setData(self.molecule)

    def callback(self, pickevent):
        if (pickevent.state == ivw.PickingState.Updated):
            i = pickevent.pickedId
            pickevent.setToolTip("Atom id: {}\nType: {}\nPosition: {}".format(
                i, self.elemtype[i], self.atomPos[i]))
        else:
            pickevent.setToolTip("")
