# electrostatics Module

Description of the electrostatics module

This module takes as input the Morse-Smale data from the Morse-Smale complex processor
and data from the dataframe outport of the VASP reader to compute the electrostatics of the system

Computes charge as the sum of all voxels/total amount of voxels.
<a href="https://www.codecogs.com/eqnedit.php?latex=q_{tot}=\sum&space;q_{i}r_{i}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?q_{tot}=\sum&space;q_{i}r_{i}" title="q_{tot}=\sum q_{i}r_{i}" /></a>
Computes dipole moments as the sum of the charge times the vector from the atom position to the voxel.

