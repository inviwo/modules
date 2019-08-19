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

#include <modules/tensorvisbase/tensorvisbasemodule.h>

#include <modules/opengl/shader/shadermanager.h>

#include <modules/tensorvisbase/datavisualizer/hyperlicvisualizer2d.h>
#include <modules/tensorvisbase/datavisualizer/hyperlicvisualizer3d.h>
#include <modules/tensorvisbase/datavisualizer/anisotropyraycastingvisualizer.h>

#include <modules/tensorvisbase/ports/tensorfieldport.h>
#include <modules/tensorvisbase/processors/eigenvaluefieldtoimage.h>
#include <modules/tensorvisbase/processors/hyperstreamlines.h>
#include <modules/tensorvisbase/processors/imagetospherefield.h>
#include <modules/tensorvisbase/processors/invariantspacecombine.h>
#include <modules/tensorvisbase/processors/invariantspacefilter.h>
#include <modules/tensorvisbase/processors/invariantspaceselection.h>
#include <modules/tensorvisbase/processors/invariantspacetodataframe.h>
#include <modules/tensorvisbase/processors/mohrcircleprocessor.h>
#include <modules/tensorvisbase/processors/tensorfield2danisotropy.h>
#include <modules/tensorvisbase/processors/tensorfield2dsubsample.h>
#include <modules/tensorvisbase/processors/tensorfield2dsubset.h>
#include <modules/tensorvisbase/processors/tensorfield3dangle.h>
#include <modules/tensorvisbase/processors/tensorfield3danisotropy.h>
#include <modules/tensorvisbase/processors/tensorfield3dbasismanipulation.h>
#include <modules/tensorvisbase/processors/tensorfield3dboundingbox.h>
#include <modules/tensorvisbase/processors/tensorfield3dmasktovolume.h>
#include <modules/tensorvisbase/processors/tensorfield3dmetadata.h>
#include <modules/tensorvisbase/processors/tensorfield3dsubsample.h>
#include <modules/tensorvisbase/processors/tensorfield3dsubset.h>
#include <modules/tensorvisbase/processors/tensorfieldgenerator.h>
#include <modules/tensorvisbase/processors/tensorfieldlic.h>
#include <modules/tensorvisbase/processors/tensorfieldslice.h>
#include <modules/tensorvisbase/processors/tensorfieldtorgba.h>
#include <modules/tensorvisbase/processors/tensorfieldtovolume.h>
#include <modules/tensorvisbase/processors/tensorglyphprocessor.h>
#include <modules/tensorvisbase/processors/tensorglyphrenderer.h>
#include <modules/tensorvisbase/processors/tensorinformation.h>
#include <modules/tensorvisbase/processors/tensorstarplotprocessor.h>
#include <modules/tensorvisbase/processors/volumeactualdataandvaluerange.h>
#include <modules/tensorvisbase/properties/eigenvalueproperty.h>
#include <modules/tensorvisbase/properties/tensorglyphproperty.h>

namespace inviwo {

TensorVisBaseModule::TensorVisBaseModule(InviwoApplication* app)
    : InviwoModule(app, "TensorVisBase") {
    ShaderManager::getPtr()->addShaderSearchPath(getPath(ModulePath::GLSL));

    registerProcessor<EigenvalueFieldToImage>();
    registerProcessor<HyperStreamlines>();
    registerProcessor<ImageToSphereField>();
    registerProcessor<InvariantSpaceCombine>();
    registerProcessor<InvariantSpaceFilter>();
    registerProcessor<MohrCircleProcessor>();
    registerProcessor<TensorField2DAnisotropy>();
    registerProcessor<TensorField2DSubsample>();
    registerProcessor<TensorField2DSubset>();
    registerProcessor<TensorField3DAngle>();
    registerProcessor<TensorField3DAnisotropy>();
    registerProcessor<TensorField3DBasisManipulation>();
    registerProcessor<TensorField3DBoundingBox>();
    registerProcessor<TensorField3DMaskToVolume>();
    registerProcessor<TensorField3DMetaData>();
    registerProcessor<TensorField3DSubsample>();
    registerProcessor<TensorField3DSubset>();
    registerProcessor<TensorFieldGenerator>();
    registerProcessor<TensorFieldLIC>();
    registerProcessor<TensorFieldSlice>();
    registerProcessor<TensorFieldToRGBA>();
    registerProcessor<TensorFieldToVolume>();
    registerProcessor<TensorGlyphProcessor>();
    registerProcessor<TensorGlyphRenderer>();
    registerProcessor<TensorInformation>();
    registerProcessor<TensorStarPlotProcessor>();
    registerProcessor<VolumeActualDataAndValueRange>();
    registerProcessor<InvariantSpaceToDataFrame>();
    registerProcessor<InvariantSpaceSelection>();

    registerProperty<EigenValueProperty>();
    registerProperty<TensorGlyphProperty>();

    registerDataVisualizer(std::make_unique<HyperLICVisualizer2D>(app));
    registerDataVisualizer(std::make_unique<HyperLICVisualizer3D>(app));
    registerDataVisualizer(std::make_unique<AnisotropyRaycastingVisualizer>(app));
}

}  // namespace inviwo
