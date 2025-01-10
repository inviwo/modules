vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO topology-tool-kit/ttk
    REF cf44e5f40991582dbef04bb1cdcbc3758485c1eb
    SHA512 c354c025ad05c470f955fd94f2c2012978d016daa8a5d031cda1e4a753e37d4c009e19760bbbacd33375bdb53ab5d96bf85a17a4ba4358dd24515c2edf48b496
    HEAD_REF master
    PATCHES
        openmp.patch
)

if(VCPKG_TARGET_IS_WINDOWS)
    set(TTK_USE_OPENMP OFF)
else()
    set(TTK_USE_OPENMP ON)
endif()

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    PREFER_NINJA
    OPTIONS
        -DParaView_FOUND=OFF # Don't try finding paraview.
        -DTTK_BUILD_VTK_WRAPPERS=ON
        -DTTK_BUILD_DOCUMENTATION=OFF
        -DTTK_BUILD_PARAVIEW_PLUGINS=OFF
        -DTTK_BUILD_STANDALONE_APPS=OFF
        -DTTK_BUILD_VTK_PYTHON_MODULE=OFF

        -DTTK_ENABLE_EIGEN=ON
        -DTTK_ENABLE_ZLIB=ON
        -DTTK_ENABLE_GRAPHVIZ=ON
        -DTTK_ENABLE_OPENMP=${TTK_USE_OPENMP}
        -DTTK_ENABLE_SCIKIT_LEARN=OFF

        -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/libomp
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
