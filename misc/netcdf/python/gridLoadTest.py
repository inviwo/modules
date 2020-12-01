import numpy
from pathlib import Path
import sys

from netCDF4 import Dataset
import netcdfutils

print('Test!')
filePath = '/media/anke/OS/Research/Data/ClimateTHC/CM2M_coarse_BLING-EM2/00530101.ocean_daily.nc'

netcdfutils.printInfo(filePath)

with Dataset(filePath, "r", format="NETCDF4") as nc:
    # for variable in nc.variables:
    #     # At least n dimensions needed for an nD output.
    #     print(len(nc.variables[variable].dimensions))
    #     print(nc.variables[variable].datatype.ndim)

    ncVar = nc.variables['geolat_c']
    ncDims = ncVar.get_dims()
    dims = []
    for ncDim in ncDims:
        # propRange = [0, len(ncDim)]
        dims.append(slice(0, len(ncDim)))  # slice(propRange.x, propRange.y+1))
        print(ncDim.name + ', size ', len(ncDim))
        # print('\t', )
    varData = ncVar[tuple(dims)]
    # buffer = numpy.array(varData).astype(ncVar.datatype)
    print("========================")
    print(varData)
    print("========================")
    print("Num vars: ", nc.variables)
    print()
    # # Collect all dimension names in one list.
    # for dim in nc.variables[varProp.identifier].get_dims():


##############################################################################
# Grid file of 0.25° field
# filePath = '/media/anke/OS/Research/Data/ClimateTHC/global_0.25_degree_NYF/20010101.ocean_grid.nc'
#
# <class 'netCDF4._netCDF4.Dataset'>
# root group (NETCDF3_CLASSIC data model, file format NETCDF3):
#     filename
# 		ocean_grid.nc
#     title
# 		om3_core3
#     grid_type
# 		mosaic
#     grid_tile
# 		1
#     dimensions(sizes)
# 		xt_ocean(360) [ -279.5 : 79.5 : 1 ]
# 		yt_ocean(200) [  -81.5 : 89.5 : 1 ]
# 		time(1)
# 		xu_ocean(360)
# 		yu_ocean(200) [    -81 :   90 : 1 ]
#     variables(dimensions)
# 		float64 xt_ocean(xt_ocean)
# 		float64 yt_ocean(yt_ocean)
# 		float64 time(time)
# 		float64 xu_ocean(xu_ocean)
# 		float64 yu_ocean(yu_ocean)
# 		float32 area_t(yt_ocean,xt_ocean)
# 		float32 area_u(yu_ocean,xu_ocean)
# 		float32 geolat_c(yu_ocean,xu_ocean)
# 		float32 geolon_c(yu_ocean,xu_ocean)
# 		float32 geolat_t(yt_ocean,xt_ocean)
# 		float32 geolon_t(yt_ocean,xt_ocean)
# 		float32 ht(yt_ocean,xt_ocean)
# 		float32 kmt(yt_ocean,xt_ocean)
# 		float32 hu(yu_ocean,xu_ocean)
# 		float32 rescale_rho0_mask(yt_ocean,xt_ocean)
#     groups


