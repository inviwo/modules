# Name: PythonImageExample

# ********************************************************************************
#
# Inviwo - Interactive Visualization Workshop
#
# Copyright (c) 2023-2025 Inviwo Foundation
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
# ********************************************************************************

import inviwopy as ivw
from inviwopy.properties import ConstraintBehavior as cb

import PIL.Image
import numpy as np


class PythonImageExample(ivw.Processor):
    def __init__(self, id, name):
        ivw.Processor.__init__(self, id, name)
        self.outport = ivw.data.ImageOutport("outport")
        self.addOutport(self.outport, owner=False)

        self.filepath = ivw.properties.FileProperty("filepath", "File", "", "image")
        self.addProperty(self.filepath)

        self.imgdims = ivw.properties.IntVec2Property("imgDims", "Image Dimensions",
                                                      ivw.glm.ivec2(0),
                                                      min=(ivw.glm.ivec2(0), cb.Immutable),
                                                      max=(ivw.glm.ivec2(1024), cb.Ignore))
        self.imgdims.semantics = ivw.properties.PropertySemantics("Text")
        self.imgdims.readOnly = True
        self.addProperty(self.imgdims)

        self.channels = ivw.properties.IntProperty("channels", "Channels", 0,
                                                   min=(0, cb.Immutable),
                                                   max=(4, cb.Ignore))
        self.channels.semantics = ivw.properties.PropertySemantics("Text")
        self.channels.readOnly = True
        self.addProperty(self.channels)

    @staticmethod
    def processorInfo():
        return ivw.ProcessorInfo(
            classIdentifier="org.inviwo.PythonImageExample",
            displayName="Python Image Example",
            category="Python",
            codeState=ivw.CodeState.Stable,
            tags=ivw.Tags("PY, Example"),
            help=ivw.md2doc(r'''
Example processor in python demonstrating the use of `inviwopy.data.Image`,
`inviwopy.data.Layer`, and `inviwopy.data.LayerPy`.
''')
        )

    def getProcessorInfo(self):
        return PythonImageExample.processorInfo()

    def initializeResources(self):
        pass

    def process(self):
        if not self.filepath.value.is_file():
            return

        with PIL.Image.open(self.filepath.value) as im:

            # convert image first, if it uses a color palette
            if im.mode == 'P':
                im = im.convert(mode=im.palette.mode)

            channels = len(im.getbands())

            # flip image upside down since Inviwo assumes OpenGL coordinates (origin at lower-left)
            im = im.transpose(PIL.Image.Transpose.FLIP_TOP_BOTTOM)

            # create numpy representation
            npImgData = np.array(im)

            # create a LayerPy representation
            layerpy = ivw.data.LayerPy(npImgData)
            # directly access the layer data as numpy array via `layerpy.data`

            # create a Layer from the representation
            layer = ivw.data.Layer(layerpy)

            swizzleDict = {
                1: ivw.data.SwizzleMask.luminance,
                2: ivw.data.SwizzleMask.luminanceAlpha,
                3: ivw.data.SwizzleMask.rgb,
                4: ivw.data.SwizzleMask.rgba
            }
            # set swizzle mask depending on the number of channels
            layer.swizzlemask = swizzleDict[channels]

            # finally, create an Image containing the layer
            ivwImg = ivw.data.Image(layer)
            self.outport.setData(ivwImg)

            # update properties for feedback
            self.imgdims.value = ivw.glm.ivec2(im.size)
            self.channels.value = channels
