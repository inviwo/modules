#pragma once

#include <warn/push>
#include <warn/ignore/all>
#include <vtkObject.h>
#include <vtkAlgorithm.h>
#include <vtkStructuredGrid.h>
#include <vtkPoints.h>
#include <vtkDataArray.h>
#include <warn/pop>

#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/util/indexmapper.h>
#include <inviwo/core/processors/progressbar.h>

/**
 * Global method used as a callback for vtkCallbackCommand. The method needs to be global, otherwise
 * the signature will not match. clientData should be a pointer to the calling processor's
 * progressbar.
 * Note that you will need to explicitly set the client data via VTK's method SetClientData(void*)
 *
 * Example Usage:
 *
 *    auto progressCallback = vtkSmartPointer<vtkCallbackCommand>::New();
 *    progressCallback->SetCallback(vtkProgressBarCallback);
 *    progressCallback->SetClientData(&progressBar_);
 *
 *    auto someFilter = vtkSmartPointer<vtkSomeFilter>::New();
 *
 *    someFilter->AddObserver(vtkCommand::ProgressEvent, progressCallback);
 *
 */
inline void vtkProgressBarCallback(vtkObject* caller, long unsigned int, void* clientData, void*) {

    if (auto algorithm = dynamic_cast<vtkAlgorithm*>(caller)) {

        auto progressBar = static_cast<inviwo::ProgressBar*>(clientData);

        if (progressBar) {

            if (algorithm->GetProgress() > 0.0) {
                const auto f = float(algorithm->GetProgress());

                inviwo::dispatchFront([progressBar, f]() {
                    f < 1.0f ? progressBar->show() : progressBar->hide();
                    progressBar->updateProgress(f);
                });
            }
        }
    }
}

namespace inviwo {
namespace vtkutil {

inline mat4 basisAndOffsetFromStructuredGrid(vtkStructuredGrid& grid) {
    vtkPoints* points = grid.GetPoints();

    int dimArr[3];
    grid.GetDimensions(dimArr);

    size3_t dim(dimArr[0], dimArr[1], dimArr[2]);

    util::IndexMapper3D index(dim);
    dvec3 firstVoxel;
    dvec3 voxelX, voxelY, voxelZ;
    points->GetPoint(index(0, 0, 0), glm::value_ptr(firstVoxel));
    points->GetPoint(index(1, 0, 0), glm::value_ptr(voxelX));
    points->GetPoint(index(0, 1, 0), glm::value_ptr(voxelY));
    points->GetPoint(index(0, 0, 1), glm::value_ptr(voxelZ));

    vec3 voxelOffsetX = voxelX - firstVoxel;
    vec3 voxelOffsetY = voxelY - firstVoxel;
    vec3 voxelOffsetZ = voxelZ - firstVoxel;

    vec3 offset = vec3(firstVoxel) - (voxelOffsetX + voxelOffsetY + voxelOffsetZ) * 0.5f;

    return mat4{vec4(voxelOffsetX, 0) * dim.x, vec4(voxelOffsetY, 0) * dim.y,
                vec4(voxelOffsetZ, 0) * dim.z, vec4(offset, 1)};
}

inline dvec3 extentFromBounds(const double* bounds) {
    return dvec3{bounds[1] - bounds[0], bounds[3] - bounds[2], bounds[5] - bounds[4]};
}

inline dvec3 offsetFromBounds(const double* bounds) {
    return dvec3{bounds[0], bounds[2], bounds[4]};
}

inline const NumericType getNumericTypeFomVtkId(int vtkTypeId) {
    switch (vtkTypeId) {
        case VTK_CHAR:
        case VTK_SIGNED_CHAR:
        case VTK_SHORT:
        case VTK_INT:
        case VTK_LONG:
        case VTK_LONG_LONG:
#ifdef VTK___INT64
        case VTK___INT64:
#endif
            return NumericType::SignedInteger;

        case VTK_UNSIGNED_CHAR:
        case VTK_UNSIGNED_SHORT:
        case VTK_UNSIGNED_INT:
        case VTK_UNSIGNED_LONG:
        case VTK_UNSIGNED_LONG_LONG:
#ifdef VTK_UNSIGNED___INT64
        case VTK_UNSIGNED___INT64:
#endif
        case VTK_ID_TYPE:
            return NumericType::UnsignedInteger;

        case VTK_FLOAT:
        case VTK_DOUBLE:
            return NumericType::Float;
        default:
            return NumericType::NotSpecialized;
    }
}

inline const DataFormatBase* getFormatFromVtkId(int vtkTypeId, int bitSize, int numComps = 1) {
    return DataFormatBase::get(getNumericTypeFomVtkId(vtkTypeId), numComps, bitSize);
}

inline const DataFormatBase* getDataFormatOfDataArray(vtkDataArray& arr) {
    return getFormatFromVtkId(arr.GetDataType(), 8 * arr.GetDataTypeSize(),
                              arr.GetNumberOfComponents());
}

}  // namespace vtkutil
}  // namespace inviwo
