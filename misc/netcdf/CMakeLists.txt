# Inviwo netcdf Module
ivw_module(NetCDF)

set(HEADER_FILES
    include/inviwo/netcdf/netcdfmodule.h
    include/inviwo/netcdf/netcdfmoduledefine.h
)
ivw_group("Header Files" ${HEADER_FILES})

set(SOURCE_FILES
    src/netcdfmodule.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/python)
ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/python/processors)

set_property(GLOBAL APPEND PROPERTY IVW_PYTHON_ADDITIONAL_MODULES netCDF4)
