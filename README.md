# Inviwo InfraVis modules

Additional Inviwo modules for different InfraVis projects. To enable the modules of a specific category, the path of the category must be added to the CMake option `IVW_EXTERNAL_MODULES` in the Inviwo project. For example, the **astrophysics** module is enabled by setting `IVW_EXTERNAL_MODULES` to `c:/dev/inviwo/infravis/infravis`, assuming the repository was checked out into `c:/dev/inviwo/infravis`. Paths are separated by semicolon `;`.

## infravis - various modules

* AstroPhysics: primarily Python-based processors dealing with ALMA data in the FITS data format
