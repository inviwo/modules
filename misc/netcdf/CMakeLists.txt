#--------------------------------------------------------------------
# Inviwo netcdf Module
ivw_module(NetCDF)

#Check if netCDF4 python module exist
execute_process(COMMAND "${PYTHON_EXECUTABLE}" "-c"
             "try: import netCDF4; print(netCDF4.__version__);\nexcept: print('\tFailed to import NetCDF4 python package. Run pip install NetCDF4')\n"
              RESULT_VARIABLE NETCDF_STATUS
              ERROR_VARIABLE NETCDF_ERROR 
              OUTPUT_STRIP_TRAILING_WHITESPACE
            )

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
    include/inviwo/netcdf/io/netcdfdatasetreader.h
    include/inviwo/netcdf/netcdfmodule.h
    include/inviwo/netcdf/netcdfmoduledefine.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    src/io/netcdfdatasetreader.cpp
    src/netcdfmodule.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

#--------------------------------------------------------------------
# Create module
ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/python)
ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/python/processors)
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})
