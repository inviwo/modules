
#--------------------------------------------------------------------
# Add OpenMesh add set correct folders
option(OPENMESH_BUILD_PYTHON_BINDINGS "" OFF)

set(BUILD_APPS OFF CACHE BOOL "Enable or disable building of apps" )
set(OPENMESH_DOCS OFF CACHE BOOL "Enable or disable building of documentation" )

add_subdirectory(OpenMesh)

ivw_get_targets_in_dir_recursive(openmesh_targets ${CMAKE_CURRENT_LIST_DIR}/OpenMesh)
foreach(target ${openmesh_targets})
    get_property(target_type TARGET ${target} PROPERTY TYPE)
    if (NOT ${target_type} STREQUAL  "INTERFACE_LIBRARY")
        ivw_folder(${target} "ext/OpenMesh")
        ivw_suppress_compiler_warnings(${target})
    endif()
endforeach()