SUMMARY = "SentinelEdge embedded Linux control center"
DESCRIPTION = "Qt6/QML dashboard for monitoring SentinelEdge system resources"
LICENSE = "CLOSED"

SRC_URI = "file://CMakeLists.txt \
           file://src/ \
           file://qml/ \
"

S = "${WORKDIR}"

inherit qt6-cmake

DEPENDS = "qtbase qtdeclarative qtdeclarative-native"

EXTRA_OECMAKE += "-DCMAKE_BUILD_TYPE=Release"

FILES:${PN} += "${bindir}/sentinel-edge-ui"
