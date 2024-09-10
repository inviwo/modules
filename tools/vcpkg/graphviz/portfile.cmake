
vcpkg_from_gitlab(
    GITLAB_URL https://gitlab.com
    OUT_SOURCE_PATH SOURCE_PATH
    REPO graphviz/graphviz
    REF 788ed120ba9a9f6b3923bcf67f4fa532f890e027
    SHA512 11b1174e75d859511780be21807873e2e1bfa1c72d83cd683577b0ec85651946ada4ded3071fcaf7897b7a8427ecdc8004b72ee90659a0164e93112342d35d8c
    HEAD_REF main
    PATCHES
        minimal.patch
)

vcpkg_find_acquire_program(BISON)
vcpkg_find_acquire_program(FLEX)
#vcpkg_find_acquire_program(GIT)
vcpkg_find_acquire_program(PYTHON3)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        "-DVERSION=${VERSION}"
        "-DBISON_EXECUTABLE=${BISON}"
        "-DFLEX_EXECUTABLE=${FLEX}"
        "-DPython3_EXECUTABLE=${PYTHON3}"
        "-DPKG_CONFIG_EXECUTABLE=${CURRENT_HOST_INSTALLED_DIR}/tools/pkgconf/pkgconf"

        -DENABLE_D=OFF
        -DENABLE_GO=OFF
        -DENABLE_GUILE=OFF
        -DENABLE_JAVA=OFF
        -DENABLE_JAVASCRIPT=OFF
        -DENABLE_LTDL=OFF
        -DENABLE_LUA=OFF
        -DENABLE_SHARP=OFF
        -DENABLE_SWIG=OFF
        -DENABLE_TCL=OFF
        -DGRAPHVIZ_CLI=OFF
        -Duse_coverage=OFF
        -Duse_win_pre_inst_libs=OFF
        -Dwith_cxx_api=OFF
        -Dwith_cxx_tests=OFF
        -Dwith_digcola=OFF
        -DWITH_EXPAT=ON  # We need export for html table nodes
        -DWITH_GVEDIT=OFF
        -Dwith_ipsepcola=OFF
        -Dwith_ortho=OFF
        -Dwith_sfdp=OFF
        -DWITH_SMYRNA=OFF
        -DWITH_ZLIB=OFF

        -DCMAKE_REQUIRE_FIND_PACKAGE_EXPAT=ON

        -DCMAKE_DISABLE_FIND_PACKAGE_GZIP=ON
        -DCMAKE_DISABLE_FIND_PACKAGE_ANN=ON
        -DCMAKE_DISABLE_FIND_PACKAGE_CAIRO=ON
        -DCMAKE_DISABLE_FIND_PACKAGE_GTS=ON
        -DCMAKE_DISABLE_FIND_PACKAGE_GD=ON
        -DCMAKE_DISABLE_FIND_PACKAGE_GS=ON
        -DCMAKE_DISABLE_FIND_PACKAGE_DevIL=ON
        -DCMAKE_DISABLE_FIND_PACKAGE_Freetype=ON
        -DCMAKE_DISABLE_FIND_PACKAGE_PANGOCAIRO=ON

        -DCMAKE_DISABLE_FIND_PACKAGE_WEBP=ON
        -DCMAKE_DISABLE_FIND_PACKAGE_LTDL=ON

        -Dinstall_win_dependency_dlls=OFF

        ${OPTIONS}
)
vcpkg_cmake_install()
vcpkg_fixup_cmake_targets()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")

file(GLOB plugin_config "${CURRENT_PACKAGES_DIR}/lib/graphviz/config*" "${CURRENT_PACKAGES_DIR}/bin/config*")
if(NOT plugin_config)
    message(WARNING
        "In order to create the plugin configuration file, "
        "you must run `dot -c` on the target system."
    )
endif()


vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
