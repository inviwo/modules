# TensorVisBase module

This module contains some of the basic tensor visualization techniques.
For loading/saving tensor fields see the
[_tensorvisio_](https://github.com/inviwo/modules/tree/master/tensorvis/tensorvisio) module.

## Functionalities included in this module:
- HyperLIC
- Anisotropy raycasting
- Invariant space based plots (PCP, Scatterplot)

## Available visualizers
### HyperLIC
Available for 2D and 3D tensor fields (3D tensor fields will be sliced).
Based on the standard LIC method, HyperLIC is indicating the directions of
the major/minor eigenvectors.

### Anisotropy raycasting
1. A scalar volume is generated, values are given by |&sigma;<sub>1</sub>-&sigma;<sub>3</sub>|.
2. Raycasting is applied.

## Todo
- [ ] Implement/improve glyph seeding
- [ ] Arithmetic tensor field operations (combine tensor fields)
- [ ] Add more visualizers
- [ ] Automatic texture size for Tensor LIC (i.e. automatic aspect ratio with multiplier)
- [ ] Make TensorField2D and TensorField3D derive from SpatialEntity
- [ ] Investigate if it would be a good idea to use Inviwo's MetaDataOwner rather than own meta data class
