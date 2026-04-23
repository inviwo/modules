#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <path-to-vcpkg-root>" >&2
    exit 1
fi

VCPKG_ROOT="$1"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "Got VCPKG: ${VCPKG_ROOT}"

SOURCE_PORT="${VCPKG_ROOT}/ports/nanovg"
OVERLAY_PORT="${SCRIPT_DIR}/nanovg"

if [[ ! -d "${SOURCE_PORT}" ]]; then
    echo "Could not find nanovg port at ${SOURCE_PORT}" >&2
    exit 1
fi

rm -rf "${OVERLAY_PORT}"
cp -R "${SOURCE_PORT}" "${OVERLAY_PORT}"

cat <<'PATCH' | patch --directory "${OVERLAY_PORT}" -p1 --forward
--- a/portfile.cmake
+++ b/portfile.cmake
@@ -4,6 +4,8 @@
     REF f93799c078fa11ed61c078c65a53914c8782c00b
     SHA512 06f55e574ac3f73f2abe6cc614e13f29d27f2e05b2a035a19084fbf69f73cc0571d808a323cd07d25f0f1cb3097bef83d10d4315999ff21d6d3c8eee494dd7fb
     HEAD_REF master
+    PATCHES
+        glbinding-fix.patch
 )

 file(COPY "${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt" DESTINATION "${SOURCE_PATH}")
PATCH

cp "${SCRIPT_DIR}/nanovg-glbinding-fix.patch" "${OVERLAY_PORT}/glbinding-fix.patch"
