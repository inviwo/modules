if (VCPKG_TARGET_IS_WINDOWS)
    vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
endif()

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO google/or-tools
    REF "v${VERSION}"
    SHA512 9e0f0c19c9e34c1a59562d602aeaf0eb22719bdb11054bc92504b4ac3e3f71bb1e5ccd7ef86d8f973f2d32c1827326448cedab3b7fd68515512a938686786d52
    HEAD_REF stable
    PATCHES
#   disable-msvc-bundle-install.patch
    disable-build-of-executables.patch
#   fix-find-protobuf-crosscompiling.patch
#   don't-export-compiler-options.patch
)

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    FEATURES
        highs USE_HIGHS
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS ${FEATURE_OPTIONS}

        -DBUILD_GLOP=OFF
        -DBUILD_PYTHON=OFF
        -DBUILD_JAVA=OFF
        -DBUILD_DOTNET=OFF

        -DBUILD_FLATZINC=OFF
        -DBUILD_MATH_OPT=ON # Needed by gurobi

        -DBUILD_SAMPLES=OFF
        -DBUILD_EXAMPLES=OFF
        -DBUILD_DOC=OFF

        -DBUILD_DEPS=OFF
        -DBUILD_ZLIB=OFF
        -DBUILD_BZip2=OFF
        -DBUILD_absl=OFF
        -DBUILD_Protobuf=OFF
        -DBUILD_Eigen3=OFF
        -DBUILD_re2=OFF

        -DBUILD_TESTING=OFF

        -DUSE_BOP=ON       # off not supported
        -DUSE_COINOR=OFF
        -DUSE_GLOP=ON      # off not supported
        -DUSE_GLPK=OFF
        -DUSE_GUROBI=ON    # off not supported

        -DUSE_HIGHS=OFF
        -DUSE_PDLP=OFF
        -DUSE_SCIP=OFF
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(PACKAGE_NAME ortools CONFIG_PATH "lib/cmake/ortools")
vcpkg_copy_pdbs()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")

