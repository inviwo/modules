/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018-2019 Inviwo Foundation
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

#ifndef IVW_PERSISTENCEDIAGRAMPORT_H
#define IVW_PERSISTENCEDIAGRAMPORT_H

#include <inviwo/topologytoolkit/topologytoolkitmoduledefine.h>

#include <inviwo/core/ports/datainport.h>
#include <inviwo/core/ports/dataoutport.h>

#include <warn/push>
#include <warn/ignore/all>
#include <ttk/core/base/persistenceDiagram/PersistenceDiagram.h>
#include <warn/pop>

#include <tuple>
#include <vector>

namespace inviwo {

namespace topology {

// See PersistenceDiagram::computeCTPersistenceDiagram
using PersistenceDiagramData =
    std::vector<std::tuple<ttk::SimplexId, ttk::CriticalType, ttk::SimplexId, ttk::CriticalType,
                           float, ttk::SimplexId> >;

/**
 * \ingroup ports
 */
using PersistenceDiagramInport = DataInport<topology::PersistenceDiagramData>;
/**
 * \ingroup ports
 */
using PersistenceDiagramOutport = DataOutport<topology::PersistenceDiagramData>;

}  // namespace topology

template <>
struct DataTraits<topology::PersistenceDiagramData> {
    static std::string classIdentifier() { return "org.topology.persistencediagramdata"; }
    static std::string dataName() { return "PersistenceDiagramData"; }
    static uvec3 colorCode() { return uvec3(65, 122, 155); }  // uvec3(73, 153, 175); }
    static Document info(const topology::PersistenceDiagramData& data) {
        using H = utildoc::TableBuilder::Header;
        using P = Document::PathComponent;
        Document doc;
        doc.append("b", dataName(), {{"style", "color:white;"}});
        utildoc::TableBuilder tb(doc.handle(), P::end());
        tb(H("Size"), data.size());
        return doc;
    }
};

}  // namespace inviwo

#endif  // IVW_PERSISTENCEDIAGRAMPORT_H
