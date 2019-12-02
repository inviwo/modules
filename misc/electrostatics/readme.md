# electrostatics Module

Description of the electrostatics module

This module takes as input the Morse-Smale data from the Morse-Smale complex processor
and data from the dataframe outport of the VASP reader to compute the electrostatics of the system

Computes charge as the sum of all voxels/total amount of voxels.
Computes dipole moments as charge times the vector from the atom position to the voxel.