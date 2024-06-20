vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO topology-tool-kit/ttk
    REF 0d168a38be152d0843c7fa38a8d7fd278cb7279f
    SHA512 8d7c566b3627db891d9fa6dddc6df1b3566a78cca44ddc1352c2caaa09c2db432be9beab5d0d75d9fe0fce4e0283a179948fb6975dbd43c29d379b903b813b66
    HEAD_REF master
    PATCHES
        openmp.patch
        int128.patch

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
        -DTTK_ENABLE_GRAPHVIZ=ON
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

