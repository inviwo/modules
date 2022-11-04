
set(OPENVDB_CORE_SHARED         ${BUILD_SHARED_LIBS}  CACHE INTERNAL "Build dynamically linked version of the core library.")
if (${BUILD_SHARED_LIBS})
	set(OPENVDB_CORE_STATIC         OFF  CACHE INTERNAL "Build statically linked version of the core library.")
endif()

set(USE_EXR ON CACHE INTERNAL "Use OpenEXR.")
# OpenVDB uses old python2 so disable it by default.
set(OPENVDB_BUILD_PYTHON_MODULE OFF CACHE INTERNAL "Build the openvdb Python module.")

add_subdirectory(openvdb)

if(TARGET openvdb_print)
    ivw_folder(openvdb_print ext/OpenVDB)
endif()

if(TARGET openvdb_shared)
    ivw_suppress_compiler_warnings(openvdb_shared)
    ivw_folder(openvdb_shared ext/OpenVDB)
endif()
if(TARGET openvdb_static)
    ivw_suppress_compiler_warnings(openvdb_static)
    ivw_folder(openvdb_static ext/OpenVDB)
    alias()
endif()
