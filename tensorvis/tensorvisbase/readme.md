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

