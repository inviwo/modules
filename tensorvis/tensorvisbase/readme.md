# TensorVisBase module

## Synopsis

This module contains some of the basic tensor visualization techniques.
For loading/saving tensor fields see the
[_tensorvisio_](https://github.com/inviwo/modules/tree/master/tensorvis/tensorvisio) module.


## Available visualizers

### HyperLIC
Available for 2D and 3D tensor fields (3D tensor fields will be sliced).
Based on the standard LIC method, the HyperLIC indicates the directions of the major/minor eigenvectors of the tensor field.

### Anisotropy raycasting
1. A scalar volume is generated, values are given by |&sigma;<sub>1</sub>-&sigma;<sub>3</sub>|.
2. Raycasting is applied.

## Todo
- [ ] Make TensorField2D/3D inherit from SpatialEntity
- [ ] Add visualizer for TensorField2D port
- [ ] Update glyph rendering with instancing
- [ ] Extend TensorField3DSlice to non-aa slicing
- [ ] TensorField3D slice outputs wrong slice geometry
- [ ] Pickable seeding geometry
