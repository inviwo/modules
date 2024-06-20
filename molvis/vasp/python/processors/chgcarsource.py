# Name: ChgcarSource

#################################################################################
#
# Inviwo - Interactive Visualization Workshop
#
# Copyright (c) 2020-2024 Inviwo Foundation
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
import vasputil
import ivwbnl

import numpy as np

import importlib
importlib.reload(molviscommon)


# Description found at https://cms.mpi.univie.ac.at/wiki/index.php/CHGCAR
class ChgcarSource(ivw.Processor):
    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)

        self.volumeOutport = ivw.data.VolumeOutport(
            "chargedensity",
            help=ivw.md2doc('Charge density volume')
        )
        self.addOutport(self.volumeOutport, owner=False)

        self.meshOutport = ivw.data.MeshOutport(
            "atoms",
            help=ivw.md2doc('SphereMesh (positions, radii, and colors) of the atoms')
        )
        self.addOutport(self.meshOutport)

        self.dataframeOutport = df.DataFrameOutport(
            "atomInformation",
            help=ivw.md2doc('DataFrame holding atom positions and atomic type')
        )
        self.addOutport(self.dataframeOutport)

        self.addOutport(ivwmolvis.MolecularStructureOutport(
            "molecule",
            help=ivw.md2doc('MolecularStructure representing all atoms'))
        )

        self.bnlInport = ivwbnl.BrushingAndLinkingInport("bnl", [])
        self.addInport(self.bnlInport)

        self.chgcarFilePath = ivw.properties.FileProperty("chgcar", "CHGCAR", "", "chgcarfile")
        self.addProperty(self.chgcarFilePath)

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

        self.margin = ivw.properties.FloatProperty(
            "margin", "Border Repetition Margin", 0.05,
            min=(0.0, ivw.properties.ConstraintBehavior.Immutable),
            max=(0.5, ivw.properties.ConstraintBehavior.Editable), increment=0.01)
        self.addProperty(self.margin)

        self.radiusScaling = ivw.properties.FloatProperty(
            "radiusScaling", "Radius Scaling", 0.25, 0.0, 2.0, 0.01)
        self.addProperty(self.radiusScaling)
        # Add wrapping option for VASP. Default is repeat since the data is most likely periodic.
        self.wrapX = ivw.properties.OptionPropertyInt(
            "wrapX", "Wrapping X",
            [ivw.properties.IntOption("clamp", "Clamp", ivw.data.Wrapping.Clamp),
             ivw.properties.IntOption("repeat", "Repeat", ivw.data.Wrapping.Repeat),
             ivw.properties.IntOption("mirror", "Mirror", ivw.data.Wrapping.Mirror)], 1)
        self.addProperty(self.wrapX, owner=False)
        self.wrapY = ivw.properties.OptionPropertyInt(
            "wrapY", "Wrapping Y",
            [ivw.properties.IntOption("clamp", "Clamp", ivw.data.Wrapping.Clamp),
             ivw.properties.IntOption("repeat", "Repeat", ivw.data.Wrapping.Repeat),
             ivw.properties.IntOption("mirror", "Mirror", ivw.data.Wrapping.Mirror)], 1)
        self.addProperty(self.wrapY, owner=False)
        self.wrapZ = ivw.properties.OptionPropertyInt(
            "wrapZ", "Wrapping Z",
            [ivw.properties.IntOption("clamp", "Clamp", ivw.data.Wrapping.Clamp),
             ivw.properties.IntOption("repeat", "Repeat", ivw.data.Wrapping.Repeat),
             ivw.properties.IntOption("mirror", "Mirror", ivw.data.Wrapping.Mirror)], 1)
        self.addProperty(self.wrapZ, owner=False)

        self.colormap = ivw.properties.OptionPropertyInt(
            "colormap", "Colormap",
            [ivw.properties.IntOption("cpk", "Rasmol CPK",
                                      ivwmolvis.atomicelement.Colormap.RasmolCPK),
             ivw.properties.IntOption("cpknew", "Rasmol CPK new",
                                      ivwmolvis.atomicelement.Colormap.RasmolCPKnew),
             ivw.properties.IntOption("jmol", "Jmol", ivwmolvis.atomicelement.Colormap.Jmol)], 1)
        self.addProperty(self.colormap, owner=False)

        self.pm = ivw.PickingMapper(self, 1, lambda x: self.callback(x))

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.vasp.ChgcarSource",
            displayName="Chgcar Source",
            category="Source",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags([ivw.Tags.PY, ivw.Tag("VASP"),
                           ivw.Tag("Volume"), ivw.Tag("Mesh"), ivw.Tag("MolVis")]),
            help=ivw.md2doc(r'''
Loads CHGCAR files stemming from [VASP](https://www.vasp.at) calculations.
''')
        )

    def getProcessorInfo(self):
        return ChgcarSource.processorInfo()

    def initializeResources(self):
        pass

    def process(self):
        if not self.chgcarFilePath.value.is_file():
            return

        if not self.chgcarFilePath.value.exists():
            return

        self.volume, self.atomPos, self.elem, self.nelem, self.atomTypes = vasputil.parseFile(
            self.chgcarFilePath.value, self.flipSign.value, self.centerData.value)
        self.dataRange.value = self.volume.dataMap.dataRange

        self.volume.dataMap.dataRange = (
            self.customDataRange.value if self.useCustomRange.value else self.dataRange.value
        )
        self.volume.dataMap.valueRange = self.volume.dataMap.dataRange

        self.volume.wrapping = [ivw.data.Wrapping(self.wrapX.value), ivw.data.Wrapping(
            self.wrapY.value), ivw.data.Wrapping(self.wrapZ.value)]
        self.mesh = molviscommon.createMesh(
            pos=self.atomPos, elements=self.atomTypes,
            basis=self.volume.basis, offset=self.volume.offset,
            pm=self.pm, margin=self.margin.value,
            radiusscaling=self.radiusScaling.value,
            colormap=ivwmolvis.atomicelement.Colormap(self.colormap.value)
        )

        self.dataframe = molviscommon.createDataFrame(pos=self.atomPos, elements=self.atomTypes,
                                                      modelmat=self.volume.modelMatrix)

        offset = ivw.glm.dvec3(self.volume.offset) if self.centerData.value else ivw.glm.dvec3(0)
        self.molecule = molviscommon.createMolecularStructure(
            pos=self.atomPos, elements=self.atomTypes, margin=self.margin.value,
            basis=self.volume.basis, offset=offset)

        self.volumeOutport.setData(self.volume)
        self.meshOutport.setData(self.mesh)
        self.dataframeOutport.setData(self.dataframe)
        self.outports.molecule.setData(self.molecule)

    def callback(self, pickEvent):
        if pickEvent.pressState == ivw.PickingPressState.NoPress:
            if pickEvent.hoverState == ivw.PickingHoverState.Enter:
                i = pickEvent.pickedId
                self.bnlInport.highlight(ivw.data.BitSet([i]))
                pos = np.dot(np.array(self.volume.basis), self.atomPos[i])
                elem = ivwmolvis.atomicelement.symbol(self.atomTypes[i])
                pickEvent.setToolTip(
                    f"Atom id: {i}\nType: {elem}\nPosition: {pos}\nFractional: {self.atomPos[i]}")

            elif pickEvent.hoverState == ivw.PickingHoverState.Exit:
                self.bnlInport.highlight(ivw.data.BitSet())
                pickEvent.setToolTip("")

        if (pickEvent.pressState == ivw.PickingPressState.Release
            and pickEvent.pressItem == ivw.PickingPressItem.Primary
            and abs(pickEvent.deltaPressedPosition.x) < 0.01
                and abs(pickEvent.deltaPressedPosition.y) < 0.01):

            selection = self.bnlInport.getSelectedIndices()
            selection.flip(pickEvent.pickedId)
            self.bnlInport.select(selection)
