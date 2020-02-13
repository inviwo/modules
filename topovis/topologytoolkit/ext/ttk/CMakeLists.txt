
if (TARGET eigen)
	get_target_property(EIGEN_SOURCE_DIR eigen SOURCE_DIR)
	message("eigen source dir: ${EIGEN_SOURCE_DIR}")
endif()

option(TTK_BUILD_VTK_WRAPPERS "Build the TTK VTK Wrappers" OFF)
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

ivw_get_targets_in_dir_recursive(ttk_targets ${CMAKE_CURRENT_LIST_DIR}/ttk)
foreach(target ${ttk_targets})
	get_property(target_type TARGET ${target} PROPERTY TYPE)
	if (NOT ${target_type} STREQUAL  "INTERFACE_LIBRARY")
    	ivw_folder(${target} "ext/ttk")
	    ivw_suppress_compiler_warnings(${target})
    endif()
endforeach()

if(TTK_BUILD_VTK_WRAPPERS)
	if (VTK_VERSION VERSION_GREATER_EQUAL "8.90.0")
		ivw_get_targets_in_dir_recursive(vtk_targets ${CMAKE_CURRENT_LIST_DIR}/ttk/core/vtk)
		foreach(target ${vtk_targets})
			vtk_module_autoinit(TARGETS ${target} MODULES ${VTK_LIBRARIES})
			get_property(target_type TARGET ${target} PROPERTY TYPE)
		endforeach()
	endif()
endif()
