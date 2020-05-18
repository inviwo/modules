#--------------------------------------------------------------------
# Inviwo netcdf Module
ivw_module(NetCDF)

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
    include/inviwo/netcdf/netcdfmodule.h
    include/inviwo/netcdf/netcdfmoduledefine.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    src/netcdfmodule.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

#--------------------------------------------------------------------
# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})
