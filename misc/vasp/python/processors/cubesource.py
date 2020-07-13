# Name: CubeSource

import inviwopy as ivw
import ivwdataframe as df
import atomdata
import vasputil

import functools
import math
import numpy
from pathlib import Path

# Descriotion found at https://h5cube-spec.readthedocs.io/en/latest/cubeformat.html
class CubeSource(ivw.Processor):
    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)

        self.volumeOutport = ivw.data.VolumeOutport("chargedensity")
        self.addOutport(self.volumeOutport, owner=False)

        self.meshOutport = ivw.data.MeshOutport("atoms")
        self.addOutport(self.meshOutport)

        self.dataframeOutport = df.DataFrameOutport("atomInformation")
        self.addOutport(self.dataframeOutport)

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

        self.pm = inviwopy.PickingMapper(self, 1, lambda x: self.callback(x))

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.vasp.CubeSource",
            displayName="Cube Source",
            category="Source",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags([ivw.Tag.PY, ivw.Tag("VASP"), ivw.Tag("Cube"), 
                           ivw.Tag("Gaussian"), ivw.Tag("Volume"), ivw.Tag("Mesh")])
        )

    def getProcessorInfo(self):
        return CubeSource.processorInfo()

    def initializeResources(self):
        pass

    def process(self):
        if len(self.cubeFilePath.value) == 0 or not Path(self.cubeFilePath.value).exists():
            return

        self.volume, self.atomPos, self.elem, self.nelem, self.elemtype = vasputil.parseCubeFile(
            self.cubeFilePath.value)
        self.volumeDataRange = self.volume.dataMap.dataRange

        self.volume.dataMap.dataRange = self.customDataRange.value if self.useCustomRange.value else self.volumeDataRange
        self.volume.dataMap.valueRange = self.customDataRange.value if self.useCustomRange.value else self.volumeDataRange

        self.mesh = vasputil.createMeshForCube(self.atomPos, self.elemtype,
                                        self.volume.basis, self.volume.offset, self.pm)

        self.dataframe = vasputil.createDataFrameForCube(self.atomPos, self.elemtype)

        print("Loaded Cube file: {}\nDims:  {}\nElem:  {}\nNElem  {}\nRange: {}".format(
            self.cubeFilePath.value, self.volume.dimensions, self.elem, self.nelem, self.volume.dataMap.dataRange))

        self.volumeOutport.setData(self.volume)
        self.meshOutport.setData(self.mesh)
        self.dataframeOutport.setData(self.dataframe)

    def callback(self, pickevent):
        if (pickevent.state == inviwopy.PickingState.Updated):
            i = pickevent.pickedId
            pickevent.setToolTip("Atom id: {}\nType: {}\nPosition: {}".format(
                i, self.elemtype[i], self.atomPos[i]))
        else:
            pickevent.setToolTip("")
