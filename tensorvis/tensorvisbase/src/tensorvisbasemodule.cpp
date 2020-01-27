/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2017-2018 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#include <inviwo/tensorvisbase/tensorvisbasemodule.h>

#include <modules/opengl/shader/shadermanager.h>

#include <inviwo/tensorvisbase/datavisualizer/hyperlicvisualizer2d.h>
#include <inviwo/tensorvisbase/datavisualizer/hyperlicvisualizer3d.h>
#include <inviwo/tensorvisbase/datavisualizer/anisotropyraycastingvisualizer.h>

#include <inviwo/tensorvisbase/ports/tensorfieldport.h>
#include <inviwo/tensorvisbase/processors/hyperstreamlines.h>
#include <inviwo/tensorvisbase/processors/imagetospherefield.h>
#include <inviwo/tensorvisbase/processors/invariantspacecombine.h>
#include <inviwo/tensorvisbase/processors/invariantspacefilter.h>
#include <inviwo/tensorvisbase/processors/invariantspacetodataframe.h>
#include <inviwo/tensorvisbase/processors/tensorfield2danisotropy.h>
#include <inviwo/tensorvisbase/processors/tensorfield2dmetadata.h>
#include <inviwo/tensorvisbase/processors/tensorfield2dsubsample.h>
#include <inviwo/tensorvisbase/processors/tensorfield2dsubset.h>
#include <inviwo/tensorvisbase/processors/tensorfield2dtoimage.h>
#include <inviwo/tensorvisbase/processors/tensorfield3dangle.h>
#include <inviwo/tensorvisbase/processors/tensorfield3dbasismanipulation.h>
#include <inviwo/tensorvisbase/processors/tensorfield3dboundingbox.h>
#include <inviwo/tensorvisbase/processors/tensorfield3dmasktovolume.h>
#include <inviwo/tensorvisbase/processors/tensorfield3dmetadata.h>
#include <inviwo/tensorvisbase/processors/tensorfield3dsubsample.h>
#include <inviwo/tensorvisbase/processors/tensorfield3dsubset.h>
#include <inviwo/tensorvisbase/processors/tensorfield3dtodataframe.h>
#include <inviwo/tensorvisbase/processors/tensorfield2dgenerator.h>
#include <inviwo/tensorvisbase/processors/tensorfieldlic.h>
#include <inviwo/tensorvisbase/processors/tensorfieldslice.h>
#include <inviwo/tensorvisbase/processors/tensorfieldtorgba.h>
#include <inviwo/tensorvisbase/processors/tensorfield3dtovolume.h>
#include <inviwo/tensorvisbase/processors/tensorglyphprocessor.h>
#include <inviwo/tensorvisbase/processors/tensorglyphrenderer.h>
#include <inviwo/tensorvisbase/processors/tensorfield3dinformation.h>
#include <inviwo/tensorvisbase/processors/volumeactualdataandvaluerange.h>
#include <inviwo/tensorvisbase/properties/eigenvalueproperty.h>
#include <inviwo/tensorvisbase/properties/tensorglyphproperty.h>

namespace inviwo {

const Tag TensorVisTag::OpenTensorVis("OpenTensorVis");

TensorVisBaseModule::TensorVisBaseModule(InviwoApplication* app)
    : InviwoModule(app, "TensorVisBase") {
    ShaderManager::getPtr()->addShaderSearchPath(getPath(ModulePath::GLSL));

    registerProcessor<HyperStreamlines>();
    registerProcessor<ImageToSphereField>();
    registerProcessor<InvariantSpaceCombine>();
    registerProcessor<InvariantSpaceFilter>();
    registerProcessor<TensorField2DAnisotropy>();
    registerProcessor<TensorField2DMetaData>();
    registerProcessor<TensorField2DSubsample>();
    registerProcessor<TensorField2DSubset>();
    registerProcessor<TensorField2DToImage>();
    registerProcessor<TensorField3DAngle>();
    registerProcessor<TensorField3DBasisManipulation>();
    registerProcessor<TensorField3DBoundingBox>();
    registerProcessor<TensorField3DMaskToVolume>();
    registerProcessor<TensorField3DMetaData>();
    registerProcessor<TensorField3DSubsample>();
    registerProcessor<TensorField3DSubset>();
    registerProcessor<TensorField3DToDataFrame>();
    registerProcessor<TensorField2DGenerator>();
    registerProcessor<TensorFieldLIC>();
    registerProcessor<TensorFieldSlice>();
    registerProcessor<TensorFieldToRGBA>();
    registerProcessor<TensorField3DToVolume>();
    registerProcessor<TensorGlyphProcessor>();
    registerProcessor<TensorGlyphRenderer>();
    registerProcessor<TensorField3DInformation>();
    registerProcessor<VolumeActualDataAndValueRange>();
    registerProcessor<InvariantSpaceToDataFrame>();

    registerProperty<EigenValueProperty>();
    registerProperty<TensorGlyphProperty>();

    registerDataVisualizer(std::make_unique<HyperLICVisualizer2D>(app));
    registerDataVisualizer(std::make_unique<HyperLICVisualizer3D>(app));
    registerDataVisualizer(std::make_unique<AnisotropyRaycastingVisualizer>(app));
}

}  // namespace inviwo
