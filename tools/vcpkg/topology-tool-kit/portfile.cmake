vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO topology-tool-kit/ttk
    REF db472ffefb9a94a2353d89bb705aef8230f85d12
    SHA512 f988e1d3bbec7794453e6b85467c984f4cf97c59f21d7aa641418e4991790980bec89ca31e8919845da7d037eeb93a5b9f85811e74fd564db24904f0563d504a
    HEAD_REF master
)

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    PREFER_NINJA
    OPTIONS
        -DTTK_BUILD_VTK_WRAPPERS=ON
        -DTTK_BUILD_DOCUMENTATION=OFF
        -DTTK_BUILD_PARAVIEW_PLUGINS=OFF
        -DTTK_BUILD_STANDALONE_APPS=OFF
        -DTTK_BUILD_VTK_PYTHON_MODULE=OFF

        -DTTK_ENABLE_EIGEN=ON
        -DTTK_ENABLE_ZLIB=ON
        -DTTK_ENABLE_OPENMP=OFF
        -DTTK_ENABLE_SCIKIT_LEARN=OFF

        -DTTK_SCRIPTS_PATH=${CURRENT_PACKAGES_DIR}/share/scripts/ttk
)
vcpkg_install_cmake()
vcpkg_copy_pdbs()
vcpkg_fixup_cmake_targets(
    CONFIG_PATH lib/cmake
    TARGET_PATH share
)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")

file(
    INSTALL ${SOURCE_PATH}/LICENSE 
    DESTINATION ${CURRENT_PACKAGES_DIR}/share/topology-tool-kit 
    RENAME copyright
)
