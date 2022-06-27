# Name: CubeSource

 #################################################################################
 #
 # Inviwo - Interactive Visualization Workshop
 #
 # Copyright (c) 2020-2022 Inviwo Foundation
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
import molviscommon
import gaussianutil

import numpy as np
from pathlib import Path

# Description found at https://h5cube-spec.readthedocs.io/en/latest/cubeformat.html
class CubeSource(ivw.Processor):
    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)

        self.volumeOutport = ivw.data.VolumeOutport("chargedensity")
        self.addOutport(self.volumeOutport, owner=False)

        self.meshOutport = ivw.data.MeshOutport("atoms")
        self.addOutport(self.meshOutport)

        self.dataframeOutport = df.DataFrameOutport("atomInformation")
        self.addOutport(self.dataframeOutport)

        self.addOutport(ivwmolvis.MolecularStructureOutport("molecule"))

        self.cubeFilePath = ivw.properties.FileProperty("cube", "cube", "", "cubefile")
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

        self.radiusScaling = ivw.properties.FloatProperty(
            "radiusScaling", "Radius Scaling", 0.25, 0.0, 2.0, 0.01)
        self.addProperty(self.radiusScaling)

        self.pm = inviwopy.PickingMapper(self, 1, lambda x: self.callback(x))

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.gaussian.CubeSource",
            displayName="Cube Source",
            category="Source",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags([ivw.Tag.PY, ivw.Tag("Cube"), ivw.Tag("Gaussian"), 
                ivw.Tag("Volume"), ivw.Tag("Mesh"), ivw.Tag("MolVis")])
        )

    def getProcessorInfo(self):
        return CubeSource.processorInfo()

    def initializeResources(self):
        pass

    def process(self):
        if len(self.cubeFilePath.value) == 0 or not Path(self.cubeFilePath.value).exists():
            return

        self.volume, self.atomPos, self.atomTypes = gaussianutil.parseCubeFile(
            self.cubeFilePath.value, self.flipSign.value, self.centerData.value)
        self.dataRange.value = self.volume.dataMap.dataRange

        self.volume.dataMap.dataRange = self.customDataRange.value if self.useCustomRange.value else self.dataRange.value
        self.volume.dataMap.valueRange = self.volume.dataMap.dataRange

        self.mesh = molviscommon.createMesh(pos=self.atomPos, elements=self.atomTypes,
                                            basis=self.volume.basis, offset=self.volume.offset,
                                            pm=self.pm, radiusscaling=self.radiusScaling.value)

        self.dataframe = molviscommon.createDataFrame(pos=self.atomPos, elements=self.atomTypes,
                                                      modelmat=self.volume.modelMatrix)

        offset = ivw.glm.dvec3(self.volume.offset) if self.centerData.value else ivw.glm.dvec3(0)
        self.molecule = molviscommon.createMolecularStructure(pos=self.atomPos, elements=self.atomTypes, 
                                                              offset=offset)
        
        self.volumeOutport.setData(self.volume)
        self.meshOutport.setData(self.mesh)
        self.dataframeOutport.setData(self.dataframe)
        self.outports.molecule.setData(self.molecule)

    def callback(self, pickevent):
        if (pickevent.state == inviwopy.PickingState.Updated):
            i = pickevent.pickedId
            pos = np.dot(np.array(self.volume.basis), self.atomPos[i])
            pickevent.setToolTip(f"Atom id: {i}\nType: {ivwmolvis.atomicelement.symbol(self.atomTypes[i])}\nPosition: {pos}\nFractional: {self.atomPos[i]}")
        else:
            pickevent.setToolTip("")