file(REMOVE_RECURSE
    "${CURRENT_PACKAGES_DIR}/include/ortools/algorithms/csharp"
    "${CURRENT_PACKAGES_DIR}/include/ortools/algorithms/java"
    "${CURRENT_PACKAGES_DIR}/include/ortools/algorithms/samples"
    "${CURRENT_PACKAGES_DIR}/include/ortools/constraint_solver/csharp"
    "${CURRENT_PACKAGES_DIR}/include/ortools/constraint_solver/docs"
    "${CURRENT_PACKAGES_DIR}/include/ortools/constraint_solver/python"
    "${CURRENT_PACKAGES_DIR}/include/ortools/constraint_solver/samples"
    "${CURRENT_PACKAGES_DIR}/include/ortools/cpp"
    "${CURRENT_PACKAGES_DIR}/include/ortools/dotnet/CreateSigningKey"
    "${CURRENT_PACKAGES_DIR}/include/ortools/dotnet/docs"
    "${CURRENT_PACKAGES_DIR}/include/ortools/doxygen"
    "${CURRENT_PACKAGES_DIR}/include/ortools/flatzinc/challenge"
    "${CURRENT_PACKAGES_DIR}/include/ortools/flatzinc/mznlib"
    "${CURRENT_PACKAGES_DIR}/include/ortools/glop/samples"
    "${CURRENT_PACKAGES_DIR}/include/ortools/graph/csharp"
    "${CURRENT_PACKAGES_DIR}/include/ortools/graph/java"
    "${CURRENT_PACKAGES_DIR}/include/ortools/graph/python"
    "${CURRENT_PACKAGES_DIR}/include/ortools/graph/samples"
    "${CURRENT_PACKAGES_DIR}/include/ortools/graph/testdata"
    "${CURRENT_PACKAGES_DIR}/include/ortools/init/csharp"
    "${CURRENT_PACKAGES_DIR}/include/ortools/init/java"
    "${CURRENT_PACKAGES_DIR}/include/ortools/java/com/google/ortools/constraintsolver"
    "${CURRENT_PACKAGES_DIR}/include/ortools/java/com/google/ortools/modelbuilder"
    "${CURRENT_PACKAGES_DIR}/include/ortools/java/com/google/ortools/sat"
    "${CURRENT_PACKAGES_DIR}/include/ortools/java/docs"
    "${CURRENT_PACKAGES_DIR}/include/ortools/julia/ORTools.jl/src/c_wrapper"
    "${CURRENT_PACKAGES_DIR}/include/ortools/julia/ORTools.jl/src/moi_wrapper"
    "${CURRENT_PACKAGES_DIR}/include/ortools/julia/ORTools.jl/test/moi"
    "${CURRENT_PACKAGES_DIR}/include/ortools/julia/ORToolsBinaries.jl/deps"
    "${CURRENT_PACKAGES_DIR}/include/ortools/julia/ORToolsBinaries.jl/src"
    "${CURRENT_PACKAGES_DIR}/include/ortools/julia/ORToolsGenerated.jl/scripts"
    "${CURRENT_PACKAGES_DIR}/include/ortools/julia/ORToolsGenerated.jl/src/genproto/google/protobuf"
    "${CURRENT_PACKAGES_DIR}/include/ortools/julia/ORToolsGenerated.jl/src/genproto/operations_research/bop"
    "${CURRENT_PACKAGES_DIR}/include/ortools/julia/ORToolsGenerated.jl/src/genproto/operations_research/glop"
    "${CURRENT_PACKAGES_DIR}/include/ortools/julia/ORToolsGenerated.jl/src/genproto/operations_research/math_opt"
    "${CURRENT_PACKAGES_DIR}/include/ortools/julia/ORToolsGenerated.jl/src/genproto/operations_research/packing/vbp"
    "${CURRENT_PACKAGES_DIR}/include/ortools/julia/ORToolsGenerated.jl/src/genproto/operations_research/pdlp"
    "${CURRENT_PACKAGES_DIR}/include/ortools/julia/ORToolsGenerated.jl/src/genproto/operations_research/sat"
    "${CURRENT_PACKAGES_DIR}/include/ortools/julia/ORToolsGenerated.jl/src/genproto/operations_research/scheduling/jssp"
    "${CURRENT_PACKAGES_DIR}/include/ortools/julia/ORToolsGenerated.jl/src/genproto/operations_research/scheduling/rcpsp"
    "${CURRENT_PACKAGES_DIR}/include/ortools/julia/docs"
    "${CURRENT_PACKAGES_DIR}/include/ortools/linear_solver/csharp"
    "${CURRENT_PACKAGES_DIR}/include/ortools/linear_solver/java"
    "${CURRENT_PACKAGES_DIR}/include/ortools/linear_solver/python"
    "${CURRENT_PACKAGES_DIR}/include/ortools/linear_solver/testdata"
    "${CURRENT_PACKAGES_DIR}/include/ortools/math_opt/core/python"
    "${CURRENT_PACKAGES_DIR}/include/ortools/math_opt/elemental/python"
    "${CURRENT_PACKAGES_DIR}/include/ortools/math_opt/io/python"
    "${CURRENT_PACKAGES_DIR}/include/ortools/math_opt/python/elemental"
    "${CURRENT_PACKAGES_DIR}/include/ortools/math_opt/python/ipc"
    "${CURRENT_PACKAGES_DIR}/include/ortools/math_opt/python/testing"
    "${CURRENT_PACKAGES_DIR}/include/ortools/math_opt/samples/cpp"
    "${CURRENT_PACKAGES_DIR}/include/ortools/math_opt/samples/python"
    "${CURRENT_PACKAGES_DIR}/include/ortools/math_opt/solver_tests/testdata"
    "${CURRENT_PACKAGES_DIR}/include/ortools/math_opt/solvers/gscip/bugs"
    "${CURRENT_PACKAGES_DIR}/include/ortools/packing/testdata"
    "${CURRENT_PACKAGES_DIR}/include/ortools/pdlp/python"
    "${CURRENT_PACKAGES_DIR}/include/ortools/pdlp/samples"
    "${CURRENT_PACKAGES_DIR}/include/ortools/python/docs"
    "${CURRENT_PACKAGES_DIR}/include/ortools/routing/parsers/testdata"
    "${CURRENT_PACKAGES_DIR}/include/ortools/routing/samples"
    "${CURRENT_PACKAGES_DIR}/include/ortools/sat/colab"
    "${CURRENT_PACKAGES_DIR}/include/ortools/sat/csharp"
    "${CURRENT_PACKAGES_DIR}/include/ortools/sat/docs"
    "${CURRENT_PACKAGES_DIR}/include/ortools/sat/fuzz_testdata"
    "${CURRENT_PACKAGES_DIR}/include/ortools/sat/go/cpmodel"
    "${CURRENT_PACKAGES_DIR}/include/ortools/sat/java"
    "${CURRENT_PACKAGES_DIR}/include/ortools/sat/samples"
    "${CURRENT_PACKAGES_DIR}/include/ortools/scheduling/python"
    "${CURRENT_PACKAGES_DIR}/include/ortools/scheduling/testdata"
    "${CURRENT_PACKAGES_DIR}/include/ortools/service/v1/mathopt"
    "${CURRENT_PACKAGES_DIR}/include/ortools/set_cover/python"
    "${CURRENT_PACKAGES_DIR}/include/ortools/set_cover/samples"
    "${CURRENT_PACKAGES_DIR}/include/ortools/util/csharp"
    "${CURRENT_PACKAGES_DIR}/include/ortools/util/java"
)
