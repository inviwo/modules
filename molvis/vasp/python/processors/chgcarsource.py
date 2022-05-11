# Name: ChgcarSource

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
import vasputil

import numpy
from pathlib import Path

# Description found at https://cms.mpi.univie.ac.at/wiki/index.php/CHGCAR
class ChgcarSource(ivw.Processor):
    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)

        self.volumeOutport = ivw.data.VolumeOutport("chargedensity")
        self.addOutport(self.volumeOutport, owner=False)

        self.meshOutport = ivw.data.MeshOutport("atoms")
        self.addOutport(self.meshOutport)

        self.dataframeOutport = df.DataFrameOutport("atomInformation")
        self.addOutport(self.dataframeOutport)

        self.chgcar = ivw.properties.FileProperty("chgcar", "CHGCAR")
        self.addProperty(self.chgcar)

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

        self.changeSign = ivw.properties.BoolProperty(
            "changeSign", "Change the sign of the charge", False)
        self.addProperty(self.changeSign)

        self.offsetData = ivw.properties.BoolProperty(
            "Offset", "Offset the data", True)
        self.addProperty(self.offsetData)

        self.margin = ivw.properties.FloatProperty(
            "margin", "Border Repetition Margin", 0.05, 0.0, 0.5, 0.01)
        self.addProperty(self.margin)

        self.radiusScaling = ivw.properties.FloatProperty(
            "radiusScaling", "Radius Scaling", 0.25, 0.0, 2.0, 0.01)
        self.addProperty(self.radiusScaling)

        self.pm = inviwopy.PickingMapper(self, 1, lambda x: self.callback(x))

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.vasp.ChgcarSource",
            displayName="Chgcar Source",
            category="Source",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags([ivw.Tag.PY, ivw.Tag("VASP"),
                           ivw.Tag("Volume"), ivw.Tag("Mesh")])
        )

    def getProcessorInfo(self):
        return ChgcarSource.processorInfo()

    def initializeResources(self):
        pass

    def process(self):
        if len(self.chgcar.value) == 0 or not Path(self.chgcar.value).exists():
            return

        self.volume, self.atomPos, self.elem, self.nelem, self.atoms = vasputil.parseFile(
            self.chgcar.value, self.changeSign.value, self.offsetData.value)
        self.volumeDataRange = self.volume.dataMap.dataRange

        self.volume.dataMap.dataRange = self.customDataRange.value if self.useCustomRange.value else self.volumeDataRange
        self.volume.dataMap.valueRange = self.customDataRange.value if self.useCustomRange.value else self.volumeDataRange

        self.mesh = vasputil.createMesh(self.atomPos, self.atoms,
                                        self.volume.basis, self.volume.offset, 
                                        self.pm, self.margin.value,
                                        self.radiusScaling.value)

        self.dataframe = vasputil.createDataFrame(self.atomPos, self.atoms,
                                                  self.volume.modelMatrix)

        print("Loaded CHGCAR: {}\nDims:  {}\nElem:  {}\nNElem  {}\nRange: {}".format(
            self.chgcar.value, self.volume.dimensions, self.elem, self.nelem, self.volume.dataMap.dataRange))

        self.volumeOutport.setData(self.volume)
        self.meshOutport.setData(self.mesh)
        self.dataframeOutport.setData(self.dataframe)

    def callback(self, pickevent):
        if (pickevent.state == inviwopy.PickingState.Updated):
            i = pickevent.pickedId
            pos = numpy.dot(numpy.array(self.volume.basis), self.atomPos[i])
            pickevent.setToolTip("Atom id: {}\nType: {}\nPosition: {}\nFractional: {}".format(
                i, ivwmolvis.atomicelement.symbol(self.atoms[i]), pos, self.atomPos[i]))
        else:
            pickevent.setToolTip("")
