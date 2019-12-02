# Name: ChgcarSource

import inviwopy as ivw
import ivwdataframe as df
import atomdata
import vasputil

import functools
import math
import numpy
from pathlib import Path

# Descriotion found at https://cms.mpi.univie.ac.at/wiki/index.php/CHGCAR
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
        
        self.changeSign = ivw.properties.BoolProperty(
            "changeSign", "Change the sign of the data", False)
        self.addProperty(self.changeSign)

        self.customDataRange = ivw.properties.DoubleMinMaxProperty(
            "customDataRange", "Custom Data Range", 0.0, 1.0, -1.70e308, 1.79e308)
        self.addProperty(self.customDataRange)
        self.properties.customDataRange.semantics = ivw.properties.PropertySemantics(
            "Text")

        self.margin = ivw.properties.FloatProperty(
            "margin", "Border Repetition Margin", 0.05, 0.0, 0.5, 0.01)
        self.addProperty(self.margin)

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
       
        self.volume, self.atomPos, self.elem, self.nelem, self.elemtype = vasputil.parseFile(
            self.chgcar.value, self.changeSign.value)
        self.volumeDataRange = self.volume.dataMap.dataRange

        self.volume.dataMap.dataRange = self.customDataRange.value if self.useCustomRange.value else self.volumeDataRange
        self.volume.dataMap.valueRange = self.customDataRange.value if self.useCustomRange.value else self.volumeDataRange

        self.mesh = vasputil.createMesh(self.atomPos, self.elemtype,
                                        self.volume.basis, self.volume.offset, self.pm, self.margin.value)
        
        self.dataframe = vasputil.createDataFrame(self.atomPos, self.elemtype,
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
                i, self.elemtype[i], pos, self.atomPos[i]))
        else:
            pickevent.setToolTip("")
