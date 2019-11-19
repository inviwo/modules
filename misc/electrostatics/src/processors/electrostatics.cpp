/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019 Inviwo Foundation
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

#include <inviwo/electrostatics/processors/electrostatics.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo electrostatics::processorInfo_{
    "org.inviwo.electrostatics",  // Class identifier
    "electrostatics",             // Display name
    "Undefined",                  // Category
    CodeState::Experimental,      // Code state
    Tags::None,                   // Tags
};
const ProcessorInfo electrostatics::getProcessorInfo() const { return processorInfo_; }

electrostatics::electrostatics()
    : Processor()
    , inport_{"inport"}
    , volumePort_("volume")
    , volumeOUTPort_("test")
    , dataPort_("data")
   
    , index_("Segnment","Segment",8,0,200) {
    
    addProperty(index_);
    addPort(inport_);
    addPort(volumePort_);
    addPort(volumeOUTPort_);

    addPort(dataPort_);
}

void electrostatics::process() {

    auto msc = inport_.getData();
    auto dat = dataPort_.getData();

    const auto loadedVolume = volumePort_.getData();
    const auto volumeRAM = loadedVolume->getRepresentation<VolumeRAM>();
    const auto basis = loadedVolume->getBasis();  // Needed for PBC
    const auto inv = glm::inverse(basis);         // Needed for PBC
    const size3_t volumeDims = loadedVolume->getDimensions();
    int totVox = volumeDims.x * volumeDims.y * volumeDims.z;

    auto& cp = msc->criticalPoints.points;  // all critical points

    auto& seg =
        msc->segmentation.ascending;  // gives the index of the Morse-Smale complex for index
                                      // i (Use this manifold with default CHGCAR reader)

    util::IndexMapper3D im(volumeDims);
    std::vector<std::vector<double>> fnValsByIndex;
    std::vector<std::string> A_NAME;
    std::vector<vec3> A_POS;
    std::vector<vec3> MaxPoints;
    int MAX_SEG = *std::max_element(seg.begin(), seg.end());
    index_.setMaxValue(MAX_SEG);
    fnValsByIndex.resize(MAX_SEG + 1);

    LogInfo("Input Atoms from the CHGCAR");

    for (int i = 0; i < dat->getNumberOfRows(); i++) {

        const auto& A_NUM = dat->getColumn(0)->getAsDouble(i);
        const auto& A_TYPE = dat->getColumn(1)->getAsString(i);
        const auto& A_POSX = dat->getColumn(2)->getAsDouble(i);
        const auto& A_POSY = dat->getColumn(3)->getAsDouble(i);
        const auto& A_POSZ = dat->getColumn(4)->getAsDouble(i);

        A_NAME.push_back(A_TYPE);
        A_POS.push_back(vec3(A_POSX, A_POSY, A_POSZ));

        LogInfo("Atom " << A_NUM << "  " << A_TYPE << "  " << A_POSX << "," << A_POSY << ","
                        << A_POSZ);
    }

    for (int i = (msc->criticalPoints.numberOfPoints) - (MAX_SEG + 1);
         i < (msc->criticalPoints.numberOfPoints); i++) {
        MaxPoints.push_back(vec3(cp[3 * i], cp[3 * i + 1], cp[3 * i + 2]));
    }

    /*for (size_t i = 0; i < totVox; i++) {
         fnValsByIndex[seg[i]].push_back(volumeRAM->getAsDouble(im(i)));
     }
     */

    auto volOUT = volumeRAM->dispatch<std::shared_ptr<Volume>,dispatching::filter::Scalars>([&](auto inRam) {
        auto ram = std::make_shared<VolumeRAMPrecision<util::PrecisionValueType<decltype(inRam)>>>(
            loadedVolume->getDimensions());
        auto outdata = ram->getDataTyped();
        auto indata = inRam->getDataTyped();
        for (size_t i = 0; i < totVox; i++) {
            fnValsByIndex[seg[i]].push_back(static_cast<double>(indata[i]));
            if (seg[i] == index_.get()) {
                outdata[i] = indata[i];
            }
        }
        return std::make_shared<Volume>(ram);
    });
    volOUT->setModelMatrix(loadedVolume->getModelMatrix());
    volOUT->setWorldMatrix(loadedVolume->getWorldMatrix());
    volOUT->copyMetaDataFrom(*loadedVolume);

    volumeOUTPort_.setData(volOUT);

    LogInfo("Analysis results");
    int c = 0;
    for (auto i : A_POS) {
        vec3 tmppos;
        double distance = INFINITY;
        for (auto j : MaxPoints) {

            // PERIODIC boundary conditions for distances
            vec3 s1 = inv * i;
            vec3 s2 = inv * j;
            vec3 s21 = s2 - s1;
            s21.x -= rint(s21.x);
            s21.y -= rint(s21.y);
            s21.z -= rint(s21.z);
            vec3 d21 = basis * s21;
            double dist = sqrt(glm::dot(d21, d21));
            // Ends here

            if (dist < distance) {
                distance = dist;
                tmppos = s1;
            }
        }

        double Nelec = 0.0;
        // WILL cause problems if you start changing coordinate-systems.
        for (auto k : fnValsByIndex[seg[im((tmppos.x) * (volumeDims.x), (tmppos.y) * (volumeDims.y),
                                           (tmppos.z) * (volumeDims.z))]]) {

            Nelec += k;
        }
        LogInfo("Atom " << A_NAME[c] << " seg "
                        << seg[im((tmppos.x) * (volumeDims.x), (tmppos.y) * (volumeDims.y),
                                  (tmppos.z) * (volumeDims.z))]
                        << "   " << i << "  " << tmppos << "  Number of electrons "
                        << Nelec / static_cast<double>(totVox));
        c++;
    }

    LogInfo("");
    /*  for (auto fnVals : fnValsByIndex) {
          double Nelec = 0.0;
          for (auto fnVal : fnVals) {
              Nelec += fnVal;
          }
          LogInfo("Number of electrons " << Nelec / double(totVox));
      }*/

    // outport_.setData(myImage);
}

}  // namespace inviwo
