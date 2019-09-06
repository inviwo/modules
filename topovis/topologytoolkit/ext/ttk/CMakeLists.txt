find_package(VTK QUIET)

if (TARGET eigen)
	get_target_property(EIGEN_SOURCE_DIR eigen SOURCE_DIR)
	message("eigen source dir: ${EIGEN_SOURCE_DIR}")
endif()

if (VTK_FOUND)
	include(${VTK_USE_FILE})
else()
	option(TTK_BUILD_VTK_WRAPPERS "Build the TTK VTK Wrappers" OFF)
endif()
option(TTK_BUILD_PARAVIEW_PLUGINS "Build the TTK ParaView Plugins" OFF)
option(TTK_BUILD_STANDALONE_APPS "Build the TTK Standalone Applications" OFF)
option(TTK_ENABLE_CPU_OPTIMIZATION "Enable native CPU optimizations" OFF)

if (MSVC) 
	## Visual Studio supports only OpenMP 2.0 which is too old for TTK
	option(TTK_ENABLE_OPENMP "Enable OpenMP support" OFF)

	if (TTK_ENABLE_OPENMP)
		message(SEND_ERROR "Visual Studio only supports OpenMP 2.0 which is too old for TTK. "
			"Disabling option 'TTK_ENABLE_OPENMP'.")
		set(TTK_ENABLE_OPENMP OFF CACHE BOOL)
	endif()
else()
	find_package(OpenMP)
	if (OPENMP_FOUND)
	    option(TTK_ENABLE_OPENMP "Enable OpenMP support" ON)
	else()
	    option(TTK_ENABLE_OPENMP "Enable OpenMP support" OFF)
	endif()
endif()

add_subdirectory(ttk ${IVW_BINARY_DIR}/ext/ttk)

get_target_property(libs baseAll INTERFACE_LINK_LIBRARIES)
foreach(item in ${libs})
	if(TARGET ${item})
		get_property(target_type TARGET ${item} PROPERTY TYPE)
    	if (NOT ${target_type} STREQUAL  "INTERFACE_LIBRARY")
			ivw_folder(${item} ext/ttk)
			ivw_suppress_compiler_warnings(${item})
		endif()
	endif()
endforeach()

