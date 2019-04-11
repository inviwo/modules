# TensorVisBase module

This does all the basic tensor visualization stuff.
For loading/saving tensor fields see the _tensorvisio_ module

## Available visualizers
### HyperLIC
Available for 2D and 3D tensor fields (3D tensor fields will be sliced).
Based on the standard LIC method, HyperLIC is indicating the directions of the major/minor eigenvectors.

### Anisotropy raycasting
1. A scalar volume is generated, values are given by |&sigma;<sub>1</sub>-&sigma;<sub>3</sub>|.
2. Raycasting is applied.
