vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO topology-tool-kit/ttk
    REF ${VERSION} 
    SHA512 811092769fd1c5dfbd230998d544607e8644a69b4be3ff433d171f255dd29cf93746bfa39d9cd8f1e53ab48aaf0714c18fe68a71818eaeeaa998ff36b2627c89
    HEAD_REF master
    PATCHES
        "config-cmake.patch"
        "vtk-module.patch"
        "vtk-module-cmake.patch"
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
