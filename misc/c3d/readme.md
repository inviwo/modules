# C3D Module

Inviwo module for loading and visualizing C3D (Coordinate 3D) files, commonly
used in biomechanics and motion capture.

Uses the [ezc3d](https://github.com/pyomeca/ezc3d) library for reading C3D files
via vcpkg.

## Components

- **C3DReader**: DataReader for `.c3d` files
- **C3DSource**: Source processor for loading C3D files
- **C3DToDataFrame**: Converts C3D point and analog data into Inviwo DataFrames
- **C3DToMesh**: Creates a point cloud mesh from C3D marker positions
