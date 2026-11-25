SUMMARY = "SentinelEdge embedded Linux control center"
DESCRIPTION = "Qt6/QML dashboard for monitoring SentinelEdge system resources"
LICENSE = "CLOSED"

SRC_URI = "file://CMakeLists.txt \
           file://src/ \
           file://qml/ \
           file://sentinel-edge-ui.service \
"

S = "${WORKDIR}"

inherit qt6-cmake systemd

DEPENDS = "qtbase qtdeclarative qtdeclarative-native"

EXTRA_OECMAKE += "-DCMAKE_BUILD_TYPE=Release"

SYSTEMD_SERVICE:${PN} = "sentinel-edge-ui.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

FILES:${PN} += "${bindir}/sentinel-edge-ui \
                ${systemd_system_unitdir}/sentinel-edge-ui.service \
"

do_install:append() {
    install -d ${D}${systemd_system_unitdir}

    install -m 0644 \
        ${WORKDIR}/sentinel-edge-ui.service \
        ${D}${systemd_system_unitdir}/sentinel-edge-ui.service
}