##################################################################
# Normal MOM5 Output (3/3.5°)
# filePath = '/media/anke/OS/Research/Data/ClimateTHC/CM2M_coarse_BLING-EM2/00530101.ocean_daily.nc'
#
# <class 'netCDF4._netCDF4.Dataset'>
# root group (NETCDF3_CLASSIC data model, file format NETCDF3):
#     filename
# 		ocean_daily.nc
#     title
# 		CM2M_coarse_BLING-EM2
#     grid_type
# 		mosaic
#     grid_tile
# 		1
#     dimensions(sizes)
# 		xt_ocean(120)
# 		yt_ocean(80)
# 		time(365)
# 		nv(2)
# 		xu_ocean(120)
# 		yu_ocean(80)
# 		st_ocean(28)
# 		st_edges_ocean(29)
# 		sw_ocean(28)
# 		sw_edges_ocean(29)
#     variables(dimensions)
# 		float64 xt_ocean(xt_ocean)
# 		float64 yt_ocean(yt_ocean)
# 		float64 time(time)
# 		float64 nv(nv)
# 		float64 xu_ocean(xu_ocean)
# 		float64 yu_ocean(yu_ocean)
# 		float64 st_ocean(st_ocean)
# 		float64 st_edges_ocean(st_edges_ocean)
# 		float64 sw_ocean(sw_ocean)
# 		float64 sw_edges_ocean(sw_edges_ocean)
# 		float32 area_t(yt_ocean,xt_ocean)
# 		float32 area_u(yu_ocean,xu_ocean)
# 		float32 geolat_c(yu_ocean,xu_ocean)
# 		float32 geolon_c(yu_ocean,xu_ocean)
# 		float32 geolat_t(yt_ocean,xt_ocean)
# 		float32 geolon_t(yt_ocean,xt_ocean)
# 		float32 ht(yt_ocean,xt_ocean)
# 		float32 kmt(yt_ocean,xt_ocean)
# 		float32 hu(yu_ocean,xu_ocean)
# 		float32 rescale_rho0_mask(yt_ocean,xt_ocean)
# 		float32 salt(time,st_ocean,yt_ocean,xt_ocean)
# 		float32 temp(time,st_ocean,yt_ocean,xt_ocean)
# 		float32 u(time,st_ocean,yu_ocean,xu_ocean)
# 		float32 v(time,st_ocean,yu_ocean,xu_ocean)
# 		float32 rho(time,st_ocean,yt_ocean,xt_ocean)
# 		float32 wt(time,sw_ocean,yt_ocean,xt_ocean)
# 		float32 tx_trans(time,st_ocean,yt_ocean,xu_ocean)
# 		float32 ty_trans(time,st_ocean,yu_ocean,xt_ocean)
# 		float32 tz_trans(time,sw_ocean,yt_ocean,xt_ocean)
# 		float64 average_T1(time)
# 		float64 average_T2(time)
# 		float64 average_DT(time)
# 		float64 time_bounds(time,nv)
#     groups:

############################################################################################################
# 0.25 degrees:
# filePath = '/media/anke/OS/Research/Data/ClimateTHC/global_0.25_degree_NYF/20010101.ocean__2000_001.nc'

# <class 'netCDF4._netCDF4.Dataset'>
# root group (NETCDF3_CLASSIC data model
# 		file format NETCDF3):
#     filename:
# 		ocean__2000_001.nc
#     title:
# 		om3_core3
#     grid_type:
# 		mosaic
#     grid_tile:
# 		1
#     dimensions(sizes):
# 		xt_ocean(360)
# 		yt_ocean(200)
# 		st_ocean(50)
# 		st_edges_ocean(51)
# 		time(31)
# 		nv(2)
# 		xu_ocean(360)
# 		yu_ocean(200)
# 		sw_ocean(50)
# 		sw_edges_ocean(51)
#     variables(dimensions):
# 		float64 xt_ocean(xt_ocean)
# 		float64 yt_ocean(yt_ocean)
# 		float64 st_ocean(st_ocean)
# 		float64 st_edges_ocean(st_edges_ocean)
# 		float64 time(time)
# 		float64 nv(nv)
# 		float64 xu_ocean(xu_ocean)
# 		float64 yu_ocean(yu_ocean)
# 		float64 sw_ocean(sw_ocean)
# 		float64 sw_edges_ocean(sw_edges_ocean)
# 		float32 salt(time,st_ocean,yt_ocean,xt_ocean)
# 		float32 temp(time,st_ocean,yt_ocean,xt_ocean)
# 		float32 u(time,st_ocean,yu_ocean,xu_ocean)
# 		float32 v(time,st_ocean,yu_ocean,xu_ocean)
# 		float32 rho(time,st_ocean,yt_ocean,xt_ocean)
# 		float32 wt(time,sw_ocean,yt_ocean,xt_ocean)
# 		float32 tx_trans(time,st_ocean,yt_ocean,xu_ocean)
# 		float32 ty_trans(time,st_ocean,yu_ocean,xt_ocean)
# 		float32 tz_trans(time,sw_ocean,yt_ocean,xt_ocean)
# 		float64 average_T1(time)
# 		float64 average_T2(time)
# 		float64 average_DT(time)
# 		float64 time_bounds(time,nv)
#     groups:
