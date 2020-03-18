# Molecular Visualization Modules

## MolVisBase

Inviwo module providing the underlying data structures, i.e. `MolecularStructure`, and utilities for handling molecular data. 

## MolVisGL

This module is responsible for rendering molecular structures using OpenGL. Molecular data can currently be rendered with the following representations:

- van-der-Waals
- Licorice
- Ball and Stick

## MolVisPython

This module exposes the MolecularStructure data structure to Python. It also contains a processor, i.e. `MolecularStructureSource`, for loading both PDB and CIF files. BioPython is used for parsing the structures.
