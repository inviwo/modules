# OpenMesh module

Inviwo module to interface the C++ library OpenMesh.

When using vcpk, include `openmesh` as vcpkg feature in your CMake preset.
```json
"cacheVariables": {
    "VCPKG_MANIFEST_FEATURES":  "openmesh"
}
```


## Example: Mesh Decimation
```c++
// Convert the inviwo::Mesh to OpenMesh
inviwo::Mesh ivwMesh = ...;
auto omMesh = openmeshutil::fromInviwo(ivwMesh, TransformCoordinates::DataToWorld); 

openmeshutil::decimate(omMesh, FaceFraction{0.5}); // Decimate until we have 50% faces

// Convert from OpenMesh to inviwo::Mesh
auto newIvwMesh = openmeshutil::toInviwo(omMesh); 
newIvwMesh->copyMetaDataFrom(ivwMesh); // Needed to keep meta data
```
